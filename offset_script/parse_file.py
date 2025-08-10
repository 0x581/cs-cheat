from __future__ import annotations

from typing import Any

from offsets import Offset


NAMESPACE_KW = "namespace "
ENUM_KW = "enum class "
OFFSET_KW = "constexpr "


class LineHandler:
    def __init__(self, data: list[str]) -> None:
        self.data = data
        self.current_line = 0

    def __to_next_line(self) -> None:
        while True:
            line = self.data[self.current_line].strip()

            if not line or line.startswith("//") or line.startswith("#"):
                self.current_line += 1
            else:
                break

    def pop(self, pos: int = 0) -> str:
        assert pos == 0

        self.__to_next_line()

        assert self.current_line < len(self.data), "No more lines to pop"

        line = self.get()

        self.current_line += 1
        return line

    def get(self) -> str:
        self.__to_next_line()
        return self.data[self.current_line].strip()

    def __len__(self) -> int:
        return len(self.data) - self.current_line


class OffsetNode:
    def __init__(
        self, type: str, name: str, value: str, comment: str, parent: NamespaceNode
    ):
        self.type = type
        self.name = name
        self.value = value
        self.comment = comment
        self.parent = parent

    def to_dict(self):
        return {
            "type": self.type,
            "name": self.name,
            "value": self.value,
            "comment": self.comment,
        }

    def __str__(self) -> str:
        string = f"constexpr auto {self.name} = {self.value}; // Member of {self.parent.name}"

        if self.comment:
            string += f", {self.comment}"

        return string

    def check_offset_match(self, offset: Offset) -> bool:
        if self.name != offset.var_name:
            return False

        node: NamespaceNode | None = self.parent

        for ns in offset.namespaces[::-1]:
            if node is None or ns != node.name:
                return False

            node = node.parent

        return True


class NamespaceNode:
    def __init__(self, name: str, parent: "NamespaceNode" | None = None) -> None:
        self.parent = parent
        self.name = name
        self.namespaces: dict[str, "NamespaceNode"] = {}
        self.offsets: dict[str, OffsetNode] = {}

    def find_offset(self, offset: Offset) -> OffsetNode | None:
        name = offset.var_name
        if name in self.offsets and self.offsets[name].check_offset_match(offset):
            return self.offsets[name]

        for ns in self.namespaces.values():
            if found := ns.find_offset(offset):
                return found

        return None

    def to_dict(self) -> dict[str, Any]:
        return {
            "name": self.name,
            "namespaces": {k: v.to_dict() for k, v in self.namespaces.items()},
            "offsets": {k: v.to_dict() for k, v in self.offsets.items()},
        }


def parse_enum(data: LineHandler) -> dict[str, Any]:
    """
    Parses an enum in the data (Currently it is deleted)
    """
    assert data.get().startswith(ENUM_KW)

    l_bracket = data.get().find("{")

    while l_bracket == -1:
        data.pop()
        if len(data) == 0:
            raise ValueError("No opening bracket found for enum")

        l_bracket = data.get().find("{")

    dataline = data.get()[l_bracket + 1 :].strip()

    r_bracket = data.get().find("}")

    while r_bracket == -1:
        data.pop()
        if len(data) == 0:
            raise ValueError("No closing bracket found for enum")

        r_bracket = data.get().find("}")

    assert data.pop() == "};"

    return {}


def parse_offset(data: LineHandler, parent: NamespaceNode) -> dict[str, OffsetNode]:
    """
    Parses an offset line.
    """
    line = data.pop()
    assert line.startswith(OFFSET_KW)
    assert ";" in line, f"Invalid offset line: {line}"

    line = line[len(OFFSET_KW) :].strip()

    offset_type = line.split(maxsplit=1)[0]
    line = line[len(offset_type) + 1 :].strip()

    name = line.split("=", maxsplit=1)[0]
    line = line[len(name) + 1 :].strip()
    name = name.strip()

    value = line.split(";", maxsplit=1)[0]
    line = line[len(value) + 1 :].strip()
    value = value.strip()

    comment = ""

    c_split = line.split("//", 1)
    if len(c_split) > 1:
        comment = c_split[1].strip()

    return {name: OffsetNode(offset_type, name, value, comment, parent)}


def parse_namespace(
    data: LineHandler, parent: NamespaceNode
) -> dict[str, NamespaceNode]:
    """
    Parses the data into a list of NamespaceNodes
    """
    line = data.pop()
    assert line.startswith(NAMESPACE_KW), f"Invalid namespace declaration: {line}"

    ns_name = line[len(NAMESPACE_KW) :]
    assert ns_name.endswith("{"), f"Invalid namespace declaration: {line}"
    ns_name = ns_name[:-1].strip()

    current_ns = NamespaceNode(ns_name, parent)

    while data:
        line = data.get()
        if line.startswith(ENUM_KW):
            parse_enum(data)
            continue
        if line.startswith(NAMESPACE_KW):
            current_ns.namespaces.update(parse_namespace(data, current_ns))
            continue
        if line.startswith(OFFSET_KW):
            current_ns.offsets.update(parse_offset(data, current_ns))
            continue

        assert (
            line.strip() == "}"
        ), f"Invalid line in namespace {ns_name} L{data.current_line + 1}: {line}"
        data.pop()
        break

    return {ns_name: current_ns}


def parse(data: str) -> NamespaceNode:
    lines = LineHandler(data=data.splitlines())
    root = NamespaceNode("root")
    root.namespaces.update(parse_namespace(lines, root))

    return root

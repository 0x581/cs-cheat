#pragma once

namespace exit_codes {
	inline constexpr auto SUCCESS = 0x6969;

	inline constexpr auto NTDLL_ERROR = 0x100;
	inline constexpr auto HANDLE_LIMIT_EXCEEDED = 0x101;

	inline constexpr auto ENTITY_LIST_OOR = 0x200;

	inline constexpr auto MEMORY_READ_MODULES_ERROR = 0x300;
	inline constexpr auto MEMORY_INSPECT_ERROR = 0x301;
}

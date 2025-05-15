#pragma once

namespace modules {
	constexpr auto client = L"client.dll";
}

const std::vector<const wchar_t*> requiredModules{ modules::client };

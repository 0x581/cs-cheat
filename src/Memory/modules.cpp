#include "memory.hpp"

#include <chrono>
#include <iostream>
#include <thread>

bool Memory::loadModuleMap()
{
    size_t tries = 0;

    for (auto &[module_id, module_name] : required_modules)
    {
        uintptr_t address = 0;

        do
        {
            address = apd::get_module_base(driver_handle, module_name);

            if (address == 0)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                ++tries;
            }

        } while (address == 0 && tries < 10);

        if (tries >= 10)
        {
            // 10 attempts to get the modules failed, clear process
            std::cout << "Failed to fetch required Modules." << std::endl;

            clearModuleMap();

            return false;
        }

        this->modules[module_id] = address;
    }

    return true;
}

void Memory::clearModuleMap()
{
    this->modules.clear();
}

void Memory::setRequiredModulesList(std::vector<ModuleEntry> modules)
{
    this->required_modules.clear();

    for (const auto &module : modules)
        this->required_modules.push_back(module);
}

uintptr_t Memory::GetModuleAddress(const int moduleID) const
{
    return modules.at(moduleID);
}

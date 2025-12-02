#include "ModsMetadata.h"
#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>

ModMetadata ModMetadata::LoadFromTOML(const std::string& tomlContent) {
    ModMetadata metadata;
    try {
        auto table = toml::parse(tomlContent);
        auto mods_property = table["mod"];

        if (auto nameValue = mods_property["name"].value<std::string>()) {
            metadata.name = *nameValue;
        }

        if (auto versionValue = mods_property["version"].value<std::string>()) {
            metadata.version = *versionValue;
        }

        if (auto *depsTable = table["dependencies"].as_table()) {
            for (const auto& [key, value] : *depsTable) {
                if (auto depVersion = value.value<std::string>()) {
                    metadata.dependencies[std::string(key)] = *depVersion;
                }
            }
        }
    } catch (const toml::parse_error& err) {
        SPDLOG_ERROR("Failed to parse mods.toml: {}", err.what());
    }

    return metadata;
}
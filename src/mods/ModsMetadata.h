#pragma once

#ifdef __cplusplus
#include <string>
#include <map>

struct ModMetadata {
    std::string name;
    std::string version;
    std::map<std::string, std::string> dependencies;

    ModMetadata() : name(""), version(""), dependencies() {}
    ModMetadata LoadFromTOML(const std::string& tomlContent);
};
#endif
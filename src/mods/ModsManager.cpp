#include "archive/Archive.h"
#include "port/Engine.h"
#include "ModsMetadata.h"
#include "libultraship/src/resource/archive/O2rArchive.h"
#include "libultraship/src/resource/archive/FolderArchive.h"
#include "semver.hpp"
#include "utils/StringHelper.h"
#include <memory>
#include <optional>
#include <string>

void GenerateAssetsMods() {
    if (GameEngine::ShowYesNoBox("No O2R Files", "No O2R files found. Generate one now?") == IDYES) {
        if (!GameEngine::GenAssetFile()) {
            GameEngine::ShowMessage("Error", "An error occured, no O2R file was generated.\n\nExiting...");
            exit(1);
        }
    } else {
        exit(1);
    }
}

std::vector<std::string> ListMods() {
    const std::string main_path = Ship::Context::GetPathRelativeToAppDirectory("mk64.o2r");
    const std::string assets_path = Ship::Context::LocateFileAcrossAppDirs("spaghetti.o2r");

    std::vector<std::string> archiveFiles;
    if (std::filesystem::exists(main_path)) {
        archiveFiles.push_back(main_path);
    } else { // should not happen, but just in case
        GenerateAssetsMods();
        archiveFiles.push_back(main_path);
    }

    if (std::filesystem::exists(assets_path)) {
        archiveFiles.push_back(assets_path);
    }

    const std::string patches_path = Ship::Context::GetPathRelativeToAppDirectory("mods");

    if (std::filesystem::exists(patches_path) && std::filesystem::is_directory(patches_path)) {
        for (const auto& p : std::filesystem::directory_iterator(patches_path)) {
            auto ext = p.path().extension().string();
            if (StringHelper::IEquals(ext, ".zip") || StringHelper::IEquals(ext, ".o2r")) {
                archiveFiles.push_back(p.path().generic_string());
            } else if (std::filesystem::is_directory(p.path())) {
                archiveFiles.push_back(p.path().generic_string());
            }
        }
    }

    return archiveFiles;
}

std::vector<std::tuple<ModMetadata, std::shared_ptr<Ship::Archive>>> Mods = {};

std::optional<std::vector<std::string>> CheckCyclicDependencies() {
    auto list = std::vector<std::string>{};
    for (const auto& [metaA, _] : Mods) {
        for (const auto& [metaB, _] : Mods) {
            if (metaA.name != metaB.name) {
                // If A depends on B and B depends on A, we have a cycle
                if (metaA.dependencies.contains(metaB.name) &&
                    metaB.dependencies.contains(metaA.name)) {
                    list.push_back(metaA.name + " <-> " +  metaB.name);
                }
            }
        }
    }
    if (!list.empty()) {
        return list;
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> ModToUpdateDependencies(const ModMetadata& mod) {
    auto list = std::vector<std::string>{};
    for (const auto& [depName, depVersion] : mod.dependencies) {
        bool found = false;
        for (const auto& [otherMeta, _] : Mods) {
            if (otherMeta.name == depName) {
                found = true;
                if (otherMeta.version < depVersion) {
                    list.push_back(depName + " (required: " + depVersion.to_string() + ", found: " + otherMeta.version.to_string() + ")");
                }
                break;
            }
        }
        if (!found) {
            list.push_back(depName + " (required: " + depVersion.to_string() + ", found: none)");
        }
    }
    if (!list.empty()) {
        return list;
    }
    return std::nullopt;
}

void SortModsByDependencies() {
    std::sort(Mods.begin(), Mods.end(),
              [](const std::tuple<ModMetadata, std::shared_ptr<Ship::Archive>>& a,
                 const std::tuple<ModMetadata, std::shared_ptr<Ship::Archive>>& b) {
                  const ModMetadata& metaA = std::get<0>(a);
                  const ModMetadata& metaB = std::get<0>(b);

                  // If A depends on B, A should come after B
                  if (metaA.dependencies.contains(metaB.name)) {
                      return false;
                  }
                  // If B depends on A, B should come after A
                  if (metaB.dependencies.contains(metaA.name)) {
                      return true;
                  }
                  // Otherwise, maintain original order
                  return false;
              });
}

void AddModMetadata(const ModMetadata& metadata, const std::shared_ptr<Ship::Archive>& archive) {
    Mods.push_back(std::make_tuple(metadata, archive));
}

void AddCoreDependencies() {
    ModMetadata meta;
    meta.name = "spaghettikart-core";
    semver::parse("1.0.0", meta.version);

    semver::version<int, int, int> mk64Ver;
    semver::parse("1.0.0-alpha1", mk64Ver);
    semver::version<int, int, int> assetsVer;
    semver::parse("1.0.0-alpha1", assetsVer);
    meta.dependencies = {
        {"mk64", mk64Ver},
        {"spaghettikart-assets", assetsVer},
    };
    AddModMetadata(meta, nullptr);
}

void CheckMK64O2RExists() {
    const std::string main_path = Ship::Context::GetPathRelativeToAppDirectory("mk64.o2r");
    if (!std::filesystem::exists(main_path)) {
        GenerateAssetsMods();
    }
}

void ConstructTheListOfMods() {
    Mods.clear();
    AddCoreDependencies();
    auto list = ListMods();
    for (const auto& path : list) {
        const std::string extension = std::filesystem::path(path).extension().string();
        std::shared_ptr<Ship::Archive> archive = nullptr;
        if (StringHelper::IEquals(extension, ".o2r") || StringHelper::IEquals(extension, ".zip")) {
            archive = dynamic_pointer_cast<Ship::Archive>(std::make_shared<Ship::O2rArchive>(path));
        } else if (StringHelper::IEquals(extension, "") && std::filesystem::is_directory(path)) {
            archive = dynamic_pointer_cast<Ship::Archive>(std::make_shared<Ship::FolderArchive>(path));
        } else if (StringHelper::IEquals(extension, ".disabled")) {
            // Skip disabled mods
            continue;
        } else {
            SPDLOG_WARN("The file {} has an unrecognized extension ({}), ignoring it.", path, extension);
            continue;
        }

        int nb_unknown_files = 0;
        archive->Load();
        ModMetadata metadata;
        auto mods_file = archive->LoadFile("mods.toml");
        if (mods_file != nullptr) {
            metadata = ModMetadata::LoadFromTOML(std::string(mods_file->Buffer->data(), mods_file->Buffer->size()));
            SPDLOG_INFO("Loaded mod: {}\n{}", metadata.name, metadata.ToString());
        } else {
            const std::string msg = "The archive at path " + path +
                                    " is missing a mods.toml file. The Mod are likely incompatible.\n\n"
                                    "Do you want to continue loading the mods?";
            if (GameEngine::ShowYesNoBox("Missing mods.toml", msg.c_str()) == IDNO) {
                exit(1);
            }
            metadata.name = std::filesystem::path(path).stem().string();
            semver::parse("0.0.0", metadata.version);
            SPDLOG_WARN("The mod at path {} is missing a mods.toml file. Using default metadata:\n{}", path, metadata.ToString());
        }

        AddModMetadata(metadata, archive);
    }
}

void DetectCyclicDependencies() {
    if (auto cyclicDeps = CheckCyclicDependencies(); cyclicDeps.has_value()) {
        std::string msg = "Cyclic dependencies detected between the following mods:\n";
        for (const auto& cycle : cyclicDeps.value()) {
            msg += " - " + cycle + "\n";
        }
        msg += "\nPlease resolve these cyclic dependencies before continuing.\n";
        GameEngine::ShowMessage("Cyclic Dependency Issues", msg.c_str());
        exit(1);
    }
}

void DetectOutdatedDependencies() {
    bool exitDueToErrors = false;
    std::string allDepIssues;

    for (const auto& [meta, _] : Mods) {
        if (auto toUpdate = ModToUpdateDependencies(meta); toUpdate.has_value()) {
            exitDueToErrors = true;
            allDepIssues += "The mod \"" + meta.name + "\" has the following missing or outdated dependencies:\n";
            for (const auto& dep : toUpdate.value()) {
                allDepIssues += " - " + dep + "\n";
            }
        }
    }

    if (exitDueToErrors) {
        allDepIssues += "\nPlease resolve these dependency issues before continuing.\n";
        GameEngine::ShowMessage("Dependency Issues", allDepIssues.c_str());
        exit(1);
    }
}

void PrintListOfMods() {
    SPDLOG_INFO("List of loaded mods:");
    for (const auto& [meta, _] : Mods) {
        SPDLOG_INFO(" - {} v{}", meta.name, meta.version.to_string());
    }
}

void InitModsSystem() {
    CheckMK64O2RExists();

    ConstructTheListOfMods();

    PrintListOfMods();

    DetectCyclicDependencies();

    DetectOutdatedDependencies();

    SortModsByDependencies();

    std::vector<std::shared_ptr<Ship::Archive>> loadedArchives;
    loadedArchives.reserve(Mods.size());
    for (const auto& [_, archive] : Mods) {
        if (archive == nullptr) {
            continue;
        }
        loadedArchives.push_back(archive);
    }
    auto context = GameEngine::Instance->context;
    auto resourceManager = context->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    archiveManager->SetArchives(std::make_shared<std::vector<std::shared_ptr<Ship::Archive>>>(loadedArchives));
}

void UnInitModsSystem() {
    Mods.clear();
}
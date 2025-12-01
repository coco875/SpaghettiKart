#include "port/Engine.h"

void list_mods() {
    auto context = GameEngine::Instance->context;
    auto resourceManager = context->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    auto archives = archiveManager->GetArchives();
    for (const auto& arch : *archives) {
        SPDLOG_INFO("Archive: {}", arch->GetPath());
        auto mods_file = arch->LoadFile("mods.toml");
        if (mods_file != nullptr) {
            SPDLOG_INFO("Mods Metadata:\n{}", std::string(mods_file->Buffer->data(), mods_file->Buffer->size()));
        } else {
            SPDLOG_INFO("No mods metadata found in this archive.");
        }
    }
}
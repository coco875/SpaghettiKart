#pragma once

#include <libultraship/libultraship.h>
#include "CoreMath.h"
#include "engine/courses/Course.h"
#include <optional>
#include <nlohmann/json.hpp>

namespace Editor {
    void SaveLevel();
    void LoadLevel(Course* course, std::string sceneFile);
    void Load_AddStaticMeshActor(const nlohmann::json& actorJson);
    void SetSceneFile(std::shared_ptr<Ship::Archive> archive, std::string sceneFile);
    void LoadMinimap(Course* course, std::string filePath);

    void SaveActors(nlohmann::json& actorList);
    void SpawnActors(std::vector<std::pair<std::string, SpawnParams>> spawnList);

    extern std::shared_ptr<Ship::Archive> CurrentArchive; // This is used to retrieve and write the scene data file
    extern std::string SceneFile;
}

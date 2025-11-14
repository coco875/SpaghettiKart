#pragma once

#include <libultraship.h>
#include "SpawnParams.h"

struct ActorRegistryEntry {
    std::function<void(const SpawnParams&)> spawnFunc;
};

extern std::unordered_map<std::string, ActorRegistryEntry> gActorRegistry;

void Registry_SpawnActor(SpawnParams& params);
void RegisterActor(const std::string& name,
                   std::function<void(const SpawnParams&)> spawnFunc);

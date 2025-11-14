#include <functional>
#include <unordered_map>
#include <string>

#include "Registry.h"
#include "engine/CoreMath.h"

extern "C" {
#include "actors.h"
#include "actor_types.h"
}

std::unordered_map<std::string, ActorRegistryEntry> gActorRegistry;

void RegisterActor(const std::string& name,
                   std::function<void(const SpawnParams&)> spawnFunc)
{
    gActorRegistry[name] = { spawnFunc };
}

void Registry_SpawnActor(SpawnParams& params) {
    auto it = gActorRegistry.find(params.Name);
    if (it != gActorRegistry.end() && it->second.spawnFunc) {
        printf("[Registry] Spawned %s\n", params.Name.c_str());
        it->second.spawnFunc(params);
    }
}

// @arg name Must be a resource name such as mk:car
bool Registry_Find(const std::string& name) {
    return gActorRegistry.find(name) != gActorRegistry.end();
}

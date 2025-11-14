#include "SpawnParams.h"
#include "engine/CoreMath.h"
#include "Registry.h"
#include "engine/World.h"

#include "AllActors.h"

extern "C" {
#include "common_structs.h"
#include "actors.h"
#include "actor_types.h"
}

void RegisterGameActors() {
    RegisterActor("mk:item_box",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            spawn_item_box(pos);
        }
    );

    RegisterActor("mk:fake_item_box",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            spawn_fake_item_box(pos);
        }
    );

    RegisterActor("mk:thwomp",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OThwomp(params));
        }
    );

    RegisterActor("mk:snowman",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OSnowman(params));
        }
    );

    RegisterActor("mk:hot_air_balloon",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OHotAirBalloon(params));
        }
    );

    RegisterActor("mk:hedgehog",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OHedgehog(params));
        }
    );

    RegisterActor("mk:grand_prix_balloons",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OGrandPrixBalloons(params));
        }
    );

    RegisterActor("mk:flagpole",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OFlagpole(params));
        }
    );

    RegisterActor("mk:crab",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OCrab(params));
        }
    );

    RegisterActor("mk:cheep_cheep",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OCheepCheep(params));
        }
    );

    RegisterActor("mk:bomb_kart",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OBombKart(params));
        }
    );

    RegisterActor("mk:bat",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OBat(params));
        }
    );

    RegisterActor("mk:boos",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OBoos(params));
        }
    );

    RegisterActor("mk:trophy",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OTrophy(params));
        }
    );

    RegisterActor("mk:trash_bin",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OTrashBin(params));
        }
    );

    RegisterActor("mk:seagull",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OSeagull(params));
        }
    );

    RegisterActor("mk:chain_chomp",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OChainChomp());
        }
    );

    RegisterActor("mk:podium",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OPodium(params));
        }
    );

    RegisterActor("mk:penguin",
        [](const SpawnParams& params) {
            gWorldInstance.AddObject(new OPenguin(params));
        }
    );

    RegisterActor("mk:banana",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ABanana(params));
        }
    );

    RegisterActor("mk:mario_sign",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AMarioSign(params));
        }
    );

    RegisterActor("mk:wario_sign",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AWarioSign(params));
        }
    );

    RegisterActor("mk:falling_rock",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AFallingRock(params));
        }
    );

    RegisterActor("mk:yoshi_egg",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_YOSHI_EGG);
        }
    );

    RegisterActor("mk:piranha_plant",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_PIRANHA_PLANT);
        }
    );

    RegisterActor("mk:tree_mario_raceway",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_MARIO_RACEWAY);
        }
    );

    RegisterActor("mk:tree_yoshi_valley",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_YOSHI_VALLEY);
        }
    );

    RegisterActor("mk:tree_royal_raceway",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_ROYAL_RACEWAY);
        }
    );

    RegisterActor("mk:tree_moo_moo_farm",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_MOO_MOO_FARM);
        }
    );

    RegisterActor("mk:palm_tree",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_PALM_TREE);
        }
    );

    RegisterActor("mk:unknown_0x1a",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_LUIGI_RACEWAY);
        }
    );

    RegisterActor("mk:unknown_0x1b",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_UNKNOWN_0x1B);
        }
    );

    RegisterActor("mk:tree_peach_castle",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_PEACH_CASTLE);
        }
    );

    RegisterActor("mk:tree_frappe_snowland",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_TREE_FRAPPE_SNOWLAND);
        }
    );

    RegisterActor("mk:cactus1_kalamari_desert",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_CACTUS1_KALAMARI_DESERT);
        }
    );

    RegisterActor("mk:cactus2_kalamari_desert",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_CACTUS2_KALAMARI_DESERT);
        }
    );

    RegisterActor("mk:cactus3_kalamari_desert",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_CACTUS3_KALAMARI_DESERT);
        }
    );

    RegisterActor("mk:bush_bowsers_castle",
        [](const SpawnParams& params) {
            FVector loc = params.Location.value_or(FVector{0, 0, 0});
            Vec3f pos = { loc.x, loc.y, loc.z };
            Vec3s rot = {0, 0, 0};
            Vec3f vel = {0, 0, 0};
            add_actor_to_empty_slot(pos, rot, vel, ACTOR_BUSH_BOWSERS_CASTLE);
        }
    );

    RegisterActor("mk:train",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ATrain(params));
        }
    );

    RegisterActor("mk:paddle_boat",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ABoat(params));
        }
    );

    RegisterActor("mk:car",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ACar(params));
        }
    );

    RegisterActor("mk:truck",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ATankerTruck(params));
        }
    );

    RegisterActor("mk:tanker_truck",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ATankerTruck(params));
        }
    );

    RegisterActor("mk:bus",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ATankerTruck(params));
        }
    );

    RegisterActor("hm:spaghetti_ship",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ASpaghettiShip(params));
        }
    );

    RegisterActor("hm:ship",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AShip(params));
        }
    );

    RegisterActor("hm:starship",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AStarship(params));
        }
    );

    RegisterActor("hm:cloud",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new ACloud(params));
        }
    );

    RegisterActor("hm:text",
        [](const SpawnParams& params) {
            gWorldInstance.AddActor(new AText(params));
        }
    );
}

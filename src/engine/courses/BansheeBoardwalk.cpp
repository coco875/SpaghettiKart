#include <cstdint>
#include <libultraship.h>
#include <libultra/gbi.h>
#include <unordered_map>
#include <vector>
#include <memory>

#include "BansheeBoardwalk.h"
#include "World.h"
#include "align_asset_macro.h"
#include "engine/actors/Finishline.h"
#include "engine/objects/BombKart.h"
#include "engine/objects/CheepCheep.h"
#include "engine/objects/TrashBin.h"
#include "engine/objects/Bat.h"
#include "engine/objects/Boos.h"
#include "assets/models/tracks/banshee_boardwalk/banshee_boardwalk_data.h"
#include "assets/other/tracks/banshee_boardwalk/banshee_boardwalk_data.h"
#include "assets/textures/boo_frames.h"

extern "C" {
#include "main.h"
#include "camera.h"
#include "course_offsets.h"
#include "code_800029B0.h"
#include "render_courses.h"
#include "code_8006E9C0.h"
#include "code_80057C60.h"
#include "defines.h"
#include "math_util.h"
#include "external.h"
#include "code_80005FD0.h"
#include "spawn_players.h"
#include "render_objects.h"
#include "assets/models/common_data.h"
#include "save.h"
#include "replays.h"
#include "actors.h"
#include "collision.h"
#include "memory.h"
#include "course.h"
extern const char *banshee_boardwalk_dls[];
}

const course_texture banshee_boardwalk_textures[] = {
    { gTexture6447C4, 0x0106, 0x0800, 0x0 }, // 0x05000000
    { gTexture676FB0, 0x0525, 0x0800, 0x0 }, // 0x05000800
    { gTexture643B3C, 0x0798, 0x0800, 0x0 }, // 0x05001000
    { gTexture64BB60, 0x0169, 0x0800, 0x0 }, // 0x05001800
    { gTexture64BCCC, 0x0450, 0x0800, 0x0 }, // 0x05002000
    { gTexture64FBF4, 0x0274, 0x0800, 0x0 }, // 0x05002800
    { gTexture651B20, 0x041D, 0x0800, 0x0 }, // 0x05003000
    { gTexture66262C, 0x02F7, 0x0800, 0x0 }, // 0x05003800
    { gTexture668728, 0x01F5, 0x0800, 0x0 }, // 0x05004000
    { gTexture66A3DC, 0x07C5, 0x0800, 0x0 }, // 0x05004800
    { gTexture66CA98, 0x02C9, 0x0800, 0x0 }, // 0x05005000
    { gTexture66CD64, 0x02C0, 0x0800, 0x0 }, // 0x05005800
    { gTexture66D698, 0x0370, 0x0800, 0x0 }, // 0x05006000
    { gTexture66E608, 0x05E8, 0x0800, 0x0 }, // 0x05006800
    { gTexture67B388, 0x03D2, 0x0800, 0x0 }, // 0x05007000
    { gTextureSignWelcome0, 0x0A2E, 0x1000, 0x0 }, // 0x05007800
    { gTextureSignWelcome1, 0x0A0A, 0x1000, 0x0 }, // 0x05008800
    { gTextureSignWoodenBack0, 0x06EF, 0x1000, 0x0 }, // 0x05009800
    { gTextureSignWoodenBack1, 0x06D1, 0x1000, 0x0 }, // 0x0500A800
    { gTextureSignWoodRedArrow, 0x04E1, 0x1000, 0x0 }, // 0x0500B800
    { gTexture68D940, 0x057D, 0x0800, 0x0 }, // 0x0500C800
    { gTexture685AC0, 0x07CC, 0x1000, 0x0 }, // 0x0500D000
    { 0x00000000, 0x0000, 0x0000, 0x0 },
};

BansheeBoardwalk::BansheeBoardwalk() {
    this->vtx = d_course_banshee_boardwalk_vertex;
    this->gfx = d_course_banshee_boardwalk_packed_dls;
    this->gfxSize = 3689;
    Props.textures = banshee_boardwalk_textures;
    Props.Minimap.Texture = minimap_banshee_boardwalk;
    Props.Minimap.Width = ResourceGetTexWidthByName(Props.Minimap.Texture);
    Props.Minimap.Height = ResourceGetTexHeightByName(Props.Minimap.Texture);
    Props.Minimap.Pos[0].X = 262;
    Props.Minimap.Pos[0].Y = 170;
    Props.Minimap.PlayerX = 55;
    Props.Minimap.PlayerY = 39;
    Props.Minimap.PlayerScaleFactor = 0.016f;
    Props.Minimap.FinishlineX = 0;
    Props.Minimap.FinishlineY = 0;
    ResizeMinimap(&Props.Minimap);

    Id = "mk:banshee_boardwalk";

    Props.SetText(Props.Name, "banshee boardwalk", sizeof(Props.Name));
    Props.SetText(Props.DebugName, "ghost", sizeof(Props.DebugName));
    Props.SetText(Props.CourseLength, "747m", sizeof(Props.CourseLength));

    Props.AIBehaviour = D_0D009058;
    Props.AIMaximumSeparation = 40.0f;
    Props.AIMinimumSeparation = 0.4f;
    Props.AISteeringSensitivity = 53;

    Props.NearPersp = 2.0f;
    Props.FarPersp = 2700.0f;

    Props.PathSizes = { 0x2EE, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };

    Props.CurveTargetSpeed[0] = 4.1666665f;
    Props.CurveTargetSpeed[1] = 5.5833334f;
    Props.CurveTargetSpeed[2] = 6.1666665f;
    Props.CurveTargetSpeed[3] = 6.75f;

    Props.NormalTargetSpeed[0] = 3.75f;
    Props.NormalTargetSpeed[1] = 5.1666665f;
    Props.NormalTargetSpeed[2] = 5.75f;
    Props.NormalTargetSpeed[3] = 6.3333334f;

    Props.D_0D0096B8[0] = 3.3333332f;
    Props.D_0D0096B8[1] = 3.9166667f;
    Props.D_0D0096B8[2] = 4.5f;
    Props.D_0D0096B8[3] = 5.0833334f;

    Props.OffTrackTargetSpeed[0] = 3.75f;
    Props.OffTrackTargetSpeed[1] = 5.1666665f;
    Props.OffTrackTargetSpeed[2] = 5.75f;
    Props.OffTrackTargetSpeed[3] = 6.3333334f;

    Props.PathTable[0] = (TrackPathPoint*) LOAD_ASSET_RAW(d_course_banshee_boardwalk_unknown_waypoints);
    Props.PathTable[1] = NULL;
    Props.PathTable[2] = NULL;
    Props.PathTable[3] = NULL;

    Props.PathTable2[0] = (TrackPathPoint*) LOAD_ASSET_RAW(d_course_banshee_boardwalk_track_waypoints);
    Props.PathTable2[1] = NULL;
    Props.PathTable2[2] = NULL;
    Props.PathTable2[3] = NULL;

    Props.Clouds = NULL; // no clouds
    Props.CloudList = NULL;

    Props.Skybox.TopRight = { 0, 0, 0 };
    Props.Skybox.BottomRight = { 0, 0, 0 };
    Props.Skybox.BottomLeft = { 0, 0, 0 };
    Props.Skybox.TopLeft = { 0, 0, 0 };
    Props.Skybox.FloorTopRight = { 0, 0, 0 };
    Props.Skybox.FloorBottomRight = { 0, 0, 0 };
    Props.Skybox.FloorBottomLeft = { 0, 0, 0 };
    Props.Skybox.FloorTopLeft = { 0, 0, 0 };
    Props.Sequence = MusicSeq::MUSIC_SEQ_BANSHEE_BOARDWALK;

    Props.WaterLevel = -80.0f;
    // for (size_t i = 0; i < 100; i++) {
    //     replace_segmented_textures_with_o2r_textures((Gfx*) banshee_boardwalk_dls[i], Props.textures);
    // }
}

std::unordered_map<uint32_t, const char*> banshee_boardwalk_convert = {
    { 0x1DA0, d_course_banshee_boardwalk_packed_dl_1DA0 },
    { 0x5498, d_course_banshee_boardwalk_packed_dl_5498 },
    { 0x27D0, d_course_banshee_boardwalk_packed_dl_27D0 },
    { 0x28D0, d_course_banshee_boardwalk_packed_dl_28D0 },
    { 0x29A0, d_course_banshee_boardwalk_packed_dl_29A0 },
    { 0x2A70, d_course_banshee_boardwalk_packed_dl_2A70 },
    { 0x26E8, d_course_banshee_boardwalk_packed_dl_26E8 },
    { 0x22C8, d_course_banshee_boardwalk_packed_dl_22C8 },
    { 0x2398, d_course_banshee_boardwalk_packed_dl_2398 },
    { 0x2440, d_course_banshee_boardwalk_packed_dl_2440 },
    { 0x2520, d_course_banshee_boardwalk_packed_dl_2520 },
    { 0x2608, d_course_banshee_boardwalk_packed_dl_2608 },
    { 0x2B78, d_course_banshee_boardwalk_packed_dl_2B78 },
    { 0x3240, d_course_banshee_boardwalk_packed_dl_3240 },
    { 0x3310, d_course_banshee_boardwalk_packed_dl_3310 },
    { 0x33C8, d_course_banshee_boardwalk_packed_dl_33C8 },
    { 0x34A0, d_course_banshee_boardwalk_packed_dl_34A0 },
    { 0x1830, d_course_banshee_boardwalk_packed_dl_1830 },
    { 0x18E0, d_course_banshee_boardwalk_packed_dl_18E0 },
    { 0x1968, d_course_banshee_boardwalk_packed_dl_1968 },
    { 0x1A00, d_course_banshee_boardwalk_packed_dl_1A00 },
    { 0x30F0, d_course_banshee_boardwalk_packed_dl_30F0 },
    { 0x2C20, d_course_banshee_boardwalk_packed_dl_2C20 },
    { 0x2D40, d_course_banshee_boardwalk_packed_dl_2D40 },
    { 0x2E40, d_course_banshee_boardwalk_packed_dl_2E40 },
    { 0x2F38, d_course_banshee_boardwalk_packed_dl_2F38 },
    { 0x3020, d_course_banshee_boardwalk_packed_dl_3020 },
    { 0x3930, d_course_banshee_boardwalk_packed_dl_3930 },
    { 0x3AA0, d_course_banshee_boardwalk_packed_dl_3AA0 },
    { 0x3BA0, d_course_banshee_boardwalk_packed_dl_3BA0 },
    { 0x3C40, d_course_banshee_boardwalk_packed_dl_3C40 },
    { 0x3830, d_course_banshee_boardwalk_packed_dl_3830 },
    { 0x3638, d_course_banshee_boardwalk_packed_dl_3638 },
    { 0x36C8, d_course_banshee_boardwalk_packed_dl_36C8 },
    { 0x3740, d_course_banshee_boardwalk_packed_dl_3740 },
    { 0x37C0, d_course_banshee_boardwalk_packed_dl_37C0 },
    { 0x3D08, d_course_banshee_boardwalk_packed_dl_3D08 },
    { 0x40D0, d_course_banshee_boardwalk_packed_dl_40D0 },
    { 0x4070, d_course_banshee_boardwalk_packed_dl_4070 },
    { 0x4008, d_course_banshee_boardwalk_packed_dl_4008 },
    { 0x3F78, d_course_banshee_boardwalk_packed_dl_3F78 },
    { 0x3D90, d_course_banshee_boardwalk_packed_dl_3D90 },
    { 0x3E10, d_course_banshee_boardwalk_packed_dl_3E10 },
    { 0x3E88, d_course_banshee_boardwalk_packed_dl_3E88 },
    { 0x3F00, d_course_banshee_boardwalk_packed_dl_3F00 },
    { 0x60, d_course_banshee_boardwalk_packed_dl_60 },
    { 0x1CF8, d_course_banshee_boardwalk_packed_dl_1CF8 },
    { 0x1748, d_course_banshee_boardwalk_packed_dl_1748 },
};

void BansheeBoardwalk::Load() {
    Course::Load();

    D_800DC5BC = 1;
    D_801625EC = 0;
    D_801625F4 = 0;
    D_801625F0 = 0;
    parse_course_displaylists((TrackSections*) LOAD_ASSET_RAW(d_course_banshee_boardwalk_track_sections));
    func_80295C6C();
    find_vtx_and_set_colours((Gfx*) d_course_banshee_boardwalk_packed_dl_878, 128, 0, 0, 0);
}

void BansheeBoardwalk::LoadTextures() {
}

void BansheeBoardwalk::BeginPlay() {
    spawn_all_item_boxes((struct ActorSpawnData*)LOAD_ASSET_RAW(d_course_banshee_boardwalk_item_box_spawns));

    gWorldInstance.AddObject(new OCheepCheep(FVector(xOrientation * -1650.0, -200.0f, -1650.0f),
                                             OCheepCheep::CheepType::RACE, IPathSpan(160, 170)));

    OTrashBin::Behaviour bhv;
    if (gModeSelection == TIME_TRIALS) {
        bhv = OTrashBin::Behaviour::STATIC;
    } else {
        bhv = OTrashBin::Behaviour::MUNCHING;
    }

    if (gIsMirrorMode) {
        gWorldInstance.AddObject(new OTrashBin(FVector(1765.0f, 45.0f, 195.0f), IRotator(0, 180, 0), 1.0f, bhv));
    } else {
        gWorldInstance.AddObject(new OTrashBin(FVector(-1765.0f, 45.0f, 70.0f), IRotator(0, 0, 0), 1.0f, bhv));
    }

    if ((gGamestate != CREDITS_SEQUENCE) && (gModeSelection != TIME_TRIALS)) {
        gWorldInstance.AddObject(new OBat(FVector(0,0,0), IRotator(0, 0, 90)));
        gWorldInstance.AddObject(new OBoos(5, IPathSpan(180, 190), IPathSpan(200, 210), IPathSpan(280, 290)));
        gWorldInstance.AddObject(new OBoos(5, IPathSpan(490, 500), IPathSpan(510, 520), IPathSpan(620, 630)));
    }

    if (gModeSelection == VERSUS) {
        FVector pos = { 0, 0, 0 };

        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][110], 110, 3, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][190], 190, 1, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][250], 250, 3, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][475], 475, 1, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][610], 610, 3, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][0], 0, 0, 0.8333333f));
        gWorldInstance.AddObject(new OBombKart(pos, &gTrackPaths[0][0], 0, 0, 0.8333333f));
    }
}

void BansheeBoardwalk::InitCourseObjects() {
    size_t objectId = 0;
    if (gGamestate != CREDITS_SEQUENCE) {
        init_object(indexObjectList1[2], 0);
    }
}

void BansheeBoardwalk::UpdateCourseObjects() {
    if (gGamestate != CREDITS_SEQUENCE) {
        func_8007E4C4();
        if (gModeSelection != TIME_TRIALS) {
            // update_bat();
        }
        // wrapper_update_boos();
    }
}

void BansheeBoardwalk::RenderCourseObjects(s32 cameraId) {
    if (gGamestate != CREDITS_SEQUENCE) {
        // render_object_bat(cameraId);
        // render_object_boos(cameraId);
    }
}

void BansheeBoardwalk::SomeSounds() {
}

void BansheeBoardwalk::WhatDoesThisDo(Player* player, int8_t playerId) {
    if (((s16) gNearestPathPointByPlayerId[playerId] >= 0x180) &&
        ((s16) gNearestPathPointByPlayerId[playerId] < 0x1E1)) {
        if (D_80165300[playerId] != 1) {
            func_800CA288(playerId, 0x41);
        }
        D_80165300[playerId] = 1;
    } else {
        if (D_80165300[playerId] != 0) {
            func_800CA2B8(playerId);
            D_80165300[playerId] = 0;
        }
    }
}

void BansheeBoardwalk::WhatDoesThisDoAI(Player* player, int8_t playerId) {
    if (((s16) gNearestPathPointByPlayerId[playerId] >= 0x180) &&
        ((s16) gNearestPathPointByPlayerId[playerId] < 0x1E1)) {
        if (D_80165300[playerId] != 1) {
            func_800CA2E4(playerId, 0x41);
        }
        D_80165300[playerId] = 1;
    } else {
        if (D_80165300[playerId] != 0) {
            func_800CA30C(playerId);
            D_80165300[playerId] = 0;
        }
    }
}

void BansheeBoardwalk::Render(struct UnkStruct_800DC5EC* arg0) {
    Camera* camera = arg0->camera;
    Mat4 spCC;
    UNUSED s32 pad[6];
    Vec3f spA8;
    UNUSED s32 pad2[6];

    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    // d_course_banshee_boardwalk_packed_dl_7228
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_7228);

    gSPFogPosition(gDisplayListHead++, D_802B87B0, D_802B87B4);

    gDPPipeSync(gDisplayListHead++);

    gSPClearGeometryMode(gDisplayListHead++,
                         G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gSPSetGeometryMode(gDisplayListHead++, G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIA, G_CC_MODULATEIA);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    // d_course_banshee_boardwalk_packed_dl_5CD0
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_5CD0);
    // d_course_banshee_boardwalk_packed_dl_4E60
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_4E60);
    // d_course_banshee_boardwalk_packed_dl_69B0
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_69B0);

    render_course_segments(banshee_boardwalk_dls, arg0);

    gSPSetGeometryMode(gDisplayListHead++, G_CULL_BACK);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIA, G_CC_MODULATEIA);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gSPSetGeometryMode(gDisplayListHead++, G_SHADE | G_SHADING_SMOOTH);
    // d_course_banshee_boardwalk_packed_dl_580
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_580);
    // d_course_banshee_boardwalk_packed_dl_60
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_60);
    // d_course_banshee_boardwalk_packed_dl_540
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_540);

    if (camera->pos[1] < -20.0f) {
        // d_course_banshee_boardwalk_packed_dl_6310
        gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_6310);
    }
    spA8[0] = camera->pos[0];
    spA8[1] = -82.0f;
    spA8[2] = camera->pos[2];
    mtxf_translate(spCC, spA8);
    render_set_position(spCC, 0);

    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_dl_B278);
    gDPPipeSync(gDisplayListHead++);
}

void BansheeBoardwalk::RenderCredits() {
    gSPDisplayList(gDisplayListHead++, (Gfx*) (d_course_banshee_boardwalk_dl_B308));
}

void BansheeBoardwalk::ScrollingTextures() {
    D_802B87BC++;

    if (D_802B87BC >= 0x100) {
        D_802B87BC = 0;
    }
    find_and_set_tile_size((uintptr_t) LOAD_ASSET_RAW(d_course_banshee_boardwalk_dl_B278), 0, D_802B87BC);
}

void BansheeBoardwalk::Waypoints(Player* player, int8_t playerId) {
    s16 waypoint = gNearestPathPointByPlayerId[playerId];
    if ((waypoint >= 0x12C) && (waypoint < 0x13C)) {
        player->nearestPathPointId = 0x12CU;
    } else {
        player->nearestPathPointId = gNearestPathPointByPlayerId[playerId];
        if (player->nearestPathPointId < 0) {
            player->nearestPathPointId = gPathCountByPathIndex[0] + player->nearestPathPointId;
        }
    }
}

void BansheeBoardwalk::DrawWater(struct UnkStruct_800DC5EC* screen, uint16_t pathCounter, uint16_t cameraRot,
                                 uint16_t playerDirection) {
    gDPPipeSync(gDisplayListHead++);
    gSPTexture(gDisplayListHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_XLU_INTER, G_RM_NOOP2);
    gDPSetBlendMask(gDisplayListHead++, 0xFF);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIA, G_CC_MODULATEIA);
    // d_course_banshee_boardwalk_packed_dl_878
    gSPDisplayList(gDisplayListHead++, (Gfx*) d_course_banshee_boardwalk_packed_dl_878);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    gDPPipeSync(gDisplayListHead++);
}

void BansheeBoardwalk::CreditsSpawnActors() {
    find_vtx_and_set_colours((Gfx*) d_course_banshee_boardwalk_packed_dl_878, 0x32, 0, 0, 0);
}

void BansheeBoardwalk::Destroy() {
}

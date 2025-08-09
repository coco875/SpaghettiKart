import os
import shutil

folder = "MK64 Reloaded (SK)"
new_folder = folder + "_Migrated"

path_change = [
    ["banshee_boardwalk_data", "textures/tracks/banshee_boardwalk/banshee_boardwalk_data"],
    ["boo_frames", "textures/boo_frames"],
    ["bowser_kart", "textures/karts/bowser_kart"],
    ["bowsers_castle_data", "textures/tracks/bowsers_castle/bowsers_castle_data"],
    ["ceremony_data", "textures/ceremony_data"],
    ["choco_mountain_data", "textures/tracks/choco_mountain/choco_mountain_data"],
    ["common_data", "textures/common_data"],
    ["dks_jungle_parkway_data", "textures/tracks/dks_jungle_parkway/dks_jungle_parkway_data"],
    ["donkeykong_kart", "textures/karts/donkeykong_kart"],
    ["frappe_snowland_data", "textures/tracks/frappe_snowland/frappe_snowland_data"],
    ["kalimari_desert_data", "textures/tracks/kalimari_desert/kalimari_desert_data"],
    ["koopa_troopa_beach_data", "textures/tracks/koopa_troopa_beach/koopa_troopa_beach_data"],
    ["luigi_kart", "textures/karts/luigi_kart"],
    ["luigi_raceway_data", "textures/tracks/luigi_raceway/luigi_raceway_data"],
    ["mario_kart", "textures/karts/mario_kart"],
    ["mario_raceway_data", "textures/tracks/mario_raceway/mario_raceway_data"],
    ["moo_moo_farm_data", "textures/tracks/moo_moo_farm/moo_moo_farm_data"],
    ["other_textures", "textures/other_textures"],
    ["peach_kart", "textures/karts/peach_kart"],
    ["player_selection", "textures/player_selection"],
    ["rainbow_road_data", "textures/tracks/rainbow_road/rainbow_road_data"],
    ["royal_raceway_data", "textures/tracks/royal_raceway/royal_raceway_data"],
    ["sherbet_land_data", "textures/tracks/sherbet_land/sherbet_land_data"],
    ["startup_logo", "textures/startup_logo"],
    ["texture_data_2", "textures/texture_data_2"],
    ["texture_tkmk00", "textures/texture_tkmk00"],
    ["toad_kart", "textures/karts/toad_kart"],
    ["toads_turnpike_data", "textures/tracks/toads_turnpike/toads_turnpike_data"],
    ["wario_kart", "textures/karts/wario_kart"],
    ["wario_stadium_data", "textures/tracks/wario_stadium/wario_stadium_data"],
    ["yoshi_kart", "textures/karts/yoshi_kart"],
    ["yoshis_valley_data", "textures/tracks/yoshis_valley/yoshis_valley_data"],
]

def gen_list_kart_frame(name:str):
    return [(f"{name}_frame{int(i):03d}", [f"{name}_frame{int(i):03d}_wheel{j}" for j in range(4)]) for i in range(320)]

# duplicate kart textures
names_replacement = []
names_replacement += gen_list_kart_frame("bowser_kart")
names_replacement += gen_list_kart_frame("donkeykong_kart")
names_replacement += gen_list_kart_frame("luigi_kart")
names_replacement += gen_list_kart_frame("mario_kart")
names_replacement += gen_list_kart_frame("peach_kart")
names_replacement += gen_list_kart_frame("toad_kart")
names_replacement += gen_list_kart_frame("wario_kart")
names_replacement += gen_list_kart_frame("yoshi_kart")

def walk_directory(path):
    for root, dirs, files in os.walk(path):
        for filename in files:
            yield os.path.join(root, filename)

for path in walk_directory(folder):
    for old, new in path_change:
        if path.replace(folder + os.sep, "").startswith(old):
            new_path = path.replace(old, new)
            new_path = new_path.replace(folder, new_folder)
            os.makedirs(os.path.dirname(new_path), exist_ok=True)
            for name, new_files in names_replacement:
                if name in path:
                    for new_file in new_files:
                        new_file_path = new_path.replace(name, new_file)
                        # copy the file to the new path
                        shutil.copy2(path, new_file_path)
                    break
            else:
                # if no kart frame replacement was found, just copy the file to the new path
                shutil.copy2(path, new_path)

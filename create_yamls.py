#!/usr/bin/env python3
import sys
import zipfile

emit_debug_comments = False

# Opcodes du flux PACKED_GFX (parité avec tools/displaylist_packer.c / src/racing/memory.c)
PG_SETCOMBINE_CC_MODULATERGBA = 0x15
PG_SETCOMBINE_CC_MODULATERGBDECALA = 0x16
PG_SETCOMBINE_CC_SHADE = 0x17
PG_ENDDL  = 0x2A
PG_DL     = 0x2B
PG_RMODE_OPA = 0x18
PG_RMODE_TEXEDGE = 0x19
PG_TILECFG_A = 0x1A
PG_TILECFG_B = 0x1B
PG_TILECFG_C = 0x1C
PG_TILECFG_D = 0x1D
PG_TILECFG_E = 0x1E
PG_TILECFG_F = 0x1F
PG_TILECFG_G = 0x2C
PG_TIMG_LOADBLOCK_0 = 0x20
PG_TIMG_LOADBLOCK_1 = 0x21
PG_TIMG_LOADBLOCK_2 = 0x22
PG_TIMG_LOADBLOCK_3 = 0x23
PG_TIMG_LOADBLOCK_4 = 0x24
PG_TIMG_LOADBLOCK_5 = 0x25
PG_TEXTURE_ON  = 0x26
PG_TEXTURE_OFF = 0x27
PG_TRI1   = 0x29
PG_TRI2   = 0x58
PG_SPLINE3D = 0x30
PG_VTX1   = 0x28
PG_CULLDL = 0x2D
PG_SETCOMBINE_ALT = 0x2E
PG_RMODE_XLU = 0x2F
PG_VTX_BASE = 0x32  # 0x32..0x52 (incl.)
PG_SETCOMBINE_CC_DECALRGBA = 0x53
PG_RMODE_OPA_DECAL = 0x54
PG_RMODE_XLU_DECAL = 0x55
PG_SETGEOMETRYMODE = 0x56
PG_CLEARGEOMETRYMODE = 0x57
PG_EOF    = 0xFF

def opcode_to_string(opcode: int) -> str:
	"""Convertit un opcode en chaîne de caractères."""
	if 0x00 <= opcode <= 0x14:
		return f"PG_LIGHT_{opcode:02X}"
	if opcode == PG_SETCOMBINE_CC_MODULATERGBA:
		return "PG_SETCOMBINE_CC_MODULATERGBA"
	if opcode == PG_SETCOMBINE_CC_MODULATERGBDECALA:
		return "PG_SETCOMBINE_CC_MODULATERGBDECALA"
	if opcode == PG_SETCOMBINE_CC_SHADE:
		return "PG_SETCOMBINE_CC_SHADE"
	if opcode == PG_ENDDL:
		return "PG_ENDDL"
	if opcode == PG_DL:
		return "PG_DL"
	if opcode == PG_RMODE_OPA:
		return "PG_RMODE_OPA"
	if opcode == PG_RMODE_TEXEDGE:
		return "PG_RMODE_TEXEDGE"
	if opcode == PG_TILECFG_A:
		return "PG_TILECFG_A"
	if opcode == PG_TILECFG_B:
		return "PG_TILECFG_B"
	if opcode == PG_TILECFG_C:
		return "PG_TILECFG_C"
	if opcode == PG_TILECFG_D:
		return "PG_TILECFG_D"
	if opcode == PG_TILECFG_E:
		return "PG_TILECFG_E"
	if opcode == PG_TILECFG_F:
		return "PG_TILECFG_F"
	if opcode == PG_TILECFG_G:
		return "PG_TILECFG_G"
	if opcode == PG_TIMG_LOADBLOCK_0:
		return "PG_TIMG_LOADBLOCK_0"
	if opcode == PG_TIMG_LOADBLOCK_1:
		return "PG_TIMG_LOADBLOCK_1"
	if opcode == PG_TIMG_LOADBLOCK_2:
		return "PG_TIMG_LOADBLOCK_2"
	if opcode == PG_TIMG_LOADBLOCK_3:
		return "PG_TIMG_LOADBLOCK_3"
	if opcode == PG_TIMG_LOADBLOCK_4:
		return "PG_TIMG_LOADBLOCK_4"
	if opcode == PG_TIMG_LOADBLOCK_5:
		return "PG_TIMG_LOADBLOCK_5"
	if opcode == PG_TEXTURE_OFF:
		return "PG_TEXTURE_OFF"
	if opcode == PG_TEXTURE_ON:
		return "PG_TEXTURE_ON"
	if opcode == PG_TRI1:
		return "PG_TRI1"
	if opcode == PG_TRI2:
		return "PG_TRI2"
	if opcode == PG_SPLINE3D:
		return "PG_SPLINE3D"
	if opcode == PG_VTX1:
		return "PG_VTX1"
	if opcode == PG_CULLDL:
		return "PG_CULLDL"
	if opcode == PG_SETCOMBINE_ALT:
		return "PG_SETCOMBINE_ALT"
	if opcode == PG_RMODE_XLU:
		return "PG_RMODE_XLU"
	if PG_VTX_BASE <= opcode <= PG_VTX_BASE + 0x20:
		return f"PG_VTX_BASE + {opcode - PG_VTX_BASE:X}"
	if opcode == PG_SETCOMBINE_CC_DECALRGBA:
		return "PG_SETCOMBINE_CC_DECALRGBA"
	if opcode == PG_RMODE_OPA_DECAL:
		return "PG_RMODE_OPA_DECAL"
	if opcode == PG_RMODE_XLU_DECAL:
		return "PG_RMODE_XLU_DECAL"
	if opcode == PG_SETGEOMETRYMODE:
		return "PG_SETGEOMETRYMODE"
	if opcode == PG_CLEARGEOMETRYMODE:
		return "PG_CLEARGEOMETRYMODE"
	if opcode == PG_EOF:
		return "PG_EOF"
	return f"PG_UNKNOWN_{opcode:02X}"

# Tailles d'immédiats après opcode (en octets)
IMMEDIATE_SIZES = {
	PG_TRI1: 2,        # unpack_triangle lit 2 octets
	PG_TRI2: 4,        # unpack_quadrangle lit 4 octets
	PG_DL: 2,          # unpack_displaylist lit 2 octets (u16 big-endian)
	PG_VTX1: 4,        # unpack_vtx1 lit 4 octets
	PG_SPLINE3D: 3,    # unpack_spline_3D lit 3 octets
	PG_TIMG_LOADBLOCK_0: 3,  # unpack_tile_load_sync lit 3 octets
	PG_TIMG_LOADBLOCK_1: 3,
	PG_TIMG_LOADBLOCK_2: 3,
	PG_TIMG_LOADBLOCK_3: 3,
	PG_TIMG_LOADBLOCK_4: 3,
	PG_TIMG_LOADBLOCK_5: 3,
	PG_TILECFG_A: 2,   # unpack_tile_sync lit 2 octets
	PG_TILECFG_B: 2,
	PG_TILECFG_C: 2,
	PG_TILECFG_D: 2,
	PG_TILECFG_E: 2,
	PG_TILECFG_F: 2,
	PG_TILECFG_G: 2,
}


def read_u16_be(b: bytes, i: int) -> int:
	if i + 2 > len(b):
		raise ValueError("EOF while reading u16")
	return (b[i] << 8) | b[i + 1]


def emit_count(op: int) -> int:
	"""Nombre de commandes Gfx émises par opcode (voir src/racing/memory.c)."""
	# Lights: 3 commandes (unpack_lights)
	if 0x00 <= op <= 0x14:
		return 3
	# Combines & render modes (1 commande chacun)
	if op in {PG_SETCOMBINE_CC_MODULATERGBA, PG_SETCOMBINE_CC_MODULATERGBDECALA, PG_SETCOMBINE_CC_SHADE, 
		  PG_SETCOMBINE_CC_DECALRGBA, PG_SETCOMBINE_ALT,
		  PG_RMODE_OPA, PG_RMODE_TEXEDGE, PG_RMODE_XLU, PG_RMODE_OPA_DECAL, PG_RMODE_XLU_DECAL}:
		return 1
	# Tilecfg A..G: 3 commandes (tileSync + settile + settilesize)
	if op in {PG_TILECFG_A, PG_TILECFG_B, PG_TILECFG_C, PG_TILECFG_D, PG_TILECFG_E, PG_TILECFG_F, PG_TILECFG_G}:
		return 3
	# TIMG loadblock 0..5: 5 commandes (settimg + tilesync + settile + loadsync + loadblock)
	if op in {PG_TIMG_LOADBLOCK_0, PG_TIMG_LOADBLOCK_1, PG_TIMG_LOADBLOCK_2, 
		  PG_TIMG_LOADBLOCK_3, PG_TIMG_LOADBLOCK_4, PG_TIMG_LOADBLOCK_5}:
		return 5
	# Texture on/off: 1 commande
	if op in {PG_TEXTURE_OFF, PG_TEXTURE_ON}:
		return 1
	# VTX1, VTX2, triangles, geometry modes, etc: 1 commande
	if op in {PG_VTX1, PG_TRI1, PG_TRI2, PG_SPLINE3D, PG_CULLDL, PG_ENDDL, 
		  PG_SETGEOMETRYMODE, PG_CLEARGEOMETRYMODE, PG_DL}:
		return 1
	# VTX2 banked range (PG_VTX_BASE+1 à PG_VTX_BASE+0x20): 1 commande
	if PG_VTX_BASE + 0x01 <= op <= PG_VTX_BASE + 0x20:
		return 1
	return 0


def immediate_size(op: int) -> int:
	"""Retourne le nombre d'octets d'immédiats après l'opcode."""
	# PG_VTX_BASE (0x32) n'a pas d'immédiats
	if op == PG_VTX_BASE:
		return 0
	# VTX2 banked (0x33..0x52): 2 octets (unpack_vtx2 lit 2 octets)
	if (PG_VTX_BASE + 0x01) <= op <= (PG_VTX_BASE + 0x20):
		return 2
	# Opcodes sans immédiats
	if op in {PG_SETCOMBINE_CC_MODULATERGBA, PG_SETCOMBINE_CC_MODULATERGBDECALA, PG_SETCOMBINE_CC_SHADE,
		  PG_SETCOMBINE_CC_DECALRGBA, PG_SETCOMBINE_ALT,
		  PG_RMODE_OPA, PG_RMODE_TEXEDGE, PG_RMODE_XLU, PG_RMODE_OPA_DECAL, PG_RMODE_XLU_DECAL,
		  PG_TEXTURE_ON, PG_TEXTURE_OFF, PG_ENDDL, PG_CULLDL, 
		  PG_SETGEOMETRYMODE, PG_CLEARGEOMETRYMODE}:
		return 0
	# Opcodes lights (0x00..0x14) n'ont pas d'immédiats
	if 0x00 <= op <= 0x14:
		return 0
	# Pour les autres, utiliser le dictionnaire
	return IMMEDIATE_SIZES.get(op, 0)

def parse_displaylists(data: bytes):
	"""Parcourt le flux et retourne une liste de (offset_comp, offset_decomp) pour chaque début de DL.

	- Le 1er DL commence à comp=0, decomp=0.
	- À chaque PG_ENDDL, on aligne comp sur 8 et le prochain début de DL a decomp=octets émis jusque-là.
	"""
	pairs: list[tuple[int, int]] = []
	start = 68
	i = start
	n = len(data)
	out = 0  # octets décompressés émis (cumulés)
	# Premier début logique
	pairs.append((0, 0))
	while i < n and data[i] != PG_EOF:
		op = data[i]
		if emit_debug_comments:
			print(f"# 0x{i-start:04X}/0x{i:04X}: {opcode_to_string(op)}")
			print(f"# 0x{i-start:04X}/0x{i:04X}: immediate size = {immediate_size(op)}")
			print(f"# 0x{i-start:04X}/0x{i:04X}: emit count = {emit_count(op)}")
		i += 1
		if op == PG_EOF:
			break
		# compter les commandes émises
		out += emit_count(op) * 8
		# sauter les immédiats
		i += immediate_size(op)
		if op == PG_ENDDL:
			pairs.append((i-start, out))
	# dédoublonner par sécurité
	return pairs


def main():
	tracks_name = "yoshi_valley"
	input_path = f"models/tracks/{tracks_name}/{tracks_name}_displaylists/d_course_{tracks_name}_packed_dls"

	symbol_prefix = f"d_course_{tracks_name}_packed_dl_"

	segment_id = 0x07
	segment_base = 0x800000

	o2r = zipfile.ZipFile("mk64.o2r", "r")
	with o2r.open(input_path) as f:
		data = f.read()
	if emit_debug_comments:
		print(f"# Size of the file: ", hex(len(data)))
	pairs = parse_displaylists(data)

	p = f"models/tracks/{tracks_name}/{tracks_name}_displaylists/"

	# En-tête YAML minimal compatible avec la toolchain
	print(":config:")
	print("  segments:")
	print(f"    - [0x{segment_id:02X}, 0x{segment_base:X}]")
	print("  manual_segments:")
	for _, decomp_off in pairs:
		print(f"    - [0x{decomp_off+0x7000000:X}, \"{p}{symbol_prefix}{decomp_off:X}\"]")
	print("  header:")
	print("    code:")
	print("      - '#include <libultraship.h>'")
	print("    header:")
	print("      - '#include <libultraship.h>'")
	print("      - '#include <libultra/gbi.h>'")
	print("      - '#include <align_asset_macro.h>'")
	print("  references_packed_displaylists: true")

	# Sortie dans l'ordre d'apparition
	for comp_off, decomp_off in pairs:
		name = f"{symbol_prefix}{decomp_off:X}"
		print(f"{name}:")
		print(f"  symbol: {name}")
		print(f"  type: MK64:PACKED_GFX")
		print(f"  offset: 0x{comp_off:X}")


if __name__ == "__main__":
	sys.exit(main())


/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/rfl_format.h
*  PURPOSE:     RFL format documentation
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef RFL_FORMAT_H_INCLUDED
#define RFL_FORMAT_H_INCLUDED

#include <stdint.h>

#ifdef C_ASSERT
#undef C_ASSERT
#define HAS_C_ASSERT
#endif
#define C_ASSERT(e) extern void __C_ASSERT__##__LINE__(int [(e)?1:-1])

C_ASSERT(sizeof(float) == 4);

#ifndef HAS_C_ASSERT
#undef C_ASSERT
#endif

#ifdef __BIG_ENDIAN__
#error Big Endian not supported
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif

#pragma pack(push, 1)

#define RFL_SIGNATURE 0xD4BADA55

typedef struct _rfl_string
{
    uint16_t len;
    char str[];
} rfl_string;

typedef struct _rfl_header_t
{
    uint32_t signature; /* RFL_SIGNATURE */
    uint32_t version; /* 0xC8 is the last supported version in RF 1.2, standard maps has version 0xB4 */
    uint32_t timestamp; /* last map modification */
    uint32_t player_start_offset; /* rfl_player_start section */
    uint32_t level_info_offset; /* rfl_level_info section */
    uint32_t sections_count;
    uint32_t unknown; // sections data size - 8
    //rfl_string level_name;
    //rfl_string mod_name;
} rfl_header_t;

typedef struct _rfl_section_header_t
{
    uint32_t type;
    uint32_t size;
} rfl_section_header_t;

typedef struct _rfl_pos_t
{
    float x, y, z;
} rfl_pos_t;

/* Note: section is often followed by elements count */

enum rfl_section_type_t
{
    RFL_END                  = 0x00000000, // exists at the end of level file
    RFL_STATIC_GEOMETRY      = 0x00000100,
    RFL_GEO_REGIONS          = 0x00000200, // geo and non-geo regions
    RFL_LIGHTS               = 0x00000300,
    RFL_CUTSCENE_CAMERAS     = 0x00000400,
    RFL_AMBIENT_SOUNDS       = 0x00000500,
    RFL_EVENTS               = 0x00000600,
    RFL_MP_RESPAWNS          = 0x00000700,
    RFL_LEVEL_PROPERTIES     = 0x00000900,
    RFL_PARTICLE_EMITTERS    = 0x00000A00,
    RFL_GAS_REGIONS          = 0x00000B00,
    RFL_ROOM_EFFECTS         = 0x00000C00,
    RFL_BOLT_EMITTERS        = 0x00000E00,
    RFL_TARGETS              = 0x00000F00,
    RFL_DECALS               = 0x00001000,
    RFL_PUSH_REGIONS         = 0x00001100,
    RFL_LIGHTMAPS            = 0x00001200,
    RFL_MOVERS               = 0x00002000,
    RFL_MOVING_GROUPS        = 0x00003000,
    RFL_CUT_SCENE_PATH_NODES = 0x00005000,
    //RFL_UNKNOWN_SECT       = 0x00006000,
    RFL_TGA_UNKNOWN          = 0x00007000, // unknown purpose
    RFL_VCM_UNKNOWN          = 0x00007001, // unknown purpose
    RFL_MVF_UNKNOWN          = 0x00007002, // unknown purpose
    RFL_V3D_UNKNOWN          = 0x00007003, // unknown purpose
    RFL_VFX_UNKNOWN          = 0x00007004, // unknown purpose
    RFL_EAX_EFFECTS          = 0x00008000,
    //RFL_UNKNOWN_SECT       = 0x00010000,
    RFL_NAV_POINTS           = 0x00020000,
    RFL_ENTITIES             = 0x00030000,
    RFL_ITEMS                = 0x00040000,
    RFL_CLUTTERS             = 0x00050000,
    RFL_TRIGGERS             = 0x00060000, // trigger event, auto, door
    RFL_PLAYER_START         = 0x00070000,
    RFL_LEVEL_INFO           = 0x01000000,
    RFL_BRUSHES              = 0x02000000,
    RFL_GROUPS               = 0x03000000,
};

typedef struct _rfl_spawn_properties_t
{
    uint32_t team_id;
    uint8_t red_team;
    uint8_t blue_team;
    uint8_t bot;
} rfl_spawn_properties_t;

#if 0

/**
RFL file starts with rfl_header_t. Then goes sections. Each sction starts with rfl_section_header_t. Last section is RFL_END.

RED editor sections order:
RFL_TGA_UNKNOWN
RFL_VCM_UNKNOWN
RFL_MVF_UNKNOWN
RFL_V3D_UNKNOWN
RFL_VFX_UNKNOWN
RFL_LEVEL_PROPERTIES
...

Note: rotation matrixes are written in order: 2nd, 3rd, 1st row
**/

struct rfl_room_t
{
    uint32_t id; // uid of room effect element or big numbers (>0x70000000)
    float aabb_x1, aabb_y1, aabb_z1; // axis aligned bounding box - point 1
    float aabb_x2, aabb_y2, aabb_z2; // axis aligned bounding box - point 2
    uint8_t is_skyroom; // 1 or 0
    uint8_t is_cold; // 1 or 0
    uint8_t is_outside; // 1 or 0
    uint8_t is_airlock; // 1 or 0
    uint8_t liquid_room; // 1 or 0
    uint8_t ambient_light; // 1 or 0
    uint8_t is_subroom; // 1 or 0
    uint8_t unknown;
    float life; // -1 == infinite
    rfl_string_t eax_effect;
    float liquid_depth; // exists only if liquid_room == 1
    uint32_t liquid_color; // exists only if liquid_room == 1
    rfl_string_t liquid_surface_texture; // exists only if liquid_room == 1
    float liquid_visibility; // exists only if liquid_room == 1
    uint32_t liquid_type; // exists only if liquid_room == 1
    uint32_t liquid_alpha; // exists only if liquid_room == 1
    char liquid_unknown[13]; // exists only if liquid_room == 1
    float liquid_waveform; // 0xFFFFFFF for None, exists only if liquid_room == 1
    float liquid_surface_texture_scroll_u; // exists only if liquid_room == 1
    float liquid_surface_texture_scroll_v; // exists only if liquid_room == 1
    uint32_t ambient_color; // exists only if ambient_light == 1
};

struct rfl_vertex_t
{
    uint32_t index; // index in rfl_rooms_sect_t::vertices
    float tex_u;
    float tex_v;
    /* float unknown[2]; * lightmap texture coordinates? * */
};

#endif // 0

enum rfl_face_flags_t
{
    RFL_FF_SHOW_SKY    = 0x01,
    RFL_FF_MIRRORED    = 0x02,
    RFL_FF_UNKNOWN     = 0x04,
    RFL_FF_UNKNOWN2    = 0x08,
    RFL_FF_FULL_BRIGHT = 0x20,
    RFL_FF_UNKNOWN3    = 0x40,
    RFL_FF_UNKNOWN4    = 0x80,
    RFL_FF_MASK        = RFL_FF_SHOW_SKY | RFL_FF_MIRRORED | RFL_FF_UNKNOWN
                       | RFL_FF_UNKNOWN2 | RFL_FF_FULL_BRIGHT | RFL_FF_UNKNOWN3 | RFL_FF_UNKNOWN4,
};

#if 0

struct rfl_face_t
{
    float unknown[4]; // plane (normal vector and distance from 0)?
    uint32_t texture;
    uint32_t unknown2; // if not 0xFFFFFFFF rfl_vertex_t has lightmap coordinates; it's not lightmap id
    uint32_t unknown3; // face id? sometimes is repeated.
    char unknown4[8]; // FF FF FF FF FF FF FF FF
    uint32_t unknown5; // not 0 for portals
    uint8_t flags; // see rfl_face_flags_t
    uint8_t lightmap_res; // 1 - default, 8 - lowest, 9 - low, A - high, B - highest
    uint8_t unknown6[6];
    uint32_t room_index;
    uint32_t vertices_count;
    rfl_vertex_t vertices[vertices_count];
};

struct rfl_face_scroll_t
{
    uint32_t face_id;
    float uv;
    float vv;
};

struct rfl_rooms_sect_t
{
    char unknown[10]; // 6 for version 0xB4
    uint32_t textures_count;
    rfl_string_t textures[textures_count];
    uint32_t scroll_count;
    rfl_face_scroll_t scroll[scroll_count];
    uint32_t rooms_count; // only compiled geometry
    rfl_room_t rooms[rooms_count];
    uint32_t unknown_count; // equal to rooms_count, only compiled geometry
    struct
    {
        uint32_t mesh_index;
        uint32_t links_count; // contained meshes?
        uint32_t links[links_count];
    } unknown2[unknown_count];
    uint32_t unknown_count2;
    char unknown3[unknown_count2 * 32];
    uint32_t vertices_count;
    rfl_pos_t vertices[vertices_count];
    uint32_t faces_count;
    rfl_face_t faces[faces_count];
    uint32_t unknown_count3;
    struct
    {
        //char unk[96];
        uint32_t lightmap;
        char unk[88];
        uint32_t face; // index in faces
    } unknown4[unknown_count3];
    /* uint32_t unknown5; * only in version 0xB4 * */
};

struct rfl_level_info
{
    uint32_t unknown; // 0x00000001
    rfl_string level_name;
    rfl_string author;
    rfl_string date;
    char unknown; // 00
    uint8_t multiplayer_level; // 0 or 1
    char unknown2[220];
};

struct rfl_tga_files_t
{
    uint32_t tga_files_count;
    rfl_string tga_files[tga_files_count]; // many files, not textures
};

struct rfl_vcm_files_t
{
    uint32_t vcm_files_count;
    rfl_string vcm_files[vcm_files_count];
    uint32_t unknown[vcm_files_count]; // 0x00000001
};

struct rfl_mvf_files_t
{
    uint32_t mvf_files_count;
    rfl_string mvf_files[mvf_files_count];
    uint32_t unknown[mvf_files_count];
};

struct rfl_v3d_files_t
{
    uint32_t v3d_files_count;
    rfl_string v3d_files[v3d_files_count];
    uint32_t unknown[v3d_files_count];
};

struct rfl_vfx_files_t
{
    uint32_t vfx_files_count;
    rfl_string vfx_files[vfx_files_count];
    uint32_t unknown[vfx_files_count];
};

struct rfl_level_properies_t
{
    rfl_string geomod_texture;
    uint32_t hardness;
    uint32_t ambient_color;
    char unknown;
    uint32_t fog_color;
    float fog_near_plane;
    float fog_far_plane;
};

struct rfl_lightmap_t
{
    uint32_t w, h; // size of lightmap
    uint8_t bitmap[w * h * 3]; // bitmap (24 bpp)
};

struct rfl_lightmaps_t
{
    uint32_t lightmaps_count;
    rfl_lightmap_t lightmaps[lightmaps_count];
};

struct rfl_lights_t
{
    uint32_t lights_count;
    rfl_light_t lights[];
};

struct rfl_light_t
{
    uint32_t uid;
    rfl_string class_name; // "Light"
    char unknown[48];
    rfl_string script_name;
    char unknown2[57];
};

struct rfl_cutscene_camera_t
{
    uint32_t uid;
    rfl_string class_name; // "Cutscene Camera"
    char unknown[48];
    rfl_string script_name;
    char unknown2; /* 0x00 */
};

struct rfl_event_t
{
    uint32_t uid;
    rfl_string class_name; // depends on type
    float x, y, z;
    rfl_string script_name;
    uint8_t unknown;
    float delay;
    uint8_t bool1;
    uint8_t bool2;
    int32_t int1;
    int32_t int2;
    float float1;
    float float2;
    rfl_string str;
    rfl_string str2;
    uint32_t links_count;
    uint32_t links[links_count];
    /* float rot_matrix[3][3]; * Only if class_name is Alarm, Teleport, Teleport_Player or Play_Vclip... */
    uint32_t color;
};

struct rfl_mp_respawns_t
{
    uint32_t respawns_count;
    rfl_mp_respawn_t respawns[];
};

struct rfl_mp_respawn_t
{
    uint32_t uid;
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char zero; /* 0x00 */
    uint32_t team;
    uint8_t red_team;
    uint8_t blue_team;
    uint8_t bot;
};

struct rfl_gas_region
{
    uint32_t uid;
    rfl_string class_name;  // "Gas Region"
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char unknown2[17];
};

struct rlf_climbing_region_t
{
    uint32_t uid;
    rfl_string class_name;  // "Climbing Region"
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char unknown2[17];
};

struct rlf_bolt_emiter_t
{
    uint32_t uid;
    rfl_string class_name; // "Bolt Emiter"
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char unknown2[45];
    rfl_string image;
    char unknown3[5];
};

struct rfl_nav_points_t
{
    uint32_t nav_points_count;
    rfl_nav_point_t nav_points[nav_points_count];
    uint16_t unknown;
};

struct rfl_nav_point_t
{
    uint32_t uid;
    char unknown[33];
    uint32_t links_count;
    uint32_t links[links_count];
};

struct rfl_entity
{
    uint32_t uid;
    rfl_string class_name; // depends on type
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    uint8_t unknown;
    uint32_t cooperation;
    uint32_t friendliness;
    uint32_t team_id;
    rfl_string waypoint_list;
    rfl_string waypoint_method;
    char unknown2;
    uint8_t boarded; // 1 or 0
    uint8_t ready_to_fire_state; // 1 or 0
    uint8_t only_attack_player; // 1 or 0
    uint8_t weapon_is_holstered; // 1 or 0
    uint8_t deaf; // 1 or 0
    uint32_t sweep_min_angle;
    uint32_t sweep_max_angle;
    uint8_t ignore_terrain_when_firing; // 1 or 0
    char unknown3;
    uint8_t start_crouched; // 1 or 0
    float life;
    float armor;
    uint32_t fov;
    rfl_string default_primary_weapon;
    rfl_string default_secondary_weapon;
    rfl_string item_drop;
    rfl_string state_anim;
    rfl_string corpse_pose;
    rfl_string skin;
    rfl_string death_anim;
    uint8_t ai_mode;
    uint8_t ai_attack_style;
    char unknown4[4];
    uint32_t turret_uid;
    uint32_t alert_camera_uid;
    uint32_t alarm_event_uid;
    uint8_t run; // 1 or 0
    uint8_t start_hidden; // 1 or 0
    uint8_t wear_helmet; // 1 or 0
    uint8_t end_game_if_killed; // 1 or 0
    uint8_t cower_from_weapon; // 1 or 0
    uint8_t question_unarmed_player; // 1 or 0
    uint8_t dont_hum; // 1 or 0
    uint8_t no_shadow; // 1 or 0
    uint8_t always_simulate; // 1 or 0
    uint8_t perfect_aim; // 1 or 0
    uint8_t permanent_corpse; // 1 or 0
    uint8_t never_fly; // 1 or 0
    uint8_t never_leave; // 1 or 0
    uint8_t no_persona_messages; // 1 or 0
    uint8_t fade_corpse_immediately; // 1 or 0
    uint8_t never_collide_with_player; // 1 or 0
    uint8_t use_custom_attack_range; // 1 or 0
    /* float custom_attack_range; * if use_custom_attack_range==1 */
    rfl_string left_hand_holding;
    rfl_string right_hand_holding;
};

struct rfl_item_t
{
    uint32_t uid;
    rfl_string class_name; // depends on type
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char zero; /* 0x00 */
    uint32_t count;
    uint32_t respawn_time;
    uint32_t team_id;
};

struct rfl_clutter_t
{
    uint32_t uid;
    rfl_string class_name; // depends on type
    float x, y, z;
    float rot_matrix[3][3];
    rfl_string script_name;
    char unknown2[5];
    rfl_string skin;
    uint32_t links_count;
    uint32_t links[links_count];
};

struct rfl_trigger_t
{
    uint32_t uid;
    rfl_string script_name; // depends on type
    uint8_t unknown;
    uint8_t is_box; // 1 or 0
    char unknown2[3];
    float resets_after;
    uint16_t resets_count; // 0xFFFF - infinity
    uint16_t unknown3;
    uint8_t is_use_key_required; // 1 or 0
    rfl_string key_name;
    uint8_t weapon_activates; // 1 or 0
    uint8_t unknown4;
    uint8_t is_npc; // 1 or 0
    uint8_t is_auto; // 1 or 0
    uint8_t in_vehicle; // 1 or 0
    float x, y, z; // position
    
    float sphere_radius; //     !is_box
    float rot_matrix[3][3]; //  is_box
    float box_height; //        is_box
    float box_width; //         is_box
    float box_depth; //         is_box
    uint8_t one_way; // 1 or 0, is_box
    
    uint32_t airlock_room; // UID
    uint32_t attached_to; // UID
    uint32_t use_clutter; // UID
    uint8_t disabled; // 1 or 0
    float button_active_time;
    float inside_time;
    uint32_t unknown5; // 0xFFFFFFFF
    uint32_t links_count;
    uint32_t links[links_count];
};

struct rfl_brushes_sect_t
{
    uint32_t brushes_count;
    rfl_brush_t brushes[brushes_count];
};

struct rfl_brush_t
{
    uint32_t uid;
    float x, y, z;
    float rot_matrix[3][3];
    char unknown[10]; /* 00 00 ... */
    uint32_t textures_count;
    rfl_string_t textures[textures_count];
    char unknown2[16]; /* 00 00 ... */
    uint32_t vertices_count;
    rfl_pos_t vertices[vertices_count];
    uint32_t faces_count;
    rfl_face_t faces[faces_count];
    uint32_t unknown3; // 0
    uint32_t flags;
    uint32_t life;
    uint32_t unknown4; // 3? 0?
};

enum rfl_brush_flags
{
    RFLB_PORTAL = 0x1,
    RFLB_AIR = 0x2,
    RFLB_DETAIL = 0x4,
    RFLB_EMIT_STEAM = 0x10,
};

#endif // 0

#pragma pack(pop)

#endif // RFL_FORMAT_H_INCLUDED

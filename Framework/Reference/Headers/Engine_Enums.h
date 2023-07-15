#pragma once

namespace Engine
{
    enum TEXTURETYPE {
        TYPE_NONE = 0,
        TYPE_DIFFUSE = 1,
        TYPE_SPECULAR = 2,
        TYPE_AMBIENT = 3,
        TYPE_EMISSIVE = 4,
        TYPE_HEIGHT = 5,
        TYPE_NORMALS = 6,
        TYPE_SHININESS = 7,
        TYPE_OPACITY = 8,
        TYPE_DISPLACEMENT = 9,
        TYPE_LIGHTMAP = 10, 
        TYPE_REFLECTION = 11,
        TYPE_BASE_COLOR = 12,
        TYPE_NORMAL_CAMERA = 13,
        TYPE_EMISSION_COLOR = 14,
        TYPE_METALNESS = 15,
        TYPE_DIFFUSE_ROUGHNESS = 16,
        TYPE_AMBIENT_OCCLUSION = 17,
        TYPE_SHEEN = 19,
        TYPE_CLEARCOAT = 20,
        TYPE_TRANSMISSION = 21,
        TYPE_UNKNOWN = 18,
    };
}
#define TEXTURE_TYPE_MAX Engine::TYPE_TRANSMISSION
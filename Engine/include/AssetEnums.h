#pragma once

enum class AssetType {
    TEXTURE,
    MESH,
    SHADER,
    MATERIAL,
    SCRIPT,
    CONFIG
};

enum class AssetFormat {
    UNKNOWN = 0,
    PNG,
    DDS,
    USD,
    FBX,
    GLTF,
    TXT,
    JPEG,
    HLSL,
    TOML
};
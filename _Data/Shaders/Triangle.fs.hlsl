// © 2021 NVIDIA Corporation

#include "NRICompatibility.hlsli"
#include "ml.hlsli"

struct ModelPushConstants
{
    float4x4 proj;
    float4x4 view;
    float4x4 world;
};

NRI_ROOT_CONSTANTS( ModelPushConstants, g_ModelPushConstants, 0, 0 );


NRI_RESOURCE(
cbuffer , CommonConstants, b, 1, 0 )
{
    float4x4 gProj;
    float4x4 gView;
    float4x4 gWorld;
};



NRI_RESOURCE( Texture2D, DiffuseMap, t, 0, 1 );
NRI_RESOURCE( Texture2D, SpecularMap, t, 1, 1 );
NRI_RESOURCE( Texture2D, NormalMap, t, 2, 1 );
NRI_RESOURCE( Texture2D, EmissiveMap, t, 3, 1 );
NRI_RESOURCE( SamplerState, AnisotropicSampler, s, 0, 0 );

struct Attributes
{
    float4 Position : SV_Position;
    float4 Normal : TEXCOORD0; //.w = TexCoord.x
    float4 View : TEXCOORD1; //.w = TexCoord.y
    float4 Tangent : TEXCOORD2;
};

[earlydepthstencil]
float4 main( in Attributes input ) : SV_Target
{
    const float exposure = 0.00025;
    float4 output;
    float2 uv = float2( input.Normal.w, input.View.w );

    float4 diffuse =  SpecularMap.Sample( AnisotropicSampler, uv);
    output = diffuse;
    //output.xyz = Color::HdrToLinear(output.xyz * exposure);

    return float4(output.xyz,1.0f);
}

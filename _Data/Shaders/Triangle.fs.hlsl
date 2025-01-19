// © 2021 NVIDIA Corporation

#include "NRICompatibility.hlsli"

NRI_RESOURCE( cbuffer, CommonConstants, b, 0, 0 )
{
    float4x4 gProj;
    float4x4 gView;
};

struct PushConstants
{
    float transparency;
};

NRI_ROOT_CONSTANTS( PushConstants, g_PushConstants, 1, 0 );

NRI_RESOURCE( Texture2D, g_DiffuseTexture, t, 0, 1 );
NRI_RESOURCE( SamplerState, g_Sampler, s, 0, 1 );

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
    float4 output;
    float2 uv = float2( input.Normal.w, input.View.w );
    output.xyz = g_DiffuseTexture.Sample( g_Sampler, uv).xyz;
    output.w = g_PushConstants.transparency;
    //output = input.Normal;

    return output;
}

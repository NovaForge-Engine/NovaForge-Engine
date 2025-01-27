// © 2021 NVIDIA Corporation

#include "NRICompatibility.hlsli"

NRI_RESOURCE( cbuffer, CommonConstants, b, 0, 0 )
{
    float4x4 gProj;
    float4x4 gView;
    float4x4 gWorld;
};

struct Input
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
};


struct Attributes
{
    float4 Position : SV_Position;
    float4 Normal : TEXCOORD0; //.w = TexCoord.x
    float4 View : TEXCOORD1; //.w = TexCoord.y
    float4 Tangent : TEXCOORD2;
};

Attributes main(in Input input)
{
    Attributes output;

    float3 N = input.Normal * 2.0 -1.0;
    float4 T = input.Tangent * 2.0 - 1.0;
    float3 V = -input.Position;    

    output.Position = mul( gWorld , float4(input.Position,1));
    output.Position = mul( gView , output.Position);
    output.Position = mul( gProj , output.Position);

    output.Normal= float4( N, input.TexCoord.x);
    output.View = float4( V, input.TexCoord.y);
    output.Tangent = T;

    return output;
}

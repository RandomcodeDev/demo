// Code is derived from various files in https://github.com/SaschaWillems/Vulkan/tree/master/shaders

#include "common.hlsli"

cbuffer UBO : register(b0)
{
    float4x4 projection;
}

cbuffer OBJECT_UBO : register(b0)
{
    float4x4 model;
}

struct VERTEX_INPUT
{
    float2 position : position;
    float2 textureCoordinate : TEXCOORD;
    float4 colour : COLOR;
};

struct VERTEX_OUTPUT
{
    float4 position : SV_position;
    float2 textureCoordinate : TEXCOORD;
    float4 colour : COLOR;
};

VERTEX_OUTPUT VertexMain(VERTEX_INPUT input)
{
    VERTEX_OUTPUT output;

    const float3 Lightposition = float3(0.0, 0.0, 8.0);

    float4x4 MP = mul(model, projection);
    output.position = mul(float4(input.position, 0.0, 1.0), MP);
    output.textureCoordinate = input.textureCoordinate;
    output.colour = input.colour;

    return output;
}

Texture2D texture : register(t0);
SamplerState textureSampler : register(s0);

float4 PixelMain(VERTEX_OUTPUT input) : SV_TARGET
{
    return texture.Sample(textureSampler, input.textureCoordinate) * input.colour;
}

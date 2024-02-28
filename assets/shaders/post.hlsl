#include "common.hlsli"

Texture2D Texture : register(t2);
SamplerState TextureSampler : register(s0);

struct POST_VERTEX_OUTPUT
{
    float4 Position : SV_Position;
    VK_LOCATION(0)
    float2 Uv : TEXCOORD0;
};

POST_VERTEX_OUTPUT VertexMain(uint VertexIndex : SV_VertexID)
{
    POST_VERTEX_OUTPUT Output = (POST_VERTEX_OUTPUT) 0;
    
    Output.Uv = float2((VertexIndex << 1) & 2, VertexIndex & 2);
    Output.Position = float4(Output.Uv * 2.0f - 1.0f, 0.0f, 1.0f);
    
    return Output;
}

float4 PixelMain(VK_LOCATION(0)
float2 Uv : TEXCOORD0) : SV_TARGET
{
    return Texture.Sample(TextureSampler, Uv);
}

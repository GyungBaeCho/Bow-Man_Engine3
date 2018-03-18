#include "ConstantBuffer.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoordBase : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float2 texCoordBase : TEXCOORD0;
    float3x3 tbn : TBN;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.position = mul(float4(output.positionW, 1.0f), gmtxVP);
    output.texCoordBase = input.texCoordBase;

    float3 bitangent = normalize(cross(input.normal, input.tangent));
    output.tbn = float3x3(input.tangent, bitangent, input.normal);
    output.tbn = mul(output.tbn, gmtxWorld);
    return (output);
}
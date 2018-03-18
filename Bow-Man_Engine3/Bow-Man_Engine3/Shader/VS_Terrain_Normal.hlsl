#include "ConstantBuffer.hlsli"
#include "Function.hlsli"
#include "GBuffer.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoordBase : TEXCOORD0;
    float2 texCoordDetail : TEXCOORD1;
//    float3 tangent : TANGENT;
//    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 positionW : POSITION;
    float2 texCoordBase : TEXCOORD0;
    float2 texCoordDetail : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.position = mul(float4(output.positionW, 1.0f), gmtxVP);
    output.texCoordBase = input.texCoordBase;
    output.texCoordDetail = input.texCoordDetail;
    return output;
}
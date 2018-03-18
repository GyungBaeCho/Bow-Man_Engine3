#include "ConstantBuffer.hlsli"
#include "Texture.hlsli"

StructuredBuffer<float> gtxtAverageValues1D : register(t0);
StructuredBuffer<float> gtexAverageLum : register(t1);
StructuredBuffer<float> gtxtPrevAverageLum : register(t1);
Texture2D<float4> gtexBloom : register(t2);

RWStructuredBuffer<float> grwAverageLum : register(u0);
RWTexture2D<float4> grwDownScale : register(u1);

RWTexture2D<float4> grwOutput : register(u0);

groupshared float SharedPosition[1024];

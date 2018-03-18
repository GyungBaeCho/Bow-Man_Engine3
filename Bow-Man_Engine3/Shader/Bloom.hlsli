#include "Texture.hlsli"
#include "Function.hlsli"
#include "ConstantBuffer.hlsli"

//Factor
Texture2D<float4> gtxtHdrDownScale : register(t0);
StructuredBuffer<float> gbuffAverageLum : register(t1);
RWTexture2D<float4> grwBloom : register(u0);


Texture2D<float4> gtxtInput : register(t0);
RWTexture2D<float4> grwOutput : register(u0);

static const float SampleWeights[13] =
{
    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033,
								0.199471,
	0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216,
};

#define KERNEL_HALF		6
#define GROUP_THREAD	128

groupshared float4 SharedInput[GROUP_THREAD];

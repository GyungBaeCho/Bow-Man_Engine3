#ifndef __TERRAIN__
	#define __TERRAIN__

#include "ConstantBuffer.hlsli"
#include "Function.hlsli"
#include "GBuffer.hlsli"

#define NUM_CONTROL_POINTS 4

struct VS_INPUT
{
    float3 position			: POSITION;
    float2 texCoordBase		: TEXCOORD0;
    float2 texCoordDetail	: TEXCOORD1;
//    float3 tangent : TANGENT;
//    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float3 positionW		: POSITION;
    float2 texCoordBase		: TEXCOORD0;
    float2 texCoordDetail	: TEXCOORD1;
};


// Output control point
struct HS_OUTPUT
{
    float3 positionW		: POSITION;
    float2 texCoordBase		: TEXCOORD0;
    float2 texCoordDetail	: TEXCOORD1;
//    float4 color : COLOR0;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[NUM_CONTROL_POINTS] : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor[2] : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

struct DS_OUTPUT
{
    float4 position			: SV_POSITION;
    float3 positionW		: POSITION;
    float2 texCoordBase		: TEXCOORD0;
    float2 texCoordDetail	: TEXCOORD1;
};

#endif
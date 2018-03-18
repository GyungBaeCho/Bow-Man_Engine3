#include "Terrain.hlsli"

//fxc /E main /T ps_5_0 /Od /Zi /Fo CompiledGs.fxo PS_Terrain_Foward.hlsl
float4 main(DS_OUTPUT input) : SV_Target
{
    float4 vColor = Fog(float4(1,1,1,1), input.positionW);
    return vColor;
}
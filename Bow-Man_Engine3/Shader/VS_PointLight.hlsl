#include "PointLight.hlsli"

//fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledGs.fxo PointLightInstance.fx
VS_OUT main(VS_IN input, uint uInstanceID : SV_InstanceID)
{
    VS_OUT output;
    output.position = float4(0, 0, 0, 1);
    output.uInstanceID = uInstanceID;
    return output;
}

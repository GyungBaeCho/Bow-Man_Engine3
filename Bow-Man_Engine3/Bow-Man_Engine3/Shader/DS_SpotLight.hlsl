#include "SpotLight.hlsli"

[domain("quad")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUT, 4> quad)
{
    float2 posClipSpace = UV.xy * float2(2.0, -2.0) + float2(-1.0, 1.0);

    float2 posClipSpaceAbs = abs(posClipSpace.xy);
    float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);
	
    float2 posClipSpaceNoCylAbs = saturate(posClipSpaceAbs * ExpendAmount);
    float2 posClipSpaceNoCyl = sign(posClipSpace.xy) * posClipSpaceNoCylAbs;
    float maxLenNoCapsule = max(posClipSpaceNoCylAbs.x, posClipSpaceNoCylAbs.y);

    float3 halfSpherePos = normalize(float3(posClipSpaceNoCyl.xy, 1.0 - maxLenNoCapsule));
    halfSpherePos = normalize(float3(halfSpherePos.xy * gArrSpotLight[quad[0].uInstanceID].m_fSinOuter, gArrSpotLight[quad[0].uInstanceID].m_fCosOuter));

    float cylinderOffsetZ = saturate((maxLen * ExpendAmount - 1.0) / CylinderPortion);
	
    float4 posLS = float4(halfSpherePos.xy * (1.0 - cylinderOffsetZ), halfSpherePos.z - cylinderOffsetZ * gArrSpotLight[quad[0].uInstanceID].m_fCosOuter, 1.0);

    float3 f3Right = float3(0,0,0);
    float3 f3Up = float3(0,1,0);
    float3 f3Look = gArrSpotLight[quad[0].uInstanceID].m_vDirection;
	f3Right = normalize(cross(f3Up, f3Look));
    f3Up = normalize(cross(f3Look, f3Right));

    float4x4 LightProjection = float4x4(f3Right, 0, f3Up, 0, f3Look, 0, gArrSpotLight[quad[0].uInstanceID].m_vPosition, 1);
    LightProjection = mul(LightProjection, gmtxVP);

    DS_OUT output;
    posLS.xyz *= gArrSpotLight[quad[0].uInstanceID].m_fRange;
    output.position = mul(posLS, LightProjection);
    output.uInstanceID = quad[0].uInstanceID;
    output.cpPos = output.position.xy / output.position.w;
    return output;
}

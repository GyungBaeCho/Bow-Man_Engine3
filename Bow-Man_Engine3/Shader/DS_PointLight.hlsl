#include "PointLight.hlsli"

[domain("quad")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUT, 4> quad)
{
	float2 posClipSpace = UV.xy * 2.0 - 1.0;

	float2 posClipSpaceAbs = abs(posClipSpace.xy);
	float fMaxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

    float3 normDir = normalize(float3(posClipSpace.xy, fMaxLen - 1.0) * quad[0].HemiDir);
	float4 posLS = float4(normDir.xyz, 1.0);

	//Get Light Volume Data
	float3 f3Position = gArrPointLight[quad[0].uInstanceID].m_vPosition;
	float fRange = gArrPointLight[quad[0].uInstanceID].m_fRange + 0.1f;

	float4x4 LightProjection = { fRange, 0, 0, 0, 0, fRange, 0, 0, 0, 0, fRange, 0, f3Position, 1 };
	LightProjection = mul(LightProjection, gmtxVP);

	DS_OUT output;
	output.position = mul(posLS, LightProjection);
	output.uInstanceID = quad[0].uInstanceID;
	output.cpPos = output.position.xy / output.position.w;
	return output;
}


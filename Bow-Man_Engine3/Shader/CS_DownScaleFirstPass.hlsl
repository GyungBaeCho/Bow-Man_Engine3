#include "HDR.hlsli"

float DownScale4x4(uint2 CurPixel, uint groupThreadId)
{
	float avgLum = 0.0f;

	if (CurPixel.y < gRes.y)
	{
		int3 nFullResPos = int3(CurPixel * 4, 0);
		float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);
		
		[unroll] 
		for (int i = 0; i < 4; ++i)
		{
			[unroll] 
			for (int j = 0; j < 4; ++j)
			{
				downScaled += gtexHdr.Load(nFullResPos, int2(j, i));
			}
		}
		downScaled /= 16.0;
	//블루밍 추가
		grwDownScale[CurPixel.xy] = downScaled;
	
		avgLum = dot(downScaled, LUM_FACTOR);
		
		SharedPosition[groupThreadId] = avgLum;
		
	}
	GroupMemoryBarrierWithGroupSync();

	return avgLum;
}

float DownScale1024to4(uint dispatchThreadId, uint groupThreadId, float avgLum)
{
	[unroll]
	for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3; groupSize < 1024; groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4)
	{
		if (groupThreadId % groupSize == 0)
		{
			float stepAvgLum = avgLum;
			stepAvgLum += dispatchThreadId + step1 < gDomain ? SharedPosition[groupThreadId + step1] : avgLum;
			stepAvgLum += dispatchThreadId + step2 < gDomain ? SharedPosition[groupThreadId + step2] : avgLum;
			stepAvgLum += dispatchThreadId + step3 < gDomain ? SharedPosition[groupThreadId + step3] : avgLum;
			avgLum = stepAvgLum;
			SharedPosition[groupThreadId] = stepAvgLum;
		}
		GroupMemoryBarrierWithGroupSync();
	}
	return avgLum;
}

void DownScale4to1(uint dispatchThreadId, uint groupThreadId, uint groupId, float avgLum)
{
	if (groupThreadId == 0)
	{
		float fFinalAvgLum = avgLum;
		fFinalAvgLum += dispatchThreadId + 256 < gDomain ? SharedPosition[groupThreadId + 256] : avgLum;
		fFinalAvgLum += dispatchThreadId + 512 < gDomain ? SharedPosition[groupThreadId + 512] : avgLum;
		fFinalAvgLum += dispatchThreadId + 768 < gDomain ? SharedPosition[groupThreadId + 768] : avgLum;
		fFinalAvgLum /= 1024.0f;
		grwAverageLum[groupId] = fFinalAvgLum;
	}
}

[numthreads(1024, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 dispatchThreadId : SV_DispatchThreadID, uint3 groupThreadId : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadId.x % gRes.x, dispatchThreadId.x / gRes.x);

	float avgLum = DownScale4x4(CurPixel, groupThreadId.x);

	avgLum = DownScale1024to4(dispatchThreadId.x, groupThreadId.x, avgLum);

	DownScale4to1(dispatchThreadId.x, groupThreadId.x, groupId.x, avgLum);
}
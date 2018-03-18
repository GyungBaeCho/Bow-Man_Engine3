#include "HDR.hlsli"

#define MAX_GROUP 64

groupshared float SharedAvgFinal[MAX_GROUP];

[numthreads(MAX_GROUP, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID)
{
	float avgLum = 0.0f;

	if (dispatchThreadId.x < gGroupSize)
	{
		avgLum = gtxtAverageValues1D[dispatchThreadId.x];
	}
	SharedAvgFinal[dispatchThreadId.x] = avgLum;

	GroupMemoryBarrierWithGroupSync();

	if (dispatchThreadId.x % 4 == 0)
	{
		float stepAvgLum = avgLum;
		stepAvgLum += dispatchThreadId.x + 1 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 1] : avgLum;
		stepAvgLum += dispatchThreadId.x + 2 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 2] : avgLum;
		stepAvgLum += dispatchThreadId.x + 3 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 3] : avgLum;
		avgLum = stepAvgLum;
		SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
	}
	GroupMemoryBarrierWithGroupSync();
	
	if (dispatchThreadId.x % 16 == 0)
	{
		float stepAvgLum = avgLum;
		stepAvgLum += dispatchThreadId.x + 4 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 4] : avgLum;
		stepAvgLum += dispatchThreadId.x + 8 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 8] : avgLum;
		stepAvgLum += dispatchThreadId.x + 12 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 12] : avgLum;
		avgLum = stepAvgLum;
		SharedAvgFinal[dispatchThreadId.x] = stepAvgLum;
	}
	GroupMemoryBarrierWithGroupSync();

	if (dispatchThreadId.x == 0)
	{
		float fFinalLumValue = avgLum;
		fFinalLumValue += dispatchThreadId.x + 16 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 16] : avgLum;
		fFinalLumValue += dispatchThreadId.x + 32 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 32] : avgLum;
		fFinalLumValue += dispatchThreadId.x + 48 < gGroupSize ? SharedAvgFinal[dispatchThreadId.x + 48] : avgLum;
		fFinalLumValue /= 64.0;

		float fAdaptedAverageLum = lerp(gtxtPrevAverageLum[0], fFinalLumValue, gfAdaption);

		grwAverageLum[0] = max(fAdaptedAverageLum, 0.0001);
	}
}

#ifndef __CONSTBUFFER__
	#define __CONSTBUFFER__

#define ETC_SLOT			b0
#define HDR_SLOT			b0
#define CAMERA_SLOT			b1
#define WORLD_MATRIX_SLOT	b2
#define LIGHT_SLOT			b3
#define MATERIAL_SLOT		b4
#define FOG_SLOT			b5
#define BONE_SLOT

#define LINEAR_FOG			1.0f
#define EXP_FOG				2.0f
#define EXP2_FOG			3.0f

#define _WITH_THETA_PHI_CONES
#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_REFLECT


//----------------------------------------------------------------
//cbuffer cbCamera : register(ETC_SLOT)
//{
//	matrix gmtxViewInversed : packoffset(c0);
//	matrix gmtxOrtho : packoffset(c4);
//	matrix gmtxVP : packoffset(c8);
//	matrix gmtxShadowVP : packoffset(c12);
//	float4 gvPerspectiveValue : packoffset(c16);
//	float4 gvCameraPosition : packoffset(c17);
//	float4 gvCameraDirection : packoffset(c18);
//};	

//----------------------------------------------------------------
static const float4 LUM_FACTOR = float4(0.299f, 0.587f, 0.114f, 0);
//static const float4 LUM_FACTOR = float4(0.0, 0.0, 1, 0);

cbuffer cbHdr : register(HDR_SLOT)
{
	uint2 gRes				: packoffset(c0.x);
	uint gDomain			: packoffset(c0.z);
	uint gGroupSize			: packoffset(c0.w);
	float gfAdaption		: packoffset(c1.x);
	float gfBloomThreshold	: packoffset(c1.y);
}

cbuffer cbToneMapping : register(HDR_SLOT)
{
	float gfWhite		: packoffset(c0.x);
	float gMiddleGrey	: packoffset(c0.y);
	float gLumWhiteSqr	: packoffset(c0.z);
	float gfBloomScale	: packoffset(c0.w);
}

//----------------------------------------------------------------
cbuffer cbCamera : register(CAMERA_SLOT)
{
	matrix gmtxView				: packoffset(c0);
    matrix gmtxViewInversed		: packoffset(c4);
	matrix gmtxOrtho			: packoffset(c8);
	matrix gmtxProjection		: packoffset(c12);
	matrix gmtxVP				: packoffset(c16);
	matrix gmtxShadowVP			: packoffset(c20);
	float4 gvPerspectiveValue	: packoffset(c24);
	float3 gvCameraPosition		: packoffset(c25);
    float  gfNearPlaneDistant	: packoffset(c25.w);
	float3 gvCameraDirection	: packoffset(c26);
    float  gfFarPlaneDistant	: packoffset(c26.w);
    float4 garrFrustum[6]		: packoffset(c27);
	//							: packoffset(c33);
};

//----------------------------------------------------------------
cbuffer cbWorldMatrix : register(WORLD_MATRIX_SLOT)
{
	matrix gmtxWorld : packoffset(c0);
};

//----------------------------------------------------------------
struct DirLight
{
	float4	m_vUpperColor;
	float4	m_vDownColor;
	float4	m_vColorSpecular;
	float3	m_vDirection;
	float	m_fPower;
};

struct PointLight
{
	float4	m_vColorSpecular;
	float3	m_vPosition;
	float	m_fPower;
	float	m_fRange;
	float	m_padding1;
	float	m_padding2;
	float	m_padding3;
};

struct SpotLight
{
	float4	m_vColorSpecular;
	float3	m_vPosition;
	float	m_fPower;
	float3	m_vDirection;
	float	m_fRange;
	float	m_fCosOuter;
	float	m_fSinOuter;
	float	m_fCosAttRcp;
	float	m_padding1;
};

#define MAX_DIR_LIGHTS		5
#define MAX_POINT_LIGHTS	500
#define MAX_SPOT_LIGHTS		500

cbuffer cbLight : register(LIGHT_SLOT)
{
	DirLight		gArrDirLight[MAX_DIR_LIGHTS];
	PointLight		gArrPointLight[MAX_POINT_LIGHTS];
	SpotLight		gArrSpotLight[MAX_SPOT_LIGHTS];
	int4			gnLight;
};

//----------------------------------------------------------------
struct MATERIAL
{
	float4 m_vDiffuseSpec;
};

cbuffer cbMaterial : register(MATERIAL_SLOT)
{
	MATERIAL gcMaterial;
};

//----------------------------------------------------------------
cbuffer cbFog : register(FOG_SLOT)
{
	float4 gcFogColor;
	float4 gvFogParameter; //(Mode, Start, End, Density)
};

//----------------------------------------------------------------
#define MAX_BONE_MATRICES	50

cbuffer cbBoneWorld : register(b6)
{
	matrix gcBoneMatrix[MAX_BONE_MATRICES];
};


#endif
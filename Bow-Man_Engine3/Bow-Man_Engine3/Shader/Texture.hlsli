#ifndef __TEXTURE__
	#define __TEXTURE__


//텍스쳐 매핑을 위한 쉐이더 변수
SamplerState gSSWarp		: register(s0);
SamplerState gSSClamp		: register(s1);
//SamplerState gssSkyBox					: register(s4);

Texture2D gtexAlbedo		: register(t0);
Texture2D gtexHdr			: register(t0);

Texture2D gtexNormalDisp	: register(t1);
Texture2D gtexNormalDepth	: register(t1);
Texture2D gtexHeightMap		: register(t1);

Texture2D gtexPositionSpec	: register(t2);
Texture2D gtexNormalMap		: register(t2);

Texture2D gtexShadowMap		: register(t3);
Texture2D gtexDepth			: register(t3);
Texture2D gtexDetail1		: register(t3);

Texture2D gtexSpecular		: register(t4);

//TextureCube gtxtSkyBox	: register(t4);

Texture2DArray gTreeTextureArray : register(t4);

#endif
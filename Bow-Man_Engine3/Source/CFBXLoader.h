// *********************************************************************************************************************

// 외부 FBX Master 참조중...

// 분석을 위해 외부에서 가져온 Code임을 밝힘

// *********************************************************************************************************************

#pragma once
#ifndef FBXSDK_NEW_API
#define FBXSDK_NEW_API	// 륷궢궋긫?긙깈깛럊궎궴궖뾭
#endif

// UVSet뼹, 뮯?볙궻UV긜긞긣룈룜
typedef std::tr1::unordered_map<std::string, int> UVsetID;
// UVSet뼹, 긡긏긚?긿긬긚뼹(괦궰궻UVSet궸븸릶궻긡긏긚?긿궕귆귞돷궕궯궲귡궞궴궕궇귡)
typedef std::tr1::unordered_map<std::string, std::vector<std::string>> TextureSet;

namespace FBX_LOADER
{

struct FBX_MATRIAL_ELEMENT
{
	enum MATERIAL_ELEMENT_TYPE
	{
		ELEMENT_NONE = 0,
		ELEMENT_COLOR,
		ELEMENT_TEXTURE,
		ELEMENT_BOTH,
		ELEMENT_MAX,
	};

	MATERIAL_ELEMENT_TYPE type;
	float r, g, b, a;
	TextureSet			textureSetArray;

	FBX_MATRIAL_ELEMENT()
	{
		textureSetArray.clear();
	}

	~FBX_MATRIAL_ELEMENT()
	{
		Release();
	}

	void Release()
	{
		for (TextureSet::iterator it=textureSetArray.begin();it!=textureSetArray.end();++it)
		{
			it->second.clear();
		}

		textureSetArray.clear();
	}
};

struct FBX_MATERIAL_NODE
{
	// FBX궻?긡깏귺깑궼Lambert궴Phong궢궔궶궋
	enum eMATERIAL_TYPE
	{
		MATERIAL_LAMBERT = 0,
		MATERIAL_PHONG,
	};

	eMATERIAL_TYPE type;
	FBX_MATRIAL_ELEMENT ambient;
	FBX_MATRIAL_ELEMENT diffuse;
	FBX_MATRIAL_ELEMENT emmisive;
	FBX_MATRIAL_ELEMENT specular;

	float shininess;
	float TransparencyFactor;		// 벁됡뱗
};

// 긽긞긘깄?맟뾴멹
struct MESH_ELEMENTS
{
	unsigned int	numPosition;		// 뮯?띆뷭궻긜긞긣귩궋궘궰렃궰궔
	unsigned int	numNormal;			//
	unsigned int	numUVSet;			// UV긜긞긣릶
};

//
struct FBX_MESH_NODE
{
	std::string		name;			// 긩?긤뼹
	std::string		parentName;		// 릂긩?긤뼹(릂궕궋궶궋궶귞"null"궴궋궎뼹뤝궕볺귡.root긩?긤궻뫮돒)
	
	MESH_ELEMENTS	elements;		// 긽긞긘깄궕뺎렃궥귡긢???몾
	std::vector<FBX_MATERIAL_NODE> m_materialArray;		// ?긡깏귺깑
	UVsetID		uvsetID;

	std::vector<unsigned int>		indexArray;				// 귽깛긢긞긏긚봹쀱
	std::vector<FbxVector4>			m_positionArray;		// ?긙긘깈깛봹쀱
	std::vector<FbxVector4>			m_normalArray;			// ?멄봹쀱
	std::vector<FbxVector2>			m_texcoordArray;		// 긡긏긚?긿띆뷭봹쀱

	float	mat4x4[16];	// Matrix

	~FBX_MESH_NODE()
	{
		Release();
	}

	void Release()
	{
		uvsetID.clear();
		m_texcoordArray.clear();
		m_materialArray.clear();
		indexArray.clear();
		m_positionArray.clear();
		m_normalArray.clear();
	}
};

class CFBXLoader
{
public:
	enum eAXIS_SYSTEM
	{
		eAXIS_DIRECTX = 0,
		eAXIS_OPENGL,
	};

protected:
	// FBX SDK
	FbxManager* mSdkManager;
	FbxScene*	mScene;
	FbxImporter * mImporter;
    FbxAnimLayer * mCurrentAnimLayer;
	std::vector<FBX_MESH_NODE>		m_meshNodeArray;

protected:
	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	void TriangulateRecursive(FbxNode* pNode);

	void SetupNode(FbxNode* pNode, std::string parentName);
	void Setup();

	void CopyVertexData(FbxMesh*	pMesh, FBX_MESH_NODE* meshNode);
	void CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat);

	void ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode);

	void SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor);
	FbxDouble3 GetMaterialProperty(
		const FbxSurfaceMaterial * pMaterial,
		const char * pPropertyName,
        const char * pFactorPropertyName,
        FBX_MATRIAL_ELEMENT*			pElement);

	static void FBXMatrixToFloat16(FbxMatrix* src, float dest[16])
	{
		unsigned int nn = 0;
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				dest[nn] = static_cast<float>( src->Get(i,j) );
				nn++;
			}
		}
	}

public:
	CFBXLoader();
	~CFBXLoader();

	void Release();
	
	// 벶귒뜛귒
	HRESULT LoadFBX(const char* filename, const eAXIS_SYSTEM axis);
	FbxNode&	GetRootNode();

	size_t GetNodesCount(){ return m_meshNodeArray.size(); };		// 긩?긤릶궻롦벦

	FBX_MESH_NODE&	GetNode(const unsigned int id);
};

}	// FBX_LOADER
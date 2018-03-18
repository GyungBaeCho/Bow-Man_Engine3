#pragma once

//Dummy Class
class CModelAssembly;
class CTexture;
class CMaterial;
class CUiObject;

class CResource{
private:
	std::wstring m_wsName;
	static UINT m_idxDefault;

public:
	CResource(){
		WCHAR idx[20];
		swprintf_s(idx, 20,L"No_ID_%04d", m_idxDefault);
		m_wsName = std::wstring(idx);
		m_idxDefault++;
	}
	virtual ~CResource(){}

	void SetName(const std::wstring& wID){
		m_wsName = wID;
	}
	void SetName(const std::string& sID) {
		WCHAR arr[50];
		mbstowcs(arr, sID.c_str(), sID.length());
		m_wsName = arr;
	}

	virtual void Release() = 0;
	
	//순수 가상 함수로 변환 필요.
	virtual void CreateUI();

	virtual void Render(ID3D11DeviceContext *pDeviceContext) = 0;

	const std::wstring GetName() const{
		return m_wsName;
	}
};
__declspec(selectany) UINT CResource::m_idxDefault = 0;

#define ResourceManager		CResourceManager::GetInstance()
#define Model(key)			CResourceManager::GetInstance()->GetModel(key)
#define Texture(key)		CResourceManager::GetInstance()->GetTexture(key)
#define Material(key)		CResourceManager::GetInstance()->GetMaterial(key)
class CResourceManager{
private:
	static CResourceManager *m_pInstance;

	std::map<std::wstring, CModelAssembly*> m_mpModel;
	std::map<std::wstring, CTexture*> m_mpTexture;
	std::map<std::wstring, CMaterial*> m_mpMaterial;

	CUiObject *m_pTextureUi		= nullptr;

private:
	CResourceManager(){}
	~CResourceManager(){}
public:
	static void CreateInstance(){
		if(m_pInstance) return;
		m_pInstance = new CResourceManager;
	}
	static CResourceManager* GetInstance(){
		return m_pInstance;
	}

	void Initialize(ID3D11Device *pDevice);

	void CreateUI(const POINT& vPosition);
	void CreateMaterialUI(const POINT& vPosition);
	void CreateModelUI(const POINT& vPosition);

	void AddResourceFromFile(ID3D11Device *pDevice, std::wstring wFileDirectory);
	void AddTexture(std::wstring wID,CTexture* pTexture);
	void AddMaterial(std::wstring wID,CMaterial* pMaterial);

	CModelAssembly* GetModel(std::wstring wID);
	CTexture* GetTexture(std::wstring wID);
	CMaterial* GetMaterial(std::wstring wID);

};
_declspec(selectany) CResourceManager *CResourceManager::m_pInstance = nullptr;
#pragma once

#include "GmtryEnum.h"
#include "UiObject.h"
#include "UtilityFunction.h"

enum class EShaderType : char{
	NONE = -1,
	VS	 = 0,
	GS,
	HS,
	DS,
	PS,
	CS,
};

class CShader {
private:
	std::wstring m_wsName;
	UINT m_uInputLayout = 0;
	EShaderType m_eShaderType = EShaderType::NONE;

public:
	CShader() {}
	~CShader() {}

	virtual bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		m_uInputLayout = uInputLayout;
		m_wsName = wsShaderFileName;
		return true;
	}
	virtual void Release() = 0;

	void CreateUI() {

	}

	ID3DBlob* ReadCSOFile(ID3D11Device* pDevice, std::wstring wsShaderFileName) {
		ID3DBlob* pShaderBlob = nullptr;
		std::wstring wShaderDirectory = _T("Shader/") + wsShaderFileName + _T(".cso");
		HR(D3DReadFileToBlob(wShaderDirectory.c_str(), &pShaderBlob));
		return pShaderBlob;
	}

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) = 0;

	void SetShaderType(EShaderType eShaderType) {
		m_eShaderType = eShaderType;
	}
	
	UINT GetInputLayout() const {
		return m_uInputLayout;
	}
	EShaderType GetShaderType() const {
		return m_eShaderType;
	}
	const std::wstring& GetName() const {
		return m_wsName;
	}
};

class CVertexShader : public CShader{
private:
	ID3D11InputLayout		*m_pInputLayout = nullptr;
	ID3D11VertexShader		*m_pVertexShader = nullptr;

public:
	CVertexShader() {}
	~CVertexShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);

		SetShaderType(EShaderType::VS);
		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		UINT nSlot = 0;
		std::vector<D3D11_INPUT_ELEMENT_DESC> vpInputElementDesc;

		if (uInputLayout & static_cast<UINT>(EVertexElement::POSITION))
			vpInputElementDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::COLOR))
			vpInputElementDesc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::TEXCOORD1))
			vpInputElementDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::TEXCOORD2))
			vpInputElementDesc.push_back({ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::TANGENT))
			vpInputElementDesc.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::NORMAL))
			vpInputElementDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, nSlot++, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });

		if (uInputLayout & static_cast<UINT>(EVertexElement::BONE));

		if (uInputLayout & static_cast<UINT>(EVertexElement::INSTANCE));

		//InputLayout이 0인 경우는 정점을 CPU에서 GPU로 넘기지 않고, Shader 내부에 정의된 정점을 사용한다 (Post Process를 위한 Quad Shader)
		if (uInputLayout != 0) {
			HR(pDevice->CreateInputLayout(
				vpInputElementDesc.data(),
				vpInputElementDesc.size(),
				pShaderBlob->GetBufferPointer(),
				pShaderBlob->GetBufferSize(),
				&m_pInputLayout
			));
		}

		HR(pDevice->CreateVertexShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pInputLayout);
		SAFE_RELEASE(m_pVertexShader);
	}

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
//#if defined(_DEBUG) || defined(DEBUG)
//		(m_pInputLayout) ? pDeviceContext->IASetInputLayout(m_pInputLayout) : MyMessageBox();
//		pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
//#else
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
//#endif
	};
};

class CGeometryShader : public CShader {
private:
	ID3D11GeometryShader *m_pGeometryShader = nullptr;

public:
	CGeometryShader() {}
	~CGeometryShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout){
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);
		SetShaderType(EShaderType::GS);

		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		HR(pDevice->CreateGeometryShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_pGeometryShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pGeometryShader);
	};

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->GSSetShader(m_pGeometryShader, NULL, 0);
	};
};

class CHullShader : public CShader {
private:
	ID3D11HullShader *m_pHullShader = nullptr;

public:
	CHullShader() {}
	~CHullShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);
		SetShaderType(EShaderType::HS);

		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		HR(pDevice->CreateHullShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_pHullShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pHullShader);
	};

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->HSSetShader(m_pHullShader, NULL, 0);
	};
};

class CDomainShader : public CShader {
private:
	ID3D11DomainShader *m_pDomainShader = nullptr;

public:
	CDomainShader() {}
	~CDomainShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);
		SetShaderType(EShaderType::DS);

		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		HR(pDevice->CreateDomainShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_pDomainShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pDomainShader);
	};

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->DSSetShader(m_pDomainShader, NULL, 0);
	};
};

class CPixelShader : public CShader {
private:
	ID3D11PixelShader *m_PixelShader = nullptr;

public:
	CPixelShader() {}
	~CPixelShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);
		SetShaderType(EShaderType::PS);

		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		HR(pDevice->CreatePixelShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_PixelShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_PixelShader);
	};

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->PSSetShader(m_PixelShader, NULL, 0);
	};
};

class CComputeShader : public CShader {
private:
	ID3D11ComputeShader *m_pComputeShader = nullptr;

public:
	CComputeShader() {}
	~CComputeShader() {}

	bool CreateShader(ID3D11Device* pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout) {
		CShader::CreateShader(nullptr, wsShaderFileName, uInputLayout);
		SetShaderType(EShaderType::CS);
		ID3DBlob* pShaderBlob = ReadCSOFile(pDevice, wsShaderFileName);
		if (!pShaderBlob) return false;

		HR(pDevice->CreateComputeShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&m_pComputeShader
		));

		pShaderBlob->Release();
		return true;
	}

	virtual void Release() {
		SAFE_RELEASE(m_pComputeShader);
	};

	virtual void SetShaderToDevice(ID3D11DeviceContext *pDeviceContext) {
		pDeviceContext->CSSetShader(m_pComputeShader, NULL, 0);
	};

	void Dispatch(ID3D11DeviceContext* pDeviceContext, UINT x, UINT y, UINT z) {
		pDeviceContext->CSSetShader(m_pComputeShader, NULL, 0);
		pDeviceContext->Dispatch(x, y, z);
	}
};

_declspec(selectany) CShader *gNullVS = new CVertexShader;
_declspec(selectany) CShader *gNullPS = new CPixelShader;

enum class EPixelShaderType : char{
	NONE,
	SKYBOX,
	SHADOW,
	DEFERRED,
	FOWARD,
	BLEND,
};

const int g_nShaderType = 5;
class CShaderPack {
private:
	std::wstring		m_wsName;
	UINT				m_uInputLayout = -1;
	CShader				*m_arrShader[g_nShaderType];
	EPixelShaderType	m_ePixelShaderType = EPixelShaderType::FOWARD;
	UINT				m_ID = m_nCount++;
	static UINT			m_nCount;

public:
	CShaderPack(const std::wstring& wsShaderPackName, EPixelShaderType ePixType) {
		m_wsName = wsShaderPackName;
		m_arrShader[static_cast<int>(EShaderType::VS)] = nullptr;
		m_arrShader[static_cast<int>(EShaderType::GS)] = nullptr;
		m_arrShader[static_cast<int>(EShaderType::HS)] = nullptr;
		m_arrShader[static_cast<int>(EShaderType::DS)] = nullptr;
		m_arrShader[static_cast<int>(EShaderType::PS)] = nullptr;
		m_ePixelShaderType = ePixType;
	}
	~CShaderPack() {}
	
	void Create() {

	}
	void Release() {
		m_wsName.clear();
		delete this;
	}

	CUiObject* CreateUI(const POINT& vMousePos = POINT{ 0, 0 }, UINT element = UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE | UiElement::ON_TOP, bool bBodyFlag = true) {
		CUiObject *m_pUiObject = UiManager->CreateUi(L"Shader", element, bBodyFlag);
			CUiObject *pChild = new CUiObject;
			pChild->SetData(this);
			pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
			pChild->SetRenderFunc(RenderAddress);
			m_pUiObject->AddChild(pChild);
			pChild = new CUiObject;
			pChild->SetData(&m_wsName);
			pChild->m_uCoordAttri = UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE;
			pChild->SetRenderFunc(RenderWstring);
		m_pUiObject->AddChild(pChild);
	//	m_pUiObject->SetSize(200, 200);
		m_pUiObject->SetPosition(vMousePos.x, vMousePos.y);
		return m_pUiObject;
	}

	bool AddShader(CShader *pNewShader) {
		if (pNewShader == nullptr) {
			MyMessageBox(_T("Error!! : Null Shader Input"));
			exit(-1);
		}

		if(m_uInputLayout == -1)
			m_uInputLayout = pNewShader->GetInputLayout();
		else {
			if (m_uInputLayout != pNewShader->GetInputLayout()) {
				MyMessageBox(_T("Diffrent InputLayout Type : ["), m_uInputLayout, _T("] ---- ["), pNewShader->GetInputLayout(), _T("] : "), pNewShader->GetName());
				return false;
			}
		}

		m_arrShader[static_cast<int>(pNewShader->GetShaderType())] = pNewShader;
		return true;
	}

	void PrepareRender(ID3D11DeviceContext* pDeviceContext) {
		//Vertex Shader Set
		if (m_arrShader[static_cast<int>(EShaderType::VS)])
			m_arrShader[static_cast<int>(EShaderType::VS)]->SetShaderToDevice(pDeviceContext);
		else {
			pDeviceContext->IASetInputLayout(nullptr);
			pDeviceContext->VSSetShader(nullptr, NULL, 0);
		}

		//Geometry Shader Set
		m_arrShader[static_cast<int>(EShaderType::GS)] ? 
			m_arrShader[static_cast<int>(EShaderType::GS)]->SetShaderToDevice(pDeviceContext) :
			pDeviceContext->GSSetShader(nullptr, NULL, 0);

		//Hull Shader Set
		m_arrShader[static_cast<int>(EShaderType::HS)] ? 
			m_arrShader[static_cast<int>(EShaderType::HS)]->SetShaderToDevice(pDeviceContext) :
			pDeviceContext->HSSetShader(nullptr, NULL, 0);

		//Domain Shader Set
		m_arrShader[static_cast<int>(EShaderType::DS)] ? 
			m_arrShader[static_cast<int>(EShaderType::DS)]->SetShaderToDevice(pDeviceContext) :
			pDeviceContext->DSSetShader(nullptr, NULL, 0);

		//Pixel Shader Set
		m_arrShader[static_cast<int>(EShaderType::PS)] ? 
			m_arrShader[static_cast<int>(EShaderType::PS)]->SetShaderToDevice(pDeviceContext) :
			pDeviceContext->PSSetShader(nullptr, NULL, 0);
	}

	//For Post Processing
	void QuadRender(ID3D11DeviceContext* pDeviceContext) {
		pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		(m_arrShader[static_cast<int>(EShaderType::VS)]) ? m_arrShader[static_cast<int>(EShaderType::VS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->VSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::GS)]) ? m_arrShader[static_cast<int>(EShaderType::GS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->GSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::HS)]) ? m_arrShader[static_cast<int>(EShaderType::HS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->HSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::DS)]) ? m_arrShader[static_cast<int>(EShaderType::DS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->DSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::PS)]) ? m_arrShader[static_cast<int>(EShaderType::PS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->PSSetShader(nullptr, NULL, 0);
		pDeviceContext->Draw(4, 0);
	}

	//For DirLight Instancing Process
	void QuadRenderInstance(ID3D11DeviceContext* pDeviceContext, UINT nInstance, UINT nInstanceStart=0) {
		static bool bFlag = true;
		static ID3D11Buffer *pBuffer = nullptr;
		UINT stride = 12;
		UINT offset = 0;
		if (bFlag) {
			ID3D11Device *pDevice;
			pDeviceContext->GetDevice(&pDevice);

			static std::vector<XMFLOAT3> v;
			v.push_back(XMFLOAT3{});
			v.push_back(XMFLOAT3{});
			v.push_back(XMFLOAT3{});
			v.push_back(XMFLOAT3{});

			D3D11_BUFFER_DESC BufferDesc;
			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.ByteWidth = sizeof(XMFLOAT3) * 4;
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA BufferData;
			ZeroMemory(&BufferData, sizeof(D3D11_SUBRESOURCE_DATA));
			BufferData.pSysMem = v.data();
	
			pDevice->CreateBuffer(&BufferDesc, &BufferData, &pBuffer);

			pDevice->Release();
			bFlag = false;
		}

		pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
//		pDeviceContext->IASetVertexBuffers(0, 1, nullptr, NULL, NULL);
		pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		(m_arrShader[static_cast<int>(EShaderType::VS)]) ? m_arrShader[static_cast<int>(EShaderType::VS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->VSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::GS)]) ? m_arrShader[static_cast<int>(EShaderType::GS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->GSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::HS)]) ? m_arrShader[static_cast<int>(EShaderType::HS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->HSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::DS)]) ? m_arrShader[static_cast<int>(EShaderType::DS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->DSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::PS)]) ? m_arrShader[static_cast<int>(EShaderType::PS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->PSSetShader(nullptr, NULL, 0);
		pDeviceContext->DrawInstanced(4,nInstance,0,nInstanceStart);
	}

	//For PointLight & SpotLight Instancing Process
	void PointRenderInstance(ID3D11DeviceContext* pDeviceContext, UINT nVertex, UINT nInstance, UINT nInstanceStart=0) {
		static bool bFlag = true;
		static ID3D11Buffer *pBuffer = nullptr;
		UINT stride = 12;
		UINT offset = 0;
		if (bFlag) {
			ID3D11Device *pDevice;
			pDeviceContext->GetDevice(&pDevice);

			static std::vector<XMFLOAT3> v;
			v.push_back(XMFLOAT3{});
			v.push_back(XMFLOAT3{});

			D3D11_BUFFER_DESC BufferDesc;
			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.ByteWidth = sizeof(XMFLOAT3) * 4;
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA BufferData;
			ZeroMemory(&BufferData, sizeof(D3D11_SUBRESOURCE_DATA));
			BufferData.pSysMem = v.data();
	
			pDevice->CreateBuffer(&BufferDesc, &BufferData, &pBuffer);

			pDevice->Release();
			bFlag = false;
		}

		pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
	//	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
		(m_arrShader[static_cast<int>(EShaderType::VS)]) ? m_arrShader[static_cast<int>(EShaderType::VS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->VSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::GS)]) ? m_arrShader[static_cast<int>(EShaderType::GS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->GSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::HS)]) ? m_arrShader[static_cast<int>(EShaderType::HS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->HSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::DS)]) ? m_arrShader[static_cast<int>(EShaderType::DS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->DSSetShader(nullptr, NULL, 0);
		(m_arrShader[static_cast<int>(EShaderType::PS)]) ? m_arrShader[static_cast<int>(EShaderType::PS)]->SetShaderToDevice(pDeviceContext) : pDeviceContext->PSSetShader(nullptr, NULL, 0);
		pDeviceContext->DrawInstanced(nVertex,nInstance,0,nInstanceStart);
	}

	bool IsWorkable() {
		return true;
	}

	UINT GetInputLayout() {
		return m_uInputLayout;
	}
	EPixelShaderType GetPixelShaderType() {
		return m_ePixelShaderType;
	}
	std::wstring GetName() {
		return m_wsName;
	}
	UINT GetID() {
		return m_ID;
	}
};
_declspec(selectany) UINT CShaderPack::m_nCount = 0;

_declspec(selectany) CComputeShader *g_pDownScaleFirstPass;
_declspec(selectany) CComputeShader *g_pDownScaleSecondPass;
_declspec(selectany) CComputeShader *g_pBloomFactor;
_declspec(selectany) CComputeShader *g_pBloomHorizontal;
_declspec(selectany) CComputeShader *g_pBloomVerticle;

#define ShaderManager	CShaderManager::GetInstance()
class CShaderManager{
private:
	static CShaderManager					*m_pInstance;
	std::map<std::wstring, CShader*>		m_mpShader;
	std::map<std::wstring, CComputeShader*> m_mpComputeShader;

	//해당 InputLayout의 Default ShaderPack
	std::map<UINT, CShaderPack*>			m_mpDefaultShaderPack;
	std::map<std::wstring, CShaderPack*>	m_mpShaderPack;
	std::vector<CShaderPack*>				m_vpShaderPack;

	CUiObject								*m_pUiObject = nullptr;

private:
	CShaderManager(){}
	~CShaderManager(){}

public:
	static void CreateInstance(){
		if(!m_pInstance)
			m_pInstance = new CShaderManager;
	}
	static CShaderManager* GetInstance(){
		return m_pInstance;
	}

	void Initialize(ID3D11Device *pDevice);
	CShader* CreateShader(ID3D11Device *pDevice, const std::wstring& wsShaderFileName, UINT uInputLayout);
	void CreateUI(const POINT& vPosition);
	void AddShaderPack(CShaderPack *pShaderPack);

	CShaderPack* GetDefaultShaderPack(UINT uInputLayout) {
		return m_mpDefaultShaderPack[uInputLayout];
	}
	CShaderPack* GetShaderPack(UINT ID) {
	//	if(ID > m_vpShaderPack.size())
		return m_vpShaderPack[ID];
	}
	CShaderPack** GetShaderPackRef(UINT ID) {
		return &(m_vpShaderPack[ID]);
	}
	CShaderPack* GetShaderPack(const std::wstring& wsName) {
		try {
			return m_mpShaderPack.at(wsName);
		}
		catch(std::out_of_range){
			MyMessageBox(_T("No Shader Exsists!! : "), wsName);
			return m_mpShaderPack[_T("foward_diffuse")];
		}
	}
};
__declspec(selectany) CShaderManager *CShaderManager::m_pInstance = nullptr;
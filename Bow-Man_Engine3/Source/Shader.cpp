#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "UtilityFunction.h"

std::vector<byte> ReadCSOFile(LPCTSTR path)
{
	std::ifstream inputShaderCSO(path, std::ios::in | std::ios::binary);

	if (!inputShaderCSO.is_open())
	{
		//MessageBox(m_hWnd, TEXT("Shader Load Fail!"), TEXT("프로그램 구동 실패"), MB_OK);
		return std::vector<byte>();
	}

	// size check;
	inputShaderCSO.seekg(0, std::fstream::end);
	size_t szData = inputShaderCSO.tellg();
	inputShaderCSO.seekg(0);

	std::vector<byte> retval;
	retval.resize(szData);

	inputShaderCSO.read(reinterpret_cast<char*>(&(retval[0])), szData);
	inputShaderCSO.close();

	return retval;
}

CShader * CShaderManager::CreateShader(ID3D11Device * pDevice, const std::wstring & wsShaderFileName, UINT uInputLayout) {
	std::wstring wsName;
	std::wstring wsFormat;
	GetFileNameFormat(wsShaderFileName, wsName, wsFormat);

	try {
		m_mpShader.at(wsName);
		MyMessageBox(_T("Shader Already Exists! : "), wsShaderFileName);
		return false;
	}
	catch (std::out_of_range) {}
	try {
		m_mpComputeShader.at(wsName);
		MyMessageBox(_T("Shader Already Exists! : "), wsShaderFileName);
		return false;
	}
	catch (std::out_of_range) {}


	if (wsFormat != _T("cso")) {
		MyMessageBox(_T("It is Not a \"CSO\" FIle! : "), wsShaderFileName);
		return false;
	}

	CShader *pShader = nullptr;
	if (wsName.substr(0, 2) == L"VS")	pShader = new CVertexShader;
	else if (wsName.substr(0, 2) == L"GS")	pShader = new CGeometryShader;
	else if (wsName.substr(0, 2) == L"HS")	pShader = new CHullShader;
	else if (wsName.substr(0, 2) == L"DS")	pShader = new CDomainShader;
	else if (wsName.substr(0, 2) == L"PS")	pShader = new CPixelShader;
	else if (wsName.substr(0, 2) == L"CS")	pShader = new CComputeShader;
	else {
		MyMessageBox(_T("Shader Type Error : "), wsShaderFileName);
		exit(-1);
		return pShader;
	}

	//Success
	if (pShader->CreateShader(pDevice, wsName, uInputLayout)) {
		switch (pShader->GetShaderType()) {
		case EShaderType::CS:
			m_mpComputeShader[wsName] = dynamic_cast<CComputeShader*>(pShader);
			break;
		default:
			m_mpShader[wsName] = pShader;
			break;
		}
	}
	//Fail
	else {
		MyMessageBox(_T("Shader Creation Failed : "), wsShaderFileName);
		SAFE_RELEASE(pShader);
		exit(-1);
		return pShader;
	}
	return pShader;
}

void CShaderManager::Initialize(ID3D11Device * pDevice) {
	//※ 폴더에서 Load 하도록 수정 예정
	CreateShader(pDevice, _T("VS_Diffused.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));
	CreateShader(pDevice, _T("PS_Diffused.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::COLOR));

	CreateShader(pDevice, _T("VS_Textured.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
	CreateShader(pDevice, _T("PS_Textured.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));

	CreateShader(pDevice, _T("VS_Skybox.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));
	CreateShader(pDevice, _T("PS_Skybox.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1));

	CreateShader(pDevice, _T("VS_Illuminated.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL));
	CreateShader(pDevice, _T("PS_Illuminated.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL));
	CreateShader(pDevice, _T("PS_Illuminated_Deferred.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL));

	CreateShader(pDevice, _T("VS_IlluminatedTextured.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL) | _INT(EVertexElement::TEXCOORD1));
	CreateShader(pDevice, _T("PS_IlluminatedTextured.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL) | _INT(EVertexElement::TEXCOORD1));
	CreateShader(pDevice, _T("PS_IlluminatedTextured_Deferred.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::NORMAL) | _INT(EVertexElement::TEXCOORD1));

	CreateShader(pDevice, _T("VS_Tbn.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TANGENT) | _INT(EVertexElement::NORMAL) | _INT(EVertexElement::TEXCOORD1));
	CreateShader(pDevice, _T("PS_Tbn_Deferred.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TANGENT) | _INT(EVertexElement::NORMAL) | _INT(EVertexElement::TEXCOORD1));

	CreateShader(pDevice, _T("VS_Terrain.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));
	CreateShader(pDevice, _T("HS_Terrain.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));
	CreateShader(pDevice, _T("DS_Terrain.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));
	CreateShader(pDevice, _T("PS_Terrain_Foward.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));
	CreateShader(pDevice, _T("PS_Terrain_Deferred.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));

	CreateShader(pDevice, _T("VS_Terrain_Normal.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));
	CreateShader(pDevice, _T("PS_Terrain_Normal.cso"), _INT(EVertexElement::POSITION) | _INT(EVertexElement::TEXCOORD1) | _INT(EVertexElement::TEXCOORD2));


	//Light Shader
	CreateShader(pDevice, _T("VS_DirLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("PS_DirLight.cso"), _INT(EVertexElement::POSITION));

	CreateShader(pDevice, _T("VS_PointLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("HS_PointLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("DS_PointLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("PS_PointLight.cso"), _INT(EVertexElement::POSITION));

	CreateShader(pDevice, _T("VS_SpotLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("HS_SpotLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("DS_SpotLight.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("PS_SpotLight.cso"), _INT(EVertexElement::POSITION));

	//Post Process Shader
	CreateShader(pDevice, _T("VS_FinalPath.cso"), _INT(EVertexElement::POSITION));
	CreateShader(pDevice, _T("PS_FinalPath.cso"), _INT(EVertexElement::POSITION));

	//Compute Shader
	CreateShader(pDevice, _T("CS_DownScaleFirstPass.cso"), NULL);
	CreateShader(pDevice, _T("CS_DownScaleSecondPass.cso"), NULL);
	CreateShader(pDevice, _T("CS_BloomFactor.cso"), NULL);
	CreateShader(pDevice, _T("CS_BloomHorizontal.cso"), NULL);
	CreateShader(pDevice, _T("CS_BloomVerticle.cso"), NULL);

	CShaderPack *pShaderPack = nullptr;
	//deferred ShaderPack
	pShaderPack = new CShaderPack(_T("deferred_illuminated"), EPixelShaderType::DEFERRED);
	pShaderPack->AddShader(m_mpShader[_T("VS_Illuminated")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Illuminated_Deferred")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("deferred_illuminated_textured"), EPixelShaderType::DEFERRED);
	pShaderPack->AddShader(m_mpShader[_T("VS_IlluminatedTextured")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_IlluminatedTextured_Deferred")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("deferred_tbn"), EPixelShaderType::DEFERRED);
	pShaderPack->AddShader(m_mpShader[_T("VS_Tbn")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Tbn_Deferred")]);
	AddShaderPack(pShaderPack);

	//foward ShaderPack - 각 Input Layout별 기본 Shader
	pShaderPack = new CShaderPack(_T("foward_diffuse"), EPixelShaderType::FOWARD);
	pShaderPack->AddShader(m_mpShader[_T("VS_Diffused")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Diffused")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("foward_texture"), EPixelShaderType::FOWARD);
	pShaderPack->AddShader(m_mpShader[_T("VS_Textured")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Textured")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("foward_illuminated"), EPixelShaderType::FOWARD);
	pShaderPack->AddShader(m_mpShader[_T("VS_Illuminated")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Illuminated")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("foward_illuminated_textured"), EPixelShaderType::FOWARD);
	pShaderPack->AddShader(m_mpShader[_T("VS_IlluminatedTextured")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_IlluminatedTextured")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("deferred_terrain"), EPixelShaderType::DEFERRED);
	pShaderPack->AddShader(m_mpShader[_T("VS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("HS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("DS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Terrain_Deferred")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("deferred_terrain_normal"), EPixelShaderType::DEFERRED);
	pShaderPack->AddShader(m_mpShader[_T("VS_Terrain_Normal")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Terrain_Normal")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("foward_terrain"), EPixelShaderType::FOWARD);
	pShaderPack->AddShader(m_mpShader[_T("VS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("HS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("DS_Terrain")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Terrain_Foward")]);
	AddShaderPack(pShaderPack);

	//Special ShaderPack
	pShaderPack = new CShaderPack(_T("skybox"), EPixelShaderType::SKYBOX);
	pShaderPack->AddShader(m_mpShader[_T("VS_Skybox")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_Skybox")]);
	AddShaderPack(pShaderPack);

	//Light Process
	pShaderPack = new CShaderPack(_T("DirLightProcess"), EPixelShaderType::NONE);
	pShaderPack->AddShader(m_mpShader[_T("VS_DirLight")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_DirLight")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("PointLightProcess"), EPixelShaderType::NONE);
	pShaderPack->AddShader(m_mpShader[_T("VS_PointLight")]);
	pShaderPack->AddShader(m_mpShader[_T("HS_PointLight")]);
	pShaderPack->AddShader(m_mpShader[_T("DS_PointLight")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_PointLight")]);
	AddShaderPack(pShaderPack);

	pShaderPack = new CShaderPack(_T("SpotLightProcess"), EPixelShaderType::NONE);
	pShaderPack->AddShader(m_mpShader[_T("VS_SpotLight")]);
	pShaderPack->AddShader(m_mpShader[_T("HS_SpotLight")]);
	pShaderPack->AddShader(m_mpShader[_T("DS_SpotLight")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_SpotLight")]);
	AddShaderPack(pShaderPack);

	//Post Process
	pShaderPack = new CShaderPack(_T("FinalPath"), EPixelShaderType::NONE);
	pShaderPack->AddShader(m_mpShader[_T("VS_FinalPath")]);
	pShaderPack->AddShader(m_mpShader[_T("PS_FinalPath")]);
	AddShaderPack(pShaderPack);

	//Compute Shader
	g_pDownScaleFirstPass = m_mpComputeShader[_T("CS_DownScaleFirstPass")];
	g_pDownScaleSecondPass = m_mpComputeShader[_T("CS_DownScaleSecondPass")];
	g_pBloomFactor = m_mpComputeShader[_T("CS_BloomFactor")];
	g_pBloomHorizontal = m_mpComputeShader[_T("CS_BloomHorizontal")];
	g_pBloomVerticle = m_mpComputeShader[_T("CS_BloomVerticle")];

	CreateUI(POINT{1500, 0});
}

void CShaderManager::CreateUI(const POINT & vPosition) {
	if (m_pUiObject) return;

	m_pUiObject = UiManager->CreateUi(L"ShaderPack Database", UiElement::TITLE | UiElement::EXIT | UiElement::MINIMIZE | UiElement::RESIZE);
	m_pUiObject->SetOuterInterface(&m_pUiObject);
	m_pUiObject->SetSizeX(300);
	for (auto& data : m_mpShaderPack) {
		CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(_T("ShaderPack"));
		pChild->SetData(&(data.second));
		pChild->SetSize(0, 20);
		pChild->SetRenderFunc(RenderShaderAddress);
		pChild->SetMouseInputFunc(MouseInputShader);
		m_pUiObject->AddChild(pChild);
	}
	//	pSuperUi->SetSize(200, 800);
	m_pUiObject->SetPosition(vPosition.x, vPosition.y);
}

void CShaderManager::AddShaderPack(CShaderPack * pShaderPack) {
	//Map With wsString Index
	if (!m_mpShaderPack[pShaderPack->GetName()]) {
		m_mpShaderPack[pShaderPack->GetName()] = pShaderPack;
	}
	else {
		MyMessageBox(_T("Adding ShaderPack Error : ["), pShaderPack->GetName(), _T("] Already Exists!"));
		return;
	}

	//Vector With Int Index
	try {
		m_vpShaderPack.at(pShaderPack->GetID());
	}
	catch (std::out_of_range) {
		if (pShaderPack->GetID() == m_vpShaderPack.size())
			m_vpShaderPack.push_back(pShaderPack);
		else
			MyMessageBox(_T("Adding ShaderPack Error : ["), pShaderPack->GetName(), _T("] Index 규격이 맞지 않습니다"));
	}

	try {
		m_mpDefaultShaderPack.at(pShaderPack->GetInputLayout());
	}
	catch (std::out_of_range) {
		m_mpDefaultShaderPack[pShaderPack->GetInputLayout()] = pShaderPack;
	}
	//	m_vpShaderPack[pShaderPack->GetID()] = pShaderPack;
}

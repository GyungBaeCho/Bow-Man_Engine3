#include "stdafx.h"
#include "ResourceData.h"
#include "UiObject.h"
#include "UtilityFunction.h"
#include "Texture.h"
#include "Material.h"
#include "ModelAssembly.h"

void CResourceManager::Initialize(ID3D11Device * pDevice){
	m_pTextureUi = UiManager->CreateUi(L"Texture Database", UiElement::TITLE|UiElement::EXIT|UiElement::MINIMIZE|UiElement::RESIZE);
	m_pTextureUi->SetPosition(2000, 0);
	m_pTextureUi->SetOuterInterface(&m_pTextureUi);
	
	AddResourceFromFile(pDevice,L"../../Resource/Default/default.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/light_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/grass_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/texture_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/tile_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/weaved_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/leaf_01.dds");
	AddResourceFromFile(pDevice, L"../../Resource/Default/space_10.dds");

//	AddMaterial(L"default", new CMaterial(1.0, 1.0, 1.0, 100));
	AddMaterial(L"default", new CMaterial(1.0, 1.0, 1.0, 100));
	AddMaterial(L"metal", new CMaterial(1.0, 1.0, 1.0, 800));
}

void CResourceManager::CreateUI(const POINT & vPosition) {
	if (m_pTextureUi) return;

	m_pTextureUi = UiManager->CreateUi(L"Texture Database", UiElement::TITLE|UiElement::EXIT|UiElement::MINIMIZE|UiElement::RESIZE);
	m_pTextureUi->SetOuterInterface(&m_pTextureUi);

	for (auto& data : m_mpTexture) {
		CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
		pChild->SetTitle(data.second->GetName());
		pChild->SetData(data.second);
		pChild->SetSize(0, 50);
		pChild->SetRenderFunc(RenderTexture);
		pChild->SetMouseInputFunc(MouseInputTexture);
		m_pTextureUi->AddChild(pChild);
	}
	m_pTextureUi->SetPosition(vPosition.x, vPosition.y);
}

void CResourceManager::AddResourceFromFile(ID3D11Device *pDevice, std::wstring wFileDirectory){
	std::wstring wFileName;
	std::wstring wFileFormat;
	if(!GetFileNameFormat(wFileDirectory,wFileName,wFileFormat)){ 
		std::wstring wErrorMsg = L"Resource Add Failed, It is Not A FIle[" + wFileDirectory + L"]";
		SystemMessage(wErrorMsg);
		return;
	}

	if(wFileFormat == L"png" | wFileFormat == L"jpg" | wFileFormat == L"dds" | wFileFormat == L"tga"){
		AddTexture(wFileName,new CTexture(pDevice, wFileDirectory.c_str()));
	}
	else{
		std::wstring wErrorMsg = L"Resource Add Failed, Invalid Format - " + wFileFormat;
		SystemMessage(wErrorMsg);
	}
}

void CResourceManager::AddTexture(std::wstring wID,CTexture * pTexture){
	if(!pTexture){
		std::wstring wErrorMsg = L"Texture Add Failed[" + wID + L"] Data Is Null" ;
		SystemMessage(wErrorMsg);
		return;
	}
	if(m_mpTexture[wID]){
		std::wstring wErrorMsg = L"Texture Add Failed[" + wID + L"] ID Already Exists" ;
		SystemMessage(wErrorMsg);
		return;
	}

	CUiObject *pChild = new CUiObject(UiCoordType::DOWN_WARD | UiCoordType::SYNC_X_SIZE);
	pChild->SetTitle(wID);
	pChild->SetData(pTexture);
	pChild->SetSize(0, 50);
	pChild->SetRenderFunc(RenderTexture);
	pChild->SetMouseInputFunc(MouseInputTexture);
	m_pTextureUi->AddChild(pChild);

	pTexture->SetName(wID);
	m_mpTexture[wID] = pTexture;
	std::wstring wEMsg = L"Texture Add Successed[" + wID + L"]" ;
	SystemMessage(wEMsg);
}

void CResourceManager::AddMaterial(std::wstring wID,CMaterial * pMaterial)
{
	if(!pMaterial){
		std::wstring wErrorMsg = L"Material Add Failed[" + wID + L"] Data Is Null" ;
		SystemMessage(wErrorMsg);
		return;
	}
	if(m_mpMaterial[wID]){
		std::wstring wErrorMsg = L"Material Add Failed[" + wID + L"] ID Already Exists" ;
		SystemMessage(wErrorMsg);
		return;
	}
	pMaterial->SetName(wID);
	m_mpMaterial[wID] = pMaterial;
	std::wstring wEMsg = L"Material Add Successed[" + wID + L"]" ;
	SystemMessage(wEMsg);
}

CModelAssembly * CResourceManager::GetModel(std::wstring wID)
{
	if (!m_mpModel[wID]) {
		std::wstring wErrorMsg = L"Model Not Found[" + wID + L"] returns Default";
		SystemMessage(wErrorMsg);
		m_mpTexture.erase(wID);
		return m_mpModel[L"default"];
	}
	return m_mpModel[wID];
}

CTexture *CResourceManager::GetTexture(std::wstring wID){
	if(!m_mpTexture[wID]){
		std::wstring wErrorMsg = L"Texture Not Found[" + wID + L"] returns Default";
		SystemMessage(wErrorMsg);
		m_mpTexture.erase(wID);
		return m_mpTexture[L"default"];
	}
	return m_mpTexture[wID];
}

CMaterial * CResourceManager::GetMaterial(std::wstring wID)
{
	if(!m_mpMaterial[wID]){
		std::wstring wErrorMsg = L"Material Not Found[" + wID + L"] returns Default";
		SystemMessage(wErrorMsg);
		m_mpMaterial.erase(wID);
		return m_mpMaterial[L"default"];
	}
	return m_mpMaterial[wID];
}

//순수 가상 함수로 변환 필요.

void CResource::CreateUI() {}

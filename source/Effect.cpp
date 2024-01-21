#include "pch.h"
#include "Effect.h"
#include <sstream>

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const dae::Texture* pDiffuse, const dae::Texture* pNormal, const dae::Texture* pSpecular, const dae::Texture* pGlossiness)
{
	m_pDevice = pDevice;
	m_pEffect = LoadEffect(assetFile);
	m_pTechnique.push_back(m_pEffect->GetTechniqueByName("PointTechnique"));
	m_pTechnique.push_back(m_pEffect->GetTechniqueByName("LinearTechnique"));
	m_pTechnique.push_back(m_pEffect->GetTechniqueByName("AnisotropicTechnique"));

	for(auto& technique : m_pTechnique)
	{
		if(!technique->IsValid())
		{
			std::wcout << L"Technique not valid!\n";
		}
	}

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if(!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";
	}

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if(!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";
	}
	m_pDiffuseMapVariable->SetResource(pDiffuse->GetSRV());

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable not valid!\n";
	}
	m_pNormalMapVariable->SetResource(pNormal->GetSRV());

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable not valid!\n";
	}
	m_pSpecularMapVariable->SetResource(pSpecular->GetSRV());

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable not valid!\n";
	}
	m_pGlossinessMapVariable->SetResource(pGlossiness->GetSRV());

	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrixVariable->IsValid())
	{
		std::wcout << L"m_pWorldMatrixVariable not valid!\n";
	}

	m_pCameraPosVariable = m_pEffect->GetVariableByName("gCameraPos")->AsVector();
	if (!m_pCameraPosVariable->IsValid())
	{
		std::wcout << L"m_pCameraPosVariable not valid!\n";
	}
}

Effect::~Effect()
{
	if(m_pEffect)
	{
		m_pEffect->Release();
	}
	if(m_pMatWorldViewProjVariable)
	{
		m_pMatWorldViewProjVariable->Release();
	}
	if(m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->Release();
	}
	if(m_pNormalMapVariable)
	{
		m_pNormalMapVariable->Release();
	}
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->Release();
	}
	if(m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->Release();
	}
	if(m_pWorldMatrixVariable)
	{
		m_pWorldMatrixVariable->Release();
	}

	for (auto& technique : m_pTechnique)
	{
		if (technique)
		{
			technique->Release();
		}
	}
}

ID3DX11Effect* Effect::LoadEffect(const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile
	(
		assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		m_pDevice,
		&pEffect,
		&pErrorBlob
	);

	if(FAILED(result))
	{
		if(pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for(unsigned int index{}; index < pErrorBlob->GetBufferSize(); ++index)
			{
				ss << pErrors[index];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique(int currentTechniqueId)
{
	return m_pTechnique[currentTechniqueId];
}

void Effect::SetMatrix(const dae::Camera& camera, const dae::Matrix& worldMatrix)
{
	
	const auto tempMatrix = camera.GetViewMatrix() * camera.GetProjectionMatrix() * worldMatrix;
	const auto tempPos = camera.GetPosition();
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&(tempMatrix)));
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&(worldMatrix)));
	m_pCameraPosVariable->SetFloatVector(reinterpret_cast<const float*>(&(tempPos)));
}

int Effect::GetTechniqueCount() const
{
	return static_cast<int>(m_pTechnique.size());
}

#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);
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
	for (auto& technique : m_pTechnique)
	{
		if (technique)
		{
			technique->Release();
		}
	}
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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
		pDevice,
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

void Effect::SetMatrix(const dae::Matrix& viewProjectionMatrix, const dae::Matrix& worldMatrix)
{
	const auto tempMatrix = viewProjectionMatrix * worldMatrix;
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&(tempMatrix)));
}

void Effect::SetDiffuseMap(dae::Texture* pDiffuseTexture)
{
	if(m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	}
}

int Effect::GetTechniqueCount() const
{
	return static_cast<int>(m_pTechnique.size());
}
#pragma once
#include "Camera.h"
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const dae::Texture* pDiffuse, const dae::Texture* pNormal, const dae::Texture* pSpecular, const dae::Texture* pGlossiness);
	~Effect();

	ID3DX11Effect* LoadEffect(const std::wstring& assetFile);

	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique(int currentTechniqueId);

	void SetMatrix(const dae::Camera& camera, const dae::Matrix& worldMatrix);
	int GetTechniqueCount() const;

private:
	ID3DX11Effect* m_pEffect;
	std::vector<ID3DX11EffectTechnique*> m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
	ID3DX11EffectVectorVariable* m_pCameraPosVariable;
	ID3D11Device* m_pDevice{};

};

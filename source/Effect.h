#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique(int currentTechniqueId);

	void SetMatrix(const dae::Matrix& viewProjectionMatrix, const dae::Matrix& worldMatrix);
	void SetDiffuseMap(dae::Texture* pDiffuseTexture);
	int GetTechniqueCount() const;

private:
	ID3DX11Effect* m_pEffect;
	std::vector<ID3DX11EffectTechnique*> m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

};

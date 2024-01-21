#pragma once
#include "Camera.h"
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique(int currentTechniqueId);

	void SetMatrix(const dae::Camera& camera, const dae::Matrix& worldMatrix);
	void SetDiffuseMap(dae::Texture* pDiffuseTexture);
	void SetNormalMap(dae::Texture* pNormalTexture) const;
	void SetSpecularMap(dae::Texture* pSpecularTexture) const;
	void SetGlossinessMap(dae::Texture* pGLossinessTexture) const;
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

};

#pragma once
#include "Texture.h"

struct Vertex_PosCol
{
	dae::Vector3 position{};
	dae::ColorRGB color{};
	dae::Vector2 uv{};
};

struct Vertex_PosCol_Out
{
	dae::Vector4 position{};
	dae::ColorRGB color{};
	dae::Vector2 uv{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

class Effect;

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::string& filename);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext);
	void SetMatrix(const dae::Matrix& viewProjectionMatrix) const;
	void SetDiffuseMap(dae::Texture* pDiffuseTexture) const;
	void IncrementTechniqueId();

private:
	std::vector<Vertex_PosCol> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	uint32_t m_NumIndices{};
	PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleStrip };

	std::vector<Vertex_PosCol_Out> m_VerticesOut{};
	Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	dae::Matrix* m_pWorldMatrix;
	int m_CurrentTechniqueId;
};


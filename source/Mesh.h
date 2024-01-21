#pragma once
#include "Camera.h"
#include "Texture.h"

struct Vertex_PosCol
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

struct Vertex_PosCol_Out
{
	dae::Vector4 position{};
	dae::Vector4 worldPosition{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
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
	Mesh(ID3D11Device* pDevice, const std::string& filename, const dae::Texture* pDiffuse, const dae::Texture* pNormal, const dae::Texture* pSpecular, const dae::Texture* pGlossiness);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, const dae::Camera& camera);
	void IncrementTechniqueId();
	void Rotate(float angle);

private:
	std::vector<Vertex_PosCol> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	uint32_t m_NumIndices{};
	PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleList };

	std::vector<Vertex_PosCol_Out> m_VerticesOut{};
	Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	dae::Matrix m_pWorldMatrix;
	int m_CurrentTechniqueId;
};


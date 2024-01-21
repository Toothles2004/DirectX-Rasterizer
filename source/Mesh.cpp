#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include "Utils.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::string& filename, const dae::Texture* pDiffuse, const dae::Texture* pNormal, const dae::Texture* pSpecular, const dae::Texture* pGlossiness)
	: m_CurrentTechniqueId{0}
{
	dae::Utils::ParseOBJ(filename, m_Vertices, m_Indices);
	m_VerticesOut.resize(m_Vertices.size());

	m_pEffect = new Effect{ pDevice, L"Resources/PosCol3D.fx", pDiffuse, pNormal, pSpecular, pGlossiness };
	m_pTechnique = m_pEffect->GetTechnique(m_CurrentTechniqueId);
	m_pWorldMatrix = dae::Matrix{};

	//Create vertices layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout
	(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result))
	{
		return;
	}

	//Create vertices buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(m_Vertices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_Vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		return;
	}

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_Indices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		return;
	}
}

Mesh::~Mesh()
{
	delete m_pEffect;

	if(m_pTechnique)
	{
		m_pTechnique->Release();
	}
	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}
	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
	}
	if(m_pInputLayout)
	{
		m_pInputLayout->Release();
	}
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const dae::Camera& camera)
{
	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex_PosCol);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pEffect->SetMatrix(camera, m_pWorldMatrix);

	//4. Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique(m_CurrentTechniqueId)->GetDesc(&techDesc);
	for(UINT p{}; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique(m_CurrentTechniqueId)->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::IncrementTechniqueId()
{
	m_CurrentTechniqueId = (m_CurrentTechniqueId + 1) % m_pEffect->GetTechniqueCount();
	m_pTechnique = m_pEffect->GetTechnique(m_CurrentTechniqueId);
	std::cout << "Toggle sampling state, current state: ";
	switch(m_CurrentTechniqueId)
	{
	case 0:
		std::cout << "Point\n";
		break;
	case 1:
		std::cout << "Linear\n";
		break;
	case 2:
		std::cout << "Anisotropic\n";
		break;
	}
}

void Mesh::Rotate(float angle)
{
	//m_pWorldMatrix = m_pWorldMatrix.CreateRotationY(angle);
	m_pWorldMatrix = dae::Matrix::CreateRotation(0,angle, 0);
}

void Mesh::ToggleNormal()
{
	m_pEffect->ToggleNormal();
}



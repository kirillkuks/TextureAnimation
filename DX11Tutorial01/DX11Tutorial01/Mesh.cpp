#include "Mesh.h"

Mesh::Mesh(ID3D11Device* const& device, 
	ID3D11DeviceContext* const& context, 
	std::vector<Vertex> const& vertecies, 
	std::vector<UINT16> const& indeces)
	: m_pDevice{ device }, m_pContext{ context },
	m_pVertexBuffer{ NULL }, m_pIndexBuffer{ NULL },
	m_pVertexShader{ NULL }, m_pPixelShader{ NULL }, 
	m_pInputLayout{ NULL }, m_pConstantBuffer{ NULL },
	m_pRasterizerState{ NULL }, m_pSceneBuffer{ NULL },
	m_pTexture{ NULL }, m_pTextureSRV{ NULL }
{
	indexCount = indeces.size();

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertecies.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertecies.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(result));

	// Create index buffer
	if (SUCCEEDED(result))
	{
		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = (UINT)(sizeof(UINT16) * indeces.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indeces.data();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
		assert(SUCCEEDED(result));
	}

	if (SUCCEEDED(result))
	{
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(CBuffer);
		cbDesc.Usage = D3D11_USAGE_DEFAULT;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		result = m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pConstantBuffer);
		assert(SUCCEEDED(result));
	}

	if (SUCCEEDED(result))
	{
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(SBuffer);
		cbDesc.Usage = D3D11_USAGE_DEFAULT;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		result = m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSceneBuffer);
		assert(SUCCEEDED(result));
	}

	if (SUCCEEDED(result))
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;

		result = m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
		assert(SUCCEEDED(result));
	}
}

Mesh::~Mesh()
{
	if (m_pVertexBuffer != NULL)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if (m_pIndexBuffer != NULL)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}
	if (m_pConstantBuffer != NULL)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = NULL;
	}
	if (m_pSceneBuffer != NULL)
	{
		m_pSceneBuffer->Release();
		m_pSceneBuffer = NULL;
	}
	if (m_pRasterizerState != NULL)
	{
		m_pRasterizerState->Release();
		m_pRasterizerState = NULL;
	}
}

void Mesh::SetShaders(ID3D11VertexShader* const& vertexShader,
	ID3D11PixelShader* const& pixelShader,
	ID3D11InputLayout* const& layout)
{
	m_pVertexShader = vertexShader;
	m_pPixelShader = pixelShader;
	m_pInputLayout = layout;
}

void Mesh::SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV)
{
	m_pTexture = texture;   // Change
	m_pTextureSRV = textureSRV;   // Change
}

void Mesh::Draw(XMMATRIX matrix, XMMATRIX sceneMatrix)
{
	ID3D11Buffer* vertexBuffers[] = { m_pVertexBuffer };
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);

	ID3D11ShaderResourceView* textures[] = { m_pTextureSRV, NULL };
	m_pContext->PSSetShaderResources(0, 2, textures);

	// Change
	CBuffer cb;
	XMMATRIX tranlation = XMMatrixTranslation(0, 0, -12);
	XMMATRIX rotation = XMMatrixRotationX(90);
	cb.meshMatrix = XMMatrixTranspose(tranlation * rotation);
	// cb.meshMatrix = XMMatrixTranspose(matrix);
	// cb.meshMatrix = XMMatrixIdentity();
	cb.normalMatrix = XMMatrixIdentity();
	m_pContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);

	SBuffer sb;
	sb.VP = sceneMatrix;
	m_pContext->UpdateSubresource(m_pSceneBuffer, 0, NULL, &sb, 0, 0);

	m_pContext->RSSetState(m_pRasterizerState);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* constBuffers[] = { m_pConstantBuffer };
	m_pContext->VSSetConstantBuffers(0, 1, constBuffers);

	ID3D11Buffer* sceneBuffers[] = { m_pSceneBuffer };
	m_pContext->VSSetConstantBuffers(1, 1, sceneBuffers);
	m_pContext->PSSetConstantBuffers(1, 1, sceneBuffers);

	m_pContext->DrawIndexed((UINT)indexCount, 0, 0);
}

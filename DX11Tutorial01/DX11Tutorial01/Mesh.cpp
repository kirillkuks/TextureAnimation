#include "Mesh.h"

Mesh::Mesh(ID3D11Device* const& device, 
	ID3D11DeviceContext* const& context, 
	std::vector<Vertex> const& vertecies, 
	std::vector<UINT16> const& indeces)
	: m_pDevice{ device }, m_pContext{ context },
	m_pVertexBuffer{ nullptr }, m_pIndexBuffer{ nullptr },
	m_pVertexShader{ nullptr }, m_pPixelShader{ nullptr },
	m_pInputLayout{ nullptr }, m_pConstantBuffer{ nullptr },
	m_pRasterizerState{ nullptr }, m_pSceneBuffer{ nullptr },
	m_pTexture{ nullptr }, m_pTextureSRV{ nullptr },
	m_pAnimationTexture{ nullptr }
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
}

void Mesh::SetTexture(AnimationTexture* texture)
{
	m_pTexture = texture;
}

void Mesh::SetTexture(Texture* texture)
{
	m_pTexture = texture;
}

void Mesh::SetOriginTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV)
{
}

void Mesh::SetShaderResources(std::vector<ID3D11ShaderResourceView*> const& resources)
{
	m_aShaderResources = resources;
}

void Mesh::SetConstantBuffers(std::vector<ID3D11Buffer*> const& buffers)
{
	m_aConstantBuffers = buffers;
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

	// Как лучше?
	/// auto texts = m_pAnimationTexture->GetLayersTargetTexturesSRV();
	/// auto fields = m_pAnimationTexture->GetFields();
	/// ID3D11ShaderResourceView* textures[] = { m_pAnimationTexture->GetBackgroundTextureSRV(), texts[0], fields[0]->CurrentVectorFieldSRV() };
	auto texts = m_pTexture->GetLayersTargetTexturesSRV();
	auto fields = m_pTexture->GetFields();

	size_t size = texts.size();
	assert(size == fields.size());

	ID3D11ShaderResourceView* textures[] = { m_pTexture->GetBackGroundTextureSRV() };
	m_pContext->PSSetShaderResources(0, 1, textures);

	for (size_t i = 0; i < size; ++i)
	{
		ID3D11ShaderResourceView* animationTexturesResources[] = { texts[i], fields[i]->CurrentVectorFieldSRV() };
		m_pContext->PSSetShaderResources(2 * i + 1, 2, animationTexturesResources);
	}
	/*if (size == 0)
	{
		ID3D11ShaderResourceView* animationTexturesResources[] = { nullptr, nullptr, nullptr, nullptr };
		m_pContext->PSSetShaderResources(1, 4, animationTexturesResources);
	}*/

	/// ID3D11ShaderResourceView* textures[] = { nullptr, nullptr, nullptr };
	/// m_pContext->PSSetShaderResources(0, 3, textures);
	/// std::vector<ID3D11ShaderResourceView*> textures1 = { texts[1], fields[1]->CurrentVectorFieldSRV() };
	/// std::vector<ID3D11ShaderResourceView*> textures1 = { nullptr, nullptr };
	/// m_pContext->PSSetShaderResources(3, textures1.size(), textures1.data());

	// В планах | массив текстур?
	// auto textures = m_pAnimationTexture->GetLayersTargetTexturesSRV();
	// m_pContext->PSSetShaderResources(1, textures.size(), textures.data());

	// Change
	CBuffer cb;
	XMMATRIX tranlation = XMMatrixTranslation(0, 0, -7);
	XMMATRIX rotation = XMMatrixRotationX(90);
	cb.meshMatrix = XMMatrixTranspose(tranlation * rotation);
	// cb.meshMatrix = XMMatrixTranspose(matrix);
	cb.normalMatrix = XMMatrixIdentity();
	m_pContext->UpdateSubresource(m_pConstantBuffer, 0, NULL, &cb, 0, 0);

	SBuffer sb;
	sb.VP = sceneMatrix;
	// sb.VP = XMMatrixIdentity();
	m_pContext->UpdateSubresource(m_pSceneBuffer, 0, NULL, &sb, 0, 0);

	m_pContext->RSSetState(m_pRasterizerState);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* constBuffers[] = { m_pConstantBuffer };
	m_pContext->VSSetConstantBuffers(0, 1, constBuffers);

	ID3D11Buffer* sceneBuffers[] = { m_pSceneBuffer };
	m_pContext->VSSetConstantBuffers(1, 1, sceneBuffers);
	m_pContext->PSSetConstantBuffers(1, 1, sceneBuffers);

	{
		m_pContext->PSSetConstantBuffers(2, m_aConstantBuffers.size(), m_aConstantBuffers.data());
	}

	m_pContext->DrawIndexed((UINT)indexCount, 0, 0);
}

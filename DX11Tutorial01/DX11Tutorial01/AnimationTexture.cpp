#include <cassert>

#include "AnimationTexture.h"

#define SAFE_RELEASE(p) \
if (p != NULL) { \
	p->Release(); \
	p = NULL;\
}

AnimationTexture::TextureResources::~TextureResources()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTextureSRV);
}

AnimationTexture::AnimationTexture(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height) 
	: m_pDevice{ device }, m_pContext{ context }, m_iWidth{ width }, m_iHeight{ height },
	m_pBackgroudTexture{ nullptr }, m_pFieldSwapper{ nullptr }
{

}

AnimationTexture::~AnimationTexture()
{
	if (m_pFieldSwapper)
	{
		delete m_pFieldSwapper;
		m_pFieldSwapper = nullptr;
	}

	for (auto& pingPing : m_aLayerTextures)
	{
		if (pingPing)
		{
			delete pingPing;
		}
	}
}

void AnimationTexture::AddBackground(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV)
{
	assert(texture != nullptr && textureSRV != nullptr);

	if (m_pBackgroudTexture != nullptr)
	{
		delete m_pBackgroudTexture;
	}

	m_pBackgroudTexture = new TextureResources();
	assert(m_pBackgroudTexture);

	m_pBackgroudTexture->m_pTexture = texture;
	m_pBackgroudTexture->m_pTextureSRV = textureSRV;
}

void AnimationTexture::AddLayer(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV, ID3D11RenderTargetView* textureRTV)
{
	assert(texture != nullptr && textureSRV != nullptr && textureRTV != nullptr);

	PingPong* pingPong = new PingPong();
	assert(pingPong != nullptr);

	pingPong->SetupResources(PingPong::ResourceType::SOURCE, texture, textureRTV, textureSRV);
	
	{
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		depthDesc.ArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.Height = m_iHeight;
		depthDesc.Width = m_iWidth;
		depthDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pTextureRenderTarget(nullptr);
		ID3D11RenderTargetView* pTextureRenderTargetRTV(nullptr);
		ID3D11ShaderResourceView* pTextureRenderTargetSRV(nullptr);

		HRESULT result = m_pDevice->CreateTexture2D(&depthDesc, NULL, &pTextureRenderTarget);

		if (SUCCEEDED(result))
		{
			result = m_pDevice->CreateRenderTargetView(pTextureRenderTarget, NULL, &pTextureRenderTargetRTV);
		}
		if (SUCCEEDED(result))
		{
			result = m_pDevice->CreateShaderResourceView(pTextureRenderTarget, NULL, &pTextureRenderTargetSRV);
		}

		assert(SUCCEEDED(result));

		pingPong->SetupResources(PingPong::ResourceType::TARGET, pTextureRenderTarget, pTextureRenderTargetRTV, pTextureRenderTargetSRV);
	}

	m_aLayerTextures.push_back(pingPong);
}

void AnimationTexture::Swap()
{
	for (auto& pingPong : m_aLayerTextures)
	{
		pingPong->Swap();
	}
}

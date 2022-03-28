#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <vector>
#include <string>

#include "Texture.h"
#include "FieldSwapper.h"
#include "PingPong.h"


class AnimationTexture : public Texture
{
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	std::vector<PingPong*> m_aLayerTextures;
	std::vector<FieldSwapper*> m_aFieldSwappers;

	// Render staff
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11InputLayout* m_pInputLayout;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	ID3D11DepthStencilView* m_pDepthStencilView;

public:
	AnimationTexture(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height);
	AnimationTexture(ID3D11Device* device, ID3D11DeviceContext* context, std::string const& filename);
	~AnimationTexture();

	HRESULT CreateAnimationTextureResources(std::string const& vertexShader, std::string const& pixelShader);

	ID3D11ShaderResourceView* GetBackgroundTextureSRV() const;
	ID3D11Texture2D* GetBackgroundTexture() const;

	std::vector<ID3D11ShaderResourceView*> GetLayersSourceTexturesSRV() const;
	std::vector<ID3D11ShaderResourceView*> GetLayersTargetTexturesSRV() const override;
	std::vector<ID3D11RenderTargetView*> GetLayersTexturesRTV() const;
	std::vector<ID3D11Texture2D*> GetLayersTextures() const;

	std::vector<FieldSwapper*> GetFields() const override;

	void AddBackground(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);
	void AddLayer(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV, ID3D11RenderTargetView* textureRTV);

	void SetUpFields(std::vector<FieldSwapper*> const& fields);

	HRESULT AddBackgroundByName(std::string const& filename);
	HRESULT AddLayerByName(std::string const& filename);

	void Swap();

	void IncrementStep(size_t incSize);

	void Render(ID3D11RasterizerState* pRasterizerState,
		ID3D11SamplerState* pSamplerState,
		ID3D11Buffer* pConstantBuffer,
		FieldSwapper* pFieldSwapper);

private:
	void RenderTexture(size_t textureNum,
		ID3D11RasterizerState* pRasterizerState,
		ID3D11SamplerState* pSamplerState,
		ID3D11Buffer* pConstantBuffer,
		FieldSwapper* pFieldSwapper);

	ID3D11VertexShader* CreateVertexShader(LPCTSTR shaderSource, ID3DBlob** ppBlob);
	ID3D11PixelShader* CreatePixelShader(LPCTSTR shaderSource);
};

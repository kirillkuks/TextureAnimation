#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <vector>
#include <string>

#include "FieldSwapper.h"
#include "PingPong.h"

class AnimationTexture // : public Texture
{
private:
	struct TextureResources
	{
	public:
		ID3D11Texture2D* m_pTexture;
		ID3D11ShaderResourceView* m_pTextureSRV;

	public:
		TextureResources() = default;
		~TextureResources();
	};

private:
	UINT m_iWidth;
	UINT m_iHeight;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	TextureResources* m_pBackgroudTexture;
	std::vector<PingPong*> m_aLayerTextures;

	FieldSwapper* m_pFieldSwapper;

public:
	AnimationTexture(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height);
	~AnimationTexture();

	ID3D11ShaderResourceView* GetBackGroundTextureSRV() const;
	std::vector<ID3D11ShaderResourceView*> GetLayersTexturesSRV() const;

	void AddBackground(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);
	void AddLayer(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV, ID3D11RenderTargetView* textureRTV);

	void AddBackgroundByName(std::string const& filename);
	void AddLayerByName(std::string const& filename);

	void Swap();

	// TODO:
	// Render
	// Load by file name
};

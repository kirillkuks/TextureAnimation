#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include <string>

class Texture
{
private:
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pTextureSRV;

public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string const& filename);
	~Texture();

	ID3D11Texture2D* const SourceTexture() const;
	ID3D11ShaderResourceView* const ShaderResourceView() const;
};

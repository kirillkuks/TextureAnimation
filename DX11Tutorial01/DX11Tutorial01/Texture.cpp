#include "Texture.h"

#include <assert.h>
#include "WICTextureLoader.h"
#include "stb_image.h"
#include "stb_image_write.h"

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string const& filename) :
	m_pTexture{ NULL }, m_pTextureSRV{ NULL }
{
	HRESULT result = CreateWICTextureFromFile(
		device,
		context, 
		std::wstring(filename.begin(), filename.end()).c_str(), 
		(ID3D11Resource**)&m_pTexture, 
		&m_pTextureSRV
	);

	assert(SUCCEEDED(result));
}

Texture::~Texture()
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}

	if (m_pTextureSRV)
	{
		m_pTextureSRV->Release();
		m_pTextureSRV = NULL;
	}
}

ID3D11Texture2D* const Texture::SourceTexture() const
{
	return m_pTexture;
}

ID3D11ShaderResourceView* const Texture::ShaderResourceView() const
{
	return m_pTextureSRV;
}

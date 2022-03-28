#pragma once
#include <basetsd.h>
#include <vector>

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include "Texture.h"
#include "AnimationTexture.h"

using namespace DirectX;

struct Vertex
{
	XMVECTORF32 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
};

struct CBuffer
{
	XMMATRIX meshMatrix;
	XMMATRIX normalMatrix;
	XMVECTORF32 objColor;
};

struct SBuffer
{
	XMMATRIX VP;
};

class Mesh
{
private:
	size_t indexCount;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;

	ID3D11Buffer* m_pConstantBuffer;
	ID3D11Buffer* m_pSceneBuffer;

	ID3D11RasterizerState* m_pRasterizerState;

	ID3D11ShaderResourceView* m_pTextureSRV;

	std::vector<ID3D11ShaderResourceView*> m_aShaderResources;
	std::vector<ID3D11Buffer*> m_aConstantBuffers;

	AnimationTexture* m_pAnimationTexture;
	Texture* m_pTexture;

public:
	Mesh(ID3D11Device* const& device, 
		ID3D11DeviceContext* const& context, 
		std::vector<Vertex> const& vertecies, 
		std::vector<UINT16> const& indeces);
	~Mesh();

	void SetShaders(ID3D11VertexShader* const& vertexShader, 
		ID3D11PixelShader* const& pixelShader, 
		ID3D11InputLayout* const& layout);   // Change

	void SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);   // Change
	void SetOriginTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);   // Change
	void SetShaderResources(std::vector<ID3D11ShaderResourceView*> const& resources);   // Change
	void SetConstantBuffers(std::vector<ID3D11Buffer*> const& buffers);   // Change

	void Draw(XMMATRIX matrix, XMMATRIX sceneMatrix);

	void SetTexture(AnimationTexture* texture);
	void SetTexture(Texture* texture);
};

#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include "Image.h"
#include "PingPong.h"
#include "VectorField.h"
#include "FieldSwapper.h"

class Renderer
{
public:
	Renderer();

	bool Init(HWND hWnd);
	void Term();

	void Resize(UINT width, UINT height);

	bool Update();
	bool Render();

	void MouseMove(int dx, int dy);
	void MouseWheel(int dz);

	void SwitchNormalMode();

private:
	HRESULT SetupBackBuffer();

	HRESULT CreateTransparentObjects();
	HRESULT CreateScene();
	void DestroyScene();
	void RenderScene();
	void RenderSceneTransparent();

	HRESULT SetupAnimationTextureBuffer();
	HRESULT CreateAnimationTexturesResources();
	HRESULT CreateVectorFieldTexture();
	void RenderTexture();

	ID3D11VertexShader* CreateVertexShader(LPCTSTR shaderSource, ID3DBlob** ppBlob);
	ID3D11PixelShader*  CreatePixelShader(LPCTSTR shaderSource);

	// HRESULT 

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBufferRTV;

	ID3D11Texture2D* m_pDepth;
	ID3D11DepthStencilView* m_pDepthDSV;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;

	ID3D11Resource* m_pTexture;
	ID3D11ShaderResourceView* m_pTextureSRV;

	ID3D11Resource* m_pTextureNM;
	ID3D11ShaderResourceView* m_pTextureNMSRV;

	ID3D11SamplerState* m_pSamplerState;

	ID3D11Buffer* m_pModelBuffer;
	ID3D11Buffer* m_pModelBuffer2;
	ID3D11Buffer* m_pSceneBuffer;

	ID3D11Buffer* m_pModelBuffer3;
	ID3D11Buffer* m_pModelBuffer4;
	ID3D11Buffer* m_pTransVertexBuffer;
	ID3D11Buffer* m_pTransIndexBuffer;
	ID3D11VertexShader* m_pTransVertexShader;
	ID3D11PixelShader* m_pTransPixelShader;
	ID3D11InputLayout* m_pTransInputLayout;
	ID3D11RasterizerState* m_pTransRasterizerState;
	ID3D11BlendState* m_pTransBlendState;
	ID3D11DepthStencilState* m_pTransDepthState;

	ID3D11RasterizerState* m_pRasterizerState;

	/////////////////////////////////////////////

	ID3D11Buffer* m_pConstantBuffer;

	ID3D11Resource* m_pTextureCopy;

	ID3D11Texture2D* m_pATDepth;
	ID3D11DepthStencilView* m_pATDepthDSV;

	ID3D11VertexShader* m_pAnimationTextureVertexShader;
	ID3D11PixelShader* m_pAnimationTexturePixelShader;

	ID3D11Buffer* m_pAnimationTextureVertexBuffer;
	ID3D11Buffer* m_pAnimationTextureIndexBuffer;
	ID3D11InputLayout* m_pAnimationTextureInputLayout;

	ID3D11Texture2D* m_pAnimationTextureVectorField;
	ID3D11ShaderResourceView* m_pAnimationTextureVectorFieldSRV;

	UINT m_width;
	UINT m_height;

	size_t m_usec;
	size_t m_psec;

	size_t m_esec = 0;

	float m_frameLength = 1.0f / 60.0f;

	float m_lon;
	float m_lat;
	float m_dist;

	int m_mode;

	VectorField* vectorField;
	Image* image;

	size_t imageSize;

	PingPong* m_pPingPong;
	FieldSwapper* m_pFieldSwapper;
};

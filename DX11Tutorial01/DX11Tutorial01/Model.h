#pragma once
#include <string>
#include <basetsd.h>
#include "tiny_gltf.h"

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include "Mesh.h"
#include "Texture.h"

using namespace DirectX;

/// TODO
/// 1) Сделать отдельный класс для текстуры (для анимированной текстуры)
/// 2) Починить матрицы
/// 3) Проверить, удаляются ли все ресуры ДиректИкса
/// 4) ???
/// 5) Свой класс для массива индексов и вершин


class Model
{
private:
	tinygltf::Model model;
	std::vector<unsigned char> data;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	std::vector<Mesh*> m_aMeshes;
	std::vector<XMMATRIX> m_aMatrices;
	std::vector<Texture*> m_aTextures;

	std::string fname;

	// float scale_factor = 0.0005f;
	float scale_factor = 1;


private:
	std::vector<unsigned char> LoadData(std::string const& filename) const;

	void ParseNode(size_t nodeInd, XMMATRIX matrix = XMMatrixIdentity());

	void LoadMesh(size_t ind);

	Texture* LoadTexture(size_t imageInd);

	std::vector<Vertex> CreateVerteciesBuffer(std::vector<XMVECTORF32> const& positions,
		std::vector<XMFLOAT3> const& normals,
		std::vector<XMFLOAT3> const& tangents,
		std::vector<XMFLOAT2> const& texs) const;

	std::vector<float> StoreFloats(tinygltf::Accessor const& accessor);
	std::vector<UINT16> StoreIndeces(tinygltf::Accessor const& accessor);

	std::vector<ID3D11Texture2D*> StoreTextures();

	std::vector<XMVECTORF32> GroupPosFloats(std::vector<float> const& floats) const;
	std::vector<XMFLOAT3> GroupNormalFloats(std::vector<float> const& floats) const;
	std::vector<XMFLOAT3> GroupTangentFloats(std::vector<float> const& floats) const;
	std::vector<XMFLOAT2> GroupTexFloats(std::vector<float> const& floats) const;

public:
	Model(ID3D11Device* const& device, ID3D11DeviceContext* const& context, std::string const& filename);
	~Model();

	void SetShaders(ID3D11VertexShader* const& vertexShader,
		ID3D11PixelShader* const& pixelShader,
		ID3D11InputLayout* const layout);

	void Draw(XMMATRIX sceneMatrix);

	void SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);   // Change
	void SetOriginTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV);   // Change
	void SetShaderResources(std::vector<ID3D11ShaderResourceView*> const& resources);   // Change
	void SetConstantBuffers(std::vector<ID3D11Buffer*> const& buffers);   // Change
};

#include "Model.h"

#include "WICTextureLoader.h"
#include <fstream>


Model::Model(ID3D11Device* const& device, ID3D11DeviceContext* const& context, std::string const& filename) :
	m_pDevice{ device }, m_pContext{ context }, fname{ filename }
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);

	assert(warn.empty());
	assert(err.empty());
	assert(ret);

	data = LoadData(filename);

	ParseNode(0);

	int a = 0;
}

Model::~Model()
{
	for (size_t i = 0; i < m_aMeshes.size(); ++i)
	{
		delete m_aMeshes[i];
	}
}

void Model::Draw(XMMATRIX sceneMatrix)
{
	m_aMeshes[3]->Draw(m_aMatrices[3], sceneMatrix);   // Change

	// for (size_t i = 0; i < m_aMeshes.size(); ++i)
	// {
	// 	 m_aMeshes[i]->Draw(m_aMatrices[i], sceneMatrix);
	// }
}

void Model::SetShaders(ID3D11VertexShader* const& vertexShader,
	ID3D11PixelShader* const& pixelShader,
	ID3D11InputLayout* const layout)
{
	for (size_t i = 0; i < m_aMeshes.size(); ++i)
	{
		m_aMeshes[i]->SetShaders(vertexShader, pixelShader, layout);
	}
}

void Model::SetTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureSRV)
{
	m_aMeshes[3]->SetTexture(texture, textureSRV);   // Change
}

std::vector<unsigned char> Model::LoadData(std::string const& filename) const
{
	std::string uri = model.buffers[0].uri;
	std::string file = filename;
	std::string fileDirectory = file.substr(0, file.find_last_of('/') + 1);

	std::ifstream fin(fileDirectory + uri, std::ios::binary);
	assert(fin.is_open());

	fin.seekg(0, std::ios::end);
	int sz = (int)fin.tellg();
	fin.seekg(0, std::ios::beg);

	std::string buffer(sz, ' ');
	fin.read(&buffer[0], sz);

	return std::vector<unsigned char>(buffer.begin(), buffer.end());
}

void Model::ParseNode(size_t nodeInd, XMMATRIX matrix)
{
	tinygltf::Node node = model.nodes[nodeInd];

	float translation[] = { 0.0f, 0.0f, 0.0f };
	if (node.translation.size() != 0)
	{
		for (size_t i = 0; i < node.translation.size(); ++i)
		{
			translation[i] = (float)node.translation[i];
		}
	}
	XMMATRIX translationMatrix = XMMatrixTranslation(translation[0], translation[1], translation[2]);

	float rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	if (node.rotation.size() != 0)
	{
		for (size_t i = 0; i < node.rotation.size(); ++i)
		{
			rotation[i] = (float)node.rotation[i];
		}
	}
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion({ rotation[0], rotation[1], rotation[2], rotation[3] });

	float scale[] = {1.0f, 1.0f, 1.0f};
	if (node.scale.size() != 0)
	{
		for (size_t i = 0; i < node.scale.size(); ++i)
		{
			scale[i] = (float)node.scale[i];
		}
	}
	XMMATRIX scaleMatrix = XMMatrixScaling(scale[0], scale[1], scale[2]);

	XMMATRIX mat = XMMatrixIdentity();
	if (node.matrix.size() != 0)
	{
		float matrix[16] = {};
		for (size_t i = 0; i < node.matrix.size(); ++i)
		{
			matrix[i] = (float)node.matrix[i];
		}
		mat = XMMatrixSet(matrix[0], matrix[1], matrix[2], matrix[3],
			matrix[4], matrix[5], matrix[6], matrix[7],
			matrix[8], matrix[9], matrix[10], matrix[11],
			matrix[12], matrix[13], matrix[14], matrix[15]);
	}

	XMMATRIX newMatrix = XMMatrixMultiply(matrix, mat);
	newMatrix = XMMatrixMultiply(newMatrix, translationMatrix);
	newMatrix = XMMatrixMultiply(newMatrix, rotationMatrix);
	newMatrix = XMMatrixMultiply(newMatrix, scaleMatrix);

	// XMMATRIX newMatrix = matrix * mat * translationMatrix * rotationMatrix * scaleMatrix;

	if (node.mesh >= 0)
	{
		m_aMatrices.push_back(newMatrix);
		LoadMesh(node.mesh);
	}

	if (node.children.size() > 0)
	{
		for (size_t i = 0; i < node.children.size(); ++i)
		{
			ParseNode(node.children[i], newMatrix);
		}
	}
}

void Model::LoadMesh(size_t ind)
{
	size_t posInd = model.meshes[ind].primitives[0].attributes["POSITION"];
	size_t normalInd = model.meshes[ind].primitives[0].attributes["NORMAL"];
	size_t tangentInd = model.meshes[ind].primitives[0].attributes["TANGENT"];
	size_t texInd = model.meshes[ind].primitives[0].attributes["TEXCOORD_0"];
	size_t indecesInd = model.meshes[ind].primitives[0].indices;

	std::vector<float> posVec = StoreFloats(model.accessors[posInd]);
	std::vector<float> normalVec = StoreFloats(model.accessors[normalInd]);
	std::vector<float> tangentVec = StoreFloats(model.accessors[tangentInd]);
	std::vector<float> texVec = StoreFloats(model.accessors[texInd]);

	size_t size = texVec.size();
	assert(size % 2 == 0);
	size = size / 2;
	assert(size == posVec.size() / 3);
	assert(size == normalVec.size() / 3);
	assert(size == tangentVec.size() / 4);

	std::vector<UINT16> indeces = StoreIndeces(model.accessors[indecesInd]);

	std::vector<XMVECTORF32> positions = GroupPosFloats(posVec);
	std::vector<XMFLOAT3> normals = GroupNormalFloats(normalVec);
	std::vector<XMFLOAT3> tangents = GroupTangentFloats(tangentVec);
	std::vector<XMFLOAT2> texs = GroupTexFloats(texVec);

	assert(size == positions.size());
	assert(size == normals.size());
	assert(size == tangents.size());
	assert(size == texs.size());

	std::vector<Vertex> vertecies = CreateVerteciesBuffer(
		positions,
		normals,
		tangents,
		texs
	);

	m_aMeshes.push_back(new Mesh(m_pDevice, m_pContext, vertecies, indeces));

	int a = 1;
}

std::vector<Vertex> Model::CreateVerteciesBuffer(std::vector<XMVECTORF32> const& positions,
	std::vector<XMFLOAT3> const& normals,
	std::vector<XMFLOAT3> const& tangents,
	std::vector<XMFLOAT2> const& texs) const
{
	size_t size = positions.size();

	assert(size == normals.size());
	assert(size == tangents.size());
	assert(size == texs.size());

	std::vector<Vertex> vertecies(size);

	for (size_t i = 0; i < size; ++i)
	{
		vertecies[i] = { positions[i], texs[i], normals[i], tangents[i] };
	}

	return vertecies;
}

std::vector<float> Model::StoreFloats(tinygltf::Accessor const& accessor)
{
	int buffViewInd = accessor.bufferView;
	size_t count = accessor.count;
	size_t accByteOffset = accessor.byteOffset;
	int type = accessor.type;

	size_t byteOffset = model.bufferViews[buffViewInd].byteOffset;

	size_t numPerVert;
	if (type == TINYGLTF_TYPE_SCALAR)
	{
		numPerVert = 1;
	}
	else if (type == TINYGLTF_TYPE_VEC2)
	{
		numPerVert = 2;
	}
	else if (type == TINYGLTF_TYPE_VEC3)
	{
		numPerVert = 3;
	}
	else if (type == TINYGLTF_TYPE_VEC4)
	{
		numPerVert = 4;
	}
	else
	{
		assert(false);
	}

	size_t dataBeginning = byteOffset + accByteOffset;
	size_t dataLength = 4 * count * numPerVert;

	std::vector<float> floatVec;

	for (size_t i = dataBeginning; i < dataBeginning + dataLength;)
	{
		unsigned char bytes[4] = { data[i++], data[i++], data[i++], data[i++] };
		float val;
		std::memcpy(&val, bytes, sizeof(float));
		floatVec.push_back(val);
	}

	return floatVec;
}

std::vector<UINT16> Model::StoreIndeces(tinygltf::Accessor const& accessor)
{
	int bufferInd = accessor.bufferView;
	size_t count = accessor.count;
	size_t accByteOffset = accessor.byteOffset;
	int componentType = accessor.componentType;

	size_t byteOffset = model.bufferViews[bufferInd].byteOffset;
	size_t dataBeginning = byteOffset + accByteOffset;
	
	std::vector<UINT16> indeces;

	if (componentType == 5125)
	{
		for (size_t i = dataBeginning; i < byteOffset + accByteOffset + 4 * count;)
		{
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int val;
			std::memcpy(&val, bytes, sizeof(unsigned int));
			indeces.push_back((UINT16)val);
		}
	}
	else if (componentType == 5123)
	{
		for (size_t i = dataBeginning; i < byteOffset + accByteOffset + 2 * count;)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short val;
			std::memcpy(&val, bytes, sizeof(unsigned short));
			indeces.push_back((UINT16)val);
		}
	}
	else if (componentType == 5122)
	{
		for (size_t i = dataBeginning; i < byteOffset + accByteOffset + 2 * count;)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			short val;
			std::memcpy(&val, bytes, sizeof(short));
			indeces.push_back((UINT16)val);
		}
	}
	else
	{
		assert(false);
	}

	return indeces;
}

std::vector<ID3D11Texture2D*> Model::StoreTextures()
{
	std::vector<ID3D11Texture2D*> textures;
	std::vector<ID3D11ShaderResourceView*> texturesSRV;
	std::string fileDirectory = fname.substr(0, fname.find_last_of('/') + 1);

	for (size_t i = 0; i < model.images.size(); ++i)
	{
		std::string texturePath = model.images[i].uri;

		if (texturePath.find("baseColor") != std::string::npos)
		{
			ID3D11Texture2D* texture = NULL;
			ID3D11ShaderResourceView* textureSRV = NULL;

			HRESULT result = CreateWICTextureFromFile(m_pDevice,
				m_pContext,
				std::wstring(texturePath.begin(), texturePath.end()).c_str(),
				(ID3D11Resource**)&texture,
				&textureSRV);

			assert(SUCCEEDED(result));

			textures.push_back(texture);
			texturesSRV.push_back(textureSRV);
		}

	}

	return textures;
}

std::vector<XMVECTORF32> Model::GroupPosFloats(std::vector<float> const& floats) const
{
	size_t size = floats.size() / 3;
	std::vector<XMVECTORF32> vec(size);

	for (size_t i = 0; i < size; ++i)
	{
		vec[i] = { floats[3 * i + 0], floats[3 * i + 1], floats[3 * i + 2], 1 };
	}

	return vec;
}

std::vector<XMFLOAT3> Model::GroupNormalFloats(std::vector<float> const& floats) const
{
	size_t size = floats.size() / 3;
	std::vector<XMFLOAT3> vec(size);

	for (size_t i = 0; i < size; ++i)
	{
		vec[i] = { floats[3 * i + 0], floats[3 * i + 1], floats[3 * i + 2] };
	}

	return vec;
}

std::vector<XMFLOAT3> Model::GroupTangentFloats(std::vector<float> const& floats) const
{
	size_t size = floats.size() / 4;
	std::vector<XMFLOAT3> vec(size);

	for (size_t i = 0; i < size; ++i)
	{
		vec[i] = { floats[4 * i + 0], floats[4 * i + 1], floats[4 * i + 2] };
	}

	return vec;
}

std::vector<XMFLOAT2> Model::GroupTexFloats(std::vector<float> const& floats) const
{
	size_t size = floats.size() / 2;
	std::vector<XMFLOAT2> vec(size);

	for (size_t i = 0; i < size; ++i)
	{
		vec[i] = { floats[2 * i + 0], floats[2 * i + 1] };
	}

	return vec;
}

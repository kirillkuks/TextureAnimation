#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <vector>


class FieldSwapper
{
private:
	struct VectorFieldResources
	{
		VectorFieldResources() = default;
		VectorFieldResources(ID3D11Texture2D* vectorFieldTexture, ID3D11ShaderResourceView* vectorFieldTextureSRV);

		~VectorFieldResources();

		ID3D11Texture2D* m_pVectorFieldTexture;
		ID3D11ShaderResourceView* m_pVectorFieldTextureSRV;
	};

private:
	using fields_t = std::vector<VectorFieldResources*>;

private:
	fields_t m_aFeildsResources;
	size_t m_iCurFieldIndex;

	std::vector<size_t> m_aStepsPerField;
	size_t m_iCurStepsNum;
	size_t m_iCurStepsCounter;

public:
	FieldSwapper();
	~FieldSwapper();

	ID3D11ShaderResourceView* CurrentVectorFieldSRV() const;

	void AddField(ID3D11Texture2D* vectorFieldTexture, ID3D11ShaderResourceView* vectorFieldTextureSRV);
	void NextField();
	void SetUpStepPerFiled(std::vector<size_t> const& stepsPerField);
	void IncStep();

};

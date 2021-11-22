#include "FieldSwapper.h"

#include <assert.h>

FieldSwapper::VectorFieldResources::VectorFieldResources(ID3D11Texture2D* vectorFieldTexture, ID3D11ShaderResourceView* vectorFieldTextureSRV) :
	m_pVectorFieldTexture{ vectorFieldTexture }, m_pVectorFieldTextureSRV{ vectorFieldTextureSRV } {}

FieldSwapper::VectorFieldResources::~VectorFieldResources()
{
	if (m_pVectorFieldTexture)
	{
		m_pVectorFieldTexture->Release();
		m_pVectorFieldTexture = nullptr;
	}

	if (m_pVectorFieldTextureSRV)
	{
		m_pVectorFieldTextureSRV->Release();
		m_pVectorFieldTextureSRV = nullptr;
	}
}

FieldSwapper::FieldSwapper() : m_iCurFieldIndex{ 0 }, m_iCurStepsNum{ 0 }, m_iCurStepsCounter{ 0 } {}

FieldSwapper::~FieldSwapper()
{
	for (size_t i = 0; i < m_aFeildsResources.size(); ++i)
	{
		delete m_aFeildsResources[i];
	}
}

ID3D11ShaderResourceView* FieldSwapper::CurrentVectorFieldSRV() const
{
	return m_aFeildsResources[m_iCurFieldIndex]->m_pVectorFieldTextureSRV;
}

void FieldSwapper::AddField(ID3D11Texture2D* vectorFieldTexture, ID3D11ShaderResourceView* vectorFieldTextureSRV)
{
	assert(vectorFieldTexture && vectorFieldTextureSRV);

	m_aFeildsResources.push_back(new VectorFieldResources(vectorFieldTexture, vectorFieldTextureSRV));
}

void FieldSwapper::NextField()
{
	m_iCurFieldIndex = (m_iCurFieldIndex + 1) % m_aFeildsResources.size();

	{
		m_iCurStepsNum = 1;
	}
}

void FieldSwapper::SetUpStepPerFiled(std::vector<size_t> const& stepsPerField)
{
	m_aStepsPerField = stepsPerField;
	m_iCurStepsNum = 0;
}

void FieldSwapper::IncStep()
{
	++m_iCurStepsCounter;

	if (m_iCurStepsCounter == m_aStepsPerField[m_iCurStepsNum])
	{
		m_iCurStepsNum = (m_iCurStepsNum + 1) % m_aFeildsResources.size();
		m_iCurStepsCounter = 0;

		NextField();
	}
}

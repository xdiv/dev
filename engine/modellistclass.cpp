#include "modellistclass.h"

ModelListClass::ModelListClass()
{
	m_ModelInfoList = 0;
}

ModelListClass::ModelListClass(const ModelListClass& other)
{
}

ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	int i;
	float red, green, blue;

	//First store the number of models that will be used and then create the list array of them using the ModelInfoType structure.
	// Store the number of models.
	m_modelCount = numModels;

	// Create a list array of the model information.
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if(!m_ModelInfoList)
	{
		return false;
	}

	// Seed the random number generator with the current time and then randomly generate the position of color of the models and
	// store them in the list array.
	// Seed the random generator with the current time.
	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	for(i=0; i<m_modelCount; i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = D3DXVECTOR4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].positionX = (((float)rand()-(float)rand())/RAND_MAX) * 60.0f;
		m_ModelInfoList[i].positionY = (((float)rand()-(float)rand())/RAND_MAX)  * 60.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand()-(float)rand())/RAND_MAX) * 60.0f) + 5.0f;
	}

	return true;
}

//The Shutdown function releases the model information list array.

void ModelListClass::Shutdown()
{
	// Release the model information list.
	if(m_ModelInfoList)
	{
		delete [] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

	return;
}

//GetModelCount returns the number of models that this class maintains information about.
int ModelListClass::GetModelCount()
{
	return m_modelCount;
}
//The GetData function extracts the position and color of a sphere at the given input index location.
void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, D3DXVECTOR4& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;

	return;
}

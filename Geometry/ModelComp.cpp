#include "pch.h"
#include "ModelComp.h"

ModelComp::ModelComp()
{
	m_isDelete = true;
}

ModelComp::~ModelComp()
{
}

void ModelComp::SetReferComponent(ModelComp* comp)
{
	m_referComponents.push_back(comp);
}

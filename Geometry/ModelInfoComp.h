#pragma once
#include "ModelComp.h"
class ModelInfoComp : public ModelComp
{
private:
    wstring m_modelName; 
    bool m_isActive;
    bool m_isFrustumCulling;
public:
    ModelInfoComp();
    ModelInfoComp(const ModelInfoComp& modelInfoComp);
    ~ModelInfoComp();

    wstring GetModelName() { return m_modelName; }
    void SetModelName(wstring name) { m_modelName = name; }
    
    bool IsActive() { return m_isActive; }
    bool IsFrustum() { return m_isFrustumCulling; }

    virtual void Render(ModelNode* node);
};


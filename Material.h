#ifndef MATERIAL_H__
#define MATERIAL_H__

#include "Program.h"

namespace tessterrain {

class Material {
    
protected:
    GLSLProgram* m_shader;
    string m_shaderDir;

public:
    Material();
    ~Material();
    
    GLSLProgram* getShader() { return m_shader; }
};

/**
 */
class MeshMaterial: public Material {
    
protected:
    
public:
    MeshMaterial();
    ~MeshMaterial();
};

/**
 */
class TessMaterial: public Material {
    
protected:
    
public:
    TessMaterial();
    ~TessMaterial();
};

/**
 */
class SSMaterial: public Material {
    
protected:
    
public:
    SSMaterial();
    ~SSMaterial();
};

}; //namespace tessterrain

#endif

#include "Material.h"
#include <iostream>

namespace tessterrain {

Material::Material(): m_shader(0) {
    m_shaderDir = SHADER_DIR;
    //std::cout << "Shader dir: " << SHADER_DIR << std::endl;

    color = glm::vec3(1.0);
    Ka = glm::vec3( 0.1f, 0.1f, 0.1f );
    Kd = glm::vec3( 1.0f, 1.0f, 1.0f );
    Ks = glm::vec3( 0.3f, 0.3f, 0.3f );
    shininess = 1.0;
    fogEnabled = false;
}

Material::~Material() {
    if(m_shader)
        delete m_shader;
}

/**
 */
MeshMaterial::MeshMaterial(): Material() {
    m_shader = new GLSLProgram();
    string filename;
    filename = m_shaderDir; filename.append("mesh.vert");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("mesh.frag");
    m_shader->compileShader(filename.c_str());
    m_shader->link();
}

MeshMaterial::~MeshMaterial() {
    
}

/**
 */
TessMaterial::TessMaterial(): Material() {

    m_shader = new GLSLProgram();
    string filename;
    filename = m_shaderDir; filename.append("terrain.vert");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terrain.tcs");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terrain.tes");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terrain.geom");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terrain.frag");
    m_shader->compileShader(filename.c_str());
    m_shader->link();
}

TessMaterial::~TessMaterial() {
    
}

/**
 */
SSMaterial::SSMaterial(): Material() {
    m_shader = new GLSLProgram();
    string filename;
    filename = m_shaderDir; filename.append("ss_display.vert");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("ss_display.frag");
    m_shader->compileShader(filename.c_str());
    m_shader->link();
}

SSMaterial::~SSMaterial() {
    
}

}; //namespace tessterrain

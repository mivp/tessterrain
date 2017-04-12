#include "Material.h"
#include <iostream>

namespace tessterrain {

Material::Material(): m_shader(0) {
    m_shaderDir = SHADER_DIR;
    std::cout << "Shader dir: " << SHADER_DIR << std::endl;
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
    filename = m_shaderDir; filename.append("terrain.vert");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terrain.frag");
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
    filename = m_shaderDir; filename.append("terraintessellation.vert");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terraintessellation.tcs");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terraintessellation.tes");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terraintessellation.geom");
    m_shader->compileShader(filename.c_str());
    filename = m_shaderDir; filename.append("terraintessellation.frag");
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
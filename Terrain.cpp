#include "Terrain.h"
#include "INIReader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <float.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

using namespace std;
using namespace glm;

namespace tessterrain {

/**
 Tessellation Terrain
 */
TessTerrain::TessTerrain(): m_material(0), m_initialized(false), m_texHightmap(0), m_texture(0),
                            m_vbo(0), m_vao(0),  m_displayMode(0) {
                                                
    m_displayModeNames.push_back("shadeSimpleWireFrame");
    m_displayModeNames.push_back("shadeWorldHeight");
    m_displayModeNames.push_back("shadeWorldHeightWireFrame");
    m_displayModeNames.push_back("shadeWorldNormal");
    m_displayModeNames.push_back("shadeLightingFactors");
    m_displayModeNames.push_back("shadeTextured");
    m_displayModeNames.push_back("shadeTexturedAndLit");
}

TessTerrain::~TessTerrain() {
#ifndef OMEGALIB_MODULE
    if(m_texHightmap)
        delete m_texHightmap;
    if(m_texture)
        delete m_texture;
    if(m_vao > 0) {
        glDeleteVertexArrays(1,&m_vao);
        glDeleteBuffers(1,&m_vbo);
    }
#endif
}
    
void TessTerrain::init(string configfile) {
    cout << "Config file: " << configfile << endl;

    INIReader reader(configfile);
    string str = reader.Get("file", "heightmap", "");
    if(str.length() == 0) {
        cout << "ERROR: no highmap" << endl;
        exit(1);
    }
    m_files.push_back(str);
    str = reader.Get("file", "texture", "");
    if(str.length() > 0)
        m_files.push_back(str);
    
    m_horizontalRes = glm::vec2(reader.GetReal("horizontalres", "wres", 1), reader.GetReal("horizontalres", "hres", 1));
    
    m_heightRange = glm::vec2(reader.GetReal("heightrange", "min", 0), reader.GetReal("heightrange", "max", 1));

    float vscalemin, vscalemax;
    vscalemin = reader.GetReal("verticalscale", "min", -1);
    vscalemax = reader.GetReal("verticalscale", "max", -1);
    if(vscalemin == -1 || vscalemax == -1)
    	m_verticalScale = glm::vec2(m_heightRange[0], m_heightRange[1]-m_heightRange[0]);
    else
        m_verticalScale = glm::vec2(vscalemin, vscalemax - vscalemin);
}

void TessTerrain::printInfo() {
    
    for (int i=0; i < m_files.size(); i++)
        cout << "file: " << m_files[i] << endl;
    cout << "wres: " << m_horizontalRes[0] << " hres: " << m_horizontalRes[1] << endl;
    cout << "h min: " << m_heightRange[0] << " h max: " << m_heightRange[1] << endl;
}
    
void TessTerrain::nextDisplayMode(bool forward) {
    
    if(forward) {
        m_displayMode++;
        if(m_displayMode >= DisplayModeCount)
            m_displayMode = 0;
    }
    else {
        m_displayMode--;
        if(m_displayMode < 0)
            m_displayMode = DisplayModeCount - 1;
    }
    
    if(!m_texture && (m_displayMode == Textured || m_displayMode == TexturedAndLit) )
        nextDisplayMode(forward);
        
    cout << "display mdoe: " << m_displayModeNames[m_displayMode] << endl;
}
    
void TessTerrain::moveTo(glm::vec3 pos) {
    m_modelMatrix = glm::mat4(1.0);
    m_modelMatrix = glm::translate(m_modelMatrix, pos);
}

void TessTerrain::setup(){
    
    if(!m_material)
        m_material = new TessMaterial();
   
    // textures
    m_texHightmap = new Texture(m_files[0].c_str(), 0, false);
    if(m_files.size() > 1)
        m_texture = new Texture(m_files[1].c_str(), 1, true);
    
    // patches
    const int maxTessellationLevel = 64;
    const int trianglesPerHeightSample = 1;
    const int xDivisions = trianglesPerHeightSample * m_texHightmap->getWidth() / maxTessellationLevel;
    const int zDivisions = trianglesPerHeightSample * m_texHightmap->getHeight() / maxTessellationLevel;
    cout << "xDivisions = " << xDivisions << " zDivisions = " << zDivisions << endl;
    m_patchCount = xDivisions * zDivisions;
    vector<float> positionData( 2 * m_patchCount ); // 2 floats per vertex
    cout << "Total number of patches = " << m_patchCount << endl;
    
    const float dx = 1.0f / static_cast<float>( xDivisions );
    const float dz = 1.0f / static_cast<float>( zDivisions );
    
    for ( int j = 0; j < 2 * zDivisions; j += 2 ) {
        float z = static_cast<float>( j ) * dz * 0.5;
        for ( int i = 0; i < 2 * xDivisions; i += 2 ) {
            float x = static_cast<float>( i ) * dx * 0.5;
            const int index = xDivisions * j + i;
            positionData[index]     = x;
            positionData[index + 1] = z;
        }
    }
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, positionData.size() * sizeof( float ), &positionData[0], GL_STATIC_DRAW);
    
    GLSLProgram* shader = m_material->getShader();
    //shader->bind();
    unsigned int val = glGetAttribLocation(shader->getHandle(), "vertexPosition");
    glEnableVertexAttribArray(val);
    glVertexAttribPointer(val, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (const GLvoid*)0);
    
    glBindVertexArray(0);
    
    // Get subroutine indices
    for ( int i = 0; i < DisplayModeCount; ++i){
        m_displayModeSubroutines.push_back( glGetSubroutineIndex( shader->getHandle(), GL_FRAGMENT_SHADER, m_displayModeNames[i].c_str()) );
    }
    
    // cal values
    m_horizontalScale = glm::vec2(m_horizontalRes[0] * m_texHightmap->getWidth(), m_horizontalRes[1] * m_texHightmap->getHeight());
    if(m_horizontalScale[1] < 0)
        m_horizontalScale[1] *= -1;
    
    cout << "horizontal scale: " << m_horizontalScale[0] << " " << m_horizontalScale[1] << endl;
    cout << "vertical scale: " << m_verticalScale[0] << " " << m_verticalScale[1] << endl;
    
    m_initialized = true;
}

void TessTerrain::render(const float MV[16], const float P[16]) {
    
    if(!m_initialized)
        setup();
     
    GLSLProgram* shader = m_material->getShader();
    shader->bind();
    m_texHightmap->bind();
    if(m_texture)
        m_texture->bind();
    
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &m_displayModeSubroutines[m_displayMode] );
    
    //texture
    shader->setUniform( "heightMap", (int) m_texHightmap->index );
    if(m_texture)
        shader->setUniform( "tex0", (int) m_texture->index );
    
    //uniforms
    shader->setUniform( "line.width", 1.0f );
    shader->setUniform( "line.color", glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
    
    // Set the fog parameters
    shader->setUniform( "fog.color", glm::vec4( 0.65f, 0.77f, 1.0f, 1.0f ) );
    shader->setUniform( "fog.minDistance", 50.0f );
    shader->setUniform( "fog.maxDistance", 800.0f );
    
    // scales
    shader->setUniform( "horizontalScale", m_horizontalScale );
    shader->setUniform( "verticalScale", m_verticalScale );
    shader->setUniform( "pixelsPerTriangleEdge", 12.0f );
    shader->setUniform( "maxTrianglesPerTexel", (int)1);
    shader->setUniform( "viewportSize", m_viewportSize);
    
    // Pass in the usual transformation matrices
    glm::mat4 viewMatrix = glm::make_mat4(MV);
    //viewMatrix = glm::transpose(viewMatrix);
    glm::mat4 projMatrix = glm::make_mat4(P);
    //projMatrix = glm::transpose(projMatrix);

    glm::mat4 modelViewMatrix = viewMatrix * m_modelMatrix;
    glm::mat3 worldNormalMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));
    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
    glm::mat4 mvp = projMatrix * modelViewMatrix;

    shader->setUniform( "modelMatrix", m_modelMatrix );
    shader->setUniform( "modelViewMatrix", modelViewMatrix );
    shader->setUniform( "worldNormalMatrix", worldNormalMatrix );
    shader->setUniform( "normalMatrix", normalMatrix );
    shader->setUniform( "mvp", mvp );
    shader->setUniform( "viewportMatrix", m_viewportMatrix );
    
    // Set the lighting parameters
    glm::vec4 worldLightDirection( 0.50000019433, 0.86602529158, 0.0f, 0.0f ); //sin(30), cos(30)
    glm::mat4 worldToEyeNormal( normalMatrix );
    glm::vec4 lightDirection = worldToEyeNormal * worldLightDirection;
    shader->setUniform( "light.position", lightDirection );
    shader->setUniform( "light.intensity", glm::vec3( 1.0f, 1.0f, 1.0f ) );
    
    // Set the material properties
    shader->setUniform( "material.Ka",  glm::vec3( 0.1f, 0.1f, 0.1f ) );
    shader->setUniform( "material.Kd",  glm::vec3( 1.0f, 1.0f, 1.0f ) );
    shader->setUniform( "material.Ks",  glm::vec3( 0.3f, 0.3f, 0.3f ) );
    shader->setUniform( "material.shininess", 10.0f );
    
    shader->setUniform( "colorStop1", 0.0f );
    shader->setUniform( "colorStop2", float(m_verticalScale[0] + 0.25*m_verticalScale[1]) );
    shader->setUniform( "colorStop3", float(m_verticalScale[0] + 0.5*m_verticalScale[1]) );
    shader->setUniform( "colorStop4", float(m_verticalScale[0] + 0.75*m_verticalScale[1]) );
    shader->setUniform( "colorStop5", float(m_verticalScale[0] + m_verticalScale[1]) );
    
    // draw
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBindVertexArray(m_vao);
    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays( GL_PATCHES, 0, m_patchCount );
    glBindVertexArray(0);
    
    // clean up
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
}

void TessTerrain::calViewportMatrix(int width, int height) {
    
    m_viewportSize = glm::vec2(width, height);
    
    float w2 = width / 2.0f;
    float h2 = height / 2.0f;
    m_viewportMatrix = glm::mat4(1.0);
    m_viewportMatrix[0] = glm::vec4(w2, 0.0f, 0.0f, 0.0f);
    m_viewportMatrix[1] = glm::vec4(0.0f, h2, 0.0f, 0.0f);
    m_viewportMatrix[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    m_viewportMatrix[3] = glm::vec4(w2, h2, 0.0f, 1.0f);
}

}; //namespace tessterrain

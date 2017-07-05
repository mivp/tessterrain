#include "Mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <float.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

namespace tessterrain {
    
    /**
     Mesh Terrain
     */
    Mesh::Mesh(): m_vbo(0), m_vao(0), m_ibo(0),
    m_initialized(false), m_wireframe(false), m_material(0) {
        vertexes.clear();
        indices.clear();
        m_modelMatrix = glm::mat4(1.0);
    }
    
    Mesh::~Mesh() {
        if(m_vao > 0) {
            glDeleteVertexArrays(1,&m_vao);
            glDeleteBuffers(1,&m_vbo);
            glDeleteBuffers(1,&m_ibo);
        }
        if(m_material)
            delete m_material;
        vertexes.clear();
        indices.clear();
    }
    
    void Mesh::setup(){
        
        if(vertexes.size() < 1)
            return;
        
        if(!m_material)
            m_material = new MeshMaterial();
        
        m_material->shininess = 10.0f;
        m_material->Ka = glm::vec3(0.2, 0.2, 0.2);
        m_material->Kd = glm::vec3(1.0, 1.0, 1.0);
        m_material->Ks = glm::vec3(0.7, 0.7, 0.7);
        
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        // create vbo
        glGenBuffers(1,&m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);
        
        GLSLProgram* shader = m_material->getShader();
        //shader->bind();
        unsigned int val;
        
        val = glGetAttribLocation(shader->getHandle(), "inPosition");
        glEnableVertexAttribArray(val);
        glVertexAttribPointer( val,  3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        
        val = glGetAttribLocation(shader->getHandle(), "inNormal");
        glEnableVertexAttribArray(val);
        glVertexAttribPointer( val,  3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        
        val = glGetAttribLocation(shader->getHandle(), "inTexCoord");
        glEnableVertexAttribArray(val);
        glVertexAttribPointer( val,  2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        
        // create ibo
        glGenBuffers(1,&m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
        
        glBindVertexArray(0);
        
    }
    
    void Mesh::moveTo(glm::vec3 pos) {
        m_modelMatrix = glm::mat4(1.0);
        m_modelMatrix = glm::translate(m_modelMatrix, pos);
    }
    
    void Mesh::render(const float MV[16], const float P[16]) {
        
        if(!m_vao)
            setup();
        
        glEnable(GL_DEPTH_TEST);
        
        GLSLProgram* shader = m_material->getShader();
        shader->bind();
        //glm::mat4 viewMatrix = cam->view;
        //glm::mat4 projMatrix = cam->projection;
        glm::mat4 viewMatrix = glm::make_mat4(MV);
        glm::mat4 projMatrix = glm::make_mat4(P);
        glm::mat4 modelViewMatrix = viewMatrix * m_modelMatrix;
        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
        glm::mat4 mvp = projMatrix * modelViewMatrix;
        
        shader->setUniform("uModelView", modelViewMatrix);
        shader->setUniform("uMVP", mvp);
        shader->setUniform("uNormalMatrix", normalMatrix);
        
        // Set the lighting parameters
        glm::vec4 worldLightDirection( 0.50000019433, 0.86602529158, 0.0f, 0.0f ); //sin(30), cos(30)
        glm::mat4 worldToEyeNormal( normalMatrix );
        glm::vec4 lightDirection = worldToEyeNormal * worldLightDirection;
        shader->setUniform( "light.position", lightDirection );
        shader->setUniform( "light.intensity", glm::vec3( 1.0f, 1.0f, 1.0f ) );
        
        // Set the material properties
        shader->setUniform( "material.Ka",  m_material->Ka );
        shader->setUniform( "material.Kd",  m_material->Kd );
        shader->setUniform( "material.Ks",  m_material->Ks );
        shader->setUniform( "material.shininess", m_material->shininess );
        
        if(m_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        
        glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0 );
        
        if(m_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glBindVertexArray(0);
    }
    
    
    Mesh* MeshUtils::cube(float size) {
        Mesh* cube = new Mesh();
        
        float size2 = size / 2.0f;
        
        float v[24*3] = {
            // Front
            -size2, -size2, size2,
            size2, -size2, size2,
            size2,  size2, size2,
            -size2,  size2, size2,
            // Right
            size2, -size2, size2,
            size2, -size2, -size2,
            size2,  size2, -size2,
            size2,  size2, size2,
            // Back
            -size2, -size2, -size2,
            -size2,  size2, -size2,
            size2,  size2, -size2,
            size2, -size2, -size2,
            // Left
            -size2, -size2, size2,
            -size2,  size2, size2,
            -size2,  size2, -size2,
            -size2, -size2, -size2,
            // Bottom
            -size2, -size2, size2,
            -size2, -size2, -size2,
            size2, -size2, -size2,
            size2, -size2, size2,
            // Top
            -size2,  size2, size2,
            size2,  size2, size2,
            size2,  size2, -size2,
            -size2,  size2, -size2
        };
        
        float n[24*3] = {
            // Front
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            // Right
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            // Back
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            // Left
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            // Bottom
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            // Top
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };
        
        float tex[24*2] = {
            // Front
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            // Right
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            // Back
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            // Left
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            // Bottom
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            // Top
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };
        
        int el[] = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23
        };
        
        for(int i=0; i < 24; i++) {
            glm::vec3 position = glm::vec3(v[3*i], v[3*i+1], v[3*i+2]);
            glm::vec3 normal = glm::vec3(n[3*i], n[3*i+1], n[3*i+2]);
            glm::vec2 uv = glm::vec2(tex[2*i], tex[2*i+1]);
            cube->vertexes.push_back(Vertex(position, normal, uv));
        }
        
        for(int i=0; i < 36; i++)
            cube->indices.push_back(el[i]);
        
        return cube;
    }
    
    // SPHERE
#ifndef PI
#define PI 3.14159265359
#endif
    void generateSphereVerts(float * verts, float * norms, float * tex,
                             int * el, float radius, int slices, int stacks)
    {
        // Generate positions and normals
        GLfloat theta, phi;
        GLfloat thetaFac = (2.0 * PI) / slices;
        GLfloat phiFac = PI / stacks;
        GLfloat nx, ny, nz, s, t;
        GLuint idx = 0, tIdx = 0;
        for( int i = 0; i <= slices; i++ ) {
            theta = i * thetaFac;
            s = (GLfloat)i / slices;
            for( int j = 0; j <= stacks; j++ ) {
                phi = j * phiFac;
                t = (GLfloat)j / stacks;
                nx = sinf(phi) * cosf(theta);
                ny = sinf(phi) * sinf(theta);
                nz = cosf(phi);
                verts[idx] = radius * nx; verts[idx+1] = radius * ny; verts[idx+2] = radius * nz;
                norms[idx] = nx; norms[idx+1] = ny; norms[idx+2] = nz;
                idx += 3;
                
                tex[tIdx] = s;
                tex[tIdx+1] = t;
                tIdx += 2;
            }
        }
        
        // Generate the element list
        idx = 0;
        for( int i = 0; i < slices; i++ ) {
            GLuint stackStart = i * (stacks + 1);
            GLuint nextStackStart = (i+1) * (stacks+1);
            for( int j = 0; j < stacks; j++ ) {
                if( j == 0 ) {
                    el[idx] = stackStart;
                    el[idx+1] = stackStart + 1;
                    el[idx+2] = nextStackStart + 1;
                    idx += 3;
                } else if( j == stacks - 1) {
                    el[idx] = stackStart + j;
                    el[idx+1] = stackStart + j + 1;
                    el[idx+2] = nextStackStart + j;
                    idx += 3;
                } else {
                    el[idx] = stackStart + j;
                    el[idx+1] = stackStart + j + 1;
                    el[idx+2] = nextStackStart + j + 1;
                    el[idx+3] = nextStackStart + j;
                    el[idx+4] = stackStart + j;
                    el[idx+5] = nextStackStart + j + 1;
                    idx += 6;
                }
            }
        }
    }
    
    Mesh* MeshUtils::sphere(float radius, int slices, int stacks) {
        
        Mesh* sphere = new Mesh();
        
        int nVerts = (slices+1) * (stacks + 1);
        int elements = (slices * 2 * (stacks-1) ) * 3;
        
        // Verts
        float * v = new float[3 * nVerts];
        // Normals
        float * n = new float[3 * nVerts];
        // Tex coords
        float * tex = new float[2 * nVerts];
        // Elements
        int * el = new int[elements];
        
        // Generate the vertex data
        generateSphereVerts(v, n, tex, el, radius, slices, stacks);
        
        for(int i=0; i < nVerts; i++) {
            glm::vec3 position = glm::vec3(v[3*i], v[3*i+1], v[3*i+2]);
            glm::vec3 normal = glm::vec3(n[3*i], n[3*i+1], n[3*i+2]);
            glm::vec2 uv = glm::vec2(tex[2*i], tex[2*i+1]);
            sphere->vertexes.push_back(Vertex(position, normal, uv));
        }
        
        for(int i=0; i < elements; i++)
            sphere->indices.push_back(el[i]);
        
        return sphere;
    }
    
}; //namespace tessterrain

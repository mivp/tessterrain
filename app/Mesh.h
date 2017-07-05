#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/rotate_vector.hpp>

using std::vector;
using std::string;

#include "Camera.h"
#include "../Material.h"

namespace tessterrain {
    
    struct Vertex
    {
        glm::vec3 position; // where each dot is located
        glm::vec3 normal; // the normal for this dot
        glm::vec2 uv; // uvs for texture coordinate mapping
        Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) {
            position = p; normal = n; uv = t;
        }
    };
    
    /**
     Create full mesh from heighmap
     */
    class Mesh {
        
    private:
        unsigned int m_vbo;
        unsigned int m_ibo;
        unsigned int m_vao;
        
        glm::mat4 m_modelMatrix;
        
        bool m_initialized;
        
        
    public:
        // mesh data
        vector<Vertex> vertexes;
        vector<unsigned int> indices;
        
        // display
        bool m_wireframe;
        Material* m_material;
        
    private:
        void setup();
        
    public:
        Mesh();
        ~Mesh();
        
        void moveTo(glm::vec3 pos);
        void render(const float MV[16], const float P[16]);
    };
    
    /**
     Create meshes
     */
    class MeshUtils {
        
    public:
        static Mesh* cube(float size);
        static Mesh* sphere(float radius, int slices, int stacks);
    };
    
}; //namespace tessterrain

#endif

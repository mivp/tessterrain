#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/rotate_vector.hpp>

using std::vector;
using std::string;

#include "Material.h"
#include "Texture.h"

#ifndef OMEGALIB_MODULE
#include "Camera.h"
#endif

namespace tessterrain {

class Terrain {

protected:
    //terrain
    vector<string> m_files;     // highmap texture
	glm::vec2 m_lefttop;
    glm::vec2 m_size;
    glm::vec2 m_horizontalRes;
	glm::vec2 m_heightRange;
    bool m_initialized;
    
    //model
    glm::mat4 m_modelMatrix;

    //material
    Material* m_material;
    
public:
	Terrain();
	~Terrain();

    void init(string configfile);
    void printInfo();
    
    virtual void setup() = 0;
#ifndef OMEGALIB_MODULE
    virtual void render(Camera* cam) = 0;
#else
    virtual void render(const float MV[16], const float P[16]) = 0;
#endif
};

/** 
Create full mesh from heighmap
*/
class MeshTerrain: public Terrain {

protected:
    struct Vertex
    {
        glm::vec3 position; // where each dot is located
        glm::vec3 normal; // the normal for this dot
        glm::vec2 uv; // uvs for texture coordinate mapping
        Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) {
            position = p; normal = n; uv = t;
        }
    };

    float m_geoTrans[6];
    vector< vector<float> > m_heightmap;
    bool m_dataloaded;

    // mesh data
    vector<Vertex> vertexes;
    vector<unsigned int> indices;
    
    unsigned int m_vbo;
    unsigned int m_ibo;
    unsigned int m_vao;

    bool m_wireframe;
    
protected:
    glm::vec3 computeNormal(glm::vec3 center, int i, int j);
    
public:
    MeshTerrain();
    ~MeshTerrain();
    
    int loadHeightMap();
    int createMesh();
    int saveMeshToObj(string filename);
    void toggleWireFrame() { m_wireframe = !m_wireframe; }
    
    virtual void setup();
#ifndef OMEGALIB_MODULE
    virtual void render(Camera* cam);
#else
    virtual void render(const float MV[16], const float P[16]);
#endif
};

/**
 Tessellation terrain
 */
class TessTerrain: public Terrain {
    
protected:
    Texture* m_texHightmap;
    Texture* m_texGrass;
    Texture* m_texRock;
    Texture* m_texSnow;
    
    int m_patchCount;
    unsigned int m_vbo;
    unsigned int m_vao;
    
    bool m_initialized;
    
    vector<string> m_displayModeNames;
    vector<unsigned int> m_displayModeSubroutines;
    int m_displayMode;
    
    glm::vec2 m_viewportSize;
    glm::mat4 m_viewportMatrix;
    
public:
    enum DisplayMode {
        SimpleWireFrame = 0,
        WorldHeight,
        WorldHeightWireFrame,
        WorldNormals,
        Grass,
        GrassAndRocks,
        GrassRocksAndSnow,
        LightingFactors,
        TexturedAndLit,
        DisplayModeCount
    };
    
public:
    TessTerrain();
    ~TessTerrain();
    
    void calViewportMatrix(int width, int height);
    void nextDisplayMode(bool forward=true);
    
    virtual void setup();
#ifndef OMEGALIB_MODULE
    virtual void render(Camera* cam);
#else
    virtual void render(const float MV[16], const float P[16]);
#endif
};

}; //namespace tessterrain

#endif

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/rotate_vector.hpp>

using std::vector;
using std::string;

#include "Material.h"
#include "Texture.h"

#ifndef OMEGALIB_MODULE
#include "Camera.h"
#endif

namespace tessterrain {

/**
 Tessellation terrain
 */
class TessTerrain {
    
private:
    //terrain
    vector<string> m_files;     // highmap texture
    glm::vec2 m_size;
    glm::vec2 m_horizontalRes;
    glm::vec2 m_horizontalScale;// x scale, z scale
    glm::vec2 m_heightRange;    // (min, max)
    glm::vec2 m_verticalScale;  // (min height, max - min height)
    bool m_initialized;
    
    //heightmap
    Texture* m_texHightmap;
    
    //matrix
    glm::mat4 m_modelMatrix;
    glm::vec2 m_viewportSize;
    glm::mat4 m_viewportMatrix;
    
    //material
    Material* m_material;
    Texture* m_texture;
    
    int m_patchCount;
    unsigned int m_vbo;
    unsigned int m_vao;
    
    vector<string> m_displayModeNames;
    vector<unsigned int> m_displayModeSubroutines;
    int m_displayMode;
    
public:
    enum DisplayMode {
        SimpleWireFrame = 0,
        WorldHeight,
        WorldHeightWireFrame,
        WorldNormals,
        LightingFactor,
        Textured,
        TexturedAndLit,
        DisplayModeCount
    };
    
public:
    TessTerrain();
    ~TessTerrain();
    
    void init(string configfile);
    void printInfo();
    
    void calViewportMatrix(int width, int height);
    void nextDisplayMode(bool forward=true);
    void moveTo(glm::vec3 pos);
    void moveTo(float x, float y, float z) { moveTo(glm::vec3(x, y, z)); }
    
    void setup();
#ifndef OMEGALIB_MODULE
    void render(Camera* cam);
#else
    void render(const float MV[16], const float P[16]);
#endif
};

}; //namespace tessterrain

#endif

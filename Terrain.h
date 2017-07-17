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

namespace tessterrain {
    
    struct TerrainInfo {
        glm::dvec2 offset;
        glm::vec2 heightRange;
        glm::vec2 res;  // hres wres
        float bbox[6];
        string name;
        string terrain;
        string texture;
        string overlay;
    };
    
    enum LoadState {
        STATE_NONE = 0,
        STATE_INQUEUE,
        STATE_LOADING,
        STATE_UNLOADING,
        STATE_LOADED
    };
    
    /**
     Texture pool - to avoid recreating textures
     */
    struct TerrainTexture {
        Texture* heightmap;
        Texture* texture;
        Texture* overlay;
        TerrainTexture(): heightmap(0), texture(0), overlay(0) { }
        ~TerrainTexture() {
            if (heightmap) delete heightmap;
            if (texture) delete texture;
            if (overlay) delete overlay;
        }
    };
    
    /**
     Tessellation terrain
     */
    class TessTerrain {
        
    private:
        //terrain
        TerrainInfo m_info;
        glm::vec3 m_globalHeightRange;
        
        LoadState m_loadState;
        
        //original data read from ini file
        glm::vec2 m_size;
        glm::vec2 m_fogRange;
        bool m_initialized;
        float m_heightRangeScale;
        glm::vec2 m_horizontalScale;
        
        //to pass to shaders
        glm::vec2 m_heightRange;
        glm::vec2 m_verticalScale;
        
        //texture
        TerrainTexture* m_terrainTexture;
        
        //matrix
        glm::mat4 m_modelMatrix;
        glm::vec2 m_viewportSize;
        glm::mat4 m_viewportMatrix;
        
        //material
        Material* m_material;
        float m_overlayAlpha;
        bool m_fog;
        bool m_reload;
        
        int m_patchCount;
        unsigned int m_vbo;
        unsigned int m_vao;
        
        vector<string> m_displayModeNames;
        vector<unsigned int> m_displayModeSubroutines;
        int m_displayMode;
        
        //quad
        unsigned int m_circleVao;
        unsigned int m_circleVao2;
        Material* m_circleMaterial;
        int m_numCircle;
        
        //distance to camera;
        float m_distToCam;
        
    public:
        enum DisplayMode {
            SimpleWireFrame = 0,
            WorldHeight,
            WorldHeightWireFrame,
            WorldNormals,
            LightingFactor,
            Textured,
            TexturedAndLit,
            TexturedAndOverlay,
            TexturedAndOverlayAndLit,
            DisplayModeCount
        };
        
    private:
        void init();
        void nextDisplayMode(bool forward=true);
        void calCircleVertices(int sx, int sy, float r, vector<glm::vec2> &vertices);
        void setup(bool lowmode = false);

    public:
        TessTerrain(TerrainInfo info, glm::vec3 globalHeightRange);
        ~TessTerrain();
        
        void print();
        
        //
        string getName() { return m_info.name; }
        int getState() { return m_loadState; }
        float* getBBox() { return m_info.bbox; }
        void loadTextures();
        void initTextures(bool lowmode = false);
        void unloadTextures();
        
        void setState(LoadState state) { m_loadState = state; }
        bool inQueue() { return m_loadState == STATE_INQUEUE; }
        bool canAddToQueue() { return m_loadState == STATE_NONE; }
        bool isLoading() { return m_loadState == STATE_LOADING; }
        bool isLoaded()  { return m_loadState == STATE_LOADED; }
        
        //
        void updateDistanceToCam(glm::vec3 campos);
        float getDistanceToCam() const { return m_distToCam; };
        
        //
        void calViewportMatrix(int width, int height);
        void nextDisplayMode(int num = 1); // position ~ forward, nagative ~ backward
        void moveTo(glm::vec3 pos);
        void moveTo(float x, float y, float z) { moveTo(glm::vec3(x, y, z)); }
        void setHeightScale(float scale);
        void toggleFog() { m_fog = !m_fog; }
        float getOverlayAlpha() { return m_overlayAlpha; }
        void setOverlayAlpha(float a) { m_overlayAlpha = a; if(m_overlayAlpha<0) m_overlayAlpha=0; if(m_overlayAlpha>1) m_overlayAlpha=1;}
        void reloadOverlay();
        
        void render(const float MV[16], const float P[16], bool lowmode = false);
        void renderWithZoom(const float MV[16], const float P[16], const float PZoom[16]);
        void drawCircle(int sx, int sy, float radius);
        void drawCircle2(int sx, int sy, float radius);
    };
    
}; //namespace tessterrain

#endif

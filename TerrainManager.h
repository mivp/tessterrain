#ifndef TERRAIN_MANAGER_H__
#define TERRAIN_MANAGER_H__

#include "Terrain.h"
#include "Thread.h"
#include "wqueue.h"
#include "LRU.h"

#include <vector>
#include <list>

using std::vector;
using std::list;

namespace tessterrain {
    
    class TerrainLoaderThread: public Thread {
    private:
        wqueue<TessTerrain*>& m_queue;
        int maxLoadSize;
        
    public:
        TerrainLoaderThread(wqueue<TessTerrain*>& queue, int m) : m_queue(queue), maxLoadSize(m) {}
        
        void* run() {
            for (;;) {
                TessTerrain* terrain = (TessTerrain*)m_queue.remove();
                if(m_queue.size() < maxLoadSize) {
                    terrain->setState(STATE_LOADING);
                    terrain->loadTextures();
                }
                else {
                    terrain->setState(STATE_NONE);
                }
            }
            return NULL;
        }
    };
    
    
    class TexturePool {
        
    private:
        vector<TerrainTexture* > m_pool;
    
    public:
        TexturePool(int num, bool texture, bool overlay);
        ~TexturePool();
        TerrainTexture* findUnused();
    };
    
    
    /**
     Terrain manager
     */
    class TerrainManager {
    
    public:
        TerrainManager(string inifile);
        ~TerrainManager();
        void print();
        void render(const float MV[16], const float P[16], const float campos[3]);
        
        // call all terrains
        void calViewportMatrix(int width, int height);
        void nextDisplayMode(int num = 1);
        void setHeightScale(float scale);
        void setHeight(float height);
        void toggleFog();
        float getOverlayAlpha();
        void setOverlayAlpha(float a);
        void reloadOverlay();
        void setOpacity(float o);
        float getOpacity();
        
        //
        int updateVisibility(const float MVP[16], const float campos[3]);
        
    private:
        string strTrim(const std::string &s);
        vector<string> strSplit(const std::string &s);
        std::string strReplace(const std::string &str, const std::string& from, const std::string& to);
        
    private:
        glm::dvec2 m_refPoint;      // lat, lon
        glm::vec2 m_sizeScale;      // x, y
        glm::vec3 m_globalHeightRange; // min, max, scale
        string m_terrainDir;
        string m_textureDir;
        string m_overlayDir;
        vector<TerrainInfo> m_terrainsInfo;
        
        vector<TessTerrain*> m_terrains;
        list<TessTerrain*> m_displayList;
        int m_numVisibleTerrain;
        
        // loader threads
        int m_preloadAll;
        int m_maxTerrainDisplay;
        int m_maxTerrainInMem;
        wqueue<TessTerrain*>  m_terrainQueue;
        std::list<TerrainLoaderThread*> m_terrainLoaderThreads;
        int m_numLoaderThreads;
        // cache
        LRUCache* m_lruCache;
        //
        glm::vec3 m_prevCamPosForSort;
        glm::vec3 m_prevCamPos;
        unsigned int m_prevTime;
        unsigned int m_idleTime;
    };
    
}; // namespace tessterrain

#endif

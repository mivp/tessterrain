#include "TerrainManager.h"
#include "INIReader.h"
#include "Utils.h"

#include <iostream>
#include <assert.h>
#include <sstream>
#include <iomanip>
using namespace std;

namespace tessterrain {
    
    TerrainManager::TerrainManager(string inifile): m_numVisibleTerrain(0), m_lruCache(0), m_prevCamPos(glm::vec3(0)), m_idleTime(0), m_prevTime(0) {
        cout << "Config file: " << inifile << endl;
        
        INIReader reader(inifile);
        
        string str;
        
        // terrains
        str = reader.Get("general", "terrains", "");
        assert(str.length());
        
        // res
        m_res = glm::vec2(reader.GetReal("general", "wres", 1), reader.GetReal("general", "hres", 1));
        assert(m_res[0] > 0 && m_res[1] > 0);
        
        // ref point
        m_refPoint = glm::dvec2(reader.GetReal("general", "refLat", 0), reader.GetReal("general", "refLon", 0));
        
        // max number of terrains can be loaded
	m_preloadAll = reader.GetInteger("general", "preloadAll", 0);
        m_maxTerrainDisplay = reader.GetInteger("general", "maxTerrainDisplay", 20);
        m_maxTerrainInMem = reader.GetInteger("general", "maxTerrainInMem", 30);
        // loader thread
        m_numLoaderThreads = reader.GetInteger("general", "numLoaderThreads", 2);
        // bbox enlarge factor
        float bboxEnlargeFactor = reader.GetReal("general", "bboxEnlargeFactor", 0);
        
        // dirs
        m_terrainDir = reader.Get("general", "terrainDir", "");
        string terrainFile = reader.Get("general", "terrainFile", "");
        
        m_textureDir = reader.Get("general", "textureDir", "");
        string textureFile = reader.Get("general", "textureFile", "");
        
        m_overlayDir = reader.Get("general", "overlaydir", "");
        string overlayFile = reader.Get("general", "overlayFile", "");
        
        vector<string> terrains = strSplit(str);
        m_globalHeightRange = glm::vec3(FLT_MAX, -FLT_MAX, 1);
        for (int i=0; i < terrains.size(); i++) {
            TerrainInfo info;
            info.name = terrains[i];
            
            info.terrain = m_terrainDir;
            info.terrain.append("/");
            info.terrain.append(strReplace( terrainFile, "%s", info.name));
            
            info.texture = "";
            if (reader.GetInteger("general", "texture", 0) > 0) {
                info.texture = m_textureDir;
                info.texture.append("/");
                info.texture.append(strReplace( textureFile, "%s", info.name));
            }
            
            info.overlay = "";
            if (reader.GetInteger("general", "overlay", 0) > 0) {
                info.overlay = m_overlayDir;
                info.overlay.append("/");
                info.overlay.append(strReplace( overlayFile, "%s", info.name));
            }
            
            // res
            info.res = m_res;
            
            // heightRange
            str = reader.Get(info.name, "heightRange", "0, 1");
            vector<string> hrange = strSplit(str);
            info.heightRange = glm::vec2(::atof(hrange[0].c_str()), ::atof(hrange[1].c_str()));
            if ( m_globalHeightRange[0] > info.heightRange[0])
                m_globalHeightRange[0] = info.heightRange[0];
            if ( m_globalHeightRange[1] < info.heightRange[1])
                m_globalHeightRange[1] = info.heightRange[1];
            m_globalHeightRange[2] = reader.GetReal("general", "heightRangeScale", 1);
            
            // calculate offset and bounding box
            str = reader.Get(info.name, "pos", "");
            vector<string> pstr = strSplit(str);
            double lat = ::atof(pstr[0].c_str());
            double lon = ::atof(pstr[1].c_str());
            double x = (lon - m_refPoint[1]) * 3647 * m_res[1] / 1.0002777999999921;
            double z = (m_refPoint[0] - lat) * 3647 * m_res[0] / 1.0002777999999921;
            info.offset = glm::dvec2(x, z);
            
            lat = ::atof(pstr[2].c_str());
            lon = ::atof(pstr[3].c_str());
            x = (lon - m_refPoint[1]) * 3647 * m_res[1] / 1.0002777999999921;
            z = (m_refPoint[0] - lat) * 3647 * m_res[0] / 1.0002777999999921;
            info.bbox[0] = info.offset[0];
            info.bbox[1] = 0;
            info.bbox[2] = info.offset[1];
            info.bbox[3] = x;
            info.bbox[4] = 1000;
            info.bbox[5] = z;
            float w2 = (info.bbox[3] - info.bbox[0])/2;
            float h2 = (info.bbox[5] - info.bbox[2])/2;
            info.bbox[0] -= w2 * bboxEnlargeFactor;
            info.bbox[3] += w2 * bboxEnlargeFactor;
            info.bbox[2] -= h2 * bboxEnlargeFactor;
            info.bbox[5] += h2 * bboxEnlargeFactor;
            
            m_terrainsInfo.push_back(info);
        }
        
        for (int i = 0; i < m_terrainsInfo.size(); i++) {
            TessTerrain* terrain = new TessTerrain(m_terrainsInfo[i], m_globalHeightRange);
            m_terrains.push_back(terrain);
        }
        
        // loader
        if(m_terrainLoaderThreads.size() == 0) {
            for(int i = 0; i < m_numLoaderThreads; i++) {
                TerrainLoaderThread* t = new TerrainLoaderThread(m_terrainQueue, m_maxTerrainDisplay); //option->maxLoadSize);
                t->start();
                m_terrainLoaderThreads.push_back(t);
            }
        }
        
        // LRUCache
        if (!m_lruCache)
            m_lruCache = new LRUCache(m_maxTerrainInMem, 0);
    }
    
    TerrainManager::~TerrainManager() {
        m_lruCache->clear();
        m_displayList.clear();
        
        for(int i=0; i < m_terrains.size(); i++)
            delete m_terrains[i];
    }
    
    string TerrainManager::strTrim(const std::string &s) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && isspace(*it))
            it++;
        
        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && isspace(*rit))
            rit++;
        
        return std::string(it, rit.base());
    }
    
    vector<string> TerrainManager::strSplit(const std::string &str) {
        vector<string> arr;
        istringstream ss(str);
        string s;
        while (getline(ss, s, ',')) {
            s = strTrim(s);
            if (s.length() == 0)
                continue;
            arr.push_back(s);
        }
        return arr;
    }
    
    std::string TerrainManager::strReplace(const std::string &str, const std::string& from, const std::string& to) {
        string result = str;
        size_t start_pos = result.find(from, 0);
        if (start_pos != std::string::npos)
            result.replace(start_pos, from.length(), to);
        return result;
    }
    
    void TerrainManager::print() {
        cout << "res: " << m_res[0] << " " << m_res[1] << endl;
        cout << std::setprecision(10) << "ref point: " << m_refPoint[0] << " " << m_refPoint[1] << endl;
        cout << "terrain dir: " << m_terrainDir << endl;
        cout << "texture dir: " << m_textureDir << endl;
        cout << "overlay dir: " << m_overlayDir << endl;
        cout << "global heightRange: " << m_globalHeightRange[0] << " " << m_globalHeightRange[1] << endl;
        for(int i=0; i < m_terrainsInfo.size(); i++) {
            cout << endl << "======= " << m_terrainsInfo[i].name << " =====" << endl;
            cout << "terrain: " << m_terrainsInfo[i].terrain << endl;
            cout << "texture: " << m_terrainsInfo[i].texture << endl;
            cout << "overlay: " << m_terrainsInfo[i].overlay << endl;
            cout << "offset: " << m_terrainsInfo[i].offset[0] << " " << m_terrainsInfo[i].offset[1] << endl;
            cout << "heightRange: " << m_terrainsInfo[i].heightRange[0] << " " << m_terrainsInfo[i].heightRange[1] << endl;
            cout << "bbox: ";
            for (int j=0; j < 6; j++)
                cout << m_terrainsInfo[i].bbox[j] << " ";
            cout << endl;
        }
    }
    
    void TerrainManager::calViewportMatrix(int width, int height) {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->calViewportMatrix(width, height);
        }
    }
    
    void TerrainManager::nextDisplayMode(int num) {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->nextDisplayMode(num);
        }
    }
    
    void TerrainManager::setHeightScale(float scale) {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->setHeightScale(scale);
        }
    }
    
    void TerrainManager::toggleFog() {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->toggleFog();
        }
    }
    
    float TerrainManager::getOverlayAlpha() {
        if (m_terrains.size())
            return m_terrains[0]->getOverlayAlpha();
        return 0;
    }
    
    void TerrainManager::setOverlayAlpha(float a) {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->setOverlayAlpha(a);
        }
    }
    
    void TerrainManager::reloadOverlay() {
        for(int i=0; i < m_terrains.size(); i++) {
            m_terrains[i]->reloadOverlay();
        }
    }

    
    struct {
        bool operator()(const TessTerrain* a, const TessTerrain* b) const
        {
            return a->getDistanceToCam() < b->getDistanceToCam();
        }
    } customLess;
    
    // update list of visible terrain
    int TerrainManager::updateVisibility(const float MVP[16], const float campos[3]) {

        if (m_preloadAll == 1) {
            for(int i=0; i < m_terrains.size(); i++) {
                m_terrains[i]->loadTextures();
                m_terrains[i]->initTextures();
                m_displayList.push_back(m_terrains[i]);
            }
            m_preloadAll = -1;
            return 0;
        }
        
        if (m_preloadAll != 0)
            return 0;

        m_displayList.clear();
        
        float V[6][4];
        Utils::getFrustum(V, MVP);

        glm::vec3 curCamPos = glm::vec3(campos[0], campos[1], campos[2]);
        if (glm::length(m_prevCamPosForSort - curCamPos) > 3000) {
            for(int i=0; i < m_terrains.size(); i++)
                m_terrains[i]->updateDistanceToCam(curCamPos);
            
            std::sort(m_terrains.begin(), m_terrains.end(), customLess);
            m_prevCamPosForSort = curCamPos;
            //cout << "resorted" << endl;
        }

        for(int i=0; i < m_terrains.size(); i++) {
            TessTerrain* t = m_terrains[i];
            
            bool visible = false;
            if(Utils::testFrustum(V, t->getBBox()) >= 0 )
                visible = true;
            
            if(!visible)
                continue;
            
            if(t->canAddToQueue() ) {
                t->setState(STATE_INQUEUE);
                m_terrainQueue.add(t);
            }
            
            m_displayList.push_back(t);
            m_lruCache->insert(t->getName(), t);
            
            if (m_displayList.size() >= m_maxTerrainDisplay) {
                //cout << "WARNING: numVisible >= m_maxTerrainInMem" << endl;
                break;
            }

        }
        if (m_numVisibleTerrain != m_displayList.size()) {
            m_numVisibleTerrain = m_displayList.size();
            //cout << "# visible terrains: " << m_numVisibleTerrain << endl;
        }
        return 0;
    }
    
    void TerrainManager::render(const float MV[16], const float P[16], const float campos[3]) {

        bool lowmode = true;
        glm::vec3 curCamPos = glm::vec3(campos[0], campos[1], campos[2]);
        
        if (m_preloadAll) {
            lowmode = false;
        }
        else {
            if (glm::length(m_prevCamPos - curCamPos) > 10 || m_prevTime == 0) {
                m_prevTime = Utils::getTime();
                m_idleTime = 0;
            }
            else {
                m_idleTime += Utils::getTime() - m_prevTime;
                if(m_idleTime > 1000)
                    lowmode = false;
            }
        }
 
        m_prevCamPos = curCamPos;
        
        if(lowmode) {
            for(list<TessTerrain*>::iterator it = m_displayList.begin(); it != m_displayList.end(); it++) {
                TessTerrain* terrain = *it;
                terrain->render(MV, P, !m_preloadAll, true);
            }
        }
        else {
            int ind = 0, size2 = m_displayList.size() / 2;
            for(list<TessTerrain*>::iterator it = m_displayList.begin(); it != m_displayList.end(); it++) {
                TessTerrain* terrain = *it;
                if(ind < size2)
                    terrain->render(MV, P, !m_preloadAll, false);
                else
                    terrain->render(MV, P, !m_preloadAll, true);
                ind++;
            }
        }
        
    }
    
}; // namespace tessterrain

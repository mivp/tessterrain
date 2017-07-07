#include "TerrainManager.h"
#include "INIReader.h"

#include <iostream>
#include <assert.h>
#include <sstream>
#include <iomanip>
using namespace std;

namespace tessterrain {
    
    // =============== Utils ===============
    int Utils::testPlane( const float V[4], const float b[6])
    {
        const float k00 = b[0] * V[0];
        const float k11 = b[1] * V[1];
        const float k22 = b[2] * V[2];
        const float k30 = b[3] * V[0];
        const float k41 = b[4] * V[1];
        const float k52 = b[5] * V[2];
        
        int c = 0;
        
        // Test all 8 points of the bounding box against this plane.
        
        if (k00 + k11 + k22 + V[3] > 0) c++;
        if (k00 + k11 + k52 + V[3] > 0) c++;
        if (k00 + k41 + k22 + V[3] > 0) c++;
        if (k00 + k41 + k52 + V[3] > 0) c++;
        if (k30 + k11 + k22 + V[3] > 0) c++;
        if (k30 + k11 + k52 + V[3] > 0) c++;
        if (k30 + k41 + k22 + V[3] > 0) c++;
        if (k30 + k41 + k52 + V[3] > 0) c++;
        
        // Return the number of points in front of the plane.
        
        return c;
    }
    
    int Utils::testFrustum(float V[6][4], const float b[6])
    {
        int c0, c1, c2, c3, c4, c5;
        
        // If the bounding box is entirely behind any of the planes, return -1.
        
        if ((c0 = testPlane(V[0], b)) == 0) return -1;
        if ((c1 = testPlane(V[1], b)) == 0) return -1;
        if ((c2 = testPlane(V[2], b)) == 0) return -1;
        if ((c3 = testPlane(V[3], b)) == 0) return -1;
        if ((c4 = testPlane(V[4], b)) == 0) return -1;
        if ((c5 = testPlane(V[5], b)) == 0) return -1;
        
        // If the box is entirely in view, return +1.  If split, return 0.
        
        return (c0 + c1 + c2 + c3 + c4 + c5 == 48) ? 1 : 0;
    }
    
    void Utils::getFrustum(float V[6][4], const float X[16])
    {
        int i;
        
        /* Left plane. */
        
        V[0][0] = X[3]  + X[0];
        V[0][1] = X[7]  + X[4];
        V[0][2] = X[11] + X[8];
        V[0][3] = X[15] + X[12];
        
        /* Right plane. */
        
        V[1][0] = X[3]  - X[0];
        V[1][1] = X[7]  - X[4];
        V[1][2] = X[11] - X[8];
        V[1][3] = X[15] - X[12];
        
        /* Bottom plane. */
        
        V[2][0] = X[3]  + X[1];
        V[2][1] = X[7]  + X[5];
        V[2][2] = X[11] + X[9];
        V[2][3] = X[15] + X[13];
        
        /* Top plane. */
        
        V[3][0] = X[3]  - X[1];
        V[3][1] = X[7]  - X[5];
        V[3][2] = X[11] - X[9];
        V[3][3] = X[15] - X[13];
        
        /* Near plane. */
        
        V[4][0] = X[3]  + X[2];
        V[4][1] = X[7]  + X[6];
        V[4][2] = X[11] + X[10];
        V[4][3] = X[15] + X[14];
        
        /* Far plane. */
        
        V[5][0] = X[3]  - X[2];
        V[5][1] = X[7]  - X[6];
        V[5][2] = X[11] - X[10];
        V[5][3] = X[15] - X[14];
        
        /* Normalize all plane vectors. */
        
        for (i = 0; i < 6; ++i)
        {
            float k = (float) sqrt(DOT3(V[i], V[i]));
            
            V[i][0] /= k;
            V[i][1] /= k;
            V[i][2] /= k;
            V[i][3] /= k;
        }
    }

    
    // =============== TerrainManager ===============
    TerrainManager::TerrainManager(string inifile): m_numVisibleTerrain(0), m_lruCache(0) {
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
        m_maxTerrainInMem = reader.GetInteger("general", "maxTerrainInMem", 20);
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
                TerrainLoaderThread* t = new TerrainLoaderThread(m_terrainQueue, 10); //option->maxLoadSize);
                t->start();
                m_terrainLoaderThreads.push_back(t);
            }
        }
        
        // LRUCache
        if (!m_lruCache)
            m_lruCache = new LRUCache(m_maxTerrainInMem, 2);
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

    
    // update list of visible terrain
    int TerrainManager::updateVisibility(const float MVP[16], const float campos[3]) {
        m_displayList.clear();
        
        float V[6][4];
        Utils::getFrustum(V, MVP);
        
        for(int i=0; i < m_terrains.size(); i++) {
            TessTerrain* t = m_terrains[i];
            
            bool visible = false;
            if(Utils::testFrustum(V, t->getBBox()) >= 0 )
                visible = true;
            
            if(!visible)
                continue;
            
            if(!t->inQueue() && t->canAddToQueue() ) {
                t->setState(STATE_INQUEUE);
                m_terrainQueue.add(t);
                //cout << "add " << t->getName() << " to queue with state: " << t->getState() << endl;
                //cout << "lru size: " << m_lruCache->size() << endl;
            }
            
            m_displayList.push_back(t);
            m_lruCache->insert(t->getName(), t);
        }
        if (m_numVisibleTerrain != m_displayList.size()) {
            m_numVisibleTerrain = m_displayList.size();
            //cout << "# visible terrains: " << m_numVisibleTerrain << endl;
        }
        return 0;
    }
    
    void TerrainManager::render(const float MV[16], const float P[16]) {
        
        for(list<TessTerrain*>::iterator it = m_displayList.begin(); it != m_displayList.end(); it++) {
            TessTerrain* terrain = *it;
            terrain->render(MV, P);
        }
    }
    
}; // namespace tessterrain

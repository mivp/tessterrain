#include <omega.h>
#include <omegaGl.h>
#include <iostream>
#include <vector>

#include "Terrain.h"

using namespace std;
using namespace omega;

///////////////////////////////////////////////////////////////////////////////
class TessTerrainRenderModule : public EngineModule
{
public:
    TessTerrainRenderModule() :
        EngineModule("TessTerrainRenderModule"), visible(true), updateviewport(false)
    {
    	terrains.clear();
    }

    virtual void initializeRenderer(Renderer* r);

    virtual void update(const UpdateContext& context)
    {
        // After a frame all render passes had a chance to update their
        // textures. reset the raster update flag.       
    }
    
    virtual void dispose()
    {
        for(int i=0; i < terrains.size(); i++)
            delete terrains[i];
    }

    void addTerrain(const string& option_file)
    {
        tessterrain::TessTerrain* terrain = new tessterrain::TessTerrain();
        terrain->init(option_file);
        terrains.push_back(terrain);
    }

    void printInfo()
    {
        for(int i=0; i < terrains.size(); i++)
            terrains[i]->printInfo();
    }

    void nextDisplayMode()
    {  
        for(int i=0; i < terrains.size(); i++)
            terrains[i]->nextDisplayMode();
    }

    void moveTo(const int index, const float x, const float y, const float z)
    {
        if(index < 0 || index >= terrains.size())
            return;
        terrains[index]->moveTo(x, y, z);
    }

    void setHeightScale(const float scale)
    {
        for(int i=0; i < terrains.size(); i++)
            terrains[i]->setHeightScale(scale);
    }

    void toggleFog()
    {
        for(int i=0; i < terrains.size(); i++)
            terrains[i]->toggleFog();
    }

    vector<tessterrain::TessTerrain*> terrains;
    bool visible;
    bool updateviewport;
};

///////////////////////////////////////////////////////////////////////////////
class TessTerrainRenderPass : public RenderPass
{
public:
    TessTerrainRenderPass(Renderer* client, TessTerrainRenderModule* prm) : 
        RenderPass(client, "TessTerrainRenderPass"), 
        module(prm) {}
    
    virtual void initialize()
    {
        RenderPass::initialize();
    }

    virtual void render(Renderer* client, const DrawContext& context)
    {
    	if(context.task == DrawContext::SceneDrawTask)
        {
            glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
            client->getRenderer()->beginDraw3D(context);
	
    	    if(module->visible)
    	    { 
                if(!module->updateviewport && module->terrains.size() > 0) {

                    for(int i=0; i < module->terrains.size(); i++) {
                        module->terrains[i]->calViewportMatrix(context.viewport.width(), context.viewport.height());
                    }
                    
                    module->updateviewport = true;
                }
	        
                float* MV = context.modelview.cast<float>().data();
                float* P = context.projection.cast<float>().data();
                for(int i=0; i < module->terrains.size(); i++) {
                    module->terrains[i]->render(MV, P);
                }
                if(oglError) return;
		
    	    }
            
            client->getRenderer()->endDraw();
            glPopAttrib();
        }
        
    }

private:
    TessTerrainRenderModule* module;

};

///////////////////////////////////////////////////////////////////////////////
void TessTerrainRenderModule::initializeRenderer(Renderer* r)
{
    r->addRenderPass(new TessTerrainRenderPass(r, this));
}

///////////////////////////////////////////////////////////////////////////////
TessTerrainRenderModule* initialize()
{
    TessTerrainRenderModule* prm = new TessTerrainRenderModule();
    ModuleServices::addModule(prm);
    prm->doInitialize(Engine::instance());
    return prm;
}

///////////////////////////////////////////////////////////////////////////////
// Python API
#include "omega/PythonInterpreterWrapper.h"
BOOST_PYTHON_MODULE(tessterrain)
{
    //
    PYAPI_REF_BASE_CLASS(TessTerrainRenderModule)
    PYAPI_METHOD(TessTerrainRenderModule, addTerrain)
    PYAPI_METHOD(TessTerrainRenderModule, printInfo)
    PYAPI_METHOD(TessTerrainRenderModule, nextDisplayMode)
    PYAPI_METHOD(TessTerrainRenderModule, moveTo)
    PYAPI_METHOD(TessTerrainRenderModule, setHeightScale)
    PYAPI_METHOD(TessTerrainRenderModule, toggleFog)
    ;

    def("initialize", initialize, PYAPI_RETURN_REF);
}

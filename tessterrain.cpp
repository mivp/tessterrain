#include <omega.h>
#include <omegaGl.h>
#include <iostream>
#include <vector>

#include "TerrainManager.h"

using namespace std;
using namespace omega;
using namespace tessterrain;

///////////////////////////////////////////////////////////////////////////////
class TessTerrainRenderModule : public EngineModule
{
public:
    TessTerrainRenderModule() :
        EngineModule("TessTerrainRenderModule"), visible(true), updateviewport(false), terrains(0)
    {

    }

    virtual void initializeRenderer(Renderer* r);

    virtual void update(const UpdateContext& context)
    {
        // After a frame all render passes had a chance to update their
        // textures. reset the raster update flag.       
    }
    
    virtual void dispose()
    {
        if (terrains)
            delete terrains;
    }

    void initTerrain(const string& inifile) {
        terrains = new TerrainManager(inifile);
    }

    void printInfo()
    {
        terrains->print();
    }

    void nextDisplayMode(int num)
    {  
        terrains->nextDisplayMode(num);
    }

    void setHeightScale(const float scale)
    {
        terrains->setHeightScale(scale);
    }

    void toggleFog()
    {
        terrains->toggleFog();
    }

    void setOverlayAlpha(float a)
    {
        terrains->setOverlayAlpha(a);   
    }

    void reloadOverlay()
    {
        terrains->reloadOverlay();
    }

    TerrainManager* terrains;
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
                if(!module->updateviewport) {

                    module->terrains->calViewportMatrix(context.viewport.width(), context.viewport.height());       
                    module->updateviewport = true;
                }

                Vector3f cp = context.camera->getPosition();
                float campos[3] = {cp[0], cp[1], cp[2]};
                float* MV = context.modelview.cast<float>().data();
                float* P = context.projection.cast<float>().data();
                float* MVP = (context.projection*context.modelview).cast<float>().data();
                
                module->terrains->updateVisibility(MVP, campos);
                module->terrains->render(MV, P);
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
    PYAPI_METHOD(TessTerrainRenderModule, initTerrain)
    PYAPI_METHOD(TessTerrainRenderModule, printInfo)
    PYAPI_METHOD(TessTerrainRenderModule, nextDisplayMode)
    PYAPI_METHOD(TessTerrainRenderModule, setHeightScale)
    PYAPI_METHOD(TessTerrainRenderModule, toggleFog)
    PYAPI_METHOD(TessTerrainRenderModule, setOverlayAlpha)
    PYAPI_METHOD(TessTerrainRenderModule, reloadOverlay)
    ;

    def("initialize", initialize, PYAPI_RETURN_REF);
}

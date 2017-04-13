#include <omega.h>
#include <omegaGl.h>
#include <iostream>

#include "Terrain.h"

using namespace std;
using namespace omega;

///////////////////////////////////////////////////////////////////////////////
class TessTerrainRenderModule : public EngineModule
{
public:
    TessTerrainRenderModule() :
        EngineModule("TessTerrainRenderModule"), terrain(0), visible(true), updateviewport(false)
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
        if(terrain)
            delete terrain;
    }

    void initTerrain(const string& option_file)
    {
        terrain = new tessterrain::TessTerrain();
        terrain->init(option_file);
        terrain->printInfo();
    }

    void nextDisplayMode()
    {  
        if(terrain)
            terrain->nextDisplayMode();
    }

    void moveTo(const float x, const float y, const float z)
    {
        if(terrain)
            terrain->moveTo(x, y, z);
    }

    tessterrain::TessTerrain* terrain;
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
                if(!module->updateviewport && module->terrain) {
                    module->terrain->calViewportMatrix(context.viewport.width(), context.viewport.height());
		    module->updateviewport = true;
                }
	        
		float* MV = context.modelview.cast<float>().data();
        	float* P = context.projection.cast<float>().data();
                module->terrain->render(MV, P);
                if(oglError) return;
		
                /*
    			// Test and draw
    			// get camera location in world coordinate
            	//if(context.eye == DrawContext::EyeLeft || context.eye == DrawContext::EyeCyclop)
            	{
                	Vector3f cp = context.camera->getPosition();
                	float campos[3] = {cp[0], cp[1], cp[2]};
                	float* MVP = (context.projection*context.modelview).cast<float>().data();
                    module->pointcloud->updateVisibility(MVP, campos, context.viewport.width(), context.viewport.height());
            	}

    		    module->pointcloud->draw();
    		    if(oglError) return;
                */
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
    PYAPI_METHOD(TessTerrainRenderModule, nextDisplayMode)
    PYAPI_METHOD(TessTerrainRenderModule, moveTo)
    ;

    def("initialize", initialize, PYAPI_RETURN_REF);
}

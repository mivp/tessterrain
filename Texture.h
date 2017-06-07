#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#ifdef OMEGALIB_MODULE
#include <omegaGl.h>
#else
#include "app/GLInclude.h"
#endif

using namespace std;

namespace tessterrain {
    
    class Texture
    {
    public:
        Texture(const char* filename, unsigned int index = 0, bool mipmap = false);
        Texture(unsigned int _index, unsigned int _width, unsigned int _height, unsigned int _format=GL_RGBA, unsigned int _globalFormat=GL_RGBA);
        ~Texture();
        
        void bind();
        void unbind();
        int getWidth();
        int getHeight();
        void resize(unsigned int _width, unsigned int _height);
	void reloadData(const char* filename, bool mipmap = false);
        
        static void resetUnit(int textureUnitOffset = 0);
        static Texture* newFromNextUnit(unsigned int _width, unsigned int _height, unsigned int _format=GL_RGBA, unsigned int _globalFormat=GL_RGBA);
        static unsigned int unitFromIndex(unsigned int _index);
        
        // Needs to be public to be accessed by GL calls
        unsigned int gluid;
        unsigned int glunit;
        unsigned int index;
        
        static const unsigned int LINEAR;
        static const unsigned int NEAREST;
        static const unsigned int MIPMAP;
        
    private:
        static unsigned int unitCount;
        static float borderColor[];
        static float borderColorB[];
        
        unsigned int height;
        unsigned int width;
        unsigned int minFilter;
        unsigned int magFilter;
        unsigned int format;
        unsigned int globalFormat;
    };
    
}; //namespace tessterrain

#endif // TEXTURE_H

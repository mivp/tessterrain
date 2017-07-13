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
        Texture(int width, int height, int numchannels, unsigned int ind = 0);
        ~Texture();
        
        void loadData(const char* filename);
        void initTexture();
        void freeTexture();
        
        void bind();
        void unbind();
        int getWidth();
        int getHeight();
        
        static unsigned int unitFromIndex(unsigned int _index);
        
        // Needs to be public to be accessed by GL calls
        unsigned int gluid;
        unsigned int glunit;
        unsigned int index;
        
        static const unsigned int LINEAR;
        static const unsigned int NEAREST;
        static const unsigned int MIPMAP;
        
        string filename;
        
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
        unsigned int type;
        
        bool initialized;
        bool created;
        unsigned char* data;
        int numChannel;
        bool mipmap;
    };
    
}; //namespace tessterrain

#endif // TEXTURE_H

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#ifdef OMEGALIB_MODULE
#include <omegaGl.h>
#else
#include "app/GLInclude.h"
#endif

using namespace std;

#define QUALITY_HIGH 0
#define QUALITY_LOW  1

namespace tessterrain {
    
    class Texture
    {
    public:
        Texture(int width, int height, int numchannels, unsigned int ind = 0, bool multires=false);
        ~Texture();
        
        void loadData(const char* filename);
        void initTexture(int quality=QUALITY_HIGH);
        void freeTexture(bool freedata=true);
        
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
        unsigned int height_low, width_low;
        unsigned int minFilter;
        unsigned int magFilter;
        unsigned int format;
        unsigned int globalFormat;
        unsigned int type;
        
        bool initialized;
        bool created;
        bool multiRes;
        unsigned char* data;
        unsigned char* data_low;
        int numChannel;
        int currentQuality;
    };
    
}; //namespace tessterrain

#endif // TEXTURE_H

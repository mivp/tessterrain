#include "Texture.h"
#include <iostream>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sys/time.h>

using namespace std;

namespace tessterrain {
    
    const unsigned int Texture::LINEAR = GL_LINEAR;
    const unsigned int Texture::NEAREST = GL_NEAREST;
    const unsigned int Texture::MIPMAP = GL_LINEAR_MIPMAP_LINEAR;
    
    unsigned int getTime() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return (tp.tv_sec * 1000 + tp.tv_usec / 1000);
    }
    
    void Texture::loadData(const char* filename) {
        this->filename = filename;
        int x,y,n;
        //unsigned int t = getTime();
        if(initialized && this->data)
            stbi_image_free(data);
        this->data = stbi_load(filename, &x, &y, &n, 0);
        //cout << "Load image " <<filename << " (n= " << n << ") time: " << getTime() - t << endl;
        
        if(!data) {
            cout << "Failed to load texture " << filename  << endl;
            exit(0);
        }
        initialized = false;
    }
    
    void Texture::initTexture() {
        
        if(!created) {
            //init texture
            glunit = unitFromIndex(index);
            minFilter = GL_LINEAR;
            magFilter = GL_LINEAR;
            type = GL_UNSIGNED_BYTE;
            if(numChannel == 1) {
                format = globalFormat = GL_RED;
            }
            else if (numChannel == 3) {
                format = globalFormat = GL_RGB;
            }
            else {
                format = GL_RGBA8;
                globalFormat = GL_RGBA;
                type = GL_UNSIGNED_INT_8_8_8_8_REV;
            }
            
            glActiveTexture(glunit);
            glGenTextures(1, &gluid);
            glBindTexture(GL_TEXTURE_2D, gluid);
            
            //unsigned int t = getTime();
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, type, NULL);
            //cout << "recreate texture "  << gluid << " time: " << getTime() - t << endl;
            //cout << "init texture " << gluid << " " << width << " " << height << " " << numChannel << endl;
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_CLAMP_TO_BORDER GL_REPEAT
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            created = true;
        }
        
        // cout << "initTexture" << endl;
        
        if (!initialized) {
            glActiveTexture(glunit);
            glBindTexture(GL_TEXTURE_2D, gluid);
            
            unsigned int t = getTime();
            //glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, data); //GL_UNSIGNED_INT_8_8_8_8_REV
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, globalFormat, type, data);
            cout << numChannel << " glTexSubImage2D time: " << getTime() - t << endl;
            //cout << "update texture " << gluid << " with data from " << this->filename << endl;
            
            //if(mipmap)
            //    glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
            data = NULL;
            initialized = true;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    // for terrain
    Texture::Texture(int width, int height, int numchannels, unsigned int ind): data(0), initialized(false), created(false) {
    
        this->numChannel = numchannels;
        this->width = width;
        this->height = height;
        this->gluid = 0;
        this->index = ind;
        this->mipmap = false;
    }
    
    void Texture::freeTexture()  {
        if(gluid)
            glDeleteTextures(1, &gluid);
        cout << "free texture " << gluid << endl;
        initialized = false;
        created = false;
        gluid = 0;
    }
    
    
    Texture::~Texture() {
        //glActiveTexture(glunit);
        if(gluid)
            glDeleteTextures(1, &gluid);
    }
    
    void Texture::bind() {
        if(!gluid)
            return;
        glActiveTexture(glunit);
        glBindTexture(GL_TEXTURE_2D, gluid);
    }
    
    void Texture::unbind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    int Texture::getWidth() {
        return width;
    }
    
    int Texture::getHeight() {
        return height;
    }
    
    
    // static
    unsigned int Texture::unitCount = 0;
    float Texture::borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float Texture::borderColorB[] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    unsigned int Texture::unitFromIndex(unsigned int index)
    {
        switch(index)
        {
            case 1: return GL_TEXTURE1;
            case 2: return GL_TEXTURE2;
            case 3: return GL_TEXTURE3;
            case 4: return GL_TEXTURE4;
            case 5: return GL_TEXTURE5;
            case 6: return GL_TEXTURE6;
            case 7: return GL_TEXTURE7;
            case 8: return GL_TEXTURE8;
            case 9: return GL_TEXTURE9;
            default: return GL_TEXTURE0;
        }
    }
    
}; //namespace tessterrain

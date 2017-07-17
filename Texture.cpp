#include "Texture.h"
#include "Utils.h"
#include <iostream>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

namespace tessterrain {
    
    const unsigned int Texture::LINEAR = GL_LINEAR;
    const unsigned int Texture::NEAREST = GL_NEAREST;
    const unsigned int Texture::MIPMAP = GL_LINEAR_MIPMAP_LINEAR;
    
    void Texture::loadData(const char* filename) {
        this->filename = filename;
        int x,y,n;
        //unsigned int t = getTime();
        if(initialized && this->data) {
            stbi_image_free(data);
            if(multiRes && this->data_low)
                delete []data_low;
        }
        //unsigned int t = getTime();
        this->data = stbi_load(filename, &x, &y, &n, 0);
        //cout << "load file time: " << getTime() - t << endl;
        //cout << "Load image " <<filename << " (n= " << n << ") time: " << getTime() - t << endl;
        
        if(!data) {
            cout << "Failed to load texture " << filename  << endl;
            exit(0);
        }
        
        if(multiRes) {
            int factor = 2;
            width_low = width/factor;
            height_low = height/factor;
            
            //unsigned int t = getTime();
            data_low = new unsigned char[width_low*height_low*numChannel];
            for (int r = 0; r < height_low; r++) {
                for(int c=0; c < width_low; c++) {
                    for(int ind=0; ind < numChannel; ind++) {
                        data_low[r*width_low*numChannel + numChannel*c + ind] = data[r*width*numChannel*factor + numChannel*c*factor + ind];
                    }
                }
            }
            //cout << numChannel << " resize time: " << getTime() - t << " " << width_low << " " << height_low << endl;
            
            //unsigned int t = getTime();
            //stbir_resize_uint8( this->data , width , height , 0,
            //                   this->data_low, width_low, height_low, 0, numChannel);
            //cout << "resize time: " << getTime() - t << endl;
        }
        
        initialized = false;
    }
    
    void Texture::initTexture(int quality) {
        
        if (multiRes && quality != currentQuality) {
            //cout << gluid << " change mode to " << quality << endl;
            currentQuality = quality;
            //initialized = false;
            freeTexture(false);
        }
        
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
            
            //unsigned int t = Utils::getTime();
            if (currentQuality == QUALITY_LOW)
                glTexImage2D(GL_TEXTURE_2D, 0, format, width_low, height_low, 0, globalFormat, type, NULL);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, type, NULL);
            //cout << "recreate texture "  << gluid << " quality: " << currentQuality << endl;
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
            
            if (currentQuality == QUALITY_LOW) {
                if(data_low) {
                    //unsigned int t = Utils::getTime();
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_low, height_low, globalFormat, type, data_low);
                    //cout << numChannel << " glTexSubImage2D (low) time: " << Utils::getTime() - t << endl;
                }
            }
            else {
                if(data) {
                    //unsigned int t = Utils::getTime();
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, globalFormat, type, data);
                    //cout << numChannel << " glTexSubImage2D (height) time: " << Utils::getTime() - t << endl;
                }
            }
            
            initialized = true;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    // for terrain
    Texture::Texture(int width, int height, int numchannels, unsigned int ind, bool multires): data(0), data_low(0), initialized(false), created(false) {
    
        this->numChannel = numchannels;
        this->width = width;
        this->height = height;
        this->gluid = 0;
        this->index = ind;
        this->multiRes = multires;
        this->currentQuality = QUALITY_HIGH;
    }
    
    void Texture::freeTexture(bool freedata)  {
        if(gluid)
            glDeleteTextures(1, &gluid);
        initialized = false;
        created = false;
        gluid = 0;
        if(freedata && data) {
            stbi_image_free(data);
            data = NULL;
        }
        if(freedata && data_low) {
            stbi_image_free(data_low);
            data_low = NULL;
        }
    }
    
    
    Texture::~Texture() {
        //glActiveTexture(glunit);
        if(gluid)
            glDeleteTextures(1, &gluid);
        if(data) {
            stbi_image_free(data);
            data = NULL;
        }
        if(data_low) {
            stbi_image_free(data_low);
            data_low = NULL;
        }
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

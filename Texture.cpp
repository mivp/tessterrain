#include "Texture.h"
#include <iostream>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

namespace tessterrain {

const unsigned int Texture::LINEAR = GL_LINEAR;
const unsigned int Texture::NEAREST = GL_NEAREST;
const unsigned int Texture::MIPMAP = GL_LINEAR_MIPMAP_LINEAR;

// for colormap
Texture::Texture(const char* filename, unsigned int ind, bool mipmap, unsigned int _format, unsigned int _globalFormat) {
    
    int x,y,n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
    
    if(!data) {
        cout << "Failed to load texture"  << endl;
        exit(0);
    }
    
    cout << "img: " << filename << " width: " << x << " height: " << y << " comps: " << n << endl;
    
    width = x;
    height = y;

    //init texture
    gluid = 0;
    index = ind;
    glunit = unitFromIndex(index);
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    format = _format;
    globalFormat = _globalFormat;
    glActiveTexture(glunit);
    glGenTextures(1, &gluid);
    glBindTexture(GL_TEXTURE_2D, gluid);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, data); //GL_UNSIGNED_INT_8_8_8_8_REV
    if(mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_CLAMP_TO_BORDER GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(data);
}

// for framebuffer
Texture::Texture(unsigned int _index, unsigned int _width, unsigned int _height, unsigned int _format, unsigned int _globalFormat)
{
    index = _index;
    glunit = unitFromIndex(_index);
    gluid = 0;
    width = _width;
    height = _height;
    minFilter = GL_LINEAR;
    magFilter = GL_LINEAR;
    format = _format;
    globalFormat = _globalFormat;
    
    glActiveTexture(glunit);
    glGenTextures(1, &gluid);
    glBindTexture(GL_TEXTURE_2D, gluid);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, NULL);
    if(format == GL_DEPTH_COMPONENT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColorB);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glActiveTexture(GL_TEXTURE0);
}


Texture::~Texture() {
    glDeleteTextures(1, &gluid);
}

void Texture::bind() {
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

void Texture::resize(unsigned int _width, unsigned int _height)
{
    width = _width;
    height = _height;

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, globalFormat, GL_UNSIGNED_BYTE, NULL);
}

// static
unsigned int Texture::unitCount = 0;
float Texture::borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
float Texture::borderColorB[] = {0.0f, 0.0f, 0.0f, 0.0f};

void Texture::resetUnit(int textureUnitOffset)
{
    unitCount = textureUnitOffset;
}

Texture* Texture::newFromNextUnit(unsigned int _width, unsigned int _height, unsigned int _format, unsigned int _globalFormat)
{
    return new Texture(unitCount++, _width, _height, _format, _globalFormat);
}

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

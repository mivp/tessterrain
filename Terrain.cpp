#include "Terrain.h"
#include "INIReader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <float.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

using namespace std;
using namespace glm;

namespace tessterrain {

Terrain::Terrain(): m_material(0), m_initialized(false) {
    
}

Terrain::~Terrain() {
    if(m_material)
        delete m_material;
}

void Terrain::init(string configfile) {
    
    INIReader reader(configfile);
    m_files.push_back(reader.Get("file", "heightmap", ""));
    m_files.push_back(reader.Get("file", "texture", ""));
    
    m_lefttop = glm::vec2(reader.GetInteger("topleft", "left", 0), reader.GetInteger("topleft", "top", 0));
    
    m_horizontalRes = glm::vec2(reader.GetReal("horizontalres", "wres", 0), reader.GetReal("horizontalres", "hres", 0));
    
    m_heightRange = glm::vec2(reader.GetReal("heightrange", "min", 0), reader.GetReal("heightrange", "max", 1));
    
    //TODO: translate and update m_modelMatrix
  
    m_initialized = true;
    
}

void Terrain::printInfo() {
    for (int i=0; i < m_files.size(); i++)
        cout << "file: " << m_files[i] << endl;
    cout << "left: " << m_lefttop[0] << " top: " << m_lefttop[1] << endl;
    cout << "wres: " << m_horizontalRes[0] << " hres: " << m_horizontalRes[1] << endl;
    cout << "h min: " << m_heightRange[0] << " h max: " << m_heightRange[1] << endl;
}

/**
 Mesh Terrain
*/
MeshTerrain::MeshTerrain(): Terrain(), m_vbo(0), m_vao(0), m_ibo(0), 
                            m_dataloaded(false), m_wireframe(false) {
    m_material = new MeshMaterial();
}

MeshTerrain::~MeshTerrain() {
    if(m_vao > 0) {
        glDeleteVertexArrays(1,&m_vao);
        glDeleteBuffers(1,&m_vbo);
        glDeleteBuffers(1,&m_ibo);
    }
}

int MeshTerrain::loadHeightMap() {
    
    int width, height, n;
    unsigned char *data = stbi_load(m_files[0].c_str(), &width, &height, &n, 0);
    
    if(!data) {
        cout << "Failed to load highmap file " << m_files[0] << endl;
        exit(0);
    }
    
    cout << "width: " << width << " height: " << height << " comps: " << n << endl;
    m_size = glm::vec2(width, height);
    
    // highmap data
    m_heightmap = vector< vector<float> >(height, vector<float> (width, 0));
    
    float hrange = m_heightRange[1] - m_heightRange[0];
    int striplen = n*width;
    
    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            m_heightmap[row][col] = (float)data[row*striplen + col*n] * hrange / 255 + m_heightRange[0];
        }
    }

  	cout << "some values" << endl;
    for(int r =0; r < 5; r++) {
        for(int c=0; c < 10; c++)
            cout << m_heightmap[r][c] << " ";
        cout << endl;
    }
	
	return 0;
}

vec3 MeshTerrain::computeNormal(vec3 center, int row, int col)
{
    // Compute center of all values which is the i and j passed in
    vec3 left;
    vec3 right;
    vec3 up;
    vec3 down;
    vec3 sum = vec3(0,0,0);
    bool l = false;
    bool r = false;
    bool u = false;
    bool d = false;
    
    float wres = m_horizontalRes[0];
    float hres = m_horizontalRes[1];
    float width = m_size[0];
    float height = m_size[1];
    
    
    int count = 0;
    // Compute left
    if(col -1 >= 0) {
        left = vec3((col-1)*wres, m_heightmap[row][col-1], row*hres);
        left = center - left;
        l = true;
    }
    
    // Compute right
    if(col+1 < width) {
        right = vec3((col+1)*wres, m_heightmap[row][col+1], row*hres);
        right = center - right;
        r = true;
    }
    
    // Compute up
    if(row-1 >= 0) {
        up = vec3(col*wres, m_heightmap[row-1][col], (row-1)*hres);
        up = center-up;
        u = true;
    }
    
    // Compute down
    if(row+1 < height) {
        down = vec3(col*wres, m_heightmap[row+1][col], (row+1)*hres);
        down = center-down;
        d = true;
    }
    
    // Compute normals
    if(u  && r) {
        vec3 v1 = cross(up,right);
        if(v1.y < 0)
            v1 *= -1;
        sum += v1;
        count = count + 1;
    }
    
    if(u && l) {
        vec3 v1 = cross(up,left);
        if(v1.y < 0)
            v1 *= -1;
        sum += v1;
        count = count + 1;
    }
    
    if(d && r) {
        vec3 v1 = cross(down,right);
        if(v1.y < 0)
            v1 *= -1;
        sum += v1;
        count = count + 1;
    }
    
    if(d && l) {
        vec3 v1 = cross(down,left);
        if(v1.y < 0)
            v1 *= -1;
        sum += v1;
        count = count + 1;
    }
    
    // Compute average normal
    sum /= count;
    
    // Normalize it and return :D!!!! Enjoy your smoothed normal for some smooth shading!
    return normalize(sum);
};

int MeshTerrain::createMesh() {
	
    loadHeightMap();

	vertexes.clear();
	indices.clear();
    
    float wres = m_horizontalRes[0];
    float hres = m_horizontalRes[1];
    float width = m_size[0];
    float height = m_size[1];

	for(int r = 0; r < height-1; r++) {

		for(int c = 0; c < width-1; c++) {

		    float UL = m_heightmap[r][c]; // Upper left
		    float LL = m_heightmap[r+1][c]; // Lower left
		    float UR = m_heightmap[r][c+1]; // Upper right
		    float LR = m_heightmap[r+1][c+1]; // Lower right
		    
		    vec3 ULV = vec3(c*wres, UL, r*hres);
		    vec3 LLV = vec3(c*wres, LL, (r+1)*hres);
		    vec3 URV = vec3((c+1)*wres, UR, r*hres);
		    vec3 LRV = vec3((c+1)*wres, LR, (r+1)*hres);

		    // compute smoothed normal
		    vec3 va = computeNormal(ULV, r, c);
		    vec3 vb = computeNormal(LLV, r+1, c);
		    vec3 vc = computeNormal(URV, r, c+1);
		    vec3 vd = computeNormal(LRV, r+1, c+1);

		    // Push back vector information for these group of dots
		    vertexes.push_back(Vertex(vec3(c*wres, UL, r*hres), va, vec2((float)c/width,(float)r/height)) );
		    vertexes.push_back(Vertex(vec3(c*wres, LL, (r+1)*hres), vb, vec2((float)c/width,(float)(r+1)/height)) );
		    vertexes.push_back(Vertex(vec3((c+1)*wres, UR, r*hres), vc, vec2((float)(c+1)/width,(float)r/height)) );
		    vertexes.push_back(Vertex(vec3((c+1)*wres, LR, (r+1)*hres), vd, vec2((float)(c+1)/width,(float)(r+1)/height)) );

		    // Push back indices for these vertexes
		    indices.push_back(vertexes.size() - 4);
		    indices.push_back(vertexes.size() - 1);
		    indices.push_back(vertexes.size() - 2);
		    indices.push_back(vertexes.size() - 4);
		    indices.push_back(vertexes.size() - 3);
		    indices.push_back(vertexes.size() - 1);
		}
	}
    
    m_dataloaded = true;

	return 0;
}


int MeshTerrain::saveMeshToObj(string filename) {

    /*
    ofstream fout(filename, std::ofstream::out);

	if (!fout.is_open()) {
  		cout << "Unable to open file to write" << filename << endl;
  		return 0;
  	}

  	cout << "Number of vertexes: " << vertexes.size() << " faces: " << indices.size()/3 << endl;
    
    //fout << "o map" << endl;
    fout << "mtllib sw_lr.tif.txt.mtl" << endl;
    //fout << "mtllib mel_small.tif.txt.mtl" << endl;
    
    cout << "write vertexes" << endl;
    for(int i=0; i < vertexes.size(); i++) {
        fout << "v " << vertexes[i].position[0] << " " << vertexes[i].position[1] << " " << vertexes[i].position[2] << endl;
        fout << "vt " << vertexes[i].uv[0] << " " << vertexes[i].uv[1] << endl;
        fout << "vn " << vertexes[i].normal[0] << " " << vertexes[i].normal[1] << " " << vertexes[i].normal[2] << endl;
    }

  	// indices
    cout << "write faces..." << endl;
  	//fout << "# faces" << endl;
    //fout << "g map" << endl;
    fout << "usemtl image" << endl;
    for(int i=0; i < indices.size(); i+=3)
        fout << "f " << indices[i]+1 << "/" << indices[i]+1 << "/" << indices[i]+1 << " "
                     << indices[i+1]+1 << "/" << indices[i+1]+1 << "/" << indices[i+1]+1 << " "
                     << indices[i+2]+1 << "/" << indices[i+2]+1 << "/" << indices[i+2]+1 << endl;
    
  	fout.close();
    */

	return 0;
}

void MeshTerrain::setup(){
    
    if(!m_dataloaded)
        createMesh();
    
    if(!m_material)
        return;
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    // create vbo
    glGenBuffers(1,&m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertexes.size(), &vertexes[0], GL_STATIC_DRAW);
    
    GLSLProgram* shader = m_material->getShader();
    //shader->bind();
    unsigned int val;
  
    val = glGetAttribLocation(shader->getHandle(), "aPosition");
    glEnableVertexAttribArray(val);
    glVertexAttribPointer( val,  3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    
    val = glGetAttribLocation(shader->getHandle(), "aNormal");
    glEnableVertexAttribArray(val);
    glVertexAttribPointer( val,  3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    val = glGetAttribLocation(shader->getHandle(), "aTexCoord");
    glEnableVertexAttribArray(val);
    glVertexAttribPointer( val,  2, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    // create ibo
    glGenBuffers(1,&m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
}

#ifndef OMEGALIB_MODULE
void MeshTerrain::render(Camera* cam) {
#else
void MeshTerrain::render(const float MV[16], const float P[16]) {
#endif
    if(~m_vao)
        setup();
    
    GLSLProgram* shader = m_material->getShader();
    shader->bind();
#ifndef OMEGALIB_MODULE
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();
#else
    glm::mat4 viewMatrix = glm::make_mat4(MV);
    viewMatrix = glm::transpose(viewMatrix);
    glm::mat4 projMatrix = glm::make_mat4(P);
    projMatrix = glm::transpose(projMatrix);
#endif
    glm::mat4 modelViewMatrix = viewMatrix * m_modelMatrix;
    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
    glm::mat4 mvp = projMatrix * modelViewMatrix;
  
    shader->setUniform("uMVP", mvp);
    shader->setUniform("uNormalMatrix", normalMatrix);
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    
    glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0 );
    
    if(m_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glBindVertexArray(0);
}

/**
 Tessellation Terrain
 */
TessTerrain::TessTerrain(): Terrain(), m_texHightmap(0), m_texGrass(0), m_texRock(0), m_texSnow(0),
                                            m_vbo(0), m_vao(0), m_initialized(false), m_displayMode(0) {
                                                
    m_material = new TessMaterial();
                                                
    m_displayModeNames.push_back("shadeSimpleWireFrame");
    m_displayModeNames.push_back("shadeWorldHeight");
    m_displayModeNames.push_back("shadeWorldHeightWireFrame");
    m_displayModeNames.push_back("shadeWorldNormal");
    m_displayModeNames.push_back("shadeGrass");
    m_displayModeNames.push_back("shadeGrassAndRocks");
    m_displayModeNames.push_back("shadeGrassRocksAndSnow");
    m_displayModeNames.push_back("shadeLightingFactors");
    m_displayModeNames.push_back("shadeTexturedAndLit");
                                              
}

TessTerrain::~TessTerrain() {
    if(m_texHightmap)
        delete m_texHightmap;
    if(m_texGrass)
        delete m_texGrass;
    if(m_texRock)
        delete m_texRock;
    if(m_texSnow)
        delete m_texSnow;
    
    if(m_vao > 0) {
        glDeleteVertexArrays(1,&m_vao);
        glDeleteBuffers(1,&m_vbo);
    }
}

void TessTerrain::nextDisplayMode(bool forward) {
    if(forward) {
        m_displayMode++;
        if(m_displayMode >= DisplayModeCount)
            m_displayMode = 0;
    }
    else {
        m_displayMode--;
        if(m_displayMode < 0)
            m_displayMode = DisplayModeCount - 1;
    }
    cout << "display mdoe: " << m_displayModeNames[m_displayMode] << endl;
}

void TessTerrain::setup(){
    
    if(!m_material)
        return;
    
    // textures
    m_texHightmap = new Texture("testdata/tess/heightmap-1024x1024.png", 0, false, GL_RGBA, GL_RGBA);
    m_texGrass = new Texture("testdata/tess/grass.png", 1, true, GL_RGB, GL_RGB);
    m_texRock = new Texture("testdata/tess/rock.png", 2, true, GL_RGB, GL_RGB);
    m_texSnow = new Texture("testdata/tess/snowrocks.png", 3, true, GL_RGB, GL_RGB);
    
    // patches
    const int maxTessellationLevel = 64;
    const int trianglesPerHeightSample = 1;
    const int xDivisions = trianglesPerHeightSample * m_texHightmap->getWidth() / maxTessellationLevel;
    const int zDivisions = trianglesPerHeightSample * m_texHightmap->getHeight() / maxTessellationLevel;
    cout << "xDivisions = " << xDivisions << " zDivisions = " << zDivisions << endl;
    m_patchCount = xDivisions * zDivisions;
    vector<float> positionData( 2 * m_patchCount ); // 2 floats per vertex
    cout << "Total number of patches = " << m_patchCount << endl;
    
    const float dx = 1.0f / static_cast<float>( xDivisions );
    const float dz = 1.0f / static_cast<float>( zDivisions );
    
    for ( int j = 0; j < 2 * zDivisions; j += 2 ) {
        float z = static_cast<float>( j ) * dz * 0.5;
        for ( int i = 0; i < 2 * xDivisions; i += 2 ) {
            float x = static_cast<float>( i ) * dx * 0.5;
            const int index = xDivisions * j + i;
            positionData[index]     = x;
            positionData[index + 1] = z;
        }
    }
    
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, positionData.size() * sizeof( float ), &positionData[0], GL_STATIC_DRAW);
    
    GLSLProgram* shader = m_material->getShader();
    shader->bind();
    unsigned int val = glGetAttribLocation(shader->getHandle(), "vertexPosition");
    glEnableVertexAttribArray(val);
    glVertexAttribPointer(val, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (const GLvoid*)0);
    
    glBindVertexArray(0);
    
    // Get subroutine indices
    for ( int i = 0; i < DisplayModeCount; ++i){
        m_displayModeSubroutines.push_back( glGetSubroutineIndex( shader->getHandle(), GL_FRAGMENT_SHADER, m_displayModeNames[i].c_str()) );
    }
    
    m_modelMatrix = glm::mat4(1.0);
    
    m_initialized = true;
}

#ifndef OMEGALIB_MODULE
void TessTerrain::render(Camera* cam) {
#else
void TessTerrain::render(const float MV[16], const float P[16]) {
#endif
    
    if(!m_initialized)
        setup();
    
    GLSLProgram* shader = m_material->getShader();
    shader->bind();
    m_texHightmap->bind();
    m_texGrass->bind();
    m_texRock->bind();
    m_texSnow->bind();
    
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &m_displayModeSubroutines[m_displayMode] );
    
    //texture
    shader->setUniform( "heightMap", (int) m_texHightmap->index );
    shader->setUniform( "grassTexture", (int) m_texGrass->index );
    shader->setUniform( "rockTexture", (int) m_texRock->index );
    shader->setUniform( "snowTexture", (int) m_texSnow->index );
    
    //uniforms
    shader->setUniform( "line.width", 1.0f );
    shader->setUniform( "line.color", glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
    
    // Set the fog parameters
    shader->setUniform( "fog.color", glm::vec4( 0.65f, 0.77f, 1.0f, 1.0f ) );
    shader->setUniform( "fog.minDistance", 50.0f );
    shader->setUniform( "fog.maxDistance", 800.0f );
    
    // scales
    shader->setUniform( "horizontalScale", 500.0f );
    shader->setUniform( "verticalScale", 20.0f );
    shader->setUniform( "pixelsPerTriangleEdge", 12.0f );
    shader->setUniform( "maxTrianglesPerTexel", (int)1);
    shader->setUniform( "viewportSize", m_viewportSize);
    
    // Pass in the usual transformation matrices
#ifndef OMEGALIB_MODULE
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();
#else
    glm::mat4 viewMatrix = glm::make_mat4(MV);
    viewMatrix = glm::transpose(viewMatrix);
    glm::mat4 projMatrix = glm::make_mat4(P);
    projMatrix = glm::transpose(projMatrix);
#endif
    glm::mat4 modelViewMatrix = viewMatrix * m_modelMatrix;
    glm::mat3 worldNormalMatrix = glm::inverseTranspose(glm::mat3(m_modelMatrix));
    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));
    glm::mat4 mvp = projMatrix * modelViewMatrix;

    shader->setUniform( "modelMatrix", m_modelMatrix );
    shader->setUniform( "modelViewMatrix", modelViewMatrix );
    shader->setUniform( "worldNormalMatrix", worldNormalMatrix );
    shader->setUniform( "normalMatrix", normalMatrix );
    shader->setUniform( "mvp", mvp );
    shader->setUniform( "viewportMatrix", m_viewportMatrix );
    
    // Set the lighting parameters
    glm::vec4 worldLightDirection( 0.50000019433, 0.86602529158, 0.0f, 0.0f ); //sin(30), cos(30)
    glm::mat4 worldToEyeNormal( normalMatrix );
    glm::vec4 lightDirection = worldToEyeNormal * worldLightDirection;
    shader->setUniform( "light.position", lightDirection );
    shader->setUniform( "light.intensity", glm::vec3( 1.0f, 1.0f, 1.0f ) );
    
    // Set the material properties
    shader->setUniform( "material.Ka",  glm::vec3( 0.1f, 0.1f, 0.1f ) );
    shader->setUniform( "material.Kd",  glm::vec3( 1.0f, 1.0f, 1.0f ) );
    shader->setUniform( "material.Ks",  glm::vec3( 0.3f, 0.3f, 0.3f ) );
    shader->setUniform( "material.shininess", 10.0f );
    
    // draw
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBindVertexArray(m_vao);
    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays( GL_PATCHES, 0, m_patchCount );
    glBindVertexArray(0);
    
    // clean up
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void TessTerrain::calViewportMatrix(int width, int height) {
    
    m_viewportSize = glm::vec2(width, height);
    
    float w2 = width / 2.0f;
    float h2 = height / 2.0f;
    m_viewportMatrix = glm::mat4(1.0);
    m_viewportMatrix[0] = glm::vec4(w2, 0.0f, 0.0f, 0.0f);
    m_viewportMatrix[1] = glm::vec4(0.0f, h2, 0.0f, 0.0f);
    m_viewportMatrix[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    m_viewportMatrix[3] = glm::vec4(w2, h2, 0.0f, 1.0f);
}

}; //namespace tessterrain

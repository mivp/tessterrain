#include "GLInclude.h"
#include "GLUtils.h"
#include "../Terrain.h"
#include "../Material.h"
#include "Mesh.h"
#include "Camera.h"

#include <sstream>
#include <iostream>
#include <vector>
using std::stringstream;
using std::string;
using std::cout;
using std::endl;

using namespace tessterrain;

GLFWwindow* window;
string title;

//timing related variables
float last_time=0, current_time =0;
//delta time
float dt = 0;

#define WIDTH 1024
#define HEIGHT 768

int num_stations = 15;
float stations_loc[] =
{   157785.4006,	-10693.83491,
    177420.6376,	-5577.00931,
    29384.04148,	-136586.0948,
    200039.4272,	-187156.8063,
    11389.11211,	-42189.86366,
    5103.594053,	-79892.65485,
    75758.59579,	-50433.21111,
    137510.9092,	-97982.61689,
    226461.8312,	-117937.4362,
    124056.8904,	-12275.08654,
    89669.70268,	-132725.7355,
    23342.03458,	-123237.4277,
    35559.88801,	-126637.733,
    109057.8335,	-121908.1675,
    153255.1656,	-58332.76674 };

float stations_height[] =
{   295,
    875,
    45.4,
    82,
    130.6,
    51,
    241.1,
    130,
    261,
    1150,
    10,
    80.9,
    0,
    70.8,
    226};

TessTerrain* tessTerrain = NULL;
TessTerrain* tessTerrain2 = NULL;
vector<Mesh*> stations;

Camera* camera = NULL;

int currentkey = -1;

//camera / mouse
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstmouse = true;
bool usemouse = false;

void doMovement();

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //cout << "button: " << button << " action: " << action << endl;
    if (button == 0 && action == 1)
        usemouse = true;
    
    else if (button == 0 && action == 0)
        usemouse = false;
}


static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera->processMouseScroll(yoffset);
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if(!usemouse) {
        lastX = xpos;
        lastY = ypos;
        return;
    }
    
    if(firstmouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstmouse = false;
        cout << "first: " << lastX << " " << lastY << endl;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera->processMouseMovement(xoffset, yoffset);
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
	
}

void init_resources()
{
    // test mesh terrain
    /*
    //string in_filename = "testdata/heightmap_small.txt";
    //string in_filename = "testdata/data/drycreek.tif.txt";
    string in_filename = "testdata/mel_small/mel_small.tif.txt";
    //string in_filename = "testdata/sw_lr/sw_lr.tif.txt";
    string obj_filename = in_filename;
    obj_filename.append(".obj");
    
    meshMaterial = new MeshMaterial();
    camera = new Camera();
    camera->position = glm::vec3(3654.82, -7820.01, 971.397);
    camera->front = glm::vec3(0.00699247, 0.91324, -0.407362);
    camera->pitch = 65.957;
    camera->yaw = -89.0166;
    camera->ratio = 1.0*WIDTH/HEIGHT;
    camera->movementSpeed = 500;
    
    
    meshTerrain = new MeshTerrain(meshMaterial);
    cout << "=== Loading data ... ===" << endl;
    meshTerrain->loadHeightMap(in_filename);
    cout << "=== Creating mesh ... ===" << endl;
    meshTerrain->createMesh();
    //cout << "=== Save to obj ... ===" << endl;
    //terrain->saveMeshToObj(obj_filename);
    cout << "=== Ready to draw ...===" << endl;
    */
    
    // test tessellation terrain
    
    camera = new Camera();
    //camera->position = glm::vec3(294.059, 85.6822, 559.623);
    //camera->front = glm::vec3(-0.0794522, -0.407484, -0.90975);
    camera->position = glm::vec3(73823.5, 97650, 148957);
    camera->front = glm::vec3(0.0522058, -0.772296, -0.633114);
    camera->ratio = 1.0*WIDTH/HEIGHT;
    camera->movementSpeed = 10000;
    camera->pitch = -50.5605;
    camera->yaw = -85.2861;
    camera->near = 100.0f;
    camera->far = 1000000.0f;
    
    //meshTerrain = new MeshTerrain();
    //meshTerrain->init("testdata/tess/config.ini");
    //meshTerrain->printInfo();
    
    tessTerrain = new TessTerrain();
    //tessTerrain->init("testdata/tess/config.ini");
    tessTerrain->init("testdata/south_west/west_1sh/west_1sh.ini");
    tessTerrain->printInfo();
    tessTerrain->calViewportMatrix(WIDTH, HEIGHT);
    
    tessTerrain2 = new TessTerrain();
    tessTerrain2->init("testdata/south_west/east_1sh/east_1sh.ini");
    tessTerrain2->printInfo();
    tessTerrain2->calViewportMatrix(WIDTH, HEIGHT);
    tessTerrain2->moveTo(glm::vec3(117850.4781, 0, 6077.757799));
    
    for(int i=0; i < num_stations; i++) {
        Mesh* m = MeshUtils::sphere(500, 5, 5);
        cout << stations_loc[2*i] << " " << 3*stations_height[i] << " " << -1*stations_loc[2*i+1] << endl;
        m->moveTo(glm::vec3(stations_loc[2*i], 3*stations_height[i], -1*stations_loc[2*i+1]));
        stations.push_back(m);
    }
}

void free_resources()
{
    if(tessTerrain)
        delete tessTerrain;
    if(tessTerrain2)
        delete tessTerrain2;
    if(camera)
        delete camera;
    for(int i = 0; i < stations.size(); i++)
        if(stations[i])
            delete stations[i];
}

void doMovement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera->processKeyboard(FORWARD, dt);
    if(keys[GLFW_KEY_S])
        camera->processKeyboard(BACKWARD, dt);
    if(keys[GLFW_KEY_A])
        camera->processKeyboard(LEFT, dt);
    if(keys[GLFW_KEY_D])
        camera->processKeyboard(RIGHT, dt);
    if(keys[GLFW_KEY_I]) {
        cout << "pos: " << camera->position[0] << " " << camera->position[1] << " " << camera->position[2] << endl;
        cout << "front: " << camera->front[0] << " " << camera->front[1] << " " << camera->front[2] << endl;
        cout << "pitch: " << camera->pitch << " yaw: " << camera->yaw << endl;
        keys[GLFW_KEY_I] = false;
    }
    if(keys[GLFW_KEY_T]) {
        keys[GLFW_KEY_T] = false;
    }
    if(keys[GLFW_KEY_N]) {
        tessTerrain->nextDisplayMode();
        tessTerrain2->nextDisplayMode();
        keys[GLFW_KEY_N] = false;
    }
}

void mainLoop()
{
    const int samples = 50;
    float time[samples];
    int index = 0;

    do{
        //timing related calcualtion
        last_time = current_time;
        current_time = glfwGetTime();
        dt = current_time-last_time;
        
        glfwPollEvents();
        doMovement();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Dark blue background
        //glClearColor(1.0f,0.5f,0.5f,1.0f);
        
        //renderer->update(dt);
        //renderer->render();
        //PrepFinalBuffer();
        //ClearFinalBuffer();
        //DrawFinalBuffer();
        
        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        
        //meshTerrain->render(camera);
        tessTerrain->render(camera);
        tessTerrain2->render(camera);
        
        for(int i = 0; i < stations.size(); i++)
            if(stations[i])
                stations[i]->render(camera);

        glfwSwapBuffers(window);

        // Update FPS
        time[index] = glfwGetTime();
        index = (index + 1) % samples;

        if( index == 0 ) {
          float sum = 0.0f;
          for( int i = 0; i < samples-1 ; i++ )
            sum += time[i + 1] - time[i];
          float fps = samples / sum;

          stringstream strm;
          strm << title;
          strm.precision(4);
          strm << " (fps: " << fps << ")";
          glfwSetWindowTitle(window, strm.str().c_str());
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
}

int main(int argc, char* argv[]) {

	//scene = new SceneViewer(WIDTH, HEIGHT);

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( WIDTH, HEIGHT, "OpenGL window with GLFW", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// print GL info
	GLUtils::dumpGLInfo();

	// init resources
	init_resources();

	// Enter the main loop
	mainLoop();

	free_resources();

	// Close window and terminate GLFW
	glfwTerminate();

	// Exit program
	return EXIT_SUCCESS;
}

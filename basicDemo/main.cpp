#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "lib/ImGui/imgui.h"
#include "lib/ImGui/imgui_impl_glfw.h"
#include "lib/ImGui/imgui_impl_opengl3.h"
#include "lib/ImGui/imconfig.h"

#include "Api/RYDefine.h"
#include "Api/RYGraphics.h"
#include "Api/particle/particleSystem.h"
#include "Api/model/Mesh.h"

//assimp
Assimp::Importer importer;
// Window current width
extern unsigned int windowWidth = 800;
// Window current height
extern unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "CG3-P2";
// Window pointer
GLFWwindow *window;

// Shader object
Shader *shader, *shaderStereo;
// Index (GPU) of the geometry buffer
unsigned int VBO;
// Index (GPU) vertex array object
unsigned int VAO;
// Index (GPU) of the texture
unsigned int textureID;

//Particle system
bool pressLeft;
RYGraphics* Api;
particleSystem* parSystem;

//Load models
Mesh* mesh;


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void onMouseButton(GLFWwindow* window, int button, int action, int mods);

/* *
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
	Api->camera->updateWH(glm::vec2(width, height));
    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth, windowHeight);
}
/* *
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow()
{
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

    // The window couldn't be created
    if (!window)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);

    // Window resize callback
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, onMouseButton);
    return true;
}
/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad()
{
    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status)
    {
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}
void initImGui(){

    #if __APPLE__
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    #endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

}

void renderImGui() {

    ImGui::Begin("API Controls");
    ImGui::Text("FPS: %d", Api->getFPS());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
	if (ImGui::Button("Stereoscopy"))
	{
		Api->stereoscopy = !Api->stereoscopy;
		//std::cout << Api->stereoscopy << std::endl;
	}
	/*if (ImGui::Button("Left"))
	{
		Api->left = !Api->left;
	}
*/
    ImGui::End();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Initialize the opengl context
 * */
void initGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry()
{
    float triangleVertices[] = {
        // Bottom left vertex
        -0.5f, -0.5f, 0.0f, // Position
        1.0f, 0.0f, 0.0f,   // Color
        // Bottom right vertex
        0.5f, -0.5f, 0.0f, // Position
        0.0f, 1.0f, 0.0f,  // Color
        // Top Center vertex
        0.0f, 0.5f, 0.0f, // Position
        0.0f, 0.0f, 1.0f  // Color

    };
    // Creates on GPU the vertex array
    glGenVertexArrays(1, &VAO);
    // Creates on GPU the vertex buffer object
    glGenBuffers(1, &VBO);
    // Binds the vertex array to set all the its properties
    glBindVertexArray(VAO);
    // Sets the buffer geometry data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), &triangleVertices, GL_STATIC_DRAW);

    // Sets the vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
}

/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init()
{
    // Initialize the window, and the glad components
    if (!initWindow() || !initGlad())
        return false;

    // Initialize the opengl context
    initGL();
    
    Api = new RYGraphics();

    initImGui();
    // Loads the shader
	shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	shaderStereo = new Shader("assets/shaders/stereo.vert", "assets/shaders/stereo.frag");
    // Loads all the geometry into the GPU
    buildGeometry();
    parSystem = new particleSystem();
    // Loads the texture into the GPU
    //textureID = loadTexture("assets/textures/bricks2.jpg");
    string path = "assets/models/crate.obj";
    //path = "assets/models/crysis.fbx";
    //Loads 3D model
    mesh = new Mesh();
    if (!mesh->LoadMesh(path))
    {
        cout << "no cargó modelo" << endl;
        return false;
    }
    else {
        cout << "cargó modelo" << endl;

    }
   
    return true;
}
/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */
void processKeyboardInput(GLFWwindow *window)
{
    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Camera move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

        Api->camera->moveForward();

    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

        Api->camera->moveBackward();

    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

        Api->camera->strafeLeft();

    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

        Api->camera->strafeRight();

    }

    // Checks if the r key is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reloads the shader
		delete shader;
		delete shaderStereo;
        shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		shaderStereo = new Shader("assets/shaders/stereo.vert", "assets/shaders/stereo.frag");

    }
}

void renderStereo() {
	shaderStereo->use();
	glColorMask( GL_FALSE,
		 GL_TRUE,
		 GL_TRUE,
		 GL_TRUE);
	// Left eye matrices	
	Api->camera->stereoViewProjectionMatrices(0.5, 10.0, Api->left);
	shaderStereo->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
	shaderStereo->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
	shaderStereo->setBool("left", Api->left);
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	shaderStereo->use();
	glColorMask(GL_TRUE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE);
	// Right eye matrices	
	Api->camera->stereoViewProjectionMatrices(0.5, 10.0, !Api->left);
	shaderStereo->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
	shaderStereo->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
	shaderStereo->setBool("left", !Api->left);
	
	// Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle gemotry
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glColorMask(GL_TRUE,
		GL_TRUE,
		GL_TRUE,
		GL_TRUE);
}

/**
 * Render Function
 * */
void render()
{
    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** Draws code goes here **/
    // Use the shader
	if (!Api->stereoscopy)
	{
		shader->use();
		//Api->camera->stereoViewProjectionMatrices( 0.5, 10.0, Api->left);
		shader->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
		shader->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
		/*shader->setMat4("view", Api->camera->viewMatrix);
		shader->setMat4("projection", Api->camera->projectionMatrix);*/
		// Binds the vertex array to be drawn
        mesh->draw();
		//glBindVertexArray(VAO);
		// Renders the triangle gemotry
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);

        

	}
	else
	{
        shader->use();
       
        shader->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
        shader->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
        mesh->draw();
		//renderStereo();
	}
    // Swap the buffer
    
}
/**
 * App main loop
 * */
void update()
{
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Checks for keyboard inputs
        processKeyboardInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Renders everything
        render();
        parSystem->draw(Api->getDeltaTime(), Api->camera->getWorlToViewMatrix(Api->stereoscopy), Api->camera->getWorlToProjMatrix(Api->stereoscopy));
        
        renderImGui();
        // Check and call events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[])
{
    // Initialize all the app components
    if (!init())
    {
        // Something went wrong
        std::cin.ignore();
        return -1;
    }

    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    // Starts the app main loop
    update();

    // Deletes the texture from the gpu
    glDeleteTextures(1, &textureID);
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    // Destroy the shader
	delete shader;
	delete shaderStereo;
    delete parSystem;
    delete mesh;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Stops the glfw program
    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if(pressLeft)
        Api->camera->mouseUpdate(glm::vec2(xpos, ypos));

}

void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    pressLeft = action == GLFW_PRESS ? true : false;
}
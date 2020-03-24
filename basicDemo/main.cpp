#define GLM_ENABLE_EXPERIMENTAL
#define N_POINTLIGHTS 1
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
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
#include "Api/model/Obj.h"
#include "Api/light/light.h"
#include "Api/light/pointLight.h"
#include "Api/gbuffer/gbuffer.h"
#include "Api/ssao/SSAO.h"

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
Shader *shader, *shaderStereo, *shaderGBuff, *shaderLight, *shaderSSAO, *shaderSSAOBlur, *shaderSSAOLight;
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


std::vector<Obj*> objects;
light* dirLight;
 
vector<pointLight*> pLight;
//Load models
Mesh* mesh;

GBuffer m_gbuffer;
SSAO m_ssao;


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
    //ImGui::InputText("float", buf, IM_ARRAYSIZE(buf));

    if (ImGui::TreeNode("Directional Light"))
    {
        if (ImGui::SmallButton("ON/OFF")) {
            dirLight->changeONOFF();
        }
        ImGui::Text("Direction of light");
        ImGui::SliderFloat("float x", &dirLight->dir.x, -10.0f, 10.0f);
        ImGui::SliderFloat("float y", &dirLight->dir.y, -10.0f, 10.0f);
        ImGui::SliderFloat("float z", &dirLight->dir.z, -10.0f, 10.0f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Point Light"))
    {
        if (ImGui::SmallButton("ON/OFF")) {
            pLight[0]->changeONOFF();
        }

        ImGui::Text("Position of light");
        ImGui::InputFloat("input float x", &pLight[0]->pos.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("input float y", &pLight[0]->pos.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("input float z", &pLight[0]->pos.z, 0.01f, 1.0f, "%.3f");
        ImGui::TreePop();
    }
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

    /*glEnable(GL_FRAMEBUFFER_SRGB);*/

    ////Blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

//void renderQuad()
//{
//    if (quadVAO == 0)
//    {
//        float quadVertices[] = {
//            // positions        // texture Coords
//            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
//            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
//            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//        };
//        // setup plane VAO
//        glGenVertexArrays(1, &quadVAO);
//        glGenBuffers(1, &quadVBO);
//        glBindVertexArray(quadVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    }
//    glBindVertexArray(quadVAO);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glBindVertexArray(0);
//}

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
    shaderGBuff = new Shader("assets/shaders/gbuffer.vert", "assets/shaders/gbuffer.frag");
    shaderLight = new Shader("assets/shaders/lightPass.vert", "assets/shaders/lightPass.frag");
    shaderSSAO = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssao.frag");
    shaderSSAOBlur = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssaoBlur.frag");
    shaderSSAOLight = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssaoLight.frag");
    // Loads all the geometry into the GPU
    buildGeometry();
    parSystem = new particleSystem();
    // Loads the texture into the GPU
    //textureID = loadTexture("assets/textures/bricks2.jpg");
    string path = "assets/models/cat/catS.obj";
    path = "assets/models/cottage_obj/cottage_obj.obj";
    //Loads 3D model
    mesh = new Mesh();
    if (!mesh->LoadMesh(path))
    {
        cout << "no carg� modelo" << endl;
        return false;
    }
    else {
        cout << "carg� modelo" << endl;
        mesh->text.load("assets/textures/cottage_diffuse.png");
    }
    dirLight = new light(glm::vec3(-3.0f));
    for (int i = 0; i < N_POINTLIGHTS; i++)
    {
        pLight.push_back(new pointLight(glm::vec3(15.0f,20.0f,5.0f)));
    }

    m_gbuffer.Init(windowWidth, windowHeight);
    //objects.push_back(loadObj("assets/models/cat.obj"));
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
        delete shaderGBuff;
        delete shaderLight;
        delete shaderSSAO;
        delete shaderSSAOBlur;
        delete shaderSSAOLight;
        shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		shaderStereo = new Shader("assets/shaders/stereo.vert", "assets/shaders/stereo.frag");
        shaderGBuff = new Shader("assets/shaders/gbuffer.vert", "assets/shaders/gbuffer.frag");
        shaderLight = new Shader("assets/shaders/lightPass.vert", "assets/shaders/lightPass.frag");
        shaderSSAO = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssao.frag");
        shaderSSAOBlur = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssaoBlur.frag");
        shaderSSAOLight = new Shader("assets/shaders/lightPass.vert", "assets/shaders/ssaoLight.frag");
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
	//glBindVertexArray(VAO);
	//// Renders the triangle gemotry
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);
    mesh->draw();

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
	
	//// Binds the vertex array to be drawn
	//glBindVertexArray(VAO);
	//// Renders the triangle gemotry
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);
    mesh->draw();
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
		shader->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
		shader->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
        shader->setVec3("viewPos", Api->camera->position);
        shader->setInt("text", mesh->text.bind(0));
        //Directional light
        shader->setVec3("dirLight.pos", dirLight->pos);
        shader->setVec3("dirLight.dir", dirLight->dir);
        shader->setVec3("dirLight.ambient", dirLight->color.ambient);
        shader->setVec3("dirLight.diffuse", dirLight->color.diffuse);
        shader->setVec3("dirLight.specular", dirLight->color.specular);
        shader->setBool("dirLight.on", dirLight->ON);
        //Point light
        for (int ii = 0; ii < N_POINTLIGHTS; ii++)
        {
            std::string it = std::to_string(ii);
            shader->setVec3("pointLights[" + it + "].pos", pLight[ii]->pos);
            shader->setVec3("pointLights[" + it + "].ambientColor", pLight[ii]->color.ambient);
            shader->setVec3("pointLights[" + it + "].diffuseColor", pLight[ii]->color.diffuse);
            shader->setVec3("pointLights[" + it + "].specularColor", pLight[ii]->color.specular);
            shader->setVec3("pointLights[" + it + "].attenuationK", pLight[ii]->getKAttenuation());
            shader->setBool("pointLights[" + it + "].on", pLight[ii]->ON);
        }

	   
		// Binds the vertex array to be drawn
        mesh->draw();
		//glBindVertexArray(VAO);
		// Renders the triangle gemotry
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);
	}
	else
	{
		renderStereo();
	}

    // Swap the buffer
    
}

void geometryPass(Shader * shaderGBuff) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.getFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderGBuff->use();

    shaderGBuff->setMat4("view", Api->camera->getWorlToViewMatrix(Api->stereoscopy));
    shaderGBuff->setMat4("projection", Api->camera->getWorlToProjMatrix(Api->stereoscopy));
    shaderGBuff->setVec3("viewPos", Api->camera->position);
    shaderGBuff->setInt("text", mesh->text.bind(0));
    mesh->draw();
    //glBindVertexArray(VAO);
    //// Renders the triangle gemotry
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glBindVertexArray(0);
    //glViewport(0, 0, windowWidth, windowHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//void lightPass() {
//    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    shaderLight->use();
//    shaderLight->setInt("gPosition", 0);
//    shaderLight->setInt("gDiffuse", 1);
//    shaderLight->setInt("gNormal", 2);
//    shaderLight->setInt("gTextCoord", 3);
//    for (unsigned int i = 0; i < GBuffer::GBUFFER_NUM_TEXTURES; i++) {
//        glActiveTexture(GL_TEXTURE0 + i);
//        glBindTexture(GL_TEXTURE_2D, m_gbuffer.m_textures[i]);
//    }
//    //Directional light
//    shaderLight->setVec3("dirLight.pos", dirLight->pos);
//    shaderLight->setVec3("dirLight.dir", dirLight->dir);
//    shaderLight->setVec3("dirLight.ambient", dirLight->color.ambient);
//    shaderLight->setVec3("dirLight.diffuse", dirLight->color.diffuse);
//    shaderLight->setVec3("dirLight.specular", dirLight->color.specular);
//    shaderLight->setBool("dirLight.on", dirLight->ON);
//    //Point light
//    for (int ii = 0; ii < N_POINTLIGHTS; ii++)
//    {
//        std::string it = std::to_string(ii);
//        shaderLight->setVec3("pointLights[" + it + "].pos", pLight[ii]->pos);
//        shaderLight->setVec3("pointLights[" + it + "].ambientColor", pLight[ii]->color.ambient);
//        shaderLight->setVec3("pointLights[" + it + "].diffuseColor", pLight[ii]->color.diffuse);
//        shaderLight->setVec3("pointLights[" + it + "].specularColor", pLight[ii]->color.specular);
//        shaderLight->setVec3("pointLights[" + it + "].attenuationK", pLight[ii]->getKAttenuation());
//        shaderLight->setBool("pointLights[" + it + "].on", pLight[ii]->ON);
//    }
//    shaderLight->setVec3("viewPos", Api->camera->position);
//
//    renderQuad();
//    
//    m_gbuffer.BindForReading();
//    
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
//    glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    
//}

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

        //deferred
        geometryPass(shaderGBuff);

        m_gbuffer.lightPass(shaderLight, dirLight, pLight, N_POINTLIGHTS, windowWidth, windowHeight, Api->camera->position);
        //deferred

        //SSAO
        //geometryPass(shaderGBuff);

        //SSAO
        //render();
        //parSystem->draw(Api->getDeltaTime(), Api->camera->getWorlToViewMatrix(Api->stereoscopy), Api->camera->getWorlToProjMatrix(Api->stereoscopy),Api->camera->position);
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
    delete shaderGBuff;
    delete shaderLight;
    delete shaderSSAO;
    delete shaderSSAOBlur;
    delete shaderSSAOLight;
    delete parSystem;
    delete mesh;
    delete dirLight;
    //delete pLight;
    pLight.clear();
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
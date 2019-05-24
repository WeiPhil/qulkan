#include "examples/gt_reflection_aniso_two_bounces.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/opengl/compiler.h"
#include "framework/opengl/semantics.h"
#include "framework/opengl/vertex.h"

#include "qulkan/logger.h"
#include "qulkan/utils.h"

#include "utils/stb_image.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define sqr(i) (i * i)

GTReflectionAnisoTwoBounces::GTReflectionAnisoTwoBounces(const char *viewName, int initialRenderWidth, int initialRenderHeight)
    : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));   // top right
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));  // top left
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f))); // bottom left
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));  // bottom right

    eboManager.addTriangle(0, 1, 2);
    eboManager.addElements(2, 3, 0);
}

void GTReflectionAnisoTwoBounces::initHandles() {

    Handle layer1Text("layer1Text", Type::TEXT, "Layer 1");
    Handle u_alpha_x_1("Alpha x1", Type::FLOAT, 0.1f);
    Handle u_alpha_y_1("Alpha y1", Type::FLOAT, 0.1f);
    Handle layer2Text("layer2Text", Type::TEXT, "Layer 2");
    Handle u_alpha_x_2("Alpha x2", Type::FLOAT, 0.1f);
    Handle u_alpha_y_2("Alpha y2", Type::FLOAT, 0.1f);
    Handle u_Scale("Scale", Type::FLOAT, 1.0f);
    Handle u_Gamma("Gamma", Type::FLOAT, 1.0f);

    handleManager.addHandle(layer1Text);
    handleManager.addHandle(u_alpha_x_1);
    handleManager.addHandle(u_alpha_y_1);
    handleManager.addHandle(layer2Text);
    handleManager.addHandle(u_alpha_x_2);
    handleManager.addHandle(u_alpha_y_2);
    handleManager.addHandle(u_Scale);
    handleManager.addHandle(u_Gamma);
}

void GTReflectionAnisoTwoBounces::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_DEFAULT", "../data/shaders/pixelshader.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/gt_reflection_aniso_two_bounces.frag", GL_FRAGMENT_SHADER, compiler);

    programManager.addProgram("DEFAULT");

    glAttachShader(programManager("DEFAULT"), shaderManager("VERT_DEFAULT"));
    glAttachShader(programManager("DEFAULT"), shaderManager("FRAG_DEFAULT"));
    glLinkProgram(programManager("DEFAULT"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("DEFAULT")) && error;

    return;
}

void GTReflectionAnisoTwoBounces::initBuffer() {

    bufferManager.addBuffer("ELEMENT");
    bufferManager.addBuffer("VERTEX");

    glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT"));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboManager.getElementSize(), &eboManager.elementData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));
    glBufferData(GL_ARRAY_BUFFER, vaoManager.getVertexDataSize(), &vaoManager.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void GTReflectionAnisoTwoBounces::initTexture() {

    textureManager.addTexture("IMAGE", "../data/images/raw_vulkan.jpg");
    textureManager.addTexture("RENDERVIEW");

    glGenTextures(textureManager.size(), &textureManager.textures[0]);
    // Image texture
    glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE"));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    int width, height, nrChannels;
    unsigned char *texData = stbi_load(textureManager.texturePath("IMAGE"), &width, &height, &nrChannels, 0);

    if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture");
    }

    stbi_image_free(texData);

    return;
}

void GTReflectionAnisoTwoBounces::initVertexArray() {

    glGenVertexArrays(1, &vaoManager.id);
    glBindVertexArray(vaoManager.id);
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(semantic::attr::POSITION);

    glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoManager.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
    glEnableVertexAttribArray(semantic::attr::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT")); // Element buffer useless for now
    glBindVertexArray(0);

    return;
}

void GTReflectionAnisoTwoBounces::initFramebuffer() { return; }

void GTReflectionAnisoTwoBounces::initOpenGLOptions() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GTReflectionAnisoTwoBounces::init() {
    Qulkan::Logger::Info("%s: Initialisation\n", name());

    if (handleManager.getHandles().empty())
        initHandles();
    initProgram();
    initBuffer();
    initTexture();
    initVertexArray();
    initFramebuffer();
    initOpenGLOptions();

    if (!error) {
        Qulkan::Logger::Info("%s: Initialisation Done\n", name());
        initialized = true;
    } else
        Qulkan::Logger::Error("%s: An error Occured during initialisation\n", name());
}

void GTReflectionAnisoTwoBounces::clean() {
    glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
    glDeleteProgram(programManager("DEFAULT"));

    glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(1, &vaoManager.id);
}

/* Renders a simple OpenGL triangle in the rendering view */
void GTReflectionAnisoTwoBounces::render(int actualRenderWidth, int actualRenderHeight) {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glViewport(0, 0, actualRenderWidth, actualRenderHeight);

    glClearColor(0.09f, 0.09f, 0.09f, 0.94f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);

    glUseProgram(programManager("DEFAULT"));

    glm::vec2 mappedMouse = screenMousePos * 2.0f - 1.0f;
    mappedMouse.x = -mappedMouse.x;
    float r = glm::sqrt(sqr(mappedMouse.x) + sqr(mappedMouse.y));
    glm::vec3 wi = glm::vec3(0.0, 0.0, 1.0);

    if (r < 1.0f) {
        wi = glm::vec3(mappedMouse, glm::sqrt(1.0f - sqr(r)));
        glUniform3f(glGetUniformLocation(programManager("DEFAULT"), "wi"), wi.x, wi.y, wi.z);
    }

    glUniform1i(glGetUniformLocation(programManager("DEFAULT"), "u_PassNumber"), Qulkan::getFrameNumber());

    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_x_1"), handleManager("Alpha x1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_y_1"), handleManager("Alpha y1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_x_2"), handleManager("Alpha x1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_y_2"), handleManager("Alpha y1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Scale"), handleManager("Scale")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Gamma"), handleManager("Gamma")->getValue<float>());

    glBindVertexArray(vaoManager.id);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, eboManager.getElementCount(), GL_UNSIGNED_INT, 0, 1, 0);

    return;
}

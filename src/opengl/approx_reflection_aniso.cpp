#include "opengl/approx_reflection_aniso.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/compiler.h"
#include "framework/semantics.h"
#include "framework/vertex.h"

#include "qulkan/logger.h"
#include "qulkan/utils.h"

#include "utils/stb_image.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define sqr(i) (i * i)

ApproxReflectionAniso::ApproxReflectionAniso(const char *viewName, int renderWidth, int renderHeight)
    : Qulkan::RenderView(viewName, renderWidth, renderHeight) {
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));   // top right
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));  // top left
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f))); // bottom left
    vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));  // bottom right

    eboManager.addTriangle(0, 1, 2);
    eboManager.addElements(2, 3, 0);
}

void ApproxReflectionAniso::initHandles() {

    Handle layer1Text("layer1Text", Type::TEXT_HANDLE, "Layer 1");
    Handle u_alpha_x_1("Alpha x1", Type::FLOAT_HANDLE, 0.1f);
    Handle u_alpha_y_1("Alpha y1", Type::FLOAT_HANDLE, 0.1f);
    Handle u_Scale("Scale", Type::FLOAT_HANDLE, 1.0f);
    Handle u_Gamma("Gamma", Type::FLOAT_HANDLE, 1.0f);

    handleManager.addHandle(layer1Text);
    handleManager.addHandle(u_alpha_x_1);
    handleManager.addHandle(u_alpha_y_1);
    handleManager.addHandle(u_Scale);
    handleManager.addHandle(u_Gamma);
}

void ApproxReflectionAniso::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_DEFAULT", "../data/shaders/pixelshader.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_DEFAULT", "../data/shaders/approx_reflection_aniso_two_bounces.frag", GL_FRAGMENT_SHADER, compiler);

    programManager.addProgram("DEFAULT");

    glAttachShader(programManager("DEFAULT"), shaderManager("VERT_DEFAULT"));
    glAttachShader(programManager("DEFAULT"), shaderManager("FRAG_DEFAULT"));
    glLinkProgram(programManager("DEFAULT"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("DEFAULT")) && error;

    return;
}

void ApproxReflectionAniso::initBuffer() {

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

void ApproxReflectionAniso::readFileTo(std::ifstream &in, std::vector<float> &data) {

    std::string currentLine;
    std::string currentElem;
    int size = 0;
    while (std::getline(in, currentLine)) {
        std::istringstream iline(currentLine);
        while (getline(iline, currentElem, ',')) {
            float dataElem = stof(currentElem);
            data.push_back(dataElem);
        }
        ++size;
    }
}

void ApproxReflectionAniso::initTexture() {

    std::ifstream rtvx("../data/roughness_to_variance_on_x.data");

    if (!rtvx.good()) {
        std::cout << "Failed to open roughness_to_variance_on_x file." << std::endl;
        return;
    }

    std::ifstream rtvy("../data/roughness_to_variance_on_y.data");
    if (!rtvy.good()) {
        std::cout << "Failed to open roughness_to_variance_on_y file." << std::endl;
        return;
    }
    std::ifstream vtrx("../data/variance_to_roughness_on_x.data");
    if (!vtrx.good()) {
        std::cout << "Failed to open variance_to_roughness_on_x file." << std::endl;
        return;
    }
    std::ifstream vtry("../data/variance_to_roughness_on_y.data");
    if (!vtry.good()) {
        std::cout << "Failed to open variance_to_roughness_on_y file." << std::endl;
        return;
    }

    std::vector<float> roughness_to_variance_on_x;
    std::vector<float> roughness_to_variance_on_y;
    std::vector<float> variance_to_roughness_on_x;
    std::vector<float> variance_to_roughness_on_y;

    readFileTo(rtvx, roughness_to_variance_on_x);
    readFileTo(rtvy, roughness_to_variance_on_y);
    readFileTo(vtrx, variance_to_roughness_on_x);
    readFileTo(vtry, variance_to_roughness_on_y);

    float *roughness_to_variance = new float[512 * 512 * 4];
    float *variance_to_roughness = new float[512 * 512 * 4];

    int j = 0;
    for (int i = 0; i < 512 * 512 * 4; i += 4) {
        roughness_to_variance[i] = roughness_to_variance_on_x[j];
        roughness_to_variance[i + 1] = roughness_to_variance_on_y[j];
        roughness_to_variance[i + 2] = 0.0;
        roughness_to_variance[i + 3] = 1;
        variance_to_roughness[i] = variance_to_roughness_on_x[j];
        variance_to_roughness[i + 1] = variance_to_roughness_on_y[j];
        variance_to_roughness[i + 2] = 0.0;
        variance_to_roughness[i + 3] = 1;
        j++;
    }

    textureManager.addTexture("ROUGHNESS_TO_VARIANCE");
    textureManager.addTexture("VARIANCE_TO_ROUGHNESS");
    textureManager.addTexture("RENDERVIEW");

    glGenTextures(textureManager.size(), &textureManager.textures[0]);
    // Roughness to variance texture
    glBindTexture(GL_TEXTURE_2D, textureManager("ROUGHNESS_TO_VARIANCE"));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, &roughness_to_variance[0]);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Roughness to variance texture
    glBindTexture(GL_TEXTURE_2D, textureManager("VARIANCE_TO_ROUGHNESS"));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, &variance_to_roughness[0]);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Bind to programm
    glUseProgram(programManager("DEFAULT"));
    glUniform1i(glGetUniformLocation(programManager("DEFAULT"), "u_roughnessToVariance"), 0);
    glUniform1i(glGetUniformLocation(programManager("DEFAULT"), "u_varianceToRoughness"), 1);

    // Render texture
    glBindTexture(GL_TEXTURE_2D, textureManager("RENDERVIEW"));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] roughness_to_variance;
    delete[] variance_to_roughness;

    return;
}

void ApproxReflectionAniso::initVertexArray() {

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

void ApproxReflectionAniso::initFramebuffer() {

    framebufferManager.addFramebuffer("RENDERVIEW");

    // Create framebuffer and attach color texture
    glGenFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferManager("RENDERVIEW"));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureManager("RENDERVIEW"), 0);

    // Create a renderbuffer for depth/stencil operation and attach it to the framebuffer
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderWidth, renderHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        error = true;
        Qulkan::Logger::Error("FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}

void ApproxReflectionAniso::initOpenGLOptions() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ApproxReflectionAniso::init() {
    Qulkan::Logger::Info("%s: Initialisation\n", name());

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

void ApproxReflectionAniso::clean() {
    glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);

    glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(1, &vaoManager.id);
    glDeleteProgram(programManager("DEFAULT"));
}

/* Renders a simple OpenGL triangle in the rendering view */
ImTextureID ApproxReflectionAniso::render() {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferManager("RENDERVIEW"));

    glViewport(0, 0, renderWidth, renderHeight);

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager("ROUGHNESS_TO_VARIANCE"));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager("VARIANCE_TO_ROUGHNESS"));

    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_x_1"), handleManager("Alpha x1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_alpha_y_1"), handleManager("Alpha y1")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Scale"), handleManager("Scale")->getValue<float>());
    glUniform1f(glGetUniformLocation(programManager("DEFAULT"), "u_Gamma"), handleManager("Gamma")->getValue<float>());

    glBindVertexArray(vaoManager.id);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, eboManager.getElementCount(), GL_UNSIGNED_SHORT, 0, 1, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (ImTextureID)(intptr_t)textureManager("RENDERVIEW");
}

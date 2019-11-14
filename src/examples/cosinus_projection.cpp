#include "examples/cosinusProjection.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/opengl/compiler.h"
#include "framework/opengl/semantics.h"
#include "framework/opengl/vertex.h"
#include "qulkan/logger.h"
#include "qulkan/utils.h"
#include "utils/stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static GLuint texID = 0;

void CosinusProjection::createSphere(float radius) {

    int sectorCount = 64;
    int stackCount = 64;

    float x, y, z, xy;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
    float u, v;                                  // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);        // r * cos(u)
        z = radius * sinf(stackAngle);         // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            u = (float)j / sectorCount;
            v = (float)i / stackCount;
            // printf("%f %f %f\n", x, y, z);

            vaoManagerSphere.addVertex(glf::vertex_v3fv2f(glm::vec3(x, y, z), glm::vec2(u, v)));
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                eboManagerSphere.addTriangle(k1, k2, k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                eboManagerSphere.addTriangle(k1 + 1, k2, k2 + 1);
            }
        }
    }
}

void CosinusProjection::createDisk(float radius) {

    int res = 64;

    float step = 2.0f * M_PI / res;

    for (int i = 0; i < res; i++) {
        float t = step * i;

        vaoManagerDisk.addVertex(glf::vertex_v3fv3f(glm::vec3(radius * cos(t), radius * sin(t), 0.0f), glm::vec3(0.1f, 0.1f, 0.1f)));
    }
}

CosinusProjection::CosinusProjection(const char *viewName, int initialRenderWidth, int initialRenderHeight)
    : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
    createSphere(1.0f);
    createDisk(1.0f);
    vaoManagerVectors.addVertex(glm::vec3(1, 0, 0)); // mapped to wi
    vaoManagerVectors.addVertex(glm::vec3(0, 0, 0)); // center
    vaoManagerVectors.addVertex(glm::vec3(0, 0, 1)); // mapped to wo
}

void CosinusProjection::initHandles() {

    Handle distribution("distribution", Type::TEXT, "Cosine Projection");
    handleManager.addHandle(distribution);

    Handle scale("Scale", Type::FLOAT, 0.5f, 0.0f, 1.0f);
    handleManager.addHandle(scale);

    Handle cameraSetting("cameraSetting", Type::TEXT, "Camera Settings");
    handleManager.addHandle(cameraSetting);

    Handle perspective("Perspective", Type::BOOL, true);
    handleManager.addHandle(perspective);

    Handle fov("FoV", Type::FLOAT, 45.0f, 10.0f, 180.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->value));
    handleManager.addHandle(fov);

    Handle left("Left", Type::FLOAT, -1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(left);
    Handle right("Right", Type::FLOAT, 1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(right);
    Handle bottom("Bottom", Type::FLOAT, -1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(bottom);
    Handle top("Top", Type::FLOAT, 1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(top);

    Handle nearPlane("Near Plane", Type::FLOAT, 0.1f, 0.0001f, 50.0f);
    handleManager.addHandle(nearPlane);
    Handle farPlane("Far Plane", Type::FLOAT, 100.0f, 0.0001f, 1000.0f);
    handleManager.addHandle(farPlane);

    Handle cameraSpeed("Camera Speed", Type::FLOAT, 4.5f, 0.01f, 20.0f);
    handleManager.addHandle(cameraSpeed);
    Handle cameraSensitivity("Camera Sensitivity", Type::FLOAT, 0.3f, 0.001f, 1.0f);
    handleManager.addHandle(cameraSensitivity);
}

void CosinusProjection::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_MAIN", "../data/shaders/cosinusprojection_main.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_MAIN", "../data/shaders/cosinusprojection_main.frag", GL_FRAGMENT_SHADER, compiler);

    shaderManager.addShader("VERT_SPHERE_GRID", "../data/shaders/cosinusprojection_grid.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_SPHERE_GRID", "../data/shaders/cosinusprojection_grid.frag", GL_FRAGMENT_SHADER, compiler);

    shaderManager.addShader("VERT_DISK", "../data/shaders/cosinusprojection_disk.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_DISK", "../data/shaders/cosinusprojection_disk.frag", GL_FRAGMENT_SHADER, compiler);

    programManager.addProgram("MAIN");
    programManager.addProgram("DISK");
    programManager.addProgram("SPHERE_GRID");

    glAttachShader(programManager("MAIN"), shaderManager("VERT_MAIN"));
    glAttachShader(programManager("MAIN"), shaderManager("FRAG_MAIN"));
    glLinkProgram(programManager("MAIN"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("MAIN")) && error;

    glAttachShader(programManager("SPHERE_GRID"), shaderManager("VERT_SPHERE_GRID"));
    glAttachShader(programManager("SPHERE_GRID"), shaderManager("FRAG_SPHERE_GRID"));
    glLinkProgram(programManager("SPHERE_GRID"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("SPHERE_GRID")) && error;

    glAttachShader(programManager("DISK"), shaderManager("VERT_DISK"));
    glAttachShader(programManager("DISK"), shaderManager("FRAG_DISK"));
    glLinkProgram(programManager("DISK"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("DISK")) && error;

    return;
}

void CosinusProjection::initBuffer() {

    bufferManager.addBuffer("VERTEX_SPHERE");
    bufferManager.addBuffer("ELEMENT_SPHERE");
    bufferManager.addBuffer("VERTEX_DISK");

    glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_SPHERE"));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboManagerSphere.getElementSize(), &eboManagerSphere.elementData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_SPHERE"));
    glBufferData(GL_ARRAY_BUFFER, vaoManagerSphere.getVertexDataSize(), &vaoManagerSphere.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_DISK"));
    glBufferData(GL_ARRAY_BUFFER, vaoManagerDisk.getVertexDataSize(), &vaoManagerDisk.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void CosinusProjection::initTexture() {

    textureManager.addTexture("IMAGE_VULKAN1", "../data/images/raw_vulkan.jpg");
    textureManager.addTexture("RENDERVIEW");

    glGenTextures(textureManager.size(), &textureManager.textures[0]);
    // Image texture 1
    glBindTexture(GL_TEXTURE_2D, textureManager("IMAGE_VULKAN1"));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load first image
    int width, height, nrChannels;
    unsigned char *texData1 = stbi_load(textureManager.texturePath("IMAGE_VULKAN1"), &width, &height, &nrChannels, 0);

    if (texData1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData1);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        Qulkan::Logger::Error("Failed to load texture 1");
    }
    stbi_image_free(texData1);

    // Bind to programm
    glUseProgram(programManager("MAIN"));
    glUniform1i(glGetUniformLocation(programManager("MAIN"), "texture1"), 0);

    return;
}

void CosinusProjection::initVertexArray() {
    glGenVertexArrays(1, &vaoManagerSphere.id);
    glBindVertexArray(vaoManagerSphere.id);
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_SPHERE"));

    glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManagerSphere.getVertexSize(), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(semantic::attr::POSITION);

    glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoManagerSphere.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
    glEnableVertexAttribArray(semantic::attr::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind element buffer array to array ob
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_SPHERE"));
    glBindVertexArray(0);

    /////////////////////////////

    glGenVertexArrays(1, &vaoManagerDisk.id);
    glBindVertexArray(vaoManagerDisk.id);
    {
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_DISK"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManagerDisk.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::COLOR, 3, GL_FLOAT, GL_FALSE, vaoManagerDisk.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    /////////////////////////////

    return;
}

void CosinusProjection::initFramebuffer() { return; }

void CosinusProjection::init() {
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

void CosinusProjection::initOpenGLOptions() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void CosinusProjection::clean() {
    glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
    glDeleteProgram(programManager("MAIN"));
    glDeleteProgram(programManager("DISK"));

    glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(1, &vaoManagerSphere.id);
    glDeleteVertexArrays(1, &vaoManagerDisk.id);
}

/* Renders a simple OpenGL triangle in the rendering view */
void CosinusProjection::render(int actualRenderWidth, int actualRenderHeight) {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glViewport(0, 0, actualRenderWidth, actualRenderHeight);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera settings
    float radius = 3.0f;

    float sensitivity = handleManager("Camera Sensitivity")->getValue<float>();
    if (isMouseDragging(0)) {
        phi += -mouseDelta.x * sensitivity * Qulkan::getDeltaTime();
        theta += mouseDelta.y * sensitivity * Qulkan::getDeltaTime();

        if (theta > 90.0f)
            theta = 90.0f;
        if (theta < -90.0f)
            theta = -90.0f;

        cameraPos.x = radius * cos(glm::radians(theta)) * cos(glm::radians(phi));
        cameraPos.y = radius * cos(glm::radians(theta)) * sin(glm::radians(phi));
        cameraPos.z = radius * sin(glm::radians(theta));
    }

    auto fov = std::any_cast<float>(&handleManager("FoV")->value);
    if (mouseWheel != 0.0f) {
        if (*fov >= 1.0f && *fov <= 180.0f)
            *fov -= mouseWheel;
        if (*fov <= 1.0f)
            *fov = 1.0f;
        if (*fov >= 180.0f)
            *fov = 180.0f;
    }

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, glm::vec3(0.0f), cameraUp);

    glm::mat4 projection;
    float nearPlane = handleManager("Near Plane")->getValue<float>();
    float farPlane = handleManager("Far Plane")->getValue<float>();
    float isPerspective = handleManager("Perspective")->getValue<bool>();
    if (isPerspective) {
        projection =
            glm::perspective(glm::radians(handleManager("FoV")->getValue<float>()), (float)actualRenderWidth / actualRenderHeight, nearPlane, farPlane);
    } else {
        projection = glm::ortho(handleManager("Left")->getValue<float>(), handleManager("Right")->getValue<float>(), handleManager("Bottom")->getValue<float>(),
                                handleManager("Top")->getValue<float>(), nearPlane, farPlane);
    }

    // projection = glm::perspective(glm::radians(handleManager("FoV")->getValue<float>()), (float)actualRenderWidth / actualRenderHeight, nearPlane, farPlane);

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 mvp = projection * view * model;

    // Draw the Disk
    glUseProgram(programManager("DISK"));
    glBindVertexArray(vaoManagerDisk.id);
    {
        model = glm::mat4(1.0f);

        // Bind the model for the draw call
        // Bind view and projection matrix to shader program
        glUniformMatrix4fv(glGetUniformLocation(programManager("DISK"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("DISK"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(programManager("DISK"), "model"), 1, GL_FALSE, glm::value_ptr(model));
        // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
        glDrawArrays(GL_LINE_LOOP, 0, vaoManagerDisk.getVertexCount());
    }

    glUseProgram(programManager("MAIN"));
    glBindVertexArray(vaoManagerSphere.id);
    {
        // Bind view and projection matrix to shader program
        glUniformMatrix4fv(glGetUniformLocation(programManager("MAIN"), "mvp"), 1, GL_FALSE, glm::value_ptr(projection * view * model));
        glUniform1f(glGetUniformLocation(programManager("MAIN"), "u_scale"), handleManager("Scale")->getValue<float>());

        glDrawElements(GL_TRIANGLES, eboManagerSphere.getElementCount(), GL_UNSIGNED_INT, 0);
    }

    glUseProgram(programManager("SPHERE_GRID"));
    glBindVertexArray(vaoManagerSphere.id);
    {

        glUniformMatrix4fv(glGetUniformLocation(programManager("SPHERE_GRID"), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINES, eboManagerSphere.getElementCount(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    return;
}

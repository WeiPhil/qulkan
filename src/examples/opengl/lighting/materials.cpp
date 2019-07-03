#include "examples/opengl/lighting/materials.h"

#include <array>
#include <glm/glm.hpp>
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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace OpenGLExamples {

    void Materials::setCameraView() {
        // Camera settings
        float radius = 10.0f;

        float sensitivity = handleManager("Camera Sensitivity")->getValue<float>();
        if (isMouseDragging(0)) {
            yaw += mouseDelta.x * sensitivity * Qulkan::getDeltaTime();
            pitch += -mouseDelta.y * sensitivity * Qulkan::getDeltaTime();

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            front.y = sin(glm::radians(pitch));
            front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            cameraFront = glm::normalize(front);
        }

        if (mouseWheel != 0.0f) {
            auto fov = std::any_cast<float>(&handleManager("FoV")->value);
            if (*fov >= 1.0f && *fov <= 180.0f)
                *fov -= mouseWheel;
            if (*fov <= 1.0f)
                *fov = 1.0f;
            if (*fov >= 180.0f)
                *fov = 180.0f;
        }

        float cameraSpeed = handleManager("Camera Speed")->getValue<float>() * Qulkan::getDeltaTime();
        if (isKeyDown(GLFW_KEY_LEFT_SHIFT))
            cameraSpeed = cameraSpeed * 3.0f;
        if (isKeyDown(GLFW_KEY_W))
            cameraPos += cameraSpeed * cameraFront;
        if (isKeyDown(GLFW_KEY_S))
            cameraPos -= cameraSpeed * cameraFront;
        if (isKeyDown(GLFW_KEY_A))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (isKeyDown(GLFW_KEY_D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void Materials::createCube(VAOManager<glf::vertex_v3fv3f> &vaoManager) {

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f)));

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv3f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    void Materials::createGrid(int size) {
        glm::vec3 gridColor = glm::vec3(0.379, 0.379, 0.379);
        glm::vec3 xAxisColor = glm::vec3(0.617, 0.761, 0.384);
        glm::vec3 zAxisColor = glm::vec3(0.84, 0.476, 0.517);

        // Create a centered grid on the xz plane of side 2 * size

        for (int j = -size; j <= size; j++) {
            glm::vec3 color = j == 0 ? xAxisColor : gridColor;
            vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(j, 0.0, -(size + 1)), color));
            vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(j, 0.0, (size + 1)), color));
        }
        for (int j = -size; j <= size; j++) {
            glm::vec3 color = j == 0 ? zAxisColor : gridColor;
            vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(-(size + 1), 0.0, j), color));
            vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3((size + 1), 0.0, j), color));
        }
        for (int i = 0; i < (2 * size + 1) * 2 * 2; i += 2) {
            eboGrid.addLine(i, i + 1);
        }
    }

    Materials::Materials(const char *viewName, int initialRenderWidth, int initialRenderHeight)
        : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
        createCube(vaoLight);
        createCube(vaoCube);
        createGrid(100);
    }

    void Materials::initHandles() {

        Handle lightProperties("lightProperties", Type::TEXT_HANDLE, "Light Properties");
        handleManager.addHandle(lightProperties);

        Handle lightPosition("Light Position", Type::VEC3_HANDLE, glm::vec3(1.2, 1.0, 2.0), glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10));
        handleManager.addHandle(lightPosition);

        Handle lightAmbientColor("Ambient Color#light", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(lightAmbientColor);
        Handle lightDiffuseColor("Diffuse Color#light", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(lightDiffuseColor);
        Handle lightSpecularColor("Specular Color#light", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(lightSpecularColor);

        Handle objectMaterial("objectMaterial", Type::TEXT_HANDLE, "Object Material");
        handleManager.addHandle(objectMaterial);

        Handle ambientColor("Ambient Color#material", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(ambientColor);
        Handle diffuseColor("Diffuse Color#material", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(diffuseColor);
        Handle specularColor("Specular Color#material", Type::COLOR3_HANDLE, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(specularColor);
        Handle shininess("Shininess#material", Type::FLOAT_HANDLE, 32.0f, 0.0001f, 256.0f);
        handleManager.addHandle(shininess);

        Handle fov("FoV", Type::FLOAT_HANDLE, 45.0f, 10.0f, 180.0f);
        handleManager.addHandle(fov);

        Handle nearPlane("Near Plane", Type::FLOAT_HANDLE, 0.1f, 0.0001f, 50.0f);
        handleManager.addHandle(nearPlane);
        Handle farPlane("Far Plane", Type::FLOAT_HANDLE, 100.0f, 0.0001f, 1000.0f);
        handleManager.addHandle(farPlane);

        Handle cameraSpeed("Camera Speed", Type::FLOAT_HANDLE, 4.5f, 0.01f, 20.0f);
        handleManager.addHandle(cameraSpeed);
        Handle cameraSensitivity("Camera Sensitivity", Type::FLOAT_HANDLE, 0.1f, 0.001f, 1.0f);
        handleManager.addHandle(cameraSensitivity);
    }

    void Materials::initProgram() {

        Compiler compiler;

        // Cube shader
        shaderManager.addShader("VERT_CUBE", "../data/shaders/examples/lighting/materials_cube.vert", GL_VERTEX_SHADER, compiler);
        shaderManager.addShader("FRAG_CUBE", "../data/shaders/examples/lighting/materials_cube.frag", GL_FRAGMENT_SHADER, compiler);

        // Light shader
        shaderManager.addShader("VERT_LIGHT", "../data/shaders/examples/lighting/materials_light.vert", GL_VERTEX_SHADER, compiler);
        shaderManager.addShader("FRAG_LIGHT", "../data/shaders/examples/lighting/materials_light.frag", GL_FRAGMENT_SHADER, compiler);

        // Grid shader
        shaderManager.addShader("VERT_GRID", "../data/shaders/examples/lighting/grid.vert", GL_VERTEX_SHADER, compiler);
        shaderManager.addShader("FRAG_GRID", "../data/shaders/examples/lighting/grid.frag", GL_FRAGMENT_SHADER, compiler);

        programManager.addProgram("CUBE_SHADER");
        glAttachShader(programManager("CUBE_SHADER"), shaderManager("VERT_CUBE"));
        glAttachShader(programManager("CUBE_SHADER"), shaderManager("FRAG_CUBE"));
        glLinkProgram(programManager("CUBE_SHADER"));

        error = compiler.check() && error;
        error = compiler.check_program(programManager("CUBE_SHADER")) && error;

        programManager.addProgram("LIGHT_SHADER");
        glAttachShader(programManager("LIGHT_SHADER"), shaderManager("VERT_LIGHT"));
        glAttachShader(programManager("LIGHT_SHADER"), shaderManager("FRAG_LIGHT"));
        glLinkProgram(programManager("LIGHT_SHADER"));

        error = compiler.check() && error;
        error = compiler.check_program(programManager("LIGHT_SHADER")) && error;

        programManager.addProgram("GRID_SHADER");
        glAttachShader(programManager("GRID_SHADER"), shaderManager("VERT_GRID"));
        glAttachShader(programManager("GRID_SHADER"), shaderManager("FRAG_GRID"));
        glLinkProgram(programManager("GRID_SHADER"));

        error = compiler.check() && error;
        error = compiler.check_program(programManager("GRID_SHADER")) && error;

        return;
    }

    void Materials::initBuffer() {

        bufferManager.addBuffer("VERTEX");
        bufferManager.addBuffer("VERTEX_GRID");
        bufferManager.addBuffer("ELEMENT_GRID");

        glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

        // Bind ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_GRID"));
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboGrid.getElementSize(), &eboGrid.elementData[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Create vertex array object for the grid
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_GRID"));
        glBufferData(GL_ARRAY_BUFFER, vaoGrid.getVertexDataSize(), &vaoGrid.vertexData[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        assert(vaoCube.getVertexDataSize() == vaoLight.getVertexDataSize());

        // Create vertex array object for the cube and light (same data)
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));
        glBufferData(GL_ARRAY_BUFFER, vaoCube.getVertexDataSize(), &vaoCube.vertexData[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return;
    }

    void Materials::initTexture() { return; }

    void Materials::initVertexArray() {
        glGenVertexArrays(1, &vaoCube.id);
        glBindVertexArray(vaoCube.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoCube.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::NORMAL, 3, GL_FLOAT, GL_FALSE, vaoCube.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        /////////////////////////////

        glGenVertexArrays(1, &vaoLight.id);
        glBindVertexArray(vaoLight.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoLight.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        /////////////////////////////

        glGenVertexArrays(1, &vaoGrid.id);
        glBindVertexArray(vaoGrid.id);
        {
            glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_GRID"));

            glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoGrid.getVertexSize(), BUFFER_OFFSET(0));
            glEnableVertexAttribArray(semantic::attr::POSITION);

            glVertexAttribPointer(semantic::attr::COLOR, 3, GL_FLOAT, GL_FALSE, vaoGrid.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
            glEnableVertexAttribArray(semantic::attr::COLOR);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Bin element buffer array to array ob
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_GRID"));
        }
        glBindVertexArray(0);

        return;
    }

    void Materials::initFramebuffer() { return; }

    void Materials::init() {
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

    void Materials::initOpenGLOptions() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
    }

    void Materials::clean() {
        glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
        glDeleteProgram(programManager("CUBE_SHADER"));
        glDeleteProgram(programManager("LIGHT_SHADER"));

        glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
        glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
        glDeleteVertexArrays(1, &vaoCube.id);
        glDeleteVertexArrays(1, &vaoLight.id);
        glDeleteVertexArrays(1, &vaoGrid.id);
    }

    /* Renders a simple OpenGL triangle in the rendering view */
    void Materials::render(int actualRenderWidth, int actualRenderHeight) {
        ASSERT(initialized, std::string(name()) + ": You need to init the view first");

        glViewport(0, 0, actualRenderWidth, actualRenderHeight);

        glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setCameraView();

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        float nearPlane = handleManager("Near Plane")->getValue<float>();
        float farPlane = handleManager("Far Plane")->getValue<float>();

        projection =
            glm::perspective(glm::radians(handleManager("FoV")->getValue<float>()), (float)actualRenderWidth / actualRenderHeight, nearPlane, farPlane);

        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 lightPos = handleManager("Light Position")->getValue<glm::vec3>();
        glm::vec3 lightAmbientColor = handleManager("Ambient Color#light")->getValue<glm::vec3>();
        glm::vec3 lightDiffuseColor = handleManager("Diffuse Color#light")->getValue<glm::vec3>();
        glm::vec3 lightSpecularColor = handleManager("Specular Color#light")->getValue<glm::vec3>();
        glm::vec3 ambientColor = handleManager("Ambient Color#material")->getValue<glm::vec3>();
        glm::vec3 diffuseColor = handleManager("Diffuse Color#material")->getValue<glm::vec3>();
        glm::vec3 specularColor = handleManager("Specular Color#material")->getValue<glm::vec3>();
        float shininess = handleManager("Shininess#material")->getValue<float>();

        glUseProgram(programManager("CUBE_SHADER"));

        // Bind uniforms to the cube shader program
        // Object's material
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "material.ambient"), ambientColor.r, ambientColor.g, ambientColor.b);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "material.diffuse"), diffuseColor.r, diffuseColor.g, diffuseColor.b);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "material.specular"), specularColor.r, specularColor.g, specularColor.b);
        glUniform1f(glGetUniformLocation(programManager("CUBE_SHADER"), "material.shininess"), shininess);
        // Light properties
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "light.position"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "light.ambient"), lightAmbientColor.r, lightAmbientColor.g, lightAmbientColor.b);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "light.diffuse"), lightDiffuseColor.r, lightDiffuseColor.g, lightDiffuseColor.b);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "light.specular"), lightSpecularColor.r, lightSpecularColor.g, lightSpecularColor.b);

        // Eye position
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        // VP matrix
        glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the Cube
        glBindVertexArray(vaoCube.id);
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            // Bind the model for the draw call
            glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix3fv(glGetUniformLocation(programManager("CUBE_SHADER"), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
            glDrawArrays(GL_TRIANGLES, 0, vaoCube.getVertexCount());
        }

        glUseProgram(programManager("LIGHT_SHADER"));

        // Bind uniforms to the light shader program
        glUniform3f(glGetUniformLocation(programManager("LIGHT_SHADER"), "lightColor"), lightDiffuseColor.r, lightDiffuseColor.g, lightDiffuseColor.b);
        glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the Light
        glBindVertexArray(vaoLight.id);
        {
            model = glm::translate(glm::mat4(1.0f), lightPos);
            model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
            // Bind the model for the draw call
            glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "model"), 1, GL_FALSE, glm::value_ptr(model));
            // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
            glDrawArrays(GL_TRIANGLES, 0, vaoLight.getVertexCount());
        }

        glUseProgram(programManager("GRID_SHADER"));

        // Bind uniforms to the cube shader program
        glUniformMatrix4fv(glGetUniformLocation(programManager("GRID_SHADER"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("GRID_SHADER"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vaoGrid.id);
        {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
            glUniformMatrix4fv(glGetUniformLocation(programManager("GRID_SHADER"), "model"), 1, GL_FALSE, glm::value_ptr(model));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_GRID"));
            // glPointSize(64);
            glDrawElements(GL_LINES, eboGrid.getElementCount(), GL_UNSIGNED_INT, (void *)0);

            // glDrawArrays(GL_LINES, 0, vaoGrid.getVertexCount());
            // std::cout << eboGrid.getElementCount() << std::endl;
        }
        glBindVertexArray(0);

        return;
    }

} // namespace OpenGLExamples
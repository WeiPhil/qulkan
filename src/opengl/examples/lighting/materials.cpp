#include "opengl/examples/lighting/materials.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/compiler.h"
#include "framework/semantics.h"
#include "framework/vertex.h"
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

    void Materials::createCube(VAOManager<glf::vertex_v3fv2f> &vaoManager) {

        // Texture coordinates are of no use this time but we let them as a general framework
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));

        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f)));
        vaoManager.addVertex(glf::vertex_v3fv2f(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)));
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

    Materials::Materials(const char *viewName, int renderWidth, int renderHeight) : Qulkan::RenderView(viewName, renderWidth, renderHeight) {
        createCube(vaoLight);
        createCube(vaoCube);
        createGrid(100);
    }

    void Materials::initHandles() {

        Handle lightPosition("Light Position", Type::VEC3, glm::vec3(1.2, 1.0, 2.0), glm::vec3(-10, -10, -10), glm::vec3(10, 10, 10));
        handleManager.addHandle(lightPosition);

        Handle lightColor("Light Color", Type::COLOR3, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(lightColor);

        Handle objectColor("Object Color", Type::COLOR3, glm::vec3(1.0, 1.0, 1.0));
        handleManager.addHandle(objectColor);

        Handle fov("FoV", Type::FLOAT, 45.0f, 10.0f, 180.0f);
        handleManager.addHandle(fov);

        Handle nearPlane("Near Plane", Type::FLOAT, 0.1f, 0.0001f, 50.0f);
        handleManager.addHandle(nearPlane);
        Handle farPlane("Far Plane", Type::FLOAT, 100.0f, 0.0001f, 1000.0f);
        handleManager.addHandle(farPlane);

        Handle cameraSpeed("Camera Speed", Type::FLOAT, 4.5f, 0.01f, 20.0f);
        handleManager.addHandle(cameraSpeed);
        Handle cameraSensitivity("Camera Sensitivity", Type::FLOAT, 0.1f, 0.001f, 1.0f);
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

    void Materials::initTexture() {

        textureManager.addTexture("RENDERVIEW");

        glGenTextures(textureManager.size(), &textureManager.textures[0]);

        // Render texture
        glBindTexture(GL_TEXTURE_2D, textureManager("RENDERVIEW"));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        return;
    }

    void Materials::initVertexArray() {
        glGenVertexArrays(1, &vaoCube.id);
        glBindVertexArray(vaoCube.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoCube.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoCube.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        /////////////////////////////

        glGenVertexArrays(1, &vaoLight.id);
        glBindVertexArray(vaoLight.id);
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoLight.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoLight.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

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

    void Materials::initFramebuffer() {

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

    void Materials::init() {
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
    ImTextureID Materials::render() {
        ASSERT(initialized, std::string(name()) + ": You need to init the view first");

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferManager("RENDERVIEW"));

        glViewport(0, 0, renderWidth, renderHeight);

        glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setCameraView();

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        float nearPlane = handleManager("Near Plane")->getValue<float>();
        float farPlane = handleManager("Far Plane")->getValue<float>();

        projection = glm::perspective(glm::radians(handleManager("FoV")->getValue<float>()), (float)renderWidth / renderHeight, nearPlane, farPlane);

        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 lightColor = handleManager("Light Color")->getValue<glm::vec3>();
        glm::vec3 lightPos = handleManager("Light Position")->getValue<glm::vec3>();
        glm::vec3 objectColor = handleManager("Object Color")->getValue<glm::vec3>();

        glUseProgram(programManager("CUBE_SHADER"));

        // Bind uniforms to the cube shader program
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "lightColor"), lightColor.r, lightColor.g, lightColor.b);
        glUniform3f(glGetUniformLocation(programManager("CUBE_SHADER"), "objectColor"), objectColor.r, objectColor.g, objectColor.b);
        glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // // Draw the Cube
        // glBindVertexArray(vaoCube.id);
        // {
        //     model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        //     // Bind the model for the draw call
        //     glUniformMatrix4fv(glGetUniformLocation(programManager("CUBE_SHADER"), "model"), 1, GL_FALSE, glm::value_ptr(model));
        //     // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
        //     glDrawArrays(GL_TRIANGLES, 0, vaoCube.getVertexCount());
        // }

        glUseProgram(programManager("LIGHT_SHADER"));

        // Bind uniforms to the light shader program
        glUniform3f(glGetUniformLocation(programManager("LIGHT_SHADER"), "lightColor"), lightColor.r, lightColor.g, lightColor.b);
        glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // // Draw the Light
        // glBindVertexArray(vaoLight.id);
        // {
        //     model = glm::translate(glm::mat4(1.0f), lightPos);
        //     // Bind the model for the draw call
        //     glUniformMatrix4fv(glGetUniformLocation(programManager("LIGHT_SHADER"), "model"), 1, GL_FALSE, glm::value_ptr(model));
        //     // Draw a cube using draw arrays without any ebo to avoid having to set unique texture coordinates for each face.
        //     glDrawArrays(GL_TRIANGLES, 0, vaoLight.getVertexCount());
        // }

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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return (ImTextureID)(intptr_t)textureManager("RENDERVIEW");
    }

} // namespace OpenGLExamples
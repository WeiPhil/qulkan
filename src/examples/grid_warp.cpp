#include "examples/grid_warp.h"

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

void GridWarp::create3dGrid(float spacing, int size) {

    size = size % 2 == 0 ? size + 1 : size;
    int start = -(size - 1) / 2;
    int end = (size - 1) / 2;

    float x_pos = start * spacing;
    float y_pos = x_pos;
    float z_pos = x_pos;

    GLuint idx = 0;
    for (int depth = start; depth <= end; depth++) {
        z_pos = start * spacing;
        for (int i = start; i <= end; ++i) {
            x_pos = start * spacing;
            for (int j = start; j <= end; ++j) {
                vaoManager3dGrid.addVertex(glf::vertex_v3fv2f(glm::vec3(x_pos, y_pos, z_pos), glm::vec2(0.0, 0.0)));
                eboManager3dGrid.addPoint(idx);
                x_pos += spacing;
                idx += 1;
            }
            z_pos += spacing;
        }
        y_pos += spacing;
    }
    // for (int i = 0; i < (2 * size + 1) * 2 * 2; i += 2) {
    //     eboManager3dGrid.addLine(i, i + 1);
    // }
}

void GridWarp::createGrid(int size) {
    glm::vec3 gridColor = glm::vec3(0.379, 0.379, 0.379);
    glm::vec3 xAxisColor = glm::vec3(0.617, 0.761, 0.384);
    glm::vec3 zAxisColor = glm::vec3(0.84, 0.476, 0.517);

    // Create a centered grid on the xz plane of side 2 * size

    for (int j = -size; j <= size; j++) {
        glm::vec3 color = j == 0 ? xAxisColor : gridColor;
        vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(j, -(size + 1), 0.0), color));
        vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(j, (size + 1), 0.0), color));
    }
    for (int j = -size; j <= size; j++) {
        glm::vec3 color = j == 0 ? zAxisColor : gridColor;
        vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3(-(size + 1), j, 0.0), color));
        vaoGrid.addVertex(glf::vertex_v3fv3f(glm::vec3((size + 1), j, 0.0), color));
    }
    for (int i = 0; i < (2 * size + 1) * 2 * 2; i += 2) {
        eboGrid.addLine(i, i + 1);
    }
}

GridWarp::GridWarp(const char *viewName, int initialRenderWidth, int initialRenderHeight)
    : Qulkan::RenderView(viewName, initialRenderWidth, initialRenderHeight) {
    create3dGrid(0.05f, 20);
    createGrid(100);

    cameraPos.x = radius * cos(glm::radians(theta)) * cos(glm::radians(phi));
    cameraPos.y = radius * cos(glm::radians(theta)) * sin(glm::radians(phi));
    cameraPos.z = radius * sin(glm::radians(theta));
}

void GridWarp::initHandles() {

    Handle distribution("distribution", Type::TEXT_HANDLE, "GGX Distribution");
    handleManager.addHandle(distribution);

    Handle a00("A00", Type::FLOAT_HANDLE, 1.0f, -5.f, 5.0f);
    Handle a01("A01", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a02("A02", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a10("A10", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a11("A11", Type::FLOAT_HANDLE, 1.0f, -5.f, 5.0f);
    Handle a12("A12", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a20("A20", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a21("A21", Type::FLOAT_HANDLE, 0.0f, -5.f, 5.0f);
    Handle a22("A22", Type::FLOAT_HANDLE, 1.0f, -5.f, 5.0f);

    handleManager.addHandle(a00);
    handleManager.addHandle(a01);
    handleManager.addHandle(a02);
    handleManager.addHandle(a10);
    handleManager.addHandle(a11);
    handleManager.addHandle(a12);
    handleManager.addHandle(a20);
    handleManager.addHandle(a21);
    handleManager.addHandle(a22);

    Handle adjoint("Adjoint", Type::BOOL_HANDLE, false);
    handleManager.addHandle(adjoint);

    Handle wi_theta("Theta", Type::RADIAN_FLOAT_HANDLE, 0.0f, (float)-M_PI_2, (float)M_PI_2);
    handleManager.addHandle(wi_theta);

    Handle wi_phi("Phi", Type::RADIAN_FLOAT_HANDLE, 0.0f, 0.0f, (float)(2.0f * M_PI));
    handleManager.addHandle(wi_phi);

    Handle cameraSetting("cameraSetting", Type::TEXT_HANDLE, "Camera Settings");
    handleManager.addHandle(cameraSetting);

    Handle perspective("Perspective", Type::BOOL_HANDLE, true);
    handleManager.addHandle(perspective);

    Handle fov("FoV", Type::FLOAT_HANDLE, 45.0f, 10.0f, 180.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->value));
    handleManager.addHandle(fov);

    Handle left("Left", Type::FLOAT_HANDLE, -1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(left);
    Handle right("Right", Type::FLOAT_HANDLE, 1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(right);
    Handle bottom("Bottom", Type::FLOAT_HANDLE, -1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(bottom);
    Handle top("Top", Type::FLOAT_HANDLE, 1.05f, -10.0f, 10.0f, std::any_cast<bool>(&handleManager.getHandle("Perspective")->invValue));
    handleManager.addHandle(top);

    Handle nearPlane("Near Plane", Type::FLOAT_HANDLE, 0.1f, 0.0001f, 50.0f);
    handleManager.addHandle(nearPlane);
    Handle farPlane("Far Plane", Type::FLOAT_HANDLE, 100.0f, 0.0001f, 1000.0f);
    handleManager.addHandle(farPlane);

    Handle cameraSpeed("Camera Speed", Type::FLOAT_HANDLE, 4.5f, 0.01f, 20.0f);
    handleManager.addHandle(cameraSpeed);
    Handle cameraSensitivity("Camera Sensitivity", Type::FLOAT_HANDLE, 0.3f, 0.001f, 1.0f);
    handleManager.addHandle(cameraSensitivity);
}

void GridWarp::initProgram() {

    Compiler compiler;

    shaderManager.addShader("VERT_MAIN", "../data/shaders/grid_warp_main.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_MAIN", "../data/shaders/grid_warp_main.frag", GL_FRAGMENT_SHADER, compiler);

    // Grid shader
    shaderManager.addShader("VERT_GRID", "../data/shaders/examples/lighting/grid.vert", GL_VERTEX_SHADER, compiler);
    shaderManager.addShader("FRAG_GRID", "../data/shaders/examples/lighting/grid.frag", GL_FRAGMENT_SHADER, compiler);

    programManager.addProgram("MAIN");

    glAttachShader(programManager("MAIN"), shaderManager("VERT_MAIN"));
    glAttachShader(programManager("MAIN"), shaderManager("FRAG_MAIN"));
    glLinkProgram(programManager("MAIN"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("MAIN")) && error;

    programManager.addProgram("GRID_SHADER");
    glAttachShader(programManager("GRID_SHADER"), shaderManager("VERT_GRID"));
    glAttachShader(programManager("GRID_SHADER"), shaderManager("FRAG_GRID"));
    glLinkProgram(programManager("GRID_SHADER"));

    error = compiler.check() && error;
    error = compiler.check_program(programManager("GRID_SHADER")) && error;

    return;
}

void GridWarp::initBuffer() {

    bufferManager.addBuffer("VERTEX_3D_GRID");
    bufferManager.addBuffer("ELEMENT_3D_GRID");
    bufferManager.addBuffer("VERTEX_GRID");
    bufferManager.addBuffer("ELEMENT_GRID");

    glGenBuffers(bufferManager.size(), &bufferManager.buffers[0]);

    // Create vertex array object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_3D_GRID"));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboManager3dGrid.getElementSize(), &eboManager3dGrid.elementData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_3D_GRID"));
    glBufferData(GL_ARRAY_BUFFER, vaoManager3dGrid.getVertexDataSize(), &vaoManager3dGrid.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind grid ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_GRID"));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboGrid.getElementSize(), &eboGrid.elementData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create vertex array object for the grid
    glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_GRID"));
    glBufferData(GL_ARRAY_BUFFER, vaoGrid.getVertexDataSize(), &vaoGrid.vertexData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return;
}

void GridWarp::initTexture() {

    // Bind to programm
    glUseProgram(programManager("MAIN"));
    glUniform1i(glGetUniformLocation(programManager("MAIN"), "texture1"), 0);

    return;
}

void GridWarp::initVertexArray() {
    glGenVertexArrays(1, &vaoManager3dGrid.id);
    glBindVertexArray(vaoManager3dGrid.id);
    {
        glBindBuffer(GL_ARRAY_BUFFER, bufferManager("VERTEX_3D_GRID"));

        glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, vaoManager3dGrid.getVertexSize(), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(semantic::attr::POSITION);

        glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, vaoManager3dGrid.getVertexSize(), BUFFER_OFFSET(sizeof(glm::vec3)));
        glEnableVertexAttribArray(semantic::attr::TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bind element buffer array to array ob
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferManager("ELEMENT_3D_GRID"));
    }
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

void GridWarp::initFramebuffer() { return; }

void GridWarp::init() {
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

void GridWarp::initOpenGLOptions() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void GridWarp::clean() {
    glDeleteFramebuffers(framebufferManager.size(), &framebufferManager.framebuffers[0]);
    glDeleteProgram(programManager("MAIN"));
    glDeleteProgram(programManager("GRID_SHADER"));

    glDeleteBuffers(bufferManager.size(), &bufferManager.buffers[0]);
    glDeleteTextures(textureManager.size(), &textureManager.textures[0]);
    glDeleteVertexArrays(1, &vaoManager3dGrid.id);
    glDeleteVertexArrays(1, &vaoGrid.id);
}

/* Renders a simple OpenGL triangle in the rendering view */
void GridWarp::render(int actualRenderWidth, int actualRenderHeight) {
    ASSERT(initialized, std::string(name()) + ": You need to init the view first");

    glViewport(0, 0, actualRenderWidth, actualRenderHeight);

    glClearColor(0.233f, 0.233f, 0.233f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float sensitivity = handleManager("Camera Sensitivity")->getValue<float>();

    if (mouseWheel != 0.0f && radius >= 0.1f) {
        radius -= mouseWheel;
        if (radius < 0.1) {
            radius = 0.1f;
        }
    }

    if (isMouseDragging(0) || mouseWheel != 0.0f) {
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

    glm::mat3 transform = {{handleManager("A00")->getValue<float>(), handleManager("A01")->getValue<float>(), handleManager("A02")->getValue<float>()},
                           {handleManager("A10")->getValue<float>(), handleManager("A11")->getValue<float>(), handleManager("A12")->getValue<float>()},
                           {handleManager("A20")->getValue<float>(), handleManager("A21")->getValue<float>(), handleManager("A22")->getValue<float>()}};

    if (handleManager("Adjoint")->getValue<bool>()) {
        transform = glm::transpose(transform);
    }

    glUseProgram(programManager("MAIN"));
    glBindVertexArray(vaoManager3dGrid.id);
    {
        // Bind view and projection matrix to shader program
        glUniformMatrix4fv(glGetUniformLocation(programManager("MAIN"), "mvp"), 1, GL_FALSE, glm::value_ptr(projection * view * model));
        glUniformMatrix3fv(glGetUniformLocation(programManager("MAIN"), "transform"), 1, GL_FALSE, glm::value_ptr(transform));

        glDrawElements(GL_POINTS, eboManager3dGrid.getElementCount(), GL_UNSIGNED_INT, 0);
        // glPointSize(3);
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

#pragma once

#ifndef GRID_WARP_H
#define GRID_WARP_H

#include "qulkan/render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/opengl/buffermanager.h"
#include "framework/opengl/ebomanager.h"
#include "framework/opengl/framebuffermanager.h"
#include "framework/opengl/programmanager.h"
#include "framework/opengl/shadermanager.h"
#include "framework/opengl/texturemanager.h"
#include "framework/opengl/vaomanager.h"
#include "framework/opengl/vertex.h"

class GridWarp : public Qulkan::RenderView {

  private:
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;
    VAOManager<glf::vertex_v3fv2f> vaoManager3dGrid;
    EBOManager eboManager3dGrid;

    VAOManager<glf::vertex_v3fv3f> vaoGrid;
    EBOManager eboGrid;

    // Camera variables
    float phi = 0.0f;
    float theta = 30.0f;
    float radius = 3.0f;

    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraPos = glm::vec3(3.0f, 0.0f, 0.0f);

    void create3dGrid(float spacing, int size);
    void createGrid(int size);

  protected:
    void initHandles();

    void initProgram();

    void initBuffer();

    void initTexture();

    void initVertexArray();

    void initFramebuffer();

    void initOpenGLOptions();

    void clean();

  public:
    GridWarp(const char *viewName = "OpenGL Example: Grid Warp", int initialRenderWidth = 1920, int initialRenderHeight = 1080);
    virtual ~GridWarp() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual void render(int actualRenderWidth, int actualRenderHeight);
};

#endif // GRID_WARP_H
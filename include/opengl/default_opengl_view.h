#pragma once

#ifndef DEFAULT_OPENGL_VIEW_H
#define DEFAULT_OPENGL_VIEW_H

#include "qulkan/render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/buffermanager.h"
#include "framework/ebomanager.h"
#include "framework/framebuffermanager.h"
#include "framework/programmanager.h"
#include "framework/shadermanager.h"
#include "framework/texturemanager.h"
#include "framework/vaomanager.h"
#include "framework/vertex.h"
#include "imgui.h"

class DefaultOpenGLView : public Qulkan::RenderView {

  private:
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;
    VAOManager<glf::vertex_v3fv3f> vaoManager;
    EBOManager eboManager;

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
    DefaultOpenGLView(const char *viewName = "Default OpenGL View", int renderWidth = 1920, int renderHeight = 1080);
    virtual ~DefaultOpenGLView() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render();
};

#endif
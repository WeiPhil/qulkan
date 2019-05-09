#pragma once

#ifndef GGX_REFLECTION_H
#define GGX_REFLECTION_H

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
#include "framework/vbomanager.h"
#include "framework/vertex.h"
#include "imgui.h"

class GGXReflection : public Qulkan::RenderView {

  private:
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    TextureManager textureManager;
    ShaderManager shaderManager;
    ProgramManager programManager;
    VBOManager<glf::vertex_v3fv2f> vboManager;
    EBOManager eboManager;

  protected:
    void initProgram();

    void initBuffer();

    void initTexture();

    void initVertexArray();

    void initFramebuffer();

    void clean();

    void initHandles();

  public:
    GGXReflection(const char *viewName = "GGXReflection View", int renderWidth = 1920, int renderHeight = 1080);
    virtual ~GGXReflection() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render();
};

#endif
#pragma once

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include "imgui.h"
#include "qulkan/handlemanager.h"

#include <memory>

namespace Qulkan {

    class RenderView {

      private:
        const char *viewName;

      protected:
        int renderWidth;
        int renderHeight;
        HandleManager handleManager;

        bool initialized;
        bool error;

      public:
        RenderView(const char *viewName = "Render View", int renderWidth = 1920, int renderHeight = 1080)
            : renderWidth(renderWidth), renderHeight(renderHeight), viewName(viewName), initialized(false), error(false) {}
        virtual ~RenderView(){};

        /* Inits the render view */
        virtual void init() = 0;

        /* Returns a texture/rendered image as a ImTextureID pointer for ImGui to render to a renderview */
        virtual ImTextureID render() = 0;

        HandleManager &getHandleManager() { return handleManager; }

        int width() { return renderWidth; }
        int height() { return renderHeight; }
        const char *name() { return viewName; }
    };

} // namespace Qulkan

#endif
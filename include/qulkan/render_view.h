#pragma once

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include "imgui.h"
#include "qulkan/handlemanager.h"
#include "qulkan/utils.h"
#include <memory>

namespace Qulkan {

    // Make static singleton class maybe
    struct PreferenceManager {
        bool mouseOverlay;
        bool mouseMirror;
        int mirrorWith;
        int mirrorWithCombo;
        PreferenceManager(bool mouseOverlay, int mirrorWith, int mirrorWithCombo, bool mouseMirror)
            : mouseOverlay(mouseOverlay), mirrorWith(mirrorWith), mirrorWithCombo(mirrorWithCombo), mouseMirror(mouseMirror){};
    };

    class RenderView {

      private:
        const char *m_viewName;
        const int m_id;

        bool m_hasFocus;

      protected:
        glm::vec2 screenMousePos; // normalized inscreen mouse position
        int renderWidth;
        int renderHeight;
        HandleManager handleManager;
        PreferenceManager preferenceManager;

        bool initialized;
        bool error;

      public:
        RenderView(const char *viewName = "Render View", int renderWidth = 1920, int renderHeight = 1080)
            : m_id(Qulkan::getNextUniqueID()), m_hasFocus(false), screenMousePos(glm::vec2(0.0f, 0.0f)), renderWidth(renderWidth), renderHeight(renderHeight),
              m_viewName(viewName), initialized(false), error(false), preferenceManager(false, 0, 0, false) {}

        virtual ~RenderView(){};

        /* Inits the render view */
        virtual void init() = 0;

        /* Returns a texture/rendered image as a ImTextureID pointer for ImGui to render to a renderview */
        virtual ImTextureID render() = 0;

        HandleManager &getHandleManager() { return handleManager; }
        PreferenceManager &getPreferenceManager() { return preferenceManager; };

        int width() { return renderWidth; }
        int height() { return renderHeight; }
        void setMousePos(glm::vec2 mousePos) { screenMousePos = mousePos; }
        glm::vec2 getMousePos() { return screenMousePos; }
        void setFocus(bool focus) { m_hasFocus = focus; }
        bool hasFocus() { return m_hasFocus; }
        const char *name() { return m_viewName; }
        const int getId() const { return m_id; }
    };

} // namespace Qulkan

#endif
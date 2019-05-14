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

        bool m_isActive;

      protected:
        glm::vec2 screenMousePos; // normalized inscreen mouse position
        glm::vec2 inRectPos;
        glm::vec2 rectPosMin;
        glm::vec2 rectPosMax;
        int renderWidth;
        int renderHeight;
        HandleManager handleManager;
        PreferenceManager preferenceManager;

        bool initialized;
        bool error;

      public:
        RenderView(const char *viewName = "Render View", int renderWidth = 1920, int renderHeight = 1080)
            : m_id(Qulkan::getNextUniqueID()), m_isActive(false), screenMousePos(glm::vec2(0.5f, 0.5f)), renderWidth(renderWidth), renderHeight(renderHeight),
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

        glm::vec2 getInRectPos() { return inRectPos; }
        void setInRectPos(glm::vec2 pos) { inRectPos = pos; }

        glm::vec2 getRectPosMin() { return rectPosMin; }
        void setRectPosMin(glm::vec2 size) { rectPosMin = size; }

        glm::vec2 getRectPosMax() { return rectPosMax; }
        void setRectPosMax(glm::vec2 size) { rectPosMax = size; }

        void setMousePos(glm::vec2 mousePos) { screenMousePos = mousePos; }
        glm::vec2 getMousePos() { return screenMousePos; }

        void setActive(bool active) { m_isActive = active; }
        bool isActive() { return m_isActive; }

        const char *name() { return m_viewName; }
        const int getId() const { return m_id; }
    };

} // namespace Qulkan

#endif
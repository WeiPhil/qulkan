

#include "qulkan/inputshandler.h"

#include <glm/glm.hpp>

namespace Qulkan {

    void handleInputs(std::vector<RenderView *> &renderViews) {

        for (auto renderView : renderViews) {
            if (ImGui::IsMouseDragging(0) && ImGui::IsMouseHoveringRect(renderView->getRectPosMin(), renderView->getRectPosMax(), false) &&
                renderView->isActive()) {
                glm::vec2 renderRectSize = renderView->getRectPosMax() - renderView->getRectPosMin();

                renderView->setMousePos(renderView->getInRectPos() / renderRectSize); // set normalized coordinates
            }
        }
    }
} // namespace Qulkan
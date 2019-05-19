#pragma once

#ifndef INPUTSHANDLER_H
#define INPUTSHANDLER_H

#include "imgui.h"
#include "qulkan/render_view.h"

#include <vector>

namespace Qulkan {

    void handleInputs(std::vector<RenderView *> &renderViews);

} // namespace Qulkan

#endif

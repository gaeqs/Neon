//
// Created by gaeqs on 12/11/25.
//

#ifndef NEON_QTAPPLICATIONUTILS_H
#define NEON_QTAPPLICATIONUTILS_H
#include <imgui.h>

#ifdef USE_QT

namespace neon::vulkan
{

    ImGuiKey glfwKeyToImGuiKey(int keycode);

} // namespace neon::vulkan

#endif

#endif // NEON_QTAPPLICATIONUTILS_H

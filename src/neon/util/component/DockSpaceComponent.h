//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_DOCKSPACECOMPONENT_H
#define NEON_DOCKSPACECOMPONENT_H

#include <functional>

#include <neon/structure/Component.h>

namespace neon
{

    enum class DockSidebarPosition
    {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT
    };

    struct DockSidebar
    {
        DockSidebarPosition position;
        std::function<std::string()> nameProvider;
        float size;
        std::function<void()> compositor;
    };

    /**
     * Represents a component that manages a dock space within a GUI application.
     * <p>
     * A dock space is a customizable region that allows users to dock, move and
     * organize child windows. This component facilitates the creation and management
     * of such region, ensuring it is properly rendered and functional.
     * <p>
     * A dock space can contain sidebars: windows located at the sides of the dock space.
     * Use "addSidebar" and "removeSidebar" to manage them.
     * <p>
     * Even if you don't define any sidebar, you can create one top bar by using
     * "ImGui::BeginMainMenuBar()".
     */
    class DockSpaceComponent : public Component
    {
        int _dockFlags;
        bool _firstTime;

        std::vector<std::unique_ptr<DockSidebar>> _bars;

      public:

        DockSpaceComponent();

        void onUpdate(float deltaTime) override;

        /**
         * Registers a sidebar for this dock space.
         *
         * A sidebar is defined as a window located at one side of the dock space.
         * This region is not mutable by the user and can be used to show information
         * at the sides of the application.
         * <p>
         * To compose a sidebar, register it at the start of your application using
         * this method. Then, on each frame create an ImGui window with the same name
         * as the one provided to this method.
         * <p>
         * If you don't want to handle the creation of the window, you can provide
         * a compositor: a function that provides the elements inside the sidebar.
         * <p>
         * Sidebars have a render order: it's different to add two bottom sidebars
         * and one left sidebar than to add a bottom sidebar, a left sidebar and
         * another bottom sidebar. The result will be the following for each configuration:
         * <code>
         *  |||                 |||
         *  |||                 |||
         *  |||                 |||
         *  --------------      |||-----------
         *  --------------      --------------
         * </code>
         *
         * This method returns a pointer to the sidebar. You can modify the sidebar
         * through this struct.
         *
         * @param position the position of the sidebar: left, right, top or bottom.
         * @param name the name of the window that handles the sidebar.
         * @param size the size of the sidebar in pixels.
         * @param compositor the compositor or null.
         * @return the sidebar.
         */
        DockSidebar* addSidebar(DockSidebarPosition position, std::string name, float size,
                                std::function<void()> compositor = nullptr);

        /**
         * Registers a sidebar for this dock space.
         *
         * A sidebar is defined as a window located at one side of the dock space.
         * This region is not mutable by the user and can be used to show information
         * at the sides of the application.
         * <p>
         * To compose a sidebar, register it at the start of your application using
         * this method. Then, on each frame create an ImGui window with the same name
         * as the one provided to this method.
         * <p>
         * If you don't want to handle the creation of the window, you can provide
         * a compositor: a function that provides the elements inside the sidebar.
         * <p>
         * Sidebars have a render order: it's different to add two bottom sidebars
         * and one left sidebar than to add a bottom sidebar, a left sidebar and
         * another bottom sidebar. The result will be the following for each configuration:
         * <code>
         *  |||                 |||
         *  |||                 |||
         *  |||                 |||
         *  --------------      |||-----------
         *  --------------      --------------
         * </code>
         *
         * This method returns a pointer to the sidebar. You can modify the sidebar
         * through this struct.
         *
         * @param position the position of the sidebar: left, right, top or bottom.
         * @param nameProvider a function that provides the name of the window that handles the sidebar.
         * @param size the size of the sidebar in pixels.
         * @param compositor the compositor or null.
         * @return the sidebar.
         */
        DockSidebar* addSidebar(DockSidebarPosition position, std::function<std::string()> nameProvider, float size,
                                std::function<void()> compositor = nullptr);

        /**
         * Removes a sidebar created by the method "addSidebar".
         * @param bar the bar to erase.
         * @return whether the sidebar has been deleted.
         */
        bool removeSidebar(const DockSidebar* bar);
    };

    REGISTER_COMPONENT(DockSpaceComponent, "Dock Space")
} // namespace neon

#endif // NEON_DOCKSPACECOMPONENT_H

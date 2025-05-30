//
// Created by gaelr on 19/10/2022.
//

#ifndef NEON_COMPONENT_H
#define NEON_COMPONENT_H

#include <cstdint>

#include <neon/structure/GameObject.h>
#include <neon/structure/Identifiable.h>
#include <neon/structure/IdentifiableWrapper.h>
#include <neon/structure/ComponentRegister.h>

#define REGISTER_COMPONENT(clazz, name)                                             \
    namespace                                                                       \
    {                                                                               \
        struct _##clazz##_component_register_                                       \
        {                                                                           \
            _##clazz##_component_register_()                                        \
            {                                                                       \
                neon::ComponentRegister::instance().registerComponent<clazz>(name); \
            }                                                                       \
        };                                                                          \
        _##clazz##_component_register_ __##clazz##_register;                        \
    }
#define KEY_REGISTER_COMPONENT(key, clazz, name)                                \
    struct _##key##_component_register_                                         \
    {                                                                           \
        _##key##_component_register_()                                          \
        {                                                                       \
            neon::ComponentRegister::instance().registerComponent<clazz>(name); \
        }                                                                       \
    };                                                                          \
    _##key##_component_register_ __##key##_register;

namespace neon
{
    class Application;
    class AssetCollection;
    class TaskRunner;
    class Logger;

    /**
     * Represents a component inside a game object.
     * <p>
     * Components are represented by an identifier.
     * <p>
     * Components define behaviours to the attached game object.
     * You can add or remove components from a game object
     * using the appropriated methods.
     */
    class Component : public Identifiable
    {
        friend class GameObject;
        friend class ComponentCollection;

        template<class T>
        friend class IdentifiableWrapper;

        uint64_t _id;
        bool _started;
        bool _enabled;
        IdentifiableWrapper<GameObject> _gameObject;

      public:
        Component(const Component& component) = delete;

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         *
         * Creates a new component.
         */
        Component();

        /**
         * This game object's destructor.
         *
         * Children of this class may override
         * this destructor.
         */
        ~Component() override = default;

        /**
         * Returns the unique identifier of this component.
         * <p>
         * This value is immutable.
         *
         * @return the unique identifier.
         */
        [[nodiscard]] uint64_t getId() const override;

        /**
         * Returns a reference to this component's game object.
         * @return the game object.
         */
        [[nodiscard]] IdentifiableWrapper<GameObject> getGameObject() const;

        /**
         * @return whether this component has started.
         */
        bool hasStarted() const;

        /**
         * Returns whether this component is enabled.
         *
         * Components won't be called on any event
         * when they are disabled.
         *
         * @return whether this component is enabled.
         */
        [[nodiscard]] bool isEnabled() const;

        /**
         * Sets whether this component is enabled.
         *
         * Components won't be called on any event
         * when they are disabled.
         *
         * @param enabled whether this component is enabled.
         */
        void setEnabled(bool enabled);

        /**
         * Destroys this component.
         * <p>
         * All references to this component became invalid after this call.
         */
        void destroy();

        /**
         * Destroys this component on the next frame.
         */
        void destroyLater();

        /**
         * Virtual method invoked when this component is constructed.
         */
        virtual void onConstruction();

        /**
         * Virtual method invoked before this component's first tick.
         */
        virtual void onStart();

        /**
         * Virtual method invoked every game tick.
         * @param deltaTime the time between the last and the current tick, in seconds.
         */
        virtual void onUpdate(float deltaTime);

        /**
         * Virtual method invoked every game tick.
         * This method is invoked after all "onUpdate" methods
         * from all components have been called.
         *
         * @param deltaTime the time between the last and the current tick, in seconds.
         */
        virtual void onLateUpdate(float deltaTime);

        /**
         * Virtual method invoked before the draw pass starts.
         *
         * Use this method to upload graphic data to the GPU.
         */
        virtual void onPreDraw();

        /**
         * Virtual method invoked when a key is pressed, hold or released.
         * @param event the event.
         */
        virtual void onKey(const KeyboardEvent& event);

        /**
         * Virtual method invoked when a char from the keyboard is received.
         * @param event the event.
         */
        virtual void onChar(const CharEvent& event);

        /**
         * Virtual method invoked when a mouse button is pressed or released.
         * @param event the event.
         */
        virtual void onMouseButton(const MouseButtonEvent& event);

        /**
         * Virtual method invoked when the cursor is moved.
         * @param event the event.
         */
        virtual void onCursorMove(const CursorMoveEvent& event);

        /**
         * Virtual method invoked when the user scrolls.
         * @param event the event.
         */
        virtual void onScroll(const ScrollEvent& event);

        /**
         * Draws the editor of this component.
         *
         * Components must implement this method
         * if they want to be edited.
         */
        virtual void drawEditor();

        // region UTIL

        /**
         * Returns the room of this component's game object.
         * <p>
         * This method is an alias of getGameObject()->getRoom().
         * @return the room.
         */
        [[nodiscard]] inline Room* getRoom() const
        {
            return _gameObject->getRoom();
        }

        /**
         * Shortcut to the application.
         * @return the application where this component is running.
         */
        [[nodiscard]] Application* getApplication() const;

        /**
         * Shortcut to the application's AssetCollection.
         * @return the asset collection.
         */
        [[nodiscard]] AssetCollection& getAssets() const;

        /**
         * Shortcut to the application's TaskRunner.
         * @return the task runner.
         */
        [[nodiscard]] TaskRunner& getTaskRunner() const;

        /**
         * Shortcut to the application's Logger.
         * @return the logger.
         */
        [[nodiscard]] Logger& getLogger() const;

        /**
         * Prints an info message in the application's logger.
         * @param message the message.
         */
        void info(const std::string& message) const;

        /**
         * Prints a done message in the application's logger.
         * @param message the message.
         */
        void done(const std::string& message) const;

        /**
         * Prints a debug message in the application's logger.
         * @param message the message.
         */
        void debug(const std::string& message) const;

        /**
         * Prints a warning message in the application's logger.
         * @param message the message.
         */
        void warning(const std::string& message);

        /**
         * Prints an error message in the application's logger.
         * @param message the message.
         */
        void error(const std::string& message);

        /**
         * Concatenates the ImGUI identifier of this component to the
         * given text.
         * The identified won't be displayed in the ImGUI interface.
         * @param id the given text.
         * @return the concatenated string.
         */
        [[nodiscard]] std::string imGuiUId(const std::string& id) const
        {
            return id + "##" + std::to_string(_id);
        }

        // endregion
    };
} // namespace neon

#endif // NEON_COMPONENT_H

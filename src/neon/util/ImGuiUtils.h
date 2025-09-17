//
// Created by gaeqs on 26/7/25.
//

#ifndef NEON_IMGUIUTILS_H
#define NEON_IMGUIUTILS_H
#include <imgui.h>
#include <deque>
#include <vector>

namespace ImGui::neon
{

    struct LayoutItem
    {
        float sizeOrWeight = 0.0f;
        bool stretch = false;
        float minSize = 0.0f;
        bool useMinSize = false;
    };

    /**
     * @brief Specifies the layout flow direction for arranging items in a layout.
     *
     * Determines whether items are arranged horizontally or vertically within
     * an ImGui layout structure. Used in conjunction with layout components to
     * modify the rendering behavior.
     *
     * Enumerators:
     * - HORIZONTAL: Items are arranged in a row.
     * - VERTICAL: Items are arranged in a column.
     */
    enum class FlowDirection
    {
        HORIZONTAL,
        VERTICAL
    };

    /**
     * @brief Base class for linear layouts built on top of ImGui.
     *
     * Helps compose rows or columns where items can have a fixed size or stretch
     * to fill the remaining space proportionally to a weight. Instances must
     * persist across frames (render loops), as they store the state necessary to
     * compute sizes consistently.
     *
     * @code
     * ImGui::neon::RowLayout layout; // Save this instance between loops!
     * // ...
     * layout.begin();
     * layout.button("Test1");
     * layout.stretchedButton("Text2", 1.0f, 100.0f);
     * // If the layout does not support the element directly, you
     * // may use layout.popStretchedSize() and then layout.next().
     * ImGui::ProgressBar(0.5f, ImVec2(0.0f, layout.popStretchedSize()));
     * layout.next(true, 1.0f, ImGui::GetFrameHeight());
     * layout.text("Hello world!");
     * layout.end();
     * @endcode
     *
     * Lifecycle:
     *  - Call begin() once before emitting items for the current frame.
     *  - Emit items using helpers (button, text, stretchedButton, ...) or
     *    advance with next() to reserve a slot with custom flags.
     *  - For unsupported items you want to stretch, obtain the computed size with popStretchedSize().
     *  - Call end() once to finalize the layout for the current frame.
     *
     * Notes:
     *  - Stretch uses a weight to distribute the remaining space; minSize sets a lower bound.
     *  - Direction is given by getDirection(): rows are horizontal, columns are vertical.
     *  - The API is not thread-safe; use it only from the active ImGui context thread.
     */
    class LinearLayout
    {
      public:
        LinearLayout() = default;

        LinearLayout(const LinearLayout& other) = delete;

        virtual ~LinearLayout() = default;

        /**
         * @brief Retrieves the flow direction of the layout.
         *
         * Determines whether the layout arranges its elements horizontally or vertically.
         *
         * @return The flow direction of the layout as a FlowDirection enumeration value.
         */
        [[nodiscard]] virtual FlowDirection getDirection() const = 0;

        /**
         * @brief Prepares the layout for emitting UI elements in the current frame.
         *
         * This method must be called at the beginning of each frame before adding elements
         * to the layout. It initializes the internal state required for consistent size
         * computations and ensures proper alignment of elements based on the layout's
         * configuration.
         *
         * Notes:
         * - Failing to call this method before adding elements may result in incorrect behavior.
         * - The layout must persist between frames!
         */
        virtual void begin() = 0;

        /**
         * @brief Advances the layout to the next slot, with optional stretching behavior.
         *
         * This method moves the layout to the next position for UI element placement,
         * updating the internal state to ensure consistent alignment and spacing. It can
         * optionally stretch the current slot to occupy additional space or respect specific constraints.
         *
         * @param stretch If true, the current slot will stretch to fill the remaining available space. The default
         * value is false.
         * @param weight Specifies the proportional weight to distribute remaining space when stretch is true. The
         * default value is 1.0f.
         * @param minSize Sets the minimum size for the slot, ensuring it does not shrink below this value. The default
         * value is 0.0f.
         */
        virtual void next(bool stretch = false, float weight = 1.0f, float minSize = 0.0f) = 0;

        /**
         * @brief Retrieves and consumes the size allocated for a stretched element.
         *
         * This method returns the computed size of the next stretched slot and removes it from
         * the internal state. It is intended to be used when the layout does not directly support
         * an element, allowing manual management of stretched sizes for custom rendering.
         *
         * Notes:
         * - This method must only be called after the layout has been initialized with begin().
         * - Calling this method without a matching stretched element in the layout will result
         *   in undefined behavior.
         *
         * @return The size allocated for the next stretched slot in the layout.
         */
        [[nodiscard]] virtual float popStretchedSize() = 0;

        /**
         * @brief Finalizes the layout after emitting all UI elements for the current frame.
         *
         * This method must be called once after adding all elements to the layout within the
         * current frame. It ensures proper cleanup and resets the internal state, preparing
         * the layout for the next frame. Neglecting to call this method may result in
         * inconsistencies when rendering later frames.
         *
         * Notes:
         * - Should only be called after begin() and after all elements have been emitted.
         * - This method must be implemented by derived classes to define specific
         *   finalization behavior.
         */
        virtual void end() = 0;

        /**
         * @brief Adds a stretchable region to the layout.
         *
         * Defines a stretchable section within the layout using a weight factor to
         * proportionally allocate space and a minimum size constraint.
         *
         * @param weight The weight factor used to proportionally distribute the remaining space.
         *               A higher weight allocates more space to this region. Default is 1.0.
         * @param minSize The minimum size of the stretchable region in pixels. Default is 0.0.
         */
        virtual void stretch(float weight = 1.0f, float minSize = 0.0f) = 0;

        /**
         * @brief Creates a button within the layout and advances to the next position.
         *
         * Displays a button with the given label and size parameters, ensuring it is
         * properly placed within the layout. Upon being rendered, the layout
         * automatically advances to the next position to allow later elements to be added.
         *
         * @param label The text displayed on the button.
         * @param size The size of the button. If (0, 0) is provided, the size is auto-calculated by ImGui.
         * @return True if the button was pressed, false otherwise.
         */
        bool button(const char* label, ImVec2 size = ImVec2(0.0f, 0.0f));

        /**
         * @brief Creates a stretched button within the linear layout.
         *
         * This method places a button that stretches proportionally based on the given weight
         * and adheres to a minimum size constraint. The direction of stretching is determined by
         * the layout's flow direction (horizontal or vertical). The size perpendicular to the
         * stretching direction can also be set explicitly.
         *
         * @param label The text label displayed on the button.
         * @param weight The proportional weight used to allocate remaining space for this button.
         * @param minSize The minimum size constraint for the button along the stretching direction.
         *               If set to a negative value, the minimum size is computed based on the text size of the label.
         * @param perpendicularSize The fixed size of the button in the non-stretching direction.
         * @return True if the button was clicked during the current frame, false otherwise.
         */
        bool stretchedButton(const char* label, float weight = 1.0f, float minSize = -1.0f,
                             float perpendicularSize = 0.0f);

        /**
         * @brief Adds a text element to the linear layout.
         *
         * Renders a text component with the specified formatted string and advances the layout
         * to the next slot, accounting for the space occupied by this element.
         *
         * Notes:
         *  - The layout automatically progresses to the next slot after rendering the text.
         *  - Formatting supports standard printf-style syntax for the text input.
         *
         * @param fmt A printf-style format string specifying the text to display.
         *
         * @param ... Additional arguments that populate placeholders in the format string.
         */
        void text(const char* fmt, ...);
    };

    /**
     * @brief Represents a layout structure that arranges items in a single horizontal row.
     *
     * Manages the horizontal alignment and spacing of child elements within the layout.
     * Commonly used in user interfaces to organize elements in a linear, row-based fashion.
     */
    class RowLayout : public LinearLayout
    {
        std::vector<LayoutItem> _items;
        std::deque<float> _stretches;

        float _remainingRegionSize;
        float _totalStretchWeight;

      public:
        RowLayout();

        [[nodiscard]] FlowDirection getDirection() const override;

        void begin() override;

        void stretch(float weight = 1.0f, float minSize = 0.0f) override;

        void next(bool stretch = false, float weight = 1.0f, float minSize = 0.0f) override;

        [[nodiscard]] float popStretchedSize() override;

        void end() override;
    };

    /**
     * @brief Represents a layout structure that arranges items in a single vertical column.
     *
     * Manages the vertical alignment and spacing of child elements within the layout.
     * Commonly used in user interfaces to organize elements in a linear, column-based fashion.
     */
    class ColumnLayout : public LinearLayout
    {
        std::vector<LayoutItem> _items;
        std::deque<float> _stretches;

        float _remainingRegionSize;
        float _totalStretchWeight;

      public:
        ColumnLayout();

        [[nodiscard]] FlowDirection getDirection() const override;

        void begin() override;

        void stretch(float weight = 1.0f, float minSize = 0.0f) override;

        void next(bool stretch = false, float weight = 1.0f, float minSize = 0.0f) override;

        [[nodiscard]] float popStretchedSize() override;

        void end() override;
    };

    /**
     * @brief Begins a column-based layout with the specified identifier.
     *
     * Initializes or retrieves an existing instance of a column layout, allowing
     * for structured arrangement of UI elements in a columnar format.
     * The identifier is used to uniquely identify a specific layout within the current ImGui context.
     * If a layout with the provided ID does not exist, a new one is created.
     *
     * Remember to invoke layout.end() when the layout composition is finished.
     *
     * @param id The unique identifier for the column layout.
     * @return A reference to the initialized or retrieved ColumnLayout object.
     */
    ColumnLayout& BeginColumnLayout(const char* id);

    /**
     * @brief Begins a row-based layout with the specified identifier.
     *
     * Initializes or retrieves an existing instance of a row layout, allowing
     * for structured arrangement of UI elements in a linear format.
     * The identifier is used to uniquely identify a specific layout within the current ImGui context.
     * If a layout with the provided ID does not exist, a new one is created.
     *
     * Remember to invoke layout.end() when the layout composition is finished.
     *
     * @param id The unique identifier for the row layout.
     * @return A reference to the initialized or retrieved RowLayout object.
     */
    RowLayout& BeginRowLayout(const char* id);

} // namespace ImGui::neon

#endif // NEON_IMGUIUTILS_H

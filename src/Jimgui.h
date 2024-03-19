#pragma once

#include "Jovial/Renderer/TextRenderer.h"
#include "Jovial/Std/Vector2.h"

#ifndef JIMGUI_DEFAULT_PADDING
#define JIMGUI_DEFAULT_PADDING 6.0f
#endif

namespace jovial::jimgui {

    void set_font(Font *font);
    Font *get_font();

    bool button(Rect2 rect, const String &label);

    struct StringEditor {
        explicit StringEditor(String label = "") : label(label.move()) {}

        String string;
        String label;
        float padding = JIMGUI_DEFAULT_PADDING;

        bool double_click_to_input = false;

        enum {
            NORMAL_MODE,
            NUMARIC_INT_MODE,
            NUMARIC_FLOAT_MODE,
        } input_mode = NORMAL_MODE;

        size_t visual_begin = 0, visual_end = 0;

        size_t cursor_index = 0;
        bool has_mouse = false;
        bool no_string = true;

        void edit(Vector2 position, String &val);
        void edit_float(Vector2 position, float &val);
        void edit_int(Vector2 position, int &val);

        Rect2 get_rect(Vector2 position);

    protected:
        Vec<char> command_stack;
        enum {
            VIM_INSERT,
            VIM_NORMAL,
            VIM_VISUAL,
        } vim_mode = VIM_INSERT;

        void update_normal_mode();
        void update_insert_mode();

        void render(Vector2 position);
    };

    struct FloatEditor {
        explicit FloatEditor(String label = "") : string_editor(label.move()) {
            string_editor.double_click_to_input = true;
        }

        StringEditor string_editor;

        void edit(Vector2 position, float &val);
    };

    struct Vector2Editor {
        explicit Vector2Editor(String label = "") : label(label.move()) {}

        String label;
        FloatEditor x_editor;
        FloatEditor y_editor;

        void edit(Vector2 position, Vector2 &val);
    };

}// namespace jovial::jimgui

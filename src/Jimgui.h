#pragma once

#include "Jovial/Renderer/TextRenderer.h"
#include "Jovial/Std/Vector2.h"

#ifndef JIMGUI_DEFAULT_PADDING
#define JIMGUI_DEFAULT_PADDING 6.0f
#endif

namespace jovial::jimgui {

    void set_font(Font *font);
    Font *get_font();

    struct FloatEditor {
        explicit FloatEditor(String label = "") : label(label.move()) {}

        String string;
        String label;
        float padding = JIMGUI_DEFAULT_PADDING;

        void edit(Vector2 position, float &val);
    };

    struct StringEditor {
        explicit StringEditor(String label = "") : label(label.move()) {}

        bool no_string = true;
        String string;
        String label;
        float padding = JIMGUI_DEFAULT_PADDING;
        Vec<char> command_stack;

        enum {
            INSERT,
            NORMAL,
            VISUAL,
        } mode = INSERT;

        size_t visual_begin = 0, visual_end = 0;

        size_t cursor_index = 0;
        bool has_mouse = false;

        void edit(Vector2 position, String &val);
    };

}// namespace jovial::jimgui

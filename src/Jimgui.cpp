#include "Jimgui.h"
#include "Jovial/Components/Util/Clipboard.h"
#include "Jovial/Input/Input.h"
#include "Jovial/Renderer/TextRenderer.h"
#include "Jovial/Shapes/Color.h"
#include "Jovial/Shapes/ShapeDrawer.h"
#include "Jovial/Std/Char.h"
#include "Jovial/Std/String.h"

namespace jovial::jimgui {

    // TODO: quickly switch between input fields with ALT+[chosen shortcut]

    static Font *FONT = nullptr;
    static Vector2 DRAG_START = {-1, -1};


    void set_font(Font *font) {
        FONT = font;
    }

    Font *get_font() {
        JV_CORE_ASSERT(FONT, "You have not set Jimgui font! Set it with jimgui::set_font");
        return FONT;
    }

    bool button(Rect2 rect, const String &label) {
        bool overlaps = rect.overlaps(Input::get_mouse_position());

        Vector2 label_size = measure_text(label, get_font());
        label_size.y /= 2;
        Vector2 rect_size = rect.size();

        Vector2 label_position = rect.position() + (rect_size - label_size) / 2.0f;

        TextDrawProps text_props;
        text_props.color = Colors::White;
        text_props.z_index = 11;
        text_props.fix_start_pos = true;

        get_font()->draw(label_position, label, text_props);

        if (Input::is_pressed(Actions::LeftMouseButton) && overlaps) {
            rect = rect.expand(-5);
        }
        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);


        return Input::is_just_released(Actions::LeftMouseButton) && overlaps;
    }

    void checkbox(Rect2 rect, bool &checked) {
        bool overlaps = rect.overlaps(Input::get_mouse_position());

        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);

        if (Input::is_just_released(Actions::LeftMouseButton) && overlaps) { checked = !checked; }
        if (checked) {
            rendering::draw_line({rect.position(), rect.position() + rect.size()}, 3, {Colors::White});
        }
    }

    Rect2 draw_jimgui_box(Vector2 position, const String &label, const String &value, float padding) {
        float length = 0;

        if (!label.is_empty()) {
            length += measure_text(label.c_str(), get_font(), 0).x;
            length += padding * 2;
        }

        length += measure_text(value.c_str(), get_font()).x + padding;

        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;

        Rect2 rect{position.x, position.y - padding, position.x + length, position.y + get_font()->size};
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);

        if (!label.is_empty()) {
            draw_text(position, label, get_font(), {Colors::White});
            position.x += measure_text(label.c_str(), get_font()).x + padding;
        }

        draw_text(position, value, get_font(), {Colors::White});
        return rect;
    }

    bool update_drag_start(Rect2 &rect) {
        bool overlaps = rect.overlaps(Input::get_mouse_position());
        if (overlaps && Input::is_just_pressed(Actions::LeftMouseButton)) {
            DRAG_START = Input::get_mouse_position();
        }
        return overlaps;
    }

    void FloatEditor::edit(Vector2 position, float &val) {
        string_editor.edit_float(position, val);

        Rect2 rect = string_editor.get_rect(position);

        if (Input::is_just_pressed(Actions::LeftMouseButton)) {
            DRAG_START = Input::get_mouse_position();
        }
        if (rect.overlaps(DRAG_START) && Input::is_pressed(Actions::LeftMouseButton)) {
            auto dif = Input::get_mouse_delta().x / 5;
            if (dif != 0) {
                float signum = dif / math::abs(dif);
                if (Input::is_pressed(Actions::LeftControl)) {
                    val += dif * dif * signum / 100.0f;
                } else {
                    val += dif * dif * signum;
                }
                string_editor.no_string = true;
            }

            if (Input::is_pressed(Actions::LeftShift)) {
                val = math::round(val);
                string_editor.no_string = true;
            }
        }

        if (Input::is_just_released(Actions::LeftMouseButton)) {
            DRAG_START = {-1, -1};
        }
    }

    int get_closest_cursor_pos(const String &string, Vector2 pos, Vector2 target) {
        float test_pos = pos.x;

        float mpos = Input::get_mouse_position().x;

        for (int i = 0; i < string.size(); i++) {
            float next_test_pos = test_pos;
            auto len = measure_text(StringView(string, i, i + 1), get_font()).x;

            next_test_pos += len;

            if (math::abs(mpos - test_pos) < math::abs(mpos - next_test_pos)) {
                return i;
            }
            test_pos = next_test_pos;
        }

        return (int) string.size();
    }

    void next_pos_with_vim(int i, Vec<char> &command_stack, size_t &cursor_index, String &string) {
        switch (command_stack[i]) {
            case 'l': {
                cursor_index = math::MIN(cursor_index + 1, string.length());
                command_stack.clear();
            } break;

            case 'h': {
                if (cursor_index != 0) cursor_index -= 1;
                command_stack.clear();
            } break;

            case 'w': {
                cursor_index = math::MIN(cursor_index + 1, string.length());
                if (is_whitespace(string[cursor_index - 1])) {
                    while (is_whitespace(string[cursor_index - 1]) && cursor_index < string.length()) {
                        cursor_index++;
                    }
                } else {
                    while (is_alpha(string[cursor_index - 1]) && cursor_index < string.length()) {
                        cursor_index++;
                    }
                }
                command_stack.clear();
            } break;

            case 'b': {
                if (cursor_index != 0) cursor_index -= 1;
                if (cursor_index >= 1 && is_whitespace(string[cursor_index])) {
                    while (cursor_index >= 1 && is_whitespace(string[cursor_index - 1])) {
                        cursor_index--;
                    }
                } else {
                    while (cursor_index >= 1 && is_alpha(string[cursor_index - 1])) {
                        cursor_index--;
                    }
                }
                command_stack.clear();
            } break;

            case 'f': {
                if (i + 1 < command_stack.size()) {
                    char target = command_stack[i + 1];
                    for (size_t c = cursor_index + 1; c < string.size(); ++c) {
                        if (string[c] == target) {
                            cursor_index = c + 1;
                            break;
                        }
                    }
                    command_stack.clear();
                }
            } break;

            case '0': {
                cursor_index = 0;
                command_stack.clear();
            } break;

            default:
                break;
        }
    }

    void StringEditor::edit(Vector2 position, String &val) {
        input_mode = NORMAL_MODE;
        if (no_string) {
            string = val;
            no_string = false;
        }

        render(position);

        if (has_mouse) {
            switch (vim_mode) {
                case VIM_INSERT: {
                    update_insert_mode();

                    if (Input::is_typed(Actions::Enter)) {
                        val = string;
                    }
                } break;
                case VIM_NORMAL: {
                    update_normal_mode();
                } break;
                case VIM_VISUAL: {
                }
            }
        }
        if (Input::is_typed(Actions::Enter)) {
            no_string = true;
        }
    }

    void StringEditor::edit_float(Vector2 position, float &val) {
        input_mode = NUMARIC_FLOAT_MODE;
        if (no_string | !has_mouse) {
            string = to_string(val);
            no_string = false;
        }

        render(position);

        if (has_mouse) {
            switch (vim_mode) {
                case VIM_INSERT: {
                    update_insert_mode();

                    if (Input::is_typed(Actions::Enter)) {
                        bool error = false;
                        auto res = (float) atof(string.c_str(), &error);
                        if (!error) {
                            val = res;
                        }
                    }
                } break;
                case VIM_NORMAL: {
                    update_normal_mode();
                } break;
                case VIM_VISUAL: {
                }
            }
        }
        if (Input::is_typed(Actions::Enter)) {
            no_string = true;
        }
    }

    void StringEditor::edit_int(Vector2 position, int &val) {
        input_mode = NUMARIC_INT_MODE;
        if (no_string | !has_mouse) {
            string = to_string(val);
            no_string = false;
        }

        render(position);

        if (has_mouse) {
            switch (vim_mode) {
                case VIM_INSERT: {
                    update_insert_mode();

                    if (Input::is_typed(Actions::Enter)) {
                        bool error = false;
                        auto res = atoi(string.c_str(), &error);
                        if (!error) {
                            val = res;
                        }
                        no_string = true;
                    }
                } break;
                case VIM_NORMAL: {
                    update_normal_mode();
                } break;
                case VIM_VISUAL: {
                }
            }
        }
    }

    void StringEditor::update_normal_mode() {
        command_stack.append(Input::get_chars_typed());

        for (int i = 0; i < command_stack.size(); ++i) {
            next_pos_with_vim(i, command_stack, cursor_index, string);

            if (command_stack.is_empty()) continue;
            switch (command_stack[i]) {
                case 'd': {
                    if (i + 1 < command_stack.size()) {
                        switch (command_stack[i + 1]) {
                            case 'd': {
                                string.clear();
                                cursor_index = 0;
                            } break;
                            default: {
                                size_t from_index = cursor_index;
                                next_pos_with_vim(i + 1, command_stack, cursor_index, string);
                                size_t to_index = cursor_index;
                                if (from_index < to_index) {
                                    for (int c = (int) to_index; c > from_index; c--) {
                                        string.erase(c - 1);
                                    }
                                    cursor_index = from_index;
                                } else if (from_index > to_index) {
                                    for (int c = (int) from_index; c > to_index; c--) {
                                        string.erase(c - 1);
                                    }
                                    cursor_index = to_index;
                                }
                            } break;
                        }
                        command_stack.clear();
                    }
                } break;

                case 't': {
                    if (i + 1 < command_stack.size()) {
                        char target = command_stack[i + 1];
                        for (size_t c = cursor_index + 1; c < string.size(); ++c) {
                            if (string[c] == target) {
                                cursor_index = c;
                                break;
                            }
                        }
                        command_stack.clear();
                    }
                } break;

                case 'a': {
                    vim_mode = VIM_INSERT;
                    command_stack.clear();
                } break;

                case 'A': {
                    cursor_index = string.length();
                    vim_mode = VIM_INSERT;
                    command_stack.clear();
                } break;

                case 'i': {
                    if (cursor_index != 0) cursor_index -= 1;
                    vim_mode = VIM_INSERT;
                    command_stack.clear();
                } break;

                default: {
                }
            }
        }
    }

    void StringEditor::update_insert_mode() {
        if (!string.is_empty()) {
            if (Input::is_typed(Actions::Left)) {
                if (cursor_index != 0) cursor_index -= 1;
            }
            if (Input::is_typed(Actions::Right)) {
                cursor_index = math::MIN(cursor_index + 1, string.length());
            }

            if (Input::is_typed(Actions::Backspace) && cursor_index > 0) {
                if (Input::is_pressed(Actions::LeftControl) || Input::is_pressed(Actions::RightControl)) {
                    string.clear();
                    cursor_index = 0;
                } else {
                    string.erase((int) cursor_index - 1);
                    cursor_index -= 1;
                }
            }
        }

        if (Input::is_just_pressed(Actions::V)) {
            if (Input::is_pressed(Actions::LeftControl) || Input::is_pressed(Actions::RightControl)) {
                String paste(clipboard::get());
                for (int i = 0; i < paste.size(); ++i) {
                    if (paste[i] == '\n') {
                        paste[i] = ' ';
                    }
                }
                string += paste;
                cursor_index += paste.size();
            }
        }

        if (Input::is_just_pressed(Actions::C)) {
            if (Input::is_pressed(Actions::LeftControl) || Input::is_pressed(Actions::RightControl)) {
                clipboard::set(string.c_str());
            }
        }

        for (char ch: Input::get_chars_typed()) {
            switch (input_mode) {
                case NORMAL_MODE: {
                    string.insert(ch, (int) cursor_index);
                    cursor_index += 1;
                } break;
                case NUMARIC_INT_MODE: {
                    if (is_num(ch)) {
                        string.insert(ch, (int) cursor_index);
                        cursor_index += 1;
                    }
                } break;
                case NUMARIC_FLOAT_MODE: {
                    bool has_decimal = false;
                    bool has_e = false;
                    for (auto ch: string) {
                        if (ch == '.') has_decimal = true;
                        if (ch == 'e' || ch == 'E') has_e = true;
                    }

                    if (is_num(ch) || ch == '-' || (!has_decimal && ch == '.') || (!has_e && (ch == 'e' || ch == 'E'))) {
                        string.insert(ch, (int) cursor_index);
                        cursor_index += 1;
                    }
                } break;
                default:
                    JV_UNREACHABLE
            }
        }

        if (Input::is_typed(Actions::Escape)) {
            vim_mode = VIM_NORMAL;
        }
    }

    void StringEditor::render(Vector2 position) {
        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;

        Rect2 rect = get_rect(position);
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);

        if (!label.is_empty()) {
            draw_text(position, label, get_font(), {Colors::White});
            position.x += measure_text(label.c_str(), get_font()).x + padding;
        }

        draw_text(position, string, get_font(), {Colors::White});
        position.x += padding;

        bool overlaps = update_drag_start(rect);

        bool clicked = false;
        if (!double_click_to_input && Input::is_just_pressed(Actions::LeftMouseButton)) clicked = true;
        if (double_click_to_input && Input::just_double_clicked()) clicked = true;

        if (Input::is_just_pressed(Actions::LeftMouseButton)) {
            if (!rect.overlaps(DRAG_START)) has_mouse = false;
        }

        if (clicked) {
            has_mouse = rect.overlaps(DRAG_START);
            cursor_index = get_closest_cursor_pos(string, position, Input::get_mouse_position());
        }

        if (has_mouse) {
            Vector2 pos = position;
            pos.x += measure_text(StringView(string, 0, (int) cursor_index), get_font()).x;

            rendering::draw_line({pos, {pos.x, pos.y + get_font()->size}}, 2);
        }
    }

    Rect2 StringEditor::get_rect(Vector2 position) {
        float length = 0;

        if (!label.is_empty()) {
            length += measure_text(label.c_str(), get_font(), 0).x;
            length += padding * 2;
        }

        length += measure_text(string.c_str(), get_font()).x + padding;

        return {position.x, position.y - padding, position.x + length + padding, position.y + get_font()->size};
    }

    void Vector2Editor::edit(Vector2 position, Vector2 &val) {
        const float checkbox_length = get_font()->size;

        if (x_editor.string_editor.no_string) {
            x_editor.string_editor.string = to_string(val.x);
            x_editor.string_editor.no_string = false;
        }
        if (y_editor.string_editor.no_string) {
            y_editor.string_editor.string = to_string(val.y);
            y_editor.string_editor.no_string = false;
        }

        float label_length = measure_text(label.c_str(), get_font(), 0).x;
        float length = label_length + padding * 2;
        float x_editor_length = x_editor.string_editor.get_rect({}).size().x;
        float y_editor_length = y_editor.string_editor.get_rect({}).size().x;
        length += x_editor_length + padding;
        length += y_editor_length + padding;

        length += checkbox_length + padding;

        Rect2 rect{position.x - padding, position.y - padding, position.x + length, position.y + get_font()->size};

        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);

        position.x += padding;
        TextDrawProps text_props;
        text_props.color = Colors::White;
        text_props.fix_start_pos = true;
        draw_text(position, label, get_font(), text_props);
        position.x += label_length;
        position.x += padding;

        position.x += padding;
        x_editor.edit(position, val.x);
        position.x += x_editor.string_editor.get_rect(position).size().x;
        position.x += padding;

        position.x += padding;
        y_editor.edit(position, val.y);
        position.x += y_editor.string_editor.get_rect(position).size().x;

        checkbox({position - Vector2(padding, padding), position + Vector2(checkbox_length, checkbox_length)}, draw_pos);
        position.x += padding;

        if (draw_pos) {
            const float radius = get_font()->size / 2;
            rendering::draw_circle({radius, val});
            if (Input::is_just_pressed(Actions::LeftMouseButton) &&
                val.distance_squared_to(Input::get_mouse_position()) < radius * radius) {
                dragging = true;
            }
            if (Input::is_just_released(Actions::LeftMouseButton)) {
                dragging = false;
            }
            if (Input::is_pressed(Actions::LeftMouseButton) && dragging) {
                val += Input::get_mouse_delta();
            }
        }
    }

    void Rect2Editor::edit(Vector2 position, Rect2 &val) {
        const float checkbox_length = get_font()->size;

        if (x_editor.string_editor.no_string) {
            x_editor.string_editor.string = to_string(val.x);
            x_editor.string_editor.no_string = false;
        }
        if (y_editor.string_editor.no_string) {
            y_editor.string_editor.string = to_string(val.y);
            y_editor.string_editor.no_string = false;
        }
        if (w_editor.string_editor.no_string) {
            w_editor.string_editor.string = to_string(val.w);
            w_editor.string_editor.no_string = false;
        }
        if (h_editor.string_editor.no_string) {
            h_editor.string_editor.string = to_string(val.h);
            h_editor.string_editor.no_string = false;
        }

        float label_length = measure_text(label.c_str(), get_font(), 0).x;
        float length = label_length + padding * 2;
        float x_editor_length = x_editor.string_editor.get_rect({}).size().x;
        float y_editor_length = y_editor.string_editor.get_rect({}).size().x;
        float w_editor_length = w_editor.string_editor.get_rect({}).size().x;
        float h_editor_length = h_editor.string_editor.get_rect({}).size().x;
        length += x_editor_length + padding;
        length += y_editor_length + padding;
        length += w_editor_length + padding;
        length += h_editor_length + padding;

        length += checkbox_length + padding;

        Rect2 rect{position.x - padding, position.y - padding, position.x + length, position.y + get_font()->size};

        rendering::ShapeDrawProperties props;
        props.color = Colors::JovialLightGray;
        rendering::draw_rect2(rect, props);
        props.color = Colors::Black;
        rendering::draw_rect2_outline(rect, 1, props);

        position.x += padding;
        TextDrawProps text_props;
        text_props.color = Colors::White;
        text_props.fix_start_pos = true;
        draw_text(position, label, get_font(), text_props);
        position.x += label_length;
        position.x += padding;

        position.x += padding;
        x_editor.edit(position, val.x);
        position.x += x_editor.string_editor.get_rect(position).size().x;
        position.x += padding;

        position.x += padding;
        w_editor.edit(position, val.w);
        position.x += w_editor.string_editor.get_rect(position).size().x;
        position.x += padding;

        position.x += padding;
        h_editor.edit(position, val.h);
        position.x += h_editor.string_editor.get_rect(position).size().x;
        position.x += padding;

        position.x += padding;
        y_editor.edit(position, val.y);
        position.x += y_editor.string_editor.get_rect(position).size().x;

        checkbox({position - Vector2(padding, padding), position + Vector2(checkbox_length, checkbox_length)}, draw_pos);
        position.x += padding;

        if (draw_pos) {
            const float radius = get_font()->size / 2;
            rendering::draw_circle({radius, {val.x, val.y}});
            rendering::draw_circle({radius, {val.w, val.h}});
            if (Input::is_just_pressed(Actions::LeftMouseButton) &&
                Vector2(val.x, val.y).distance_squared_to(Input::get_mouse_position()) < radius * radius) {
                dragging_bottom_left = true;
            }
            if (Input::is_just_pressed(Actions::LeftMouseButton) &&
                Vector2(val.w, val.h).distance_squared_to(Input::get_mouse_position()) < radius * radius) {
                dragging_top_right = true;
            }
            if (Input::is_just_released(Actions::LeftMouseButton)) {
                dragging_bottom_left = false;
                dragging_top_right = false;
            }
            if (Input::is_pressed(Actions::LeftMouseButton) && dragging_bottom_left) {
                val.x += Input::get_mouse_delta().x;
                val.y += Input::get_mouse_delta().y;
            }
            if (Input::is_pressed(Actions::LeftMouseButton) && dragging_top_right) {
                val.w += Input::get_mouse_delta().x;
                val.h += Input::get_mouse_delta().y;
            }
            rendering::draw_rect2_outline(val, 3, {Colors::White});
        }
    }
}// namespace jovial::jimgui

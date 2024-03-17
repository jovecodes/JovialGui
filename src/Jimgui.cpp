#include "Jimgui.h"
#include "Jovial/Components/Util/Clipboard.h"
#include "Jovial/Input/Input.h"
#include "Jovial/Shapes/Color.h"
#include "Jovial/Shapes/ShapeDrawer.h"

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
        String str = to_string(val);

        Rect2 rect = draw_jimgui_box(position, label, str, padding);
        bool overlaps = update_drag_start(rect);

        if (rect.overlaps(DRAG_START) && Input::is_pressed(Actions::LeftMouseButton)) {
            auto dif = Input::get_mouse_delta().x / 5;
            if (dif != 0) {
                float signum = dif / math::abs(dif);
                if (Input::is_pressed(Actions::LeftControl)) {
                    val += dif * dif * signum / 100.0f;
                } else {
                    val += dif * dif * signum;
                }
            }

            if (Input::is_pressed(Actions::LeftShift)) {
                val = math::round(val);
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

    void StringEditor::edit(Vector2 position, String &val) {
        if (no_string) {
            string = val;
            no_string = false;
        }

        float length = 0;

        if (!label.is_empty()) {
            length += measure_text(label.c_str(), get_font(), 0).x;
            length += padding * 2;
        }

        length += measure_text(string.c_str(), get_font()).x + padding;

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

        draw_text(position, string, get_font(), {Colors::White});
        position.x += padding;

        bool overlaps = update_drag_start(rect);

        if (Input::is_just_pressed(Actions::LeftMouseButton)) {
            has_mouse = rect.overlaps(DRAG_START);
            cursor_index = 0;

            cursor_index = get_closest_cursor_pos(string, position, Input::get_mouse_position());
        }

        if (has_mouse) {
            Vector2 pos = position;
            pos.x += measure_text(StringView(string, 0, (int) cursor_index), get_font()).x;

            rendering::draw_line({pos, {pos.x, pos.y + get_font()->size}}, 2);

            if (!string.is_empty()) {
                if (Input::is_just_pressed(Actions::Left)) {
                    if (cursor_index != 1) cursor_index -= 1;
                }
                if (Input::is_just_pressed(Actions::Right)) {
                    cursor_index = math::MIN(cursor_index + 1, string.length());
                }

                if (Input::is_action_just_pressed(Actions::Backspace) && cursor_index > 0) {
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
                string.insert(ch, (int) cursor_index);
                cursor_index += 1;
            }

            if (Input::is_just_pressed(Actions::Enter)) {
                val = string;
            }
        }
    }
}// namespace jovial::jimgui

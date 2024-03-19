#include "Jimgui.h"
#include "Jovial/Components/Components2D.h"
#include "Jovial/Components/Util/LazyAssets.h"
#include "Jovial/JovialEngine.h"
#include "JovialGui.h"

using namespace jovial;

#define WINDOW_NAME "Jovial GUI"
#define WINDOW_SIZE Vector2(1280, 720)
#define WINDOW_RES Vector2(0, 0)

namespace Fonts {
    // Font *Normal = new
    LazyFont Normal(fs::Path::fonts() + "jet_brains.ttf", 20);
}// namespace Fonts

class World : public Node {
public:
    World() {
        jimgui::set_font(Fonts::Normal.get());

        add_child(label);
        add_child(anchor_label);
        // add_child(background);

        // auto hello_world = SharedPtr(new gui::Label("Hello, World", Fonts::Normal.get()));
        // background.add_gui_child(hello_world);

        anchor_label.set_color(Colors::White);
        anchor_label.set_is_greedy();
        anchor_label.set_anchor(gui::Anchors::BottomLeft);

        label.set_color(Colors::White);
        label.set_is_greedy();
        label.set_anchor(gui::Anchors::TopMiddle);
        label.align_mode = gui::AlignModes::Middle;

        // background.set_is_greedy();
        // background.props.color.a = 127;
    }

    void update() override {
        int d = 0;
        if (Input::is_char_typed('j')) {
            d = 1;
        }
        if (Input::is_char_typed('k')) {
            d = -1;
        }
        if (d) {
            int anchor_number = math::CLAMP((int) label.get_anchor() + d, 0, (int) gui::Anchors::BottomRight);
            auto anchor = (gui::Anchors) anchor_number;
            anchor_label.set_text(gui::anchor_to_string(anchor));
            label.set_anchor(anchor);
        }

        static float my_float = 1.0f;

        static jimgui::FloatEditor float_editor("My Float:");
        float_editor.edit({100, 100}, my_float);

        // static jimgui::FloatEditor pad_editor("Padding:");
        // pad_editor.edit({100, 140}, float_editor.padding);


        static String my_string = "hello, world";
        static jimgui::StringEditor str_editor("My String:");

        str_editor.edit({100, 180}, my_string);

        // jimgui::edit_float({100, 100}, my_float, "MyVec");

        // rendering::draw_rect2({{}, Input::get_mouse_position()});
    }

    gui::Label anchor_label{anchor_to_string(gui::Anchors::TopMiddle), Fonts::Normal.get()};
    gui::Label label{"Hello, Gui", Fonts::Normal.get()};
    gui::ColorRect background;
};

int main() {
    Jovial game;
    game.push_plugin(Window::create({WINDOW_NAME, WINDOW_SIZE, WINDOW_RES}));
    game.push_plugin(new gui::GuiPlugin);
    game.push_plugins(&plugins::default_plugins_2d);
    game.push_plugin(new NodePlugin(new World));

    game.run();
    return 0;
}

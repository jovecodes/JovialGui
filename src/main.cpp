#include "Jovial/Components/Components2D.h"
#include "Jovial/Components/Util/LazyAssets.h"
#include "Jovial/JovialEngine.h"
#include "Jovial/Renderer/Sprite2DRenderer.h"
#include "Jovial/Renderer/TextRenderer.h"
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
        add_child(label);
        add_child(anchor_label);

        anchor_label.set_color(Colors::White);
        anchor_label.set_is_greedy(true);
        anchor_label.set_anchor(Anchors::BottomLeft);

        label.set_color(Colors::White);
        label.set_is_greedy(true);
        label.set_anchor(Anchors::TopMiddle);
        label.align_mode = AlignModes::Middle;
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
            int anchor_number = math::CLAMP((int) label.get_anchor() + d, 0, (int) Anchors::BottomRight);
            auto anchor = (Anchors) anchor_number;
            anchor_label.set_text(anchor_to_string(anchor));
            label.set_anchor(anchor);
        }
    }

    Label anchor_label{anchor_to_string(Anchors::TopMiddle), Fonts::Normal.get()};
    Label label{"Hello, Gui", Fonts::Normal.get()};
};

int main() {
    Jovial game;
    game.push_plugin(Window::create({WINDOW_NAME, WINDOW_SIZE, WINDOW_RES}));
    game.push_plugin(new GuiPlugin);
    game.push_plugins(&plugins::default_plugins_2d);
    game.push_plugin(new NodePlugin(new World));

    game.run();
    return 0;
}

#include "Jovial/Components/Components2D.h"
#include "Jovial/JovialEngine.h"
#include "Jovial/Renderer/TextRenderer.h"
#include "JovialGui.h"

using namespace jovial;

#define WINDOW_NAME "Jovial GUI"
#define WINDOW_SIZE Vector2(1280, 720)
#define WINDOW_RES Vector2(0, 0)

static std::vector<TextureBank::NamedTexture> textures_to_load() {
    return {
            // {"todo", new Texture(fs::Path::assets() + "todo.png")},
    };
}

static std::vector<FontBank::NamedFont> fonts_to_load() {
    return {
            {"normal", new Font(fs::Path::fonts() + "jet_brains.ttf", 20)},
    };
}

class MainLayer : public NodeLayer {
public:
    MainLayer() {
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
        rendering::draw_circle({5, {label.get_global_position()}});

        int d = 0;
        if (Input::is_action_just_pressed(Actions::J)) {
            d = 1;
        }
        if (Input::is_action_just_pressed(Actions::K)) {
            d = -1;
        }
        if (d) {
            int anchor_number = std::clamp((int) label.get_anchor() + d, 0, (int) Anchors::BottomRight);
            auto anchor = (Anchors) anchor_number;
            anchor_label.set_text(anchor_to_string(anchor));
            label.set_anchor(anchor);
        }
    }

    Label anchor_label = {anchor_to_string(Anchors::TopMiddle)};
    Label label = {"Hello, Gui"};
};

class Game : public Jovial {
public:
    Game() {
        push_plugin(new TextureBank(&textures_to_load));
        push_plugin(new FontBank(&fonts_to_load));
        push_plugin(Window::create({WINDOW_NAME, WINDOW_SIZE, WINDOW_RES}));
        push_plugin(new GuiPlugin);
        push_plugins(&plugins::default_plugins_2d);

        push_layer(new MainLayer);
    }
};

Jovial *jovial_main() {
    auto *game = new Game();
    return game;
}

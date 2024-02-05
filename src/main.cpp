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
        label.set_color(Colors::White);
        label.align_mode = AlignModes::Middle;
    }

    void update() override {
        label.transform.extent = {{Window::get_current_vsize() / 2.0f}, {0, 0}};
        rendering::draw_circle({5, {label.transform.extent.x, label.transform.extent.y}});
    }

    Label label = {"Hello, Gui"};
};

class Game : public Jovial {
public:
    Game() {
        push_plugin(new TextureBank(&textures_to_load));
        push_plugin(new FontBank(&fonts_to_load));
        push_plugin(Window::create({WINDOW_NAME, WINDOW_SIZE, WINDOW_RES}));
        push_plugins(&plugins::default_plugins_2d);

        push_layer(new MainLayer);
    }
};

Jovial *jovial_main() {
    auto *game = new Game();
    return game;
}

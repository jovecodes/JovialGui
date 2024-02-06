#pragma once

#include "Jovial/Components/2D/Transform2D.h"
#include "Jovial/Components/Components2D.h"
#include "Jovial/Core/Assert.h"
#include "Jovial/Core/Logger.h"
#include "Jovial/Event/ApplicationEvent.h"
#include "Jovial/Event/Event.h"
#include "Jovial/JovialEngine.h"
#include "Jovial/Renderer/Material.h"
#include "Jovial/Renderer/TextRenderer.h"

using namespace jovial;

enum class AlignModes {
    Left,
    Middle,
    Right,
};

enum class Anchors {
    TopLeft,
    TopMiddle,
    TopRight,
    MiddleLeft,
    Middle,
    MiddleRight,
    BottomLeft,
    BottomMiddle,
    BottomRight,
};

std::string anchor_to_string(Anchors anchor);

class GuiNode;

class GuiPlugin : public Plugin {
public:
    GuiPlugin() {
        instance = this;
    }

    static void add_gui_head(GuiNode *head);

    static void remove_gui_head(GuiNode *head) {
        JV_CORE_ASSERT(instance, "GuiPlugin is NULL! Make sure to add it as a plugin!")
        instance->gui_heads.erase(head);
    }

private:
    std::set<GuiNode *> gui_heads{};
    static GuiPlugin *instance;

    void init() override {}
    void deinit() override {}
    void on_event(Event &event) override;
    bool on_win_resize(WindowResizeEvent &_);
};

class GuiNode : public Node {
public:
    virtual void on_resize();
    Vector2 get_global_position() const;
    void set_is_greedy(bool greedy);
    void add_gui_child(const std::shared_ptr<GuiNode> &node);

    inline Anchors get_anchor() { return anchor; }
    inline void set_anchor(Anchors anchor) {
        this->anchor = anchor;
        on_resize();
    }

protected:
    GuiNode *gui_parent = nullptr;

    void birth() override {
        if (!gui_parent) {
            GuiPlugin::add_gui_head(this);
            on_resize();
        }
    }
    void set_extent(Rect2 extent);
    inline Rect2 get_extent() { return extent; }

    void resize_all();

private:
    Rect2 extent{};
    Rect2 max_size{};
    bool greedy = false;
    Anchors anchor = Anchors::Middle;
    std::vector<std::shared_ptr<GuiNode>> gui_children;
};

class Label : public GuiNode {
public:
    Label(std::string text, Font *font = nullptr, TextDrawProps props = {});

    void set_text(const std::string &text);

    void set_color(glm::vec4 color) { this->props.color = color; }
    std::string &get_text() { return text; }
    Vector2 get_size() { return size; }

    bool visable = true;
    AlignModes align_mode = AlignModes::Left;

protected:
    TextDrawProps props;
    Font *font;
    glm::vec2 size{};
    std::string text;

protected:
    void update() override;
    void recalc_size();
};

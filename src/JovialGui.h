#pragma once

#include "Jovial/Components/2D/Transform2D.h"
#include "Jovial/Components/Components2D.h"
#include "Jovial/JovialEngine.h"
#include "Jovial/Renderer/Material.h"
#include "Jovial/Renderer/TextRenderer.h"

using namespace jovial;

class GuiTransfrom : public Node {
public:
    GuiTransfrom(GuiTransfrom *parent = nullptr) : parent(parent) {}

    Rect2 extent;

    Vector2 get_global_position() const {
        return {extent.x, extent.y};
    }

private:
    GuiTransfrom *parent;
};

enum class AlignModes {
    Left,
    Middle,
    Right,
};

class Label : public Node {
public:
    Label(std::string text, Font *font = nullptr, TextDrawProps props = {})
        : text(std::move(text)), props(props), font(font) {
        if (!font) {
            this->font = FontBank::get_normal();
        }
        recalc_size();
        add_child(transform);
    }

    void set_text(const std::string &text) {
        this->text = text;
        recalc_size();
    }

    void set_color(glm::vec4 color) { this->props.color = color; }
    std::string &get_text() { return text; }
    Vector2 get_size() { return size; }

    bool visable = true;
    GuiTransfrom transform;
    AlignModes align_mode = AlignModes::Left;

protected:
    TextDrawProps props;
    Font *font;
    glm::vec2 size{};
    std::string text;

protected:
    void update() override {
        if (!visable) return;

        Vector2 pos = transform.get_global_position();
        switch (align_mode) {
            case AlignModes::Left:
                break;
            case AlignModes::Middle:
                pos.x -= size.x / 2.0f;
                break;
            case AlignModes::Right:
                pos.x -= size.x;
                break;
        }

        font->draw(pos, text.c_str(), props);
    }

    void recalc_size() {
        size = font->measure(text.c_str(), props.font_size);
    }
};

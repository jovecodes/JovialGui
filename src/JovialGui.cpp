#include "JovialGui.h"
#include "Jovial/Core/Logger.h"

GuiPlugin *GuiPlugin::instance = nullptr;

bool GuiPlugin::on_win_resize(WindowResizeEvent &_) {
    for (auto node: gui_heads) {
        node->on_resize();
    }
    return false;
}

void GuiPlugin::on_event(Event &event) {
    EventDispatcher d(event);
    d.dispatch<WindowResizeEvent>(BIND_EVENT_FN(GuiPlugin::on_win_resize));
}

void GuiNode::on_resize() {
    if (gui_parent) {
    } else {
        if (greedy) {
            set_extent({{0, 0}, Window::get_current_vsize()});
        } else {
            Vector2 bottom_left;
            switch (anchor) {
                case Anchors::TopLeft:
                    JV_TODO("resizing");
                case Anchors::TopMiddle:
                    JV_TODO("resizing");
                case Anchors::TopRight:
                    JV_TODO("resizing");
                case Anchors::MiddleLeft:
                    JV_TODO("resizing");
                case Anchors::Middle:
                    JV_TODO("resizing");
                case Anchors::MiddleRight:
                    JV_TODO("resizing");
                case Anchors::BottomLeft:
                    JV_TODO("resizing");
                case Anchors::BottomMiddle:
                    JV_TODO("resizing");
                case Anchors::BottomRight:
                    JV_TODO("resizing");
                    break;
            }
        }
    }
}
jovial::Vector2 GuiNode::get_global_position() const {
    Vector2 bottom_left;
    switch (anchor) {
        case Anchors::TopLeft:
            return {extent.y, extent.h};
        case Anchors::TopMiddle:
            return {extent.x + (extent.w - extent.x) / 2.0f, extent.h};
        case Anchors::TopRight:
            return {extent.w, extent.h};
        case Anchors::MiddleLeft:
            return {extent.x, extent.y + (extent.h - extent.y) / 2.0f};
        case Anchors::Middle:
            return Vector2(extent.x, extent.y) + (Vector2(extent.w, extent.h) - Vector2(extent.x, extent.y)) / 2.0f;
        case Anchors::MiddleRight:
            return {extent.w, extent.y + (extent.h - extent.y) / 2.0f};
        case Anchors::BottomLeft:
            return {extent.x, extent.y};
        case Anchors::BottomMiddle:
            return {extent.x + (extent.w - extent.x) / 2.0f, extent.y};
        case Anchors::BottomRight:
            return {extent.x, extent.w};
    }
    printj((int) anchor);
    JV_CORE_UNREACHABLE
}

void GuiNode::set_is_greedy(bool greedy) {
    this->greedy = greedy;
    on_resize();
    resize_all();
}

void GuiNode::add_gui_child(const std::shared_ptr<GuiNode> &node) {
    gui_children.push_back(node);
    node->gui_parent = this;
    add_child(node.get());

    on_resize();
    resize_all();
}

void GuiNode::set_extent(Rect2 extent) {
    this->extent = extent;
    resize_all();
}

void GuiNode::resize_all() {
    for (auto &node: gui_children) {
        node->on_resize();
    }
}

Label::Label(std::string text, Font *font, TextDrawProps props)
    : text(std::move(text)), props(props), font(font) {
    if (!font) {
        this->font = FontBank::get_normal();
    }
    recalc_size();
    resize_all();
}

void Label::set_text(const std::string &text) {
    this->text = text;
    recalc_size();
}

void Label::update() {
    if (!visable) return;

    Vector2 pos = get_global_position();
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

void Label::recalc_size() {
    size = font->measure(text.c_str(), props.font_size);
}

void GuiPlugin::add_gui_head(GuiNode *head) {
    JV_CORE_ASSERT(instance, "GuiPlugin is NULL! Make sure to add it as a plugin!")
    instance->gui_heads.insert(head);
}

std::string anchor_to_string(Anchors anchor) {
    switch (anchor) {
        case Anchors::TopLeft:
            return "TopLeft";
        case Anchors::TopMiddle:
            return "TopMiddle";
        case Anchors::TopRight:
            return "TopRight";
        case Anchors::MiddleLeft:
            return "MiddleLeft";
        case Anchors::Middle:
            return "Middle";
        case Anchors::MiddleRight:
            return "MiddleRight";
        case Anchors::BottomLeft:
            return "BottomLeft";
        case Anchors::BottomMiddle:
            return "BottomMiddle";
        case Anchors::BottomRight:
            return "BottomRight";
    }
    return fmt::format("Unknown {}", (int) anchor);
}

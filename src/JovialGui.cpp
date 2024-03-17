#include "JovialGui.h"
#include "Jovial/Core/Logger.h"

namespace jovial::gui {
    GuiPlugin *GuiPlugin::instance = nullptr;

    bool GuiPlugin::on_win_resize(WindowResizeEvent &_) {
        for (auto node: gui_heads) {
            node->on_resize();
        }
        return false;
    }

    void GuiPlugin::on_event(Event &event) {
        EventDispatcher d(event);
        d.dispatch<WindowResizeEvent>(SIGNAL_CONNECT_1(GuiPlugin::on_win_resize), this);
    }

    void GuiNode::on_resize() {
        if (gui_parent) {
            printj("hello");
        } else {
            if (greedy) {
                set_extent({{0, 0}, Window::get_current_vsize()});
            } else {
                auto win_size = Window::get_current_vsize();
                switch (anchor) {
                    case Anchors::TopLeft:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::TopMiddle:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::TopRight:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::MiddleLeft:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::Middle:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::MiddleRight:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::BottomLeft:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::BottomMiddle:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
                        break;
                    case Anchors::BottomRight:
                        set_extent({0, win_size.y - min_extent.y, min_extent.x, win_size.y});
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

    void GuiNode::add_gui_child(const SharedPtr<GuiNode> &node) {
        gui_children.push_back(node);
        node->gui_parent = this;
        add_child((Node *) node.ptr);

        on_resize();
        resize_all();
    }

    void GuiNode::set_extent(Rect2 extent) {
        this->extent = extent;
        resize_all();
    }

    void GuiNode::set_min_extent(Rect2 extent) {
        this->min_extent = extent;
        resize_all();
    }

    void GuiNode::resize_all() {
        for (auto &node: gui_children) {
            node->on_resize();
        }
    }

    Label::Label(String text, Font *font, TextDrawProps props)
        : text(text.move()), props(props), font(font) {
        recalc_size();
        resize_all();
    }

    void Label::set_text(String text) {
        this->text = text.move();
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
        instance->gui_heads.push_back(head);
    }

    String anchor_to_string(Anchors anchor) {
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
        return format_string("Unknown ", (int) anchor);
    }

    void VContainer::on_resize() {
        GuiNode::on_resize();
    }

}// namespace jovial::gui

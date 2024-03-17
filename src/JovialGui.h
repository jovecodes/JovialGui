#pragma once

#include "Jovial/Components/Components2D.h"
#include "Jovial/Core/Assert.h"
#include "Jovial/Event/ApplicationEvent.h"
#include "Jovial/Event/Event.h"
#include "Jovial/JovialEngine.h"
#include "Jovial/Renderer/TextRenderer.h"
#include "Jovial/Std/Color.h"

namespace jovial::gui {

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

    String anchor_to_string(Anchors anchor);

    class GuiNode;

    class GuiPlugin : public Plugin {
    public:
        GuiPlugin() {
            instance = this;
        }

        static void add_gui_head(GuiNode *head);

        static void remove_gui_head(GuiNode *head) {
            JV_CORE_ASSERT(instance, "GuiPlugin is NULL! Make sure to add it as a plugin!")
            for (int i = 0; i < instance->gui_heads.size(); ++i) {
                if (instance->gui_heads[i] == head) {
                    instance->gui_heads.erase_index(i);
                }
            }
        }

    private:
        Vec<GuiNode *> gui_heads{};
        static GuiPlugin *instance;

        void init() override {}
        void deinit() override {}
        void on_event(Event &event) override;
        bool on_win_resize(WindowResizeEvent &_);
    };

    class GuiNode : public Node {
    public:
        virtual void on_resize();
        [[nodiscard]] Vector2 get_global_position() const;
        void set_is_greedy(bool greedy = true);
        void add_gui_child(const SharedPtr<GuiNode> &node);

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
        void set_min_extent(Rect2 extent);
        inline Rect2 get_extent() { return extent; }

        virtual Rect2 get_free_space() { return extent; }
        virtual void alloc_space(Rect2 space) {}

        void resize_all();

    private:
        Rect2 extent{};
        Rect2 min_extent{};
        bool greedy = false;
        Anchors anchor = Anchors::Middle;
        Vec<SharedPtr<GuiNode>> gui_children;
    };

    class Label : public GuiNode {
    public:
        Label(String text, Font *font, TextDrawProps props = {});

        void set_text(String text);

        void set_color(Color color) { this->props.color = color; }
        [[nodiscard]] inline const String &get_text() const { return text; }
        [[nodiscard]] inline Vector2 get_size() const { return size; }

        bool visable = true;
        AlignModes align_mode = AlignModes::Left;

    protected:
        TextDrawProps props;
        Font *font;
        Vector2 size{};
        String text;

    protected:
        void update() override;
        void recalc_size();
    };

    class VContainer : public GuiNode {
    public:
        VContainer() = default;

    protected:
        void on_resize() override;
    };

    class ColorRect : public GuiNode {
    public:
        ColorRect() = default;

        explicit ColorRect(Color color) {
            props.color = color;
        }

        rendering::ShapeDrawProperties props{};

    protected:
        void update() override {
            rendering::draw_rect2(get_extent(), props);
        }
    };

}// namespace jovial::gui

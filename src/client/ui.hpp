#ifndef UI_H
#define UI_H

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#ifdef _MSC_VER
#include <SDL_surface.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <glm/vec2.hpp>
#include "unified_render/rectangle.hpp"
#include "client/color.hpp"

namespace UnifiedRender {
    class Texture;
}

enum class CLICK_STATE {
    NOT_CLICKED,
    NOT_HANDLED,
    HANDLED,
};

namespace UI {
    class Widget;
    class Tooltip;
    typedef void (*Callback)(Widget&, void*);
    class Context {
        int drag_x, drag_y;
        bool is_drag;
        Widget* dragged_widget;
        int width, height;

        glm::ivec2 get_pos(Widget& w, glm::ivec2 offset);
        void check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off);
        CLICK_STATE check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off, CLICK_STATE click_state, bool clickable);
        int check_wheel_recursive(Widget& w, unsigned mx, unsigned my, int x_off, int y_off, int y);
    public:
        Context();
        void load_textures();
        void add_widget(Widget* widget);
        void remove_widget(Widget* widget);

        void render_recursive(Widget& widget, UnifiedRender::Rect viewport);
        void render_all();

        void resize(const int width, const int height);

        void check_hover(unsigned mx, unsigned my);
        bool check_click(unsigned mx, unsigned my);
        void check_drag(unsigned mx, unsigned my);
        int check_wheel(unsigned mx, unsigned my, int y);
        void check_text_input(const char* input);

        int do_tick_recursive(Widget& w);
        void do_tick(void);

        void clear(void);
        void clear_dead();

        void prompt(const std::string& title, const std::string& text);

        const UnifiedRender::Texture* background, * window_top, * button, * tooltip_texture, * piechart_overlay, * border_tex, * button_border;
        TTF_Font* default_font;

        std::vector<Widget*> widgets;
        Tooltip* tooltip_widget = nullptr;
    };

    enum class Origin {
        CENTER,
        UPPER_LEFT,
        UPPER_RIGTH,
        LOWER_LEFT,
        LOWER_RIGHT,
        CENTER_SCREEN,
        UPPER_LEFT_SCREEN,
        UPPER_RIGHT_SCREEN,
        LOWER_LEFT_SCREEN,
        LOWER_RIGHT_SCREEN,
    };

    enum class WidgetType {
        BUTTON,
        INPUT,
        WINDOW,
        TOOLTIP,
        LABEL,
        IMAGE,
        CHART,
        CHECKBOX,
        PIE_CHART,
        PROGRESS_BAR,
        SLIDER,
        GROUP,
    };

    class Widget {
    public:
        Widget() {};
        Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type, const UnifiedRender::Texture* tex = nullptr);
        Widget(const Widget&) = default;
        Widget(Widget&&) noexcept = default;
        Widget& operator=(const Widget&) = default;
        /* MSVSC hates multiple definitions for movables */
#ifndef _MSC_VER
        Widget& operator=(Widget&) noexcept = default;
#endif
        virtual ~Widget();

        void move_by(int x, int y);
        void add_child(Widget* child);
        void draw_rectangle(int x, int y, unsigned w, unsigned h, UnifiedRender::Rect viewport, unsigned tex);

        virtual void on_render(Context&, UnifiedRender::Rect viewport);
        virtual void text(const std::string& text);
        virtual void set_tooltip(Tooltip* tooltip);

        template <typename T>
        void above_of(const T& rhs) {
            y = rhs.y - height;
        }

        template <typename T>
        void below_of(const T& rhs) {
            y = rhs.y + rhs.height;
        }

        template <typename T>
        void left_side_of(const T& rhs) {
            x = rhs.x - width;
        }

        template <typename T>
        void right_side_of(const T& rhs) {
            x = rhs.x + rhs.width;
        }

        bool is_pinned = false;
        bool is_render = true;

        bool is_scroll = true;

        // Used internally for managing widgets outside of window bounds
        bool is_show = true;
        // Used internally for drawing hover effects on clickable child widgets
        bool is_clickable = false;
        bool is_hover = false;
        bool is_float = false;
        bool is_fullscreen = false;
        UI::Origin origin = UI::Origin::UPPER_LEFT;

        WidgetType type;

        int scroll_x = 0, scroll_y = 0;
        int x = 0, y = 0;

        size_t width = 0, height = 0;

        const UnifiedRender::Texture* current_texture = nullptr;
        UnifiedRender::Texture* text_texture = nullptr;
        int text_offset_x = 4, text_offset_y = 4;
        Color text_color;

        Tooltip* tooltip = nullptr;

        Widget* parent = nullptr;
        std::vector<Widget*> children;

        void* user_data = nullptr;
        void kill() {
            dead = true;
        };

        std::function<void(Widget&, void*)> on_update;
        std::function<void(Widget&, void*)> on_hover;
        std::function<void(Widget&, void*)> on_click;
        std::function<void(Widget&, void*)> on_click_outside;

        std::function<void(Widget&, void*)> on_each_tick;

        bool dead = false;

        friend class Context;
    private:
        void draw_border(const UnifiedRender::Texture* border_tex, float b_w, float b_h, float b_tex_w, float b_tex_h, float x_offset, float y_offset, UnifiedRender::Rect viewport);
    };

    class ChartData {
    public:
        ChartData(float _num, std::string _info, Color _color): num{_num}, info{_info}, color{_color} {}
        ChartData(float _num, std::string _info, uint32_t rgba): num{_num}, info{_info}, color{Color::rgba32(rgba)} {}
        float num;
        std::string info; // Used for tooltips
        Color color;
    };

    class Group: public Widget {
    public:
        Group(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Group() override {};

        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport) override;
    };

    class Input: public Widget {
    public:
        static void on_click_default(Widget& w, void*) {
            Input& input = static_cast<Input&>(w);
            input.is_selected = true;
        }
        static void on_click_outside_default(Widget& w, void*) {
            Input& input = static_cast<Input&>(w);
            if(input.is_selected) {
                input.text(input.buffer);
            }
            input.is_selected = false;
        }
        static void on_update_default(Widget& w, void*) {
            Input& input = static_cast<Input&>(w);
            input.timer = (input.timer + 1) % 60;
            std::string cursor = input.timer >= 30 ? "_" : "";
            if(input.is_selected && input.timer % 30 == 0) {
                if(!input.buffer.empty()) {
                    input.text(input.buffer + cursor);
                } else if(!cursor.empty()) {
                    input.text(cursor);
                }
            }
        }
        Input(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Input() override {};

        std::function<void(Input&, const char*, void*)> on_textinput;
        std::string buffer = "";
        bool is_selected = false;
    private:
        int timer; // TODO: Needs to not be frame dependand
    };

    class Checkbox: public Widget {
    public:
        Checkbox(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Checkbox() override {};
        bool value;
    };

    class Button: public Widget {
    public:
        Button(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Button() override {};
    };

    class CloseButton: public Widget {
        static void on_click_default(Widget& w, void*) {
            w.parent->kill();
        }
    public:
        CloseButton(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~CloseButton() override {};
    };

    class Window: public Widget {
    public:
        bool is_movable = false;

        Window(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Window() override {};
    };

    class Image: public Widget {
    public:
        Image(int x, int y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* parent = nullptr);
        virtual ~Image() override {};
    };

    class Label: public Widget {
    public:
        Label(int x, int y, const std::string& text = " ", Widget* parent = nullptr);
        virtual ~Label() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        virtual void text(const std::string& _text);
    };

    class Chart: public Widget {
    public:
        Chart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~Chart() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        std::deque<double> data;
    };

    class ProgressBar: public Widget {
    public:
        ProgressBar(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
        virtual ~ProgressBar() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        float max, min, value;
    };

    class Slider: public ProgressBar {
    public:
        Slider(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
        virtual ~Slider() override {};
        //virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        float max, min, value;
    };

    // Multiline textbox
    // TODO: Markdown support?
    class Text: public Widget {
        std::vector<UI::Label*> labels;
    public:
        Text(int x, int y, unsigned w, unsigned h, Widget* parent);
        virtual ~Text() override {};

        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        virtual void text(const std::string& text);

        int min_height = 0;
    };

    class Tooltip: public Widget {
        std::vector<UI::Label*> labels;
    public:
        Tooltip();
        Tooltip(Widget* parent, unsigned w, unsigned h);
        virtual ~Tooltip() override {};
        void set_pos(int x, int y, int width, int height, int screen_width, int screen_height);
        void text(const std::string& text);
    };

    class PieChart: public Widget {
    public:
        PieChart(int x, int y, unsigned w, unsigned h, std::vector<ChartData> data = std::vector<ChartData>(), Widget* _parent = nullptr);
        PieChart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~PieChart() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        void set_data(std::vector<ChartData> data);

    private:
        void draw_triangle(float start_angle, float end_angle, Color color);
        std::vector<ChartData> data;
        float max;
    };

};  // namespace UI

#endif

#ifndef UI_H
#define UI_H

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>

#ifdef _MSC_VER
#include <SDL_surface.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "render/texture.hpp"

enum UI_WidgetType {
    UI_WIDGET_BUTTON,
    UI_WIDGET_INPUT,
    UI_WIDGET_WINDOW,
    UI_WIDGET_LABEL,
    UI_WIDGET_IMAGE,
    UI_WIDGET_CHART,
    UI_WIDGET_CHECKBOX,
    UI_WIDGET_PIE_CHART,
    UI_WIDGET_SLIDER,
};

namespace UI {
class Widget;
typedef void (*Callback)(Widget&, void*);
class Context {
    int drag_x, drag_y;
    bool is_drag;
    Widget* dragged_widget;

   public:
    Context();
    void load_textures();
    void add_widget(Widget* widget);
    void remove_widget(Widget* widget);

    void render_recursive(Widget& widget, int x_off, int y_off);
    void render_all(const int width, const int height);
    int check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off);
    void check_hover(unsigned mx, unsigned my);
    int check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off);
    int check_click(unsigned mx, unsigned my);
    void check_drag(unsigned mx, unsigned my);
    int check_wheel(unsigned mx, unsigned my, int y);
    void check_text_input(const char* input);
    void clear(void);
    void clear_dead();

    const UnifiedRender::Texture *background, *window_top, *button, *tooltip, *piechart_overlay;
    TTF_Font* default_font;

    std::vector<Widget*> widgets;
};

class Widget {
   public:
    Widget(){};
    Widget(Widget* parent, int x, int y, unsigned w, unsigned h, int type, const UnifiedRender::Texture* tex = nullptr);
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
    void text(const std::string& text);
    void draw_rectangle(int x, int y, unsigned w, unsigned h, unsigned tex);

    template <typename T>
    void above_of(const T& rhs) {
        y = rhs.y - height;
    }

    template <typename T>
    void below_of(const T& rhs) {
        y += rhs.y + rhs.height;
    }

    template <typename T>
    void left_side_of(const T& rhs) {
        x = rhs.x - width;
    }

    template <typename T>
    void right_side_of(const T& rhs) {
        x = rhs.x + rhs.width;
    }
    virtual void on_render(Context&);

    bool is_pinned = false;

    int type;

    int scroll_x = 0, scroll_y = 0;
    int x = 0, y = 0;

    // Determines if the widget should be shown or not (all child widgets should be updated accordingly too)
    bool is_show = true;
    bool is_hover = false;

    size_t width = 0, height = 0;

    const UnifiedRender::Texture* current_texture = nullptr;
    UnifiedRender::Texture* text_texture = nullptr;

    std::string tooltip_text = "";

    Widget* parent;
    std::vector<Widget*> children;

    void* user_data = nullptr;
    void kill() { dead = true; };

    std::function<void(Widget&, void*)> on_update;
    std::function<void(Widget&, void*)> on_hover;
    std::function<void(Widget&, void*)> on_click;
    bool dead = false;
};

class Input : public Widget {
   public:
    Input(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
    ~Input(){};

    std::function<void(Input&, const char*, void*)> on_textinput;
    std::string buffer = "";
};

class Checkbox : public Widget {
   public:
    Checkbox(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
    ~Checkbox(){};
};

class Button : public Widget {
   public:
    Button(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
    ~Button(){};
};

class CloseButton : public Widget {
    static void on_click_default(Widget& w, void*) {
        delete w.parent;
    }

   public:
    CloseButton(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
    ~CloseButton(){};
};

class Window : public Widget {
   public:
    bool is_movable = false;

    Window(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
    ~Window(){};
};

class Image : public Widget {
   public:
    Image(int x, int y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* parent = nullptr);
    ~Image(){};
};

class Label : public Widget {
   public:
    Label(int x, int y, const char* text = nullptr, Widget* parent = nullptr);
    ~Label(){};
    virtual void on_render(Context& ctx);
};

class Chart : public Widget {
   public:
    Chart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
    ~Chart(){};
    virtual void on_render(Context& ctx);
    std::deque<double> data;
};

class Slider : public Widget {
   public:
    Slider(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
    ~Slider(){};
    virtual void on_render(Context& ctx);
    float max, min, value;
};

struct Color {
    Color(uint8_t red, uint8_t green, uint8_t blue) : r{red / 256.f}, g{green / 256.f}, b{blue / 256.f} {}
    float r;
    float g;
    float b;
};
struct ChartData {
    ChartData(float _num, std::string _info, Color _color) : num{_num}, info{_info}, color{_color} {}
    float num;
    std::string info;  // Used for tooltips
    Color color;
};
// Example
// std::vector<UI::ChartData> data;
// data.push_back(UI::ChartData(24.f, std::string(), UI::Color(56, 97, 140)));
// data.push_back(UI::ChartData(15.f, std::string(), UI::Color(251, 231, 76)));
// data.push_back(UI::ChartData(8.f, std::string(), UI::Color(255, 89, 100)));
// UI::PieChart* pie_chart = new UI::PieChart(9, 43, 303, 100, data, nullptr);
class PieChart : public Widget {
   public:
    PieChart(int x, int y, unsigned w, unsigned h, std::vector<ChartData> data, Widget* _parent = nullptr);
    ~PieChart(){};
    virtual void on_render(Context& ctx);
    void set_data(std::vector<ChartData> data);

   private:
    void draw_triangle(float start_angle, float end_angle, Color color);
    std::vector<ChartData> data;
    float max;
};
};  // namespace UI

extern SDL_Color text_color;
static inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
    text_color.r = r;
    text_color.g = g;
    text_color.b = b;
}

#endif

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include "client/ui/piechart.hpp"
#include "client/ui/widget.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

PieChart::PieChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : data{ std::vector<ChartData>() },
    max{ 0 },
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PIE_CHART)
{

}

void PieChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data) {
        max += slice.num;
    }
}

void PieChart::draw_triangle(float start_ratio, float end_ratio, UnifiedRender::Color color) {
    float x_center = width / 2.f;
    float y_center = height / 2.f;
    float radius = std::min<float>(width, height) * 0.5;
    float x_offset, y_offset, scale;

    glColor3f(color.r, color.g, color.b);
    x_offset = cos((start_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((start_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    x_offset = cos((end_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((end_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x_center, y_center);
}

void PieChart::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    glBindTexture(GL_TEXTURE_2D, ctx.piechart_overlay->gl_tex_num);
    glBegin(GL_TRIANGLES);
    float counter = 0;
    float last_corner = -0.125f;
    float last_ratio = 0;

    for(auto& slice : data) {
        counter += slice.num;
        float ratio = counter / max;
        while(ratio > last_corner + 0.25f) {
            last_corner += 0.25f;
            draw_triangle(last_ratio, last_corner, slice.color);
            last_ratio = last_corner;
        }
        draw_triangle(last_ratio, ratio, slice.color);
        last_ratio = ratio;
    }
    glEnd();
}
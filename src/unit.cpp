#include "unit.hpp"
#include "province.hpp"
#include "world.hpp"
#include <cmath>

std::pair<float, float> Unit::get_pos(void) const {
    return province->get_pos();
}

float Unit::get_speed(Province* _province) const {
    auto start_pos = province->get_pos();
    auto end_pos = _province->get_pos();

    // Get the linear distance from the current deduced position of the unit and the target
    // the current position of the unit is relative to the move progress it has done (so if it's
    // halfway thru a province it will then be placed at half of the distance)
    const float x_dist = (end_pos.first - start_pos.first);
    const float y_dist = (end_pos.second - start_pos.second);
    const float angle = std::atan2(x_dist, y_dist);

    // TODO: The comment above makes no sense since we don't do (max_move_progress / move_progress)
    const float dist_div = move_progress;

    //const float linear_dist = std::fabs(std::sqrt(x_dist * x_dist + y_dist * y_dist) / dist_div);
    
    const float speed = type->speed / _province->terrain_type->movement_penalty;
    float radius_scale = std::cos(M_PI / (2 * World::get_instance().height) * (2 * (y_dist / dist_div) - World::get_instance().height));
    float x_scale = 1 / (std::fabs(radius_scale) + 0.001f);
    float speed_scale = std::sqrt(std::pow(std::sin(angle), 2) + std::pow(std::cos(angle) * x_scale, 2));
    return (speed * speed_scale) / 100.f;
}

#include "client/ui/slider.hpp"

using namespace UI;

Slider::Slider(int _x, int _y, unsigned w, unsigned h, const float _min, const float _max, Widget* _parent)
    : max{ _max },
    min{ _min },
    ProgressBar(_x, _y, w, h, _min, _max, _parent)
{

}
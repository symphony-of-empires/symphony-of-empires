#pragma once

#include <string>

namespace Eng3D::TrueType {
#ifdef SDL_TTF_H_
    typedef TTF_Font Font;
#else
    struct Font;
#endif
    Eng3D::TrueType::Font *open_font(const std::string& name, int size);
};

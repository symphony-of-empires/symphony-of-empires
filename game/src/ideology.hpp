// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      ideology.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <cstdint>
#include "objects.hpp"
#include "policy.hpp"

struct Ideology : RefnameEntity<IdeologyId> {
    std::uint32_t color;
    Eng3D::StringRef name;
    std::string get_icon_path() const {
        return string_format("gfx/ideology/%s.png", ref_name.c_str());
    }

    float individualism;

    struct Economic {
        float distributism; // -1 = Concentrated
                            // 0  = neutral
                            // 1  = distributism
        
        float mercantilist; // -1 = Mercantile
                            // 0  = neutral
                            // 1  = Mercantile
        
        float capitalism;   // -1 = socialism
                            // 0  = neutral
                            // 1  = capitalist
    };

    struct Political {
        float individualism; // -1 = Corporatism
                             // 0  = neutral
                             // 1  = individualist
        
        float state_power;   // -1 = democracy
                             // 0  = neutral
                             // 1  = autocracy
        
        float equalitarianism;  // -1 = elitism
                                // 0  = neutral
                                // 1  = egalitarian
        
        float secular;  // -1 = non-secular
                        // 0  = neutral
                        // 1  = secular
    };

    struct Subideology {
        Eng3D::StringRef name;
        std::uint32_t color;

        Economic economic;
        Political political;
    };
};
template<>
struct Serializer<Ideology> {
    template<bool is_const>
    using type = CondConstType<is_const, Ideology>::type;
    template<bool is_serialize>
    static inline void deser_dynamic(Archive& ar, type<is_serialize>& obj) {
        ::deser_dynamic<is_serialize>(ar, obj.cached_id);
        ::deser_dynamic<is_serialize>(ar, obj.ref_name);
        ::deser_dynamic<is_serialize>(ar, obj.name);
        ::deser_dynamic<is_serialize>(ar, obj.color);
    }
};

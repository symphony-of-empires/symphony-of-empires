// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
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
//      cppgen.hpp
//
// Abstract:
//      C++ generator from description files of the form:
//      @entity ClassName
//          @string a
//          @string b
//          @int c
//          @float d
//          @entityRef OtherClass other_class
//          @direct
//          void set_pos(glm::ivec2 pos) {
//              @nameof(d) = pos;
//          }
//          @endirect
// ----------------------------------------------------------------------------

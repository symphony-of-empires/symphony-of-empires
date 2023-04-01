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
//      luavm.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>

struct lua_State;
namespace Eng3D {
    class LuaException : public std::exception {
        std::string buffer;
    public:
        LuaException(const std::string_view message) {
            buffer = message;
        }
        virtual const char* what() const noexcept {
            return buffer.c_str();
        }
    };

    struct LuaVM {
        LuaVM();
        ~LuaVM();

        static int call_func(lua_State* L, int nargs, int nret);
        int call_func(int nargs, int nret);
        void invoke_registered_callback(const std::string_view name);
        lua_State* state = nullptr;
    private:
        static int ui_new_button(lua_State* L);
        static int ui_new_image(lua_State* L);
        static int ui_new_group(lua_State* L);
        static int ui_new_div(lua_State* L);
        static int ui_new_window(lua_State* L);
        static int ui_new_checkbox(lua_State* L);
        static int ui_set_checkbox_value(lua_State* L);
        static int ui_get_checkbox_value(lua_State* L);
        static int ui_new_slider(lua_State* L);
        static int ui_get_slider_value(lua_State* L);
        static int ui_set_slider_value(lua_State* L);
        static int ui_new_label(lua_State* L);
        static int ui_set_text(lua_State* L);
        static int ui_get_image(lua_State* L);
        static int ui_set_image(lua_State* L);
        static int ui_set_scroll(lua_State* L);
        static int ui_set_on_click(lua_State* L);
        static int ui_set_window_on_click_close_btn(lua_State* L);
        static int ui_get_widget(lua_State* L);
        static int ui_widget_kill(lua_State* L);
        static int ui_widget_set_tooltip(lua_State* L);
        static int ui_register_callback(lua_State* L);
        static int ui_widget_set_flex(lua_State* L);
    };
}

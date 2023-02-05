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
//      luavm.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <memory>
#include <unordered_map>
#include <map>

#include <lua.hpp>
extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "eng3d/state.hpp"
#include "eng3d/log.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/components.hpp"
#include "eng3d/luavm.hpp"

Eng3D::LuaVM::LuaVM()
{
    this->state = luaL_newstate();
    luaL_openlibs(this->state);

    // No translation is done
    lua_register(this->state, "_", [](lua_State* L) {
        std::string msgid = luaL_checkstring(L, 1);
        lua_pushstring(L, std::string(msgid).c_str());
        return 1;
    });
    // And for the UI too
    lua_register(this->state, "ui_new_button", Eng3D::LuaVM::ui_new_button);
    lua_register(this->state, "ui_new_div", Eng3D::LuaVM::ui_new_div);
    lua_register(this->state, "ui_new_group", Eng3D::LuaVM::ui_new_group);
    lua_register(this->state, "ui_new_image", Eng3D::LuaVM::ui_new_image);
    lua_register(this->state, "ui_new_checkbox", Eng3D::LuaVM::ui_new_checkbox);
    lua_register(this->state, "ui_set_checkbox_value", Eng3D::LuaVM::ui_set_checkbox_value);
    lua_register(this->state, "ui_get_checkbox_value", Eng3D::LuaVM::ui_get_checkbox_value);
    lua_register(this->state, "ui_new_slider", Eng3D::LuaVM::ui_new_slider);
    lua_register(this->state, "ui_get_slider_value", Eng3D::LuaVM::ui_get_slider_value);
    lua_register(this->state, "ui_set_slider_value", Eng3D::LuaVM::ui_set_slider_value);
    lua_register(this->state, "ui_new_label", Eng3D::LuaVM::ui_new_label);
    lua_register(this->state, "ui_new_window", Eng3D::LuaVM::ui_new_window);
    lua_register(this->state, "ui_get_image", Eng3D::LuaVM::ui_get_image);
    lua_register(this->state, "ui_set_image", Eng3D::LuaVM::ui_set_image);
    lua_register(this->state, "ui_set_scroll", Eng3D::LuaVM::ui_set_scroll);
    lua_register(this->state, "ui_set_text", Eng3D::LuaVM::ui_set_text);
    lua_register(this->state, "ui_set_on_click", Eng3D::LuaVM::ui_set_on_click);
    lua_register(this->state, "ui_set_window_on_click_close_btn", Eng3D::LuaVM::ui_set_window_on_click_close_btn);
    lua_register(this->state, "ui_get_widget", Eng3D::LuaVM::ui_get_widget);
    lua_register(this->state, "ui_widget_kill", Eng3D::LuaVM::ui_widget_kill);
    lua_register(this->state, "ui_widget_set_tooltip", Eng3D::LuaVM::ui_widget_set_tooltip);
    lua_register(this->state, "UI_RegisterCallback", Eng3D::LuaVM::ui_register_callback);
    lua_register(this->state, "ui_widget_set_flex", Eng3D::LuaVM::ui_widget_set_flex);
}

std::map<int, UI::Widget*> lua_widgets;
std::map<int, std::shared_ptr<Eng3D::Texture>> lua_textures;
std::map<std::string, int> lua_ui_callbacks; 

// TODO: Make this thread-safe
static int id = 1;
static inline int get_unique_id() {
    id++;
    return id;
}

int Eng3D::LuaVM::ui_new_button(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Button(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_new_image(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Image(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_new_group(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Group(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_new_div(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Div(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_new_window(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Window(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_new_checkbox(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Checkbox(x, y, w, h, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_set_checkbox_value(lua_State* L) {
    UI::Checkbox* widget = static_cast<UI::Checkbox*>(lua_widgets[luaL_checkinteger(L, 1)]);
    widget->set_value(lua_toboolean(L, 2));
    return 0;
}

int Eng3D::LuaVM::ui_get_checkbox_value(lua_State* L) {
    UI::Checkbox* widget = static_cast<UI::Checkbox*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushboolean(L, widget->get_value());
    return 1;
}

int Eng3D::LuaVM::ui_new_slider(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int w = luaL_checkinteger(L, 3); // w
    int h = luaL_checkinteger(L, 4); // h
    int parent_ref = luaL_checkinteger(L, 5); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Slider(x, y, w, h, 0.f, 1.f, parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_get_slider_value(lua_State* L) {
    UI::Slider* widget = static_cast<UI::Slider*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushnumber(L, widget->get_value());
    return 1;
}

int Eng3D::LuaVM::ui_set_slider_value(lua_State* L) {
    UI::Slider* widget = static_cast<UI::Slider*>(lua_widgets[luaL_checkinteger(L, 1)]);
    widget->set_value(luaL_checknumber(L, 2));
    return 0;
}

int Eng3D::LuaVM::ui_new_label(lua_State* L) {
    int x = luaL_checkinteger(L, 1); // x
    int y = luaL_checkinteger(L, 2); // y
    int parent_ref = luaL_checkinteger(L, 3); // parent
    UI::Widget* parent = !parent_ref ? nullptr : lua_widgets[parent_ref];

    const auto widget_id = get_unique_id();
    lua_widgets[widget_id] = new UI::Label(x, y, " ", parent);
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_set_text(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->set_text(luaL_checkstring(L, 2));
    return 0;
}

int Eng3D::LuaVM::ui_get_image(lua_State* L) {
    const std::string path = luaL_checkstring(L, 1);
    auto& s = Eng3D::State::get_instance();

    const auto widget_id = get_unique_id();
    lua_textures[widget_id] = s.tex_man.load(s.package_man.get_unique(path));
    lua_pushinteger(L, widget_id);
    return 1;
}

int Eng3D::LuaVM::ui_set_image(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    auto tex = lua_textures[luaL_checkinteger(L, 2)];
    widget->current_texture = tex;
    return 0;
}

int Eng3D::LuaVM::ui_set_scroll(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->is_scroll = lua_toboolean(L, 2);
    return 0;
}

int Eng3D::LuaVM::ui_set_on_click(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    lua_pushvalue(L, 2); // Obtain closure id
    widget->lua_on_click = luaL_ref(L, LUA_REGISTRYINDEX);
    widget->set_on_click([L](UI::Widget& w) { // Special callback for handling this closure
        lua_getglobal(L, "UI_DriverCallOnClick");
        lua_rawgeti(L, LUA_REGISTRYINDEX, w.lua_on_click);
        // Find the widget on the map
        auto it = std::find_if(lua_widgets.begin(), lua_widgets.end(), [&w](const auto& e) {
            return e.second == &w;
        });
        lua_pushinteger(L, it->first);
        if(Eng3D::LuaVM::call_func(L, 2, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            CXX_THROW(Eng3D::LuaException, "Failure on UI callback: " + err_msg);
        }
    });
    return 0;
}

int Eng3D::LuaVM::ui_set_window_on_click_close_btn(lua_State* L) {
    UI::Window* widget = static_cast<UI::Window*>(lua_widgets[luaL_checkinteger(L, 1)]);
    lua_pushvalue(L, 2); // Obtain closure id
    widget->lua_on_close_btn = luaL_ref(L, LUA_REGISTRYINDEX);
    widget->set_close_btn_function([L](UI::Widget& w) { // Special callback for handling this closure
        auto& o = (UI::Window&)(*w.parent->parent);
        lua_getglobal(L, "UI_DriverCallOnClick");
        lua_rawgeti(L, LUA_REGISTRYINDEX, o.lua_on_close_btn);
        // Find the widget on the map
        auto it = std::find_if(lua_widgets.begin(), lua_widgets.end(), [&o](const auto& e) {
            return e.second == &o;
        });
        lua_pushinteger(L, it->first);
        if(call_func(L, 2, 0)) {
            const std::string err_msg = lua_tostring(L, -1);
            Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
            lua_pop(L, 1);
            CXX_THROW(Eng3D::LuaException, "Failure on UI callback: " + err_msg);
        }
        o.kill(); // Implicitly kill object
    });
    return 0;
}

int Eng3D::LuaVM::ui_get_widget(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    lua_pushinteger(L, widget->width);
    lua_pushinteger(L, widget->height);
    lua_pushinteger(L, widget->x);
    lua_pushinteger(L, widget->y);
    return 4;
}

int Eng3D::LuaVM::ui_widget_kill(lua_State* L) {
    const int widget_id = luaL_checkinteger(L, 1);
    UI::Widget* widget = lua_widgets[widget_id];
    widget->kill(); // Kill the widget (not now, but later...)
    lua_widgets.erase(widget_id); // Delete from lua_widgets to avoid dead-pointer referencing
    return 0;
}

int Eng3D::LuaVM::ui_widget_set_tooltip(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    widget->set_tooltip(luaL_checkstring(L, 2));
    return 0;
}

int Eng3D::LuaVM::ui_register_callback(lua_State* L) {
    const std::string name = luaL_checkstring(L, 1);
    lua_pushvalue(L, 2); // Obtain closure id
    lua_ui_callbacks[name] = luaL_ref(L, LUA_REGISTRYINDEX);
    // Now the UI callback is registered
    return 0;
}

int Eng3D::LuaVM::ui_widget_set_flex(lua_State* L) {
    UI::Widget* widget = lua_widgets[luaL_checkinteger(L, 1)];
    int flex_mode = luaL_checkinteger(L, 2);
    if(flex_mode == 1) {
        widget->flex = UI::Flex::COLUMN;
    } else if(flex_mode == 2) {
        widget->flex = UI::Flex::ROW;
    } else {
        widget->flex = UI::Flex::NONE;
    }
    return 0;
}

int Eng3D::LuaVM::call_func(lua_State* L, int nargs, int nret) {
    /* calculate stack position for message handler */
    int hpos = lua_gettop(L) - nargs;
    /* push custom error message handler */
    lua_pushcfunction(L, [](lua_State* Lua) {
        lua_getglobal(Lua, "debug");
        lua_getfield(Lua, -1, "traceback");
        lua_pushvalue(Lua, 1);
        lua_pushinteger(Lua, 1);
        lua_call(Lua, 2, 1);
        return 1;
    });
    /* move it before function and arguments */
    lua_insert(L, hpos);
    /* call lua_pcall function with custom handler */
    int r = lua_pcall(L, nargs, nret, hpos);
    /* remove custom error message handler from stack */
    lua_remove(L, hpos);
    /* pass return value of lua_pcall */
    return r;
}

int Eng3D::LuaVM::call_func(int nargs, int nret) {
    return this->call_func(this->state, nargs, nret);
}

/// @brief Some UI functions are hardcoded, for example the main menu is hardcoded
/// to appear when the game starts, in order to mantain scriptability we just invoke
/// functions coded in lua
/// @param L 
/// @param name 
/// @return int 
void Eng3D::LuaVM::invoke_registered_callback(const std::string_view name) {
    lua_rawgeti(this->state, LUA_REGISTRYINDEX, lua_ui_callbacks[name.data()]);
    if(this->call_func(0, 0)) {
        const std::string err_msg = lua_tostring(this->state, -1);
        Eng3D::Log::error("lua", "lua_pcall failed: " + err_msg);
        lua_pop(this->state, 1);
        CXX_THROW(Eng3D::LuaException, "Failure on UI callback: " + err_msg);
    }
}

Eng3D::LuaVM::~LuaVM()
{
    if(this->state != nullptr)
        lua_close(this->state);
}

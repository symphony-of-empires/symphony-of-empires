-- Industrial_Era_mod - Base game files for Symphony of Empires
-- Copyright (C) 2021, Symphony of Empires contributors
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- ----------------------------------------------------------------------------
--	Name:
--  	ui.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

UI_Widget = {
    id = -1,
	width = 0,
	height = 0,
	x = 0,
	y = 0,
}
function UI_Widget:text(msg)
    ui_set_text(self.id, msg)
end
function UI_Widget:set_image(path)
    ui_set_image(self.id, path)
end
function UI_Widget:set_on_click(fn)
    ui_set_on_click(self.id, fn)
end
function UI_Widget:set_scroll(value)
    ui_set_scroll(self.id, value)
end
function UI_Widget:get(id)
	self.id = id
	self.width, self.height, self.x, self.y = ui_get_widget(id)
end
function UI_Widget:kill()
	ui_widget_kill(self.id)
end
function UI_Widget:set_tooltip(msg)
	ui_widget_set_tooltip(self.id, msg)
end

UI_Button = UI_Widget
function UI_Button:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_button(x, y, w, h, parent)
	print("New UI_Button " .. o.id)
	return o
end

UI_Image = UI_Widget
function UI_Image:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_image(x, y, w, h, parent)
	print("New UI_Image " .. o.id)
	return o
end

UI_Group = UI_Widget
function UI_Group:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_group(x, y, w, h, parent)
	print("New UI_Group " .. o.id)
	return o
end

UI_Div = UI_Widget
function UI_Div:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_div(x, y, w, h, parent)
	print("New UI_Div " .. o.id)
	return o
end

UI_Window = UI_Widget
function UI_Window:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_window(o.x, o.y, o.w, o.h, parent)
	print("New UI_Window " .. o.id)
	return o
end
function UI_Widget:set_close_btn_func(fn)
	print(self.id)
    ui_set_window_on_click_close_btn(self.id, fn)
end

UI_Checkbox = UI_Widget
function UI_Checkbox:new(x, y, w, h, parent)
	local o = { x = x, y = y, w = w, h = h }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_checkbox(x, y, w, h, parent)
	print("New UI_Checkbox " .. o.id)
	return o
end
function UI_Checkbox:set_value(value)
	ui_set_checkbox_value(self.id, value)
end

UI_Label = UI_Widget
function UI_Label:new(x, y, parent)
	local o = { x = x, y = y }
	setmetatable(o, self)
	self.__index = self
    o.id = ui_new_label(x, y, parent)
	print("New UI_Label " .. o.id)
	-- TODO: Obtain size of widget dynamically
	o.w = 24
	o.h = 24
	return o
end

function UI_ReinterpretAs(cast, obj)
	local o = cast
	o.id = obj.id
	o.x = obj.x
	o.y = obj.y
	o.w = obj.w
	o.h = obj.h
	return o
end

-- Driver on_click callback, transforms the widget_id object onto a full UI_Widget object
function UI_DriverCallOnClick(fn, widget_id)
	fn(UI_Widget:get(widget_id)) -- TODO: call function like this?
end


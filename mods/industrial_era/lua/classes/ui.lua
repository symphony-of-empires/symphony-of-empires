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
    id = 0
}
function UI_Widget:text(msg)
    ui_set_text(self.id, msg)
end
function UI_Widget:set_image(path)
    ui_set_image(self.id, path)
end

UI_Button = UI_Widget
function UI_Button:new(x, y, w, h, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_button(x, y, w, h, parent)
	return o
end

UI_Image = UI_Widget
function UI_Image:new(x, y, w, h, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_image(x, y, w, h, parent)
	return o
end

UI_Group = UI_Widget
function UI_Group:new(x, y, w, h, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_group(x, y, w, h, parent)
	return o
end

UI_Div = UI_Widget
function UI_Div:new(x, y, w, h, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_div(x, y, w, h, parent)
	return o
end

UI_Window = UI_Widget
function UI_Window:new(x, y, w, h, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_window(x, y, w, h, parent)
	return o
end

UI_Label = UI_Widget
function UI_Label:new(x, y, parent)
	local o = {}
	setmetatable(o, self)
	self.__index = self
    self.id = ui_new_label(x, y, parent)
	return o
end

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
--  	event.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

Event = {
	ref_name = "",
	conditions_fn = "",
	event_fn = "",
	title = "",
	text = "",
	checked = false,

	last_decision_id = 0,
}
function Event:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Event:register()
	self.id = add_event(self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:update()
	update_event(self.id, self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:get(ref_name)
	local o = Event:new()
	o.id, o.conditions_fn, o.event_fn, o.title, o.text, o.checked = get_event(ref_name)
	o.ref_name = ref_name
	return o
end
function Event:add_receivers(...)
	local args = table.pack(...)
	for i = 1, args.n do
		args[i] = args[i].id
	end
	add_event_receivers(self.id, args.n, table.unpack(args))
end
function Event:add_decision(decision)
	local decision_ref_name = self.ref_name .. self.last_decision_id
	self.last_decision_id = self.last_decision_id + 1.0

	add_decision(self.id, decision_ref_name, decision.name, decision.decision_fn, decision.effects)
end

Decision = {
	ref_name = "",
	decision_fn = "",
	name = "",
	effects = ""
}
function Decision:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

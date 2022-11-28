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
--  	lua/events/special.lua
--
-- 	Abstract:
--      Special events that fire under very specific conditions
-- ----------------------------------------------------------------------------

special_alliance_evhdl = Event:new{
    ref_name = "special_alliance",
    conditions_fn = function() -- Never fire normally
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        special_alliance_evhdl:add_decision(Decision:new{
            name = "Accept",
            decision_fn = function(ref_name, other_ref_name)
                local rel = Nation:get(ref_name):get_relation(Nation:get(other_ref_name))
                rel.alliance = 0.1
                rel.relation = 1.0
                rel.has_war = false
                Nation:get(ref_name):set_relation(Nation:get(other_ref_name))
            end,
            effects = ""
        })
        special_alliance_evhdl:add_decision(Decision:new{
            name = "Reject",
            decision_fn = function(ref_name, other_ref_name)

            end,
            effects = "None"
        })
        return EVENT_DO_MANY_TIMES
    end,
    title = "Alliance offer",
    text = "We have an alliance offer from [TODO]; should we accept it?"
}
special_alliance_evhdl:register()
special_alliance_evhdl:add_receivers(table.unpack(Nation:get_all()))

special_join_wardecl_evhdl = Event:new{
    ref_name = "special_join_wardecl",
    conditions_fn = function() -- Never fire normally
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        special_join_wardecl_evhdl:add_decision(Decision:new{
            name = "Accept",
            decision_fn = function(ref_name)

            end,
            effects = "Joins war against [TODO]"
        })
        special_join_wardecl_evhdl:add_decision(Decision:new{
            name = "Reject",
            decision_fn = function(ref_name)

            end,
            effects = "None"
        })
        return EVENT_DO_MANY_TIMES
    end,
    title = "Join war declaration offer",
    text = "Alliance has war started by [TODO] and against [TODO], should we join?"
}
special_join_wardecl_evhdl:register()
special_join_wardecl_evhdl:add_receivers(table.unpack(Nation:get_all()))

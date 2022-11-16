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
--  	lua/events/westernization.lua
--
-- 	Abstract:
--      Westernization event chain
-- ----------------------------------------------------------------------------

western_contact_evhdl = Event:new{
    ref_name = "western_contact",
    conditions_fn = function(ref_name)
        local year = get_year()
        if Nation:get(ref_name):get_flag("westernized") < 1.0 then
            return EVENT_CONDITIONS_MET
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        western_contact_evhdl:add_decision(Decision:new{
            name = "Yes, let them in",
            decision_fn = function(ref_name)
                Nation:get(ref_name):set_flag("westernized", Nation:get(ref_name):get_flag("westernized") + 0.1)
            end,
            effects = "+0.1 westernization"
        })
        western_contact_evhdl:add_decision(Decision:new{
            name = "They can enter, but we have to be cautious",
            decision_fn = function(ref_name)
                Nation:get(ref_name):set_flag("westernized", Nation:get(ref_name):get_flag("westernized") + 0.05)
            end,
            effects = "+0.05 westernization"
        })
        western_contact_evhdl:add_decision(Decision:new{
            name = "They're a threat to our society",
            decision_fn = function(ref_name)
                
            end,
            effects = "None"
        })
        return EVENT_DO_MANY_TIMES
    end,
    title = "Western contact",
    text = "Some foreigners have been wanting to set embassies and further improve relations with our nation."
}
western_contact_evhdl:register()
western_contact_evhdl:add_receivers(table.unpack(Nation:get_all()))

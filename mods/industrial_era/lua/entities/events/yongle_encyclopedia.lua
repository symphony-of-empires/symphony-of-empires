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
--  	lua/events/yongle_encyclopedia.lua
--
-- 	Abstract:
--      Yongle Encyclopedia event chain
-- ----------------------------------------------------------------------------

yongle_encyclopedia_is_safe = false
yongle_encyclopedia_evhdl = Event:new{
    ref_name = "yongle_encyclopedia",
    conditions_fn = function()
        local year = get_year()
        if year >= 1825 then
            return EVENT_CONDITIONS_MET
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        yongle_encyclopedia_evhdl:add_decision(Decision:new{
            ref_name = "yongle_encyclopedia_decision_0",
            name = "Okay",
            decision_fn = function(ref_name)
    
            end,
            effects = "None"
        })
        yongle_encyclopedia_evhdl:add_decision(Decision:new{
            ref_name = "yongle_encyclopedia_decision_1",
            name = "Make backups of it",
            decision_fn = function(ref_name)
                -- TODO: Remove money
                yongle_encyclopedia_is_safe = true
            end,
            effects = "-10,000 money"
        })
        yongle_encyclopedia_evhdl:add_decision(Decision:new{
            ref_name = "yongle_encyclopedia_decision_2",
            name = "Make it public",
            decision_fn = function(ref_name)
                -- TODO: Remove money
                -- TODO: Anger monarchists (a bit)
                yongle_encyclopedia_is_safe = true
            end,
            effects = "-10,000 money"
        })
        return EVENT_DO_ONE_TIME
    end,
    title = "Yongle Encyclopedia",
    text = "The Yongle Encyclopedia was an encyclopedia commisioned by the Yongle emperor in 1403; containing abundant pieces of Chinesse literature and science. Through centuries various volumes of it have been lost."
}
yongle_encyclopedia_evhdl:register()
yongle_encyclopedia_evhdl:add_receivers(Nation:get("qing"))

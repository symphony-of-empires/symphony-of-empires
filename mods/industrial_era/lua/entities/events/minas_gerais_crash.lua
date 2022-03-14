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
--  	lua/events/minas_gerais_crash.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

--
-- The minas crash was a car crash occuring on the brazilian province of minas
-- It's the first car crash ever, so it's kinda important
--
function the_minas_crash_test()
    local year = get_year()
    if year >= 1900 and year <= 1950 and math.random(0, 100) == 0 then
        -- Get the current owner of the minas province
        local nation = Province:get_owner(Province:get("minas_gerais"))
        the_minas_crash:add_receivers(nation)
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function the_minas_crash_event()
	descision = Descision:new{
		ref_name = "the_minas_crash_descision_0",
		name = "I'm sorry for such tragedy",
		descision_fn = "the_minas_crash_descision_0",
		effects = "None"
	}
	the_minas_crash:add_descision(descision)
    descision = Descision:new{
		ref_name = "the_minas_crash_descision_1",
		name = "Well yeah they should be held accountable!",
		descision_fn = "the_minas_crash_descision_1",
		effects = "None"
	}
	the_minas_crash:add_descision(descision)
	return EVENT_DO_ONE_TIME
end
function the_minas_crash_descision_0()
end
function the_minas_crash_descision_1()
end
the_minas_crash = Event:new{
	ref_name = "the_minas_crash",
	conditions_fn = "the_minas_crash_test",
	event_fn = "the_minas_crash_event",
    title = "The minas crash",
	text = "2 cars have been circulating on the minas province recently where they collided with each other on a catastrophic event"
}
the_minas_crash:register()
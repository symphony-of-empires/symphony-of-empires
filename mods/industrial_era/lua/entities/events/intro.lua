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
--  	lua/events/intro.lua
--
-- 	Abstract:
--      Flavour introductions for countries.
-- ----------------------------------------------------------------------------

-- The netherlands
dutch_intro_evhdl = Event:new{
    ref_name = "dutch_intro",
    conditions_fn = function(ref_name)
        return EVENT_CONDITIONS_MET
    end,
    event_fn = function(ref_name)
        dutch_intro_evhdl:add_decision(Decision:new{
            name = "Yes!",
            decision_fn = function(ref_name)
                -- ...
            end,
            effects = "None"
        })
        return EVENT_DO_ONE_TIME
    end,
    title = "The Netherlands",
    text = "The republic of the Netherlands; sandwhiched between France and the states of what would-be Germany; it has huge potential to build it's own colonial empire on Africa and sorroundings. And posseses the valuable colonial lands of Java and Indonesian islands: the Dutch East Indies."
}
dutch_intro_evhdl:register()
dutch_intro_evhdl:add_receivers(Nation:get("netherlands"))

-- The USA
usa_intro_evhdl = Event:new{
    ref_name = "usa_intro",
    conditions_fn = function(ref_name)
        return EVENT_CONDITIONS_MET
    end,
    event_fn = function(ref_name)
        usa_intro_evhdl:add_decision(Decision:new{
            name = "Thrive!",
            decision_fn = function(ref_name)
                -- ...
            end,
            effects = "None"
        })
        return EVENT_DO_ONE_TIME
    end,
    title = "The United States of America",
    text = "A thriving land with huge potential to become one of the world's largest economies, with just the right policies it will be able to maintain a foothold on the american continent."
}
usa_intro_evhdl:register()
usa_intro_evhdl:add_receivers(Nation:get("united_states"))

-- Mexico
mexico_intro_evhdl = Event:new{
    ref_name = "mexico_intro",
    conditions_fn = function(ref_name)
        return EVENT_CONDITIONS_MET
    end,
    event_fn = function(ref_name)
        mexico_intro_evhdl:add_decision(Decision:new{
            name = "Oh right!",
            decision_fn = function(ref_name)
                -- ...
            end,
            effects = "None"
        })
        return EVENT_DO_ONE_TIME
    end,
    title = "Mexico",
    text = "Almost a decade after gaining independence from Spain, Mexico is able to operate autonomously and as a sovereign nation; it will however face challenges from the various issues that riddle the country in this period; the entirity of Central America seceded from Mexico a few years prior; and from the looks of it, Texas might be next."
}
mexico_intro_evhdl:register()
mexico_intro_evhdl:add_receivers(Nation:get("mexico"))

-- Bavaria
bavaria_intro_evhdl = Event:new{
    ref_name = "bavaria_intro",
    conditions_fn = function(ref_name)
        return EVENT_CONDITIONS_MET
    end,
    event_fn = function(ref_name)
        bavaria_intro_evhdl:add_decision(Decision:new{
            name = "Splendid!",
            decision_fn = function(ref_name)
                -- ...
            end,
            effects = "None"
        })
        return EVENT_DO_ONE_TIME
    end,
    title = "Bavaria",
    text = "Bavaria, one of the southern German states; but with the growing nation-state movements in this era... there's nothing to worry about at this moment through."
}
bavaria_intro_evhdl:register()
bavaria_intro_evhdl:add_receivers(Nation:get("bavaria"))

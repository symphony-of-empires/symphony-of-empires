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
--  	lua/events/kalmar_union.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

function kalmar_union_test()
    local needed_provinces = {
        -- Finish key provinces
        "aland", "helsinki", "oulu", "east_oulu", "turku",
        -- Swedish key provinces
        "gotland", "oland", "varmland", "smaland", "jamtland",
        -- Norwegian key provinces
        "oslo"
    }
    local n_needed_provinces = 11

    -- Only these countries can form Scandinavia
    local sweden = Nation:get("sweden")
    local n_sweden_provinces = 0
    local finland = Nation:get("finland")
    local n_finland_provinces = 0
    local norway = Nation:get("norway")
    local n_norway_provinces = 0
    for k, v in pairs(needed_provinces) do
        province = Province:get(v)

        if province.owner_id == sweden.id then
            n_sweden_provinces = n_sweden_provinces + 1
        elseif province.owner_id == finland.id then
            n_finland_provinces = n_finland_provinces + 1
        elseif province.owner_id == norway.id then
            n_norway_provinces = n_norway_provinces + 1
        end
    end

    if n_sweden_provinces >= n_needed_provinces then
        kalmar_union_evhdl:add_receivers(Nation:get("sweden"))
        return EVENT_CONDITIONS_MET
    elseif n_finland_provinces >= n_needed_provinces then
        kalmar_union_evhdl:add_receivers(Nation:get("finland"))
        return EVENT_CONDITIONS_MET
    elseif n_norway_provinces >= n_needed_provinces then
        kalmar_union_evhdl:add_receivers(Nation:get("norway"))
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function kalmar_union_event()
    descision = Descision:new{
        ref_name = "kalmar_union_descision_0",
        name = "Unify scandinavia once again",
        descision_fn = "kalmar_union_descision_0",
        effects = "Become Kalmar Union"
    }
    kalmar_union_evhdl:add_descision(descision)

    descision = Descision:new{
        ref_name = "kalmar_union_descision_1",
        name = "We don't need the others, right?",
        descision_fn = "kalmar_union_descision_1",
        effects = "No Kalmar Union"
    }
    kalmar_union_evhdl:add_descision(descision)
    return EVENT_DO_ONE_TIME
end
function kalmar_union_descision_0()
    
end
function kalmar_union_descision_1()

end
kalmar_union_evhdl = Event:new{
    ref_name = "kalmar_union_evhdl",
    conditions_fn = "kalmar_union_test",
    event_fn = "kalmar_union_event",
    title = "Kalmar Union",
    text = "The Kalmar Union was a country that covered the whole scandinavia - now with the newly aquired provinces in your power we may be able to renew this nation"
}
kalmar_union_evhdl:register()
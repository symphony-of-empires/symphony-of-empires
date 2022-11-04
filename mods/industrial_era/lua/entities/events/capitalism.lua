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
--  	lua/events/capitalism.lua
--
-- 	Abstract:
--      Events that affect the capitalism range of a nation subtly
-- ----------------------------------------------------------------------------

function get_corporate_ideal_provinces(ref_name)
    -- TODO: Make this arbitrary threshold more dynamic
    local provinces = Nation:get(ref_name):get_owned_provinces()
    return table.remove_if(provinces, function(e)
        return e:get_total_population() < 45000
    end)
end

corporate_factory_request_evhdl = Event:new{
    ref_name = "corporate_factory_request",
    conditions_fn = function(ref_name)
        local year = get_year()
        if Nation:get(ref_name):get_flag("westernized") == 1.0 or true then
            local provinces = get_corporate_ideal_provinces(ref_name)
            if table.size(provinces) > 0 then
                -- TODO: Check that we're capitalist
                return EVENT_CONDITIONS_MET
            end
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        local provinces = get_corporate_ideal_provinces(ref_name)
        local province_names = {}
        for k, v in pairs(provinces) do
            province_names[k] = v.name
        end

        corporate_factory_request_evhdl.title = "Corporation requests to build factory"
        corporate_factory_request_evhdl.text = "A corporation has requested that we allow them to build their new industrial complexes on: " .. Locale:conjugate_comma_and(table.unpack(province_names)) .. "; However they are not liked by the locals and are viewed as a threat to the local economy."
        corporate_factory_request_evhdl:update()
        corporate_factory_request_evhdl:add_decision(Decision:new{
            ref_name = "corporate_factory_request_decision_0",
            name = "Sure!",
            decision_fn = function(ref_name)
                local provinces = get_corporate_ideal_provinces(ref_name)
				for k, province in pairs(provinces) do
                    -- Update militancy of POPs
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						if pop.ideology.ref_name == ideology.ref_name then
							goto continue
						end
						pop.militancy = pop.militancy + 0.5
						province:update_pop(pop)
						::continue::
					end

                    -- Upgrade factories
                    local buildings = province:get_buildings()
                    for k, building in pairs(buildings) do
                        building.level = building.level + 1.0
                        province:update_building(building)
                    end
					province:update_pops()
				end
                Nation:get(ref_name):relative_policy_stance(0.1)
            end,
            effects = "+1 level for all buildings\n+0.1 capitalist\n+0.5 militancy for all POPs in the provinces"
        })
        corporate_factory_request_evhdl:add_decision(Decision:new{
            ref_name = "corporate_factory_request_decision_1",
            name = "They should look somewhere else",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(-0.1)
            end,
            effects = "-0.1 capitalist"
        })
        return EVENT_DO_MANY_TIMES
    end,
}
corporate_factory_request_evhdl:register()
corporate_factory_request_evhdl:add_receivers(table.unpack(Nation:get_all()))

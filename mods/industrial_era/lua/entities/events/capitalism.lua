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
        local nation = Nation:get(ref_name)
        if nation:get_flag("westernized") == 1.0 and nation:get_flag("total_corporations") >= 1.0 and math.random(0, 100) == 0 then
            local provinces = get_corporate_ideal_provinces(ref_name)
            if table.size(provinces) > 0 then
                -- Are we capitalist?
                if nation:relative_policy_stance(POLICY_CAPITALISM, 0.0) > 0.0 then
                    return EVENT_CONDITIONS_MET
                end
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
            name = "Sure!",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, 0.1)
                local provinces = get_corporate_ideal_provinces(ref_name)
				for k, province in pairs(provinces) do
                    -- Update militancy of POPs
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						pop.militancy = pop.militancy + 0.5
						province:update_pop(pop)
					end

                    -- Upgrade factories
                    local buildings = province:get_buildings()
                    for k, building in pairs(buildings) do
                        building.level = building.level + 1.0
                        province:update_building(building)
                    end
					province:update_pops()
				end
            end,
            effects = "+1 level for all buildings\n+0.1 capitalist\n+0.5 militancy for all POPs in the provinces"
        })
        corporate_factory_request_evhdl:add_decision(Decision:new{
            name = "They should look somewhere else",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, -0.1)
            end,
            effects = "-0.1 capitalist"
        })
        return EVENT_DO_MANY_TIMES
    end,
}
corporate_factory_request_evhdl:register()
corporate_factory_request_evhdl:add_receivers(table.unpack(Nation:get_all()))

-- New corporation
new_corporation_evhdl = Event:new{
    ref_name = "new_corporation",
    conditions_fn = function(ref_name)
        local year = get_year()
        local nation = Nation:get(ref_name)
        if nation:get_flag("westernized") == 1.0 and math.random(0, 100) == 0 then
            local provinces = get_corporate_ideal_provinces(ref_name)
            if table.size(provinces) > 0 then
                -- Are we capitalist?
                if nation:relative_policy_stance(POLICY_CAPITALISM, 0.0) > 0.0 then
                    return EVENT_CONDITIONS_MET
                end
            end
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        new_corporation_evhdl:update()
        new_corporation_evhdl:add_decision(Decision:new{
            name = "Publicly aid them on their endeavour",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, 1.0)
                Nation:get(ref_name):set_flag("total_corporations", 1.0)
                Nation:get(ref_name):set_flag("corporate_favours", 10.0)
            end,
            effects = "+1 corporation\n+1.0 capitalist\n+10.0 corporate favours"
        })
        new_corporation_evhdl:add_decision(Decision:new{
            name = "Secretly aid them on their endeavour",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, 0.2)
                Nation:get(ref_name):set_flag("total_corporations", 1.0)
                Nation:get(ref_name):set_flag("corporate_favours", 1.0)
            end,
            effects = "+1 corporation\n+0.2 capitalist\n+1.0 corporate favours"
        })
        new_corporation_evhdl:add_decision(Decision:new{
            name = "Watch them closely",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, 0.1)
                Nation:get(ref_name):set_flag("total_corporations", 1.0)
            end,
            effects = "+1 corporation\n+0.1 capitalist"
        })
        new_corporation_evhdl:add_decision(Decision:new{
            name = "We can't trust them",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, -0.1)
            end,
            effects = "-0.1 capitalist"
        })
        new_corporation_evhdl:add_decision(Decision:new{
            name = "That was the last straw...",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, -1.0)
                Nation:get(ref_name):set_flag("corporate_favours", -100.0)
            end,
            effects = "-1.0 capitalist\n-100.0 corporate favours"
        })
        return EVENT_DO_MANY_TIMES
    end,
    title = "Flourishing corporation, or threatening monopoly?",
    text = "One of our nation's corporations have been particularly sucessful at dominating various markets; However some anonymous whistlers have spoken against said corporation for presumably not doing fair competition with others, albeit we need to recognize their potential as one of our milestones of capitalism, or do we need to get worried?",
}
new_corporation_evhdl:register()
new_corporation_evhdl:add_receivers(table.unpack(Nation:get_all()))

-- Bankrupt corporation
bankrupt_corporation_evhdl = Event:new{
    ref_name = "bankrupt_corporation",
    conditions_fn = function(ref_name)
        local year = get_year()
        local nation = Nation:get(ref_name)
        if nation:get_flag("westernized") == 1.0 and nation:get_flag("total_corporations") > 0 and math.random(0, 100) == 0 then
            local provinces = get_corporate_ideal_provinces(ref_name)
            if table.size(provinces) > 0 then
                -- Are we capitalist?
                if nation:relative_policy_stance(POLICY_CAPITALISM, 0.0) > 0.0 then
                    return EVENT_CONDITIONS_MET
                end
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

        bankrupt_corporation_evhdl.title = "Corporation goes bankrupt"
        bankrupt_corporation_evhdl.text = "One of our nation's corporations has declared that they're on the brink of bankruptcy. This can affect severely our economy in the long run; They've been boosting the economies of " .. Locale:conjugate_comma_and(province_names) .. "; Should we let this go through?"
        bankrupt_corporation_evhdl:update()
        bankrupt_corporation_evhdl:add_decision(Decision:new{
            name = "They should be more careful next time",
            decision_fn = function(ref_name)
                Nation:get(ref_name):set_flag("corporate_favours", 10.0)
                Nation:get(ref_name):set_flag("corporate_spoil", 5.0)
                -- TODO -5% of total budget
            end,
            effects = "+10.0 corporate favours\n+5 corporate spoil\n-5% of total budget"
        })
        bankrupt_corporation_evhdl:add_decision(Decision:new{
            name = "It's just how it works",
            decision_fn = function(ref_name)
                Nation:get(ref_name):relative_policy_stance(POLICY_CAPITALISM, 0.1)
                Nation:get(ref_name):set_flag("total_corporations", -1.0)
                local provinces = get_corporate_ideal_provinces(ref_name)
				for k, province in pairs(provinces) do
                    -- Upgrade factories
                    local buildings = province:get_buildings()
                    for k, building in pairs(buildings) do
                        building.level = building.level - 1.0
                        province:update_building(building)
                    end
					province:update_pops()
				end
            end,
            effects = "-1 corporation\n+0.1 capitalist\n-1 level of factories for every aforementioned province"
        })
        return EVENT_DO_MANY_TIMES
    end,
}
bankrupt_corporation_evhdl:register()
bankrupt_corporation_evhdl:add_receivers(table.unpack(Nation:get_all()))

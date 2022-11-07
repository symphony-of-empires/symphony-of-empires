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
--  	lua/events/generic.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

-- Generic infection events
generic_infection = Event:new{
	ref_name = "generic_infection",
	conditions_fn = function()
		if math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		local r = math.random(0, 10)
		-- "So how many cholera dynamic-events?", "YES"
		if r == 0 then
			generic_infection.title = "Cholera spreads"
			generic_infection.text = "Due to the inappropiate usage of our sewage system it has made their way into the central waters of many of our cities, we have reports of dead people all over the country"
		elseif r == 1 then
			generic_infection.title = "Tuberculosis spreads"
			generic_infection.text = "We have started to receive numerous reports about a new disease infection point in our country; said disease is tuberculosis and it's mostly lethal"
		elseif r == 2 then
			generic_infection.title = "Disentry surges"
			generic_infection.text = "A new infection of disentry has started to take a toll on the population"
		elseif r == 3 then
			generic_infection.title = "Cholera surges"
			generic_infection.text = "We're starting to see an uprise in cholera cases, an abnormal number of people report having seen or contacted with an infected person"
		else
			generic_infection.title = "Flu"
			generic_infection.text = "A new variant of the flu has appeared in one of our provinces - it is not very lethal, however we can't ignore the potential of it becoming more severe as time goes on"
		end
		generic_infection:update(generic_ideological_protest)
		generic_infection:add_decision(Decision:new{
			name = "We will do what we can to save our people!",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						pop.militancy = pop.militancy + 0.1
						province:update_pop(pop)
					end
					province:update_pops()
				end
			end,
			effects = "+0.5 militancy"
		})
		generic_infection:add_decision(Decision:new{
			name = "We can't do anything about it",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						pop.militancy = pop.militancy + 0.5
						province:update_pop(pop)
					end
					province:update_pops()
				end
			end,
			effects = "+0.5 militancy"
		})
		return EVENT_DO_MANY_TIMES
	end,
    title = "",
	text = ""
}
generic_infection:register()
generic_infection:add_receivers(table.unpack(Nation:get_all()))

-- Generic ideological protest
generic_ideological_protest_evhdl = Event:new{
	ref_name = "generic_ideological_protest",
	conditions_fn = function()
		if math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		Nation:get(ref_name):set_flag("ideological_revolution_id", math.random(0, 6))

		local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
		generic_ideological_protest_evhdl.title = ideology.name .. " outrage!"
		generic_ideological_protest_evhdl.text = "A lot of " .. ideology.name .. "s have started speaking false information about us; what's best for the " .. Nation:get(ref_name).adjective .. " people?"
		generic_ideological_protest_evhdl:update()
		generic_ideological_protest_evhdl:add_decision(Decision:new{
			name = "Shut them down",
			effects = "Every POP that supports " .. ideology.name .. " gets 0.1 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						if pop.ideology.ref_name == ideology.ref_name then
							goto continue
						end
						pop.militancy = pop.militancy + 0.1
						province:update_pop(pop)
						::continue::
					end
					province:update_pops()
				end
			end
		})
		generic_ideological_protest_evhdl:add_decision(Decision:new{
			ref_name = "generic_ideological_protest_decision_1",
			name = "Let them be",
			effects = "Every POP that supports " .. ideology.name .. " gets 0.05 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				local ideology = Ideology:get_by_id(Nation:get(ref_name):get_flag("ideological_revolution_id"))
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						if pop.ideology.ref_name == ideology.ref_name then
							goto continue
						end
						pop.militancy = pop.militancy + 0.05
						province:update_pop(pop)
						::continue::
					end
					province:update_pops()
				end
			end,
		})
		return EVENT_DO_MANY_TIMES
	end,
}
generic_ideological_protest_evhdl:register()
generic_ideological_protest_evhdl:add_receivers(table.unpack(Nation:get_all()))

-- Generic militancy nationwide
generic_militancy_nationwide_evhdl = Event:new{
	ref_name = "generic_militancy_nationwide",
	conditions_fn = function()
		if math.random(0, 100) == 0 then
			return EVENT_CONDITIONS_MET
		end
		return EVENT_CONDITIONS_UNMET
	end,
	event_fn = function(ref_name)
		local texts = {
			title = "Stop the activists",
			text = "Some activists are trying to undermine us with false statments about the efficiency of our goverment - we can always crack down on those liars but we may upset free-speech supporters"	
		}
		generic_militancy_nationwide_evhdl:add_decision(Decision:new{
			name = "Let them spread lies",
			effects = "Every POP gets +0.5 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						pop.militancy = pop.militancy + 0.5
						province:update_pop(pop)
					end
					province:update_pops()
				end
			end,
			effects = "+0.50 militancy and +0.50 militancy nationwide"
		})
		generic_militancy_nationwide_evhdl:add_decision(Decision:new{
			ref_name = "generic_militancy_nationwide_decision_1",
			name = "Take them down",
			effects = "Every POP gets +0.75 militancy",
			decision_fn = function(ref_name)
				local prov_list = Nation:get(ref_name):get_owned_provinces()
				for k, province in pairs(prov_list) do
					local pops = province:get_pops()
					for k, pop in pairs(pops) do
						pop.militancy = pop.militancy + 0.75
						province:update_pop(pop)
					end
					province:update_pops()
				end
			end,
			effects = "+0.75 militancy nationwide"
		})
		return EVENT_DO_MANY_TIMES
	end,
}
generic_militancy_nationwide_evhdl:register()
generic_militancy_nationwide_evhdl:add_receivers(table.unpack(Nation:get_all()))

generic_infection2_evhdl = Event:new{
    ref_name = "generic_infection2",
    conditions_fn = function()
        if math.random(0, 100) == 0 then
            local prov_list = Nation:get(ref_name):get_owned_provinces()
            if table.size(prov_list) > 0 then
                return EVENT_CONDITIONS_MET
            end
        end
        return EVENT_CONDITIONS_UNMET
    end,
    event_fn = function(ref_name)
        local prov_list = Nation:get(ref_name):get_owned_provinces()
        local random_province = prov_list[math.random(1, table.size(prov_list))]
        local disease_names = {
            "Disentery",
            "Cholera",
            "Tuberculosis",
        }
        local disease_names_idx = math.random(1, table.size(disease_names))
        local disease_name = disease_names[disease_names_idx]
        local texts = {
            {
                title = "Large " .. disease_name .. " Spread in " .. random_province.name,
                text = "Due to largely failed attempts in " .. random_province.name .. " to control " .. disease_name .. ", many people have simply been exposed to the disease, whether in the sewers"
            }, {
                title = "Mass Spreading Event in " .. random_province.name,
                text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of " .. disease_name .. ". The disease had a large semi-exposed sewer which could've helped spread the disease"
            }, {
                title = disease_name .. " dominates in ".. random_province.name,
                text = "A large " .. disease_name .. " outbreak in " .. random_province.name .. " has occured. Many have been infected"
            }, { 
                title = disease_name .. " ravages ".. random_province.name,
                text = "A strain of " .. disease_name .. " is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
            },
        }
        local texts_idx = math.random(1, table.size(texts))
        generic_infection2_evhdl.title = texts[texts_idx].title
        generic_infection2_evhdl.text = texts[texts_idx].title
        generic_infection2_evhdl:update()
        generic_infection2_evhdl:add_decision(Decision:new{
            name = "We have to deal with this outbreak.",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 1.0
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+1 militancy"
        })
        generic_infection2_evhdl:add_decision(Decision:new{
            name = "Send a small amount of aid to the area",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 0.5
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+0.5 militancy"
        })
        generic_infection2_evhdl:add_decision(Decision:new{
            name = "Eh, They can deal with it themselves",
            decision_fn = function(ref_name)
                local prov_list = Nation:get(ref_name):get_owned_provinces()
                for k, province in pairs(prov_list) do
                    local pops = province:get_pops()
                    for k, pop in pairs(pops) do
                        pop.militancy = pop.militancy + 1.0
                        province:update_pop(pop)
                    end
                    province:update_pops()
                end
            end,
            effects = "+1 militancy"
        })
        return EVENT_DO_MANY_TIMES
    end,
}
generic_infection2_evhdl:register()
generic_infection2_evhdl:add_receivers(table.unpack(Nation:get_all()))

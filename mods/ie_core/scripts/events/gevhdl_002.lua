TEMP_DATA = {}

function gevhdl002_test()
    if math.random(0, 300) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function gevhdl002_event(ref_name)
	TEMP_DATA.ideology = Ideology:get_by_id(math.random(0, 8))
	
	gevhdl002.title = TEMP_DATA.ideology.name .. " outrage!"
    gevhdl002.text = "A lot of " .. ideology.name .. "s have started speaking false information about us; what's best for the " .. Nation:get(ref_name).adjective .. " people?"
	gevhdl002:update(gevhdl002)
	
	descision = Descision:new{
		ref_name = "gevhdl002_descision_0",
		name = "Shut them down",
		descision_fn = "gevhdl002_descision_0"
	}
	descision.effects = "Every POP that supports " .. TEMP_DATA.ideology.name .. " gets 1 militancy"
	gevhdl002:add_descision(descision)
	
	descision = Descision:new{
		ref_name = "gevhdl002_descision_1",
		name = "Let them be",
		descision_fn = "gevhdl002_descision_1",
	}
	descision.effects = "Every POP that supports " .. TEMP_DATA.ideology.name .. " gets 1 conciousness"
	gevhdl002:add_descision(descision)
	return EVENT_DO_MANY_TIMES
end
function gevhdl002_descision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			if pop.ideology.ref_name == TEMP_DATA.ideology.ref_name then
				goto continue
			end
			
			pop.militancy = pop.militancy + 1.0
			province:update_pop(pop)
			::continue::
		end
		province:update_pops()
	end
end
function gevhdl002_descision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			if pop.ideology.ref_name == TEMP_DATA.ideology.ref_name then
				goto continue
			end
			
			pop.con = pop.con + 1.0
			province:update_pop(pop)
			::continue::
		end
		province:update_pops()
	end
end
gevhdl002 = Event:new{
	ref_name = "gevhdl002",
	conditions_fn = "gevhdl002_test",
	event_fn = "gevhdl002_event",
}
gevhdl002:register()
gevhdl002:add_receivers(table.unpack(Nation:get_all()))

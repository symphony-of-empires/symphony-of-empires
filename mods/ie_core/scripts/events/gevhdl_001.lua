function gevhdl001_test()
    if math.random(0, 300) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function gevhdl001_event(ref_name)
	descision = Descision:new{
		ref_name = "gevhdl001_descision_0",
		name = "We will do what we can to save our people!",
		descision_fn = "gevhdl001_descision_0",
		effects = "+5 con"
	}
	gevhdl001:add_descision(descision)
    descision = Descision:new{
		ref_name = "gevhdl001_descision_1",
		name = "We can't do anything about it",
		descision_fn = "gevhdl001_descision_1",
		effects = "+5 militancy"
	}
	gevhdl001:add_descision(descision)
	return EVENT_DO_MANY_TIMES
end
function gevhdl001_descision_0(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.con = pop.con + 5.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
function gevhdl001_descision_1(ref_name)
	local prov_list = Nation:get(ref_name):get_owned_provinces()
	for k, province in pairs(prov_list) do
		local pops = province:get_pops()
		for k, pop in pairs(pops) do
			pop.militancy = pop.militancy + 5.0
			province:update_pop(pop)
		end
		province:update_pops()
	end
end
gevhdl001 = Event:new{
	ref_name = "gevhdl001",
	conditions_fn = "gevhdl001_test",
	event_fn = "gevhdl001_event",
    title = "Cholera spreads",
	text = "Due to the inappropiate usage of our sewage system it has made their way into the central waters of many of our cities, we have reports of dead people all over the country"
}
gevhdl001:register()
gevhdl001:add_receivers(table.unpack(Nation:get_all()))

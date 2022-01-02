-- Catholic Relief Act of 1829
-- https://en.wikipedia.org/wiki/Roman_Catholic_Relief_Act_1829
function catholic_relief_act_of_1829_test()
    local year = get_year()
    if year == 1829 and math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function catholic_relief_act_of_1829_event()
	descision = Descision:new{
		ref_name = "catholic_relief_act_of_1829_descision_0",
		name = "Yes, allow catholics on the parliament",
		descision_fn = "catholic_relief_act_of_1829_descision_0",
		effects = "None"
	}
	catholic_relief_act_of_1829:add_descision(descision)
	
    descision = Descision:new{
		ref_name = "catholic_relief_act_of_1829_descision_1",
		name = "No, the church and the state must remain separate",
		descision_fn = "catholic_relief_act_of_1829_descision_1",
		effects = "x100 more militancy, x100 more conciousness"
	}
	catholic_relief_act_of_1829:add_descision(descision)
	return EVENT_DO_ONE_TIME
end
function catholic_relief_act_of_1829_descision_0()
    united_kingdom:add_accepted_religion(catholic)
end
function catholic_relief_act_of_1829_descision_1()
    local prov = Nation:get_owned_provinces()
    for k, v in pairs(prov) do
        k:multiply_militancy_by_religion(catholic, 100.0)
        k:multiply_con_by_religion(catholic, 100.0)
    end
end
catholic_relief_act_of_1829 = Event:new{
	ref_name = "catholic_relief_act_of_1829",
	conditions_fn = "catholic_relief_act_of_1829_test",
	event_fn = "catholic_relief_act_of_1829_event",
    title = "Catholic Relief Act of 1829",
	text = "Daniel O'Connell threatens insurrection in Ireland if the catholic relief act is not passed"
}
catholic_relief_act_of_1829:register()
function t001_test()
    if math.random(0, 50) == 0 then
        return EVENT_CONDITIONS_MET
    end
	return EVENT_CONDITIONS_UNMET
end
function t001_event(ref_name)
	descision = Descision:new{
		ref_name = "t001_descision_0",
		name = "Taza is ours, by any means!",
		descision_fn = "t001_descision_0",
		effects = "Annex morroco"
	}
	t001:add_descision(descision)

    descision = Descision:new{
		ref_name = "t001_descision_1",
		name = "Taza is rightful Spanish land!",
		descision_fn = "t001_descision_1",
		effects = "War with morrocco"
	}
	t001:add_descision(descision)

    descision = Descision:new{
		ref_name = "t001_descision_2",
		name = "Maybe we can sort this out diplomatically?",
		descision_fn = "t001_descision_2",
		effects = "+50 relations with Morrocco and France"
	}
	t001:add_descision(descision)
	return EVENT_DO_ONE_TIME
end
function t001_descision_0(ref_name)
	local nation = Nation:get("morrocco"):get_owned_provinces()
	for k, v in pairs(prov) do
		print(v.ref_name)
        v:give_and_relinquish_to(Nation:get("spain"))
	end
end
function t001_descision_1(ref_name)
    Nation:get("spain"):declare_unjustified_war(Nation:get("morrocco"))
end
function t001_descision_2(ref_name)
    local rel = {}
    
    rel = Nation:get("spain"):get_relation(Nation:get("morrocco"))
    rel.relation = rel.relation + 50.0
    Nation:get("spain"):set_relation(Nation:get("morrocco"), rel)

    rel = Nation:get("spain"):get_relation(Nation:get("france"))
    rel.relation = rel.relation + 50.0
    Nation:get("spain"):set_relation(Nation:get("france"), rel)
end
t001 = Event:new{
	ref_name = "t001",
	conditions_fn = "t001_test",
	event_fn = "t001_event",
    title = "Taza concession",
	text = "We must retake the Morrocan lands to give spain a colonial hold over Africa!"
}
t001:register()
t001:add_receivers(Nation:get("spain"))

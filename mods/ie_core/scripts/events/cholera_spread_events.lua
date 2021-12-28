function cholera_test()
    if math.random(0, 100) == 0 then
        return EVENT_CONDITIONS_MET
    end
    return EVENT_CONDITIONS_UNMET
end
function cholera_evhdl(ref_name)
    local r == math.random(0, 5)
      local prov_list = Nation:get(ref_name):get_owned_provinces()
    local n_prov_list = 0
    for _ in pairs(prov_list) do count = count + 1 end
    local random_province = prov_list[math.random(0, n_prov_list)]
    if r == 0 then
        cholera_event.title = "Large Cholera Spread in " .. random_province.name
        cholera_event.text = "Due to largely failed attempts in " .. random_province.name .. " to control cholera, many people have simply been exposed to the disease, whether in the sewers"
    elseif r == 1 then
        cholera_event.title = "Mass Spreading Event in " .. random_province.name
        cholera_event.text = "A large party in ".. random_province.name .. " is believed to be one of the causes of a major spread of cholera. The disease had a large semi-exposed sewer which could've helped spread the disease"
    elseif r == 3 then
        cholera_event.title = "Cholera Dominates in ".. random_province.name
        cholera_event.text = "A large cholera outbreak in " .. random_province.name .. " has occured. Many have been infected"
    else 
        cholera_event.title = "Cholera Ravages ".. random_province.name
        cholera_event.text = "A strain of Cholera is ravaging the small villages of " .. random_province.name .." . This is proving to be a disaster as government officials in that area prepare a rescue mission"
    end
    
    decision = Decision:new{
        ref_name = "cholera_evhdl_decision_0"
        name = "We have to deal with this outbreak."
        decision_fn = "cholera_evhdl_decision_0"
        effects = "+1 con"
    }
    decision = Decision:new{
        ref_name = "cholera_evdhl_decision_1"
        name = "Send a small amount of aid to the area"
        decision_fn = "cholera_evdhl_decision_1"
        effects = "+0.5 con"
    }
    decision = Decision:new{
        ref_name = "cholera_evdhl_decision_2"
        name = "Eh, They can deal with it themselves"
        decision_fn = "cholera_evdhl_decision_2"
        effects = "+1 militancy"
    }
    
    cholera_evdhl:add_decision(decision)
    return = EVENT_DO_MANY_TIMES
end
	
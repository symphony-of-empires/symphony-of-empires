require("api")

function ec_test()
	if(year == 4 and month == 3 and day == 25) then
		return 1
	else
		print("japan survives... for now")
		return 0
	end
end

function cisplatine_war_end_brazil_test()
	-- requirements??
end
function cisplatine_war_end_brazil_event()
	local title = "End of CIsplatine War?"
	local text = "brazil agrees"
	local descision_0 = "Fine, let them get their lands"

	local window = UI:create_popup(title, text)
	UI:add_descision(window, descision_0, cisplatina_war_end_brazil_descision_0)
end
function cisplatina_war_end_brazil_descision_0()

end

function cisplatina_war_end_test()
	-- what are the requirements???
end
function cisplatina_war_end_event()
	local title = "End of the Cisplatine War- the Treaty of Montevideo"
	local text = "After a long stalemate, Pedro I of Brazil has agreed to a treaty, granting independence to the Eastern Republic of Uruguay. Thanks to the support of the United provinces of Río de la Plata, and British mediation, The Cisplatine War comes to an end. As the Empire of Brazil falters, public unrest grows..."
	local descision_0 = "The bloodshed is at an end, and from the battlefield rises yet another South American state."
	
	local window = UI:create_popup(title, text)
	UI:add_descision(window, descision_0, cisplatina_war_end_descision_0)
	return 0
end
function cisplatina_war_end_descision_0()
	-- something happens when we take descision!
	local province = Province:get({}, "cisplatina")
	Province:give_to(province, uruguay)
	Province:rename(province, _("Montevideo"))
end
cisplatina_war_end = Event:create{ ref_name = "cisplatina_war_end", conditions_fn = "cisplatina_war_end_test", event_fn = "cisplatina_war_end_event" }
Event:register(cisplatina_war_end)
Event:add_receivers(cisplatina_war_end, Nation:get({}, "brazil"), Nation:get({}, "gran_colombia"), Nation:get({}, "mexico"))

print("loaded mod.lua")
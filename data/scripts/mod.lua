require("api")

function ec_test()
	if(year == 4 and month == 3 and day == 25) then
		return EVENT_CONDITIONS_MET
	else
		print("japan survives... for now")
		return EVENT_CONDITIONS_UNMET
	end
end

function cisplatine_war_end_brazil_test()
	-- requirements??
end
function cisplatine_war_end_brazil_event()
	local title = "End of Cisplatine War?"
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
	local descision_0 = Decision:create{
		ref_name = "cisplatine_war_end_0",
		name = "The bloodshed is at an end, and from the battlefield rises yet another South American state",
		decision_fn = "cisplatine_war_end_descision_0",
		effects = "Brazil loses the province of Cisplatina and the state of Uruguay is created - with capital in Cisplatina"
	}
	Event:add_descision(window, descision_0, cisplatina_war_end_descision_0)
	return EVENT_DO_ONE_TIME
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

function cake_test()
	-- Requirements
	return EVENT_CONDITIONS_MET
end
function cake_event()
	local title = "Cake"
	local text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
	descision_0 = Descision:create{
		ref_name = "cake_descision_0",
		name = "Very good",
		descision_fn = "cake_descision_0",
		effects = "You will consume cake"
	}
	Event:add_descision(cake_thing, descision_0)
	
	descision_1 = Descision:create{
		ref_name = "cake_descision_1",
		name = "Fuck you",
		descision_fn = "cake_descision_1",
		effects = "You will consume cake anyways"
	}
	Event:add_descision(cake_thing, descision_1)
	return EVENT_DO_ONE_TIME
end
function cake_descision_0()
	print('The cake tasted like a cake, what a surprise! and you find a bunch of peseants outside your palace... oh no')
end
function cake_descision_1()
	print('Why no eat my poisoned cake? i put so much poison in it :(')
end

-- TODO: This causes a segfault for some reason
cake_thing = Event:create{ ref_name = "cake_thing", conditions_fn = "cake_test", event_fn = "cake_event" }
Event:register(cake_thing)
Event:add_receivers(cake_thing, Nation:get({}, "hawai_i"), Nation:get({}, "gran_colombia"), Nation:get({}, "mexico"))

print("loaded mod.lua")

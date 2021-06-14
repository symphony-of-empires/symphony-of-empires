require("api")

function cake_test()
	-- Requirements
	return EVENT_CONDITIONS_MET
end
function cake_event()
	local title = "Cake"
	local text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
	local descision_0 = Descision:create{
		ref_name = "cake_descision_0",
		name = "Very good",
		descision_fn = "cake_descision_0",
		effects = "You will consume cake"
	}
	Event:add_descision(cake_thing, descision_0)
	
	local descision_1 = Descision:create{
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

cake_thing = Event:create{ ref_name = "cake_thing", conditions_fn = "cake_test", event_fn = "cake_event" }
Event:register(cake_thing)
Event:add_receivers(cake_thing, Nation:get({}, "someland"), Nation:get({}, "corner"), Nation:get({}, "aristocrat"))

print("loaded mod.lua")

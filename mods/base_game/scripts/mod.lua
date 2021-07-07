require("api")

function cake_test()
	-- Requirements
	return EVENT_CONDITIONS_MET
end
function cake_event()
	local title = "Cake"
	local text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
	descision = Descision:create{
		ref_name = "cake_descision_0",
		name = "I think i will eat it",
		descision_fn = "cake_descision_0",
		effects = "You will consume cake"
	}
	Event:add_descision(cake_thing, descision)
	
	descision = Descision:create{
		ref_name = "cake_descision_1",
		name = "Fuck you",
		descision_fn = "cake_descision_1",
		effects = "You will consume cake anyways"
	}
	Event:add_descision(cake_thing, descision)

	descision = Descision:create{
		ref_name = "cake_descision_2",
		name = "Cake!?",
		descision_fn = "cake_descision_2",
		effects = "Your compulsory desire for cake has began to grow"
	}
	Event:add_descision(cake_thing, descision)

	descision = Descision:create{
		ref_name = "cake_descision_3",
		name = "Britain, we have a problem",
		descision_fn = "cake_descision_3",
		effects = "What the fuck is britain gonna do, invade us?"
	}
	Event:add_descision(cake_thing, descision)
	return EVENT_DO_ONE_TIME
end
function cake_descision_0()
	print('The cake tasted like a cake, what a surprise! and you find a bunch of peseants outside your palace... oh no')
end
function cake_descision_1()
	print('Why no eat my poisoned cake? i put so much poison in it :(')
end
function cake_descision_2()
	print('Bruh, where the fuck am i?')
end
function cake_descision_3()
	print('Britain converted the cake into tea')
end
cake_thing = Event:create{
	ref_name = "cake_thing",
	conditions_fn = "cake_test",
	event_fn = "cake_event",
	
	title = "Cake",
	text = "Sir, a cake has just magically appeared in your desk, what would you like to do?"
}
Event:register(cake_thing)
Event:add_receivers(cake_thing,
	Nation:get({}, "hawai_i")
)

print("loaded mod.lua")

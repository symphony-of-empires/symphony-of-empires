Good = { id = 0, name = "", ref_name = "", is_edible = false }
function Good:create(good)
	good.parent = self
	return good
end
function Good:register(good)
	good.id = add_good(good.ref_name, good.name, good.is_edible)
end
function Good:get(good, ref_name)
	good.parent = self
	good.id, good.ref_name, good.name = get_good(ref_name)
	return good
end

UnitTrait = {
	id = 0,
	ref_name = "",
	supply_consumption_mod = 1.0,
	speed_mod = 1.0,
	max_health_mod = 1.0,
	defense_mod = 1.0,
	attack_mod = 1.0
}
function UnitTrait:create(unit_trait)
	unit_trait.parent = self
	return unit_trait
end
function UnitTrait:register(unit_trait)
	unit_trait.id = add_unit_trait(
		unit_trait.ref_name,
		unit_trait.supply_consumption_mod,
		unit_trait.speed_mod,
		unit_trait.max_health_mod,
		unit_trait.defense_mod,
		unit_trait.attack_mod
	)
end

Company = { id = 0, name = "", is_transport = false, is_retailer = false, is_industry = false, money = 0, }
function Company:create(company)
	company.parent = self
	return company
end
function Company:register(company)
	company.id = add_company(company.name, company.money, company.is_transport, company.is_retailer, company.is_industry)
end
function Company:add_province(company, province)
	add_op_province_to_company(company.id, province.id)
end

IndustryType = { id = 0, name = "", ref_name = "" }
function IndustryType:create(industry_type)
	industry_type.parent = self
	return industry_type
end
function IndustryType:register(industry_type)
	industry_type.id = add_industry_type(industry_type.ref_name, industry_type.name)
end
function IndustryType:get(industry_type, ref_name)
	industry_type.parent = self
	industry_type.id, industry_type.ref_name, industry_type.name = get_industry_type(ref_name)
	return industry_type
end
function IndustryType:add_input(industry_type, good)
	add_input_to_industry_type(industry_type.id, good.id)
end
function IndustryType:add_output(industry_type, good)
	add_output_to_industry_type(industry_type.id, good.id)
end

Nation = { id = 0, name = "", ref_name = "" }
function Nation:create(nation)
	nation.parent = self
	return nation
end
function Nation:register(nation)
	nation.id = add_nation(nation.ref_name, nation.name)
end
function Nation:get(nation, ref_name)
	nation.parent = self
	nation.id, nation.ref_name, nation.name = get_nation(ref_name)
	return nation
end
function Nation:set_capital(nation, province)
	set_nation_capital(nation.id, province.id)
end
function Nation:add_accepted_culture(nation, culture)
	add_nation_accepted_culture(nation.id, culture.id)
end

Province = { id = 0, name = "", ref_name = "", color = 0, }
function Province:create(province)
	province.parent = self
	return province
end
function Province:register(province)
	province.id = add_province(province.ref_name, province.color, province.name)
end
function Province:get(province, ref_name)
	province.parent = self
	province.id, province.ref_name, province.name, province.color = get_province(ref_name)
	return province
end
function Province:add_industry(province, industry_type, company)
	add_province_industry(province.id, company.id, industry_type.id)
end
function Province:give_to(province, nation)
	give_province_to(province.id, nation.id)
end
function Province:add_pop(province, pop_type, culture, religion, size, literacy)
	add_province_pop(province.id, pop_type.id, culture.id, religion.id, size, literacy)
end
function Province:rename(province, new_name)
	rename_province(province.id, new_name)
end
function Province:add_nucleus(province, nation)
	add_province_nucleus(province.id, nation.id)
end

Event = { ref_name = "", conditions_fn = "", event_fn = "", title = "", text = "" }
function Event:create(event)
	event.parent = self
	return event
end
function Event:register(event)
	event.id = add_event(event.ref_name, event.conditions_fn, event.event_fn, event.title, event.text)
end
function Event:get(event, ref_name)
	event.parent = self
	event.id, event.ref_name, event.conditions_fn, event.event_fn = get_event(ref_name)
	return event
end
function Event:add_receivers(event, ...)
	local args = table.pack(...)
	for i = 1, args.n do
		args[i] = args[i].id
	end
	add_event_receivers(event.id, args.n, table.unpack(args))
end
function Event:add_descision(event, descision)
	add_descision(event.id, descision.ref_name, descision.name, descision.descision_fn, descision.effects)
end

Descision = { ref_name = "", descision_fn = "", name = "", effects = "" }
function Descision:create(descision)
	descision.parent = self
	return descision
end

PopType = { id = 0, ref_name = "", name = "" }
function PopType:create(pop_type)
	pop_type.parent = self
	return pop_type
end
function PopType:get(pop_type, ref_name)
	pop_type.id, pop_type.ref_name, pop_type.name = get_pop_type(ref_name)
end
function PopType:register(pop_type)
	pop_type.id = add_pop_type(pop_type.ref_name, pop_type.name)
end

Culture = { id = 0, ref_name = "", name = "" }
function Culture:create(culture)
	culture.parent = self
	return culture
end
function Culture:get(culture, ref_name)
	culture.id, culture.ref_name, culture.name = get_culture(ref_name)
end
function Culture:register(culture)
	culture.id = add_culture(culture.ref_name, culture.name)
end

Religion = { id = 0, ref_name = "", name = "" }
function Religion:create(religion)
	religion.parent = self
	return religion
end
function Religion:get(religion, ref_name)
	religion.id, religion.ref_name, religion.name = get_religion(ref_name)
end
function Religion:register(religion)
	religion.id = add_religion(religion.ref_name, religion.name)
end

OutpostType = {
	id = 0,
	ref_name = "",
	is_naval = false,
	is_build_land_units = false,
	is_build_naval_units = false,
	defense_bonus = 1.0,
}
function OutpostType:create(outpost_type)
	outpost_type.parent = self
	return outpost_type
end
function OutpostType:register(outpost_type)
	outpost_type.id = add_outpost_type(outpost_type.ref_name, outpost_type.is_naval, outpost_type.is_build_land_units, outpost_type.is_build_naval_units, outpost_type.defense_bonus)
end

UnitType = {
	id = 0,
	ref_name = "",
	name = "",
	health = 100.0,
	defense = 1.0,
	attack = 1.0,
	max_defensive_ticks = 25,
	position_defense = 0.1,
}
function UnitType:create(unit_type)
	unit_type.parent = self
	return unit_type
end
function UnitType:get(unit_type, ref_name)
	unit_type.id, unit_type.ref_name, unit_type.name, unit_type.attack, unit_type.defense, unit_type.health, unit_type.max_defensive_ticks, unit_type.position_defense = get_unit_type(ref_name)
end
function UnitType:register(unit_type)
	unit_type.id = add_unit_type(unit_type.ref_name, unit_type.name, unit_type.attack, unit_type.defense, unit_type.health, unit_type.max_defensive_ticks, unit_type.position_defense)
end
function UnitType:requires_good(unit_type, good, amount)
	add_req_good_unit_type(unit_type.id, good.id, amount)
end

BoatType = { id = 0, ref_name = "", name = "", health = 100.0, defense = 1.0, attack = 1.0, capacity = 100 }
function BoatType:create(boat_type)
	boat_type.parent = self
	return boat_type
end
function BoatType:get(boat_type, ref_name)
	boat_type.id, boat_type.ref_name, boat_type.name, boat_type.attack, boat_type.defense, boat_type.health, boat_type.capacity = get_boat_type(ref_name)
end
function BoatType:register(boat_type)
	boat_type.id = add_boat_type(boat_type.ref_name, boat_type.name, boat_type.attack, boat_type.defense, boat_type.health, boat_type.capacity)
end
function BoatType:requires_good(boat_type, good, amount)
	add_req_good_boat_type(boat_type.id, good.id, amount)
end

-- For sanity
function rgb(r, g, b)
	local color = 0x000000
	color = color | (r << 16)
	color = color | (g << 8)
	color = color | (b << 0)
	return color
end

print("loaded api.lua")

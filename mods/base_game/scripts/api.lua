Technology = { id = 0, ref_name = "", name = "", description = "", cost = 1.0 }
function Technology:new(technology)
	technology.parent = self
	return technology
end
function Technology:register(self)
	self.id = add_technology(self.ref_name, self.name, self.description, self.cost)
end
function Technology:get(technology, ref_name)
	technology.parent = self
	technology.id, technology.name, technology.description = get_technology(ref_name)
	technology.ref_name = ref_name
	return technology
end
function Technology:requires_technology(self, technology)
	add_req_tech_to_tech(self.id, technology.id)
end

Invention = { id = 0, ref_name = "", name = "", description = "" }
function Invention:new(invention)
	invention.parent = self
	return invention
end
function Invention:register(self)
	self.id = add_invention(self.ref_name, self.name, self.description)
end
function Invention:get(invention, ref_name)
	invention.parent = self
	invention.id, invention.name, invention.description = get_invention(ref_name)
	invention.ref_name = ref_name
	return invention
end

Ideology = {
	id = 0,
	ref_name = "",
	name = "",
	check_policies_fn = ""
}
function Ideology:new(ideology)
	ideology.parent = self
	return ideology
end
function Ideology:register(self)
	self.id = add_ideology(self.ref_name, self.name, self.check_policies_fn)
end
function Ideology:get(ideology, ref_name)
	ideology.parent = self
	ideology.id, ideology.name = get_ideology(ref_name)
	ideology.ref_name = ref_name
	return ideology
end

Policies = {
	treatment = 0,
    migration = 0,
    immigration = 0,
    censorship = 0,
    build_infrastructure = 0,
    build_factories = 0,
    national_id = true,
    men_suffrage = true,
    men_labour = true,
    women_suffrage = true,
    women_labour = true,
    private_property = true,
    companies_allowed = true,
    public_education = true,
    secular_education = true,
    public_healthcare = true,
    social_security = true,
    slavery = true,
    legislative_parliament = true,
    executive_parliament = true,
    constitutional = true,
    foreign_trade = true,
    import_tax = 0.0,
    export_tax = 0.0,
    domestic_import_tax = 0.0,
    domestic_export_tax = 0.0,
    poor_flat_tax = 0.0,
    med_flat_tax = 0.0,
    rich_flat_tax = 0.0,
    industry_tax = 0.0,
    military_spending = 0.0,
    free_supplies = true,
    minimum_wage = 0.0
}
function Policies:new(policies)
	policies.parent = self
	return policies
end

Good = { id = 0, name = "", ref_name = "", is_edible = false }
function Good:new(good)
	good.parent = self
	return good
end
function Good:register(self)
	self.id = add_good(self.ref_name, self.name, self.is_edible)
end
function Good:get(good, ref_name)
	good.parent = self
	good.id, good.name = get_good(ref_name)
	good.ref_name = ref_name
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
function UnitTrait:new(unit_trait)
	unit_trait.parent = self
	return unit_trait
end
function UnitTrait:register(self)
	self.id = add_unit_trait(self.ref_name, self.supply_consumption_mod, self.speed_mod, self.max_health_mod, self.defense_mod, self.attack_mod)
end

Company = { id = 0, name = "", is_transport = false, is_retailer = false, is_industry = false, money = 0, }
function Company:new(company)
	company.parent = self
	return company
end
function Company:register(self)
	self.id = add_company(self.name, self.money, self.is_transport, self.is_retailer, self.is_industry)
end

-- Add a province to the list of operational provinces
function Company:add_province(self, province)
	add_op_province_to_company(self.id, province.id)
end

IndustryType = { id = 0, name = "", ref_name = "" }
function IndustryType:new(industry_type)
	industry_type.parent = self
	return industry_type
end
function IndustryType:register(self)
	self.id = add_industry_type(self.ref_name, self.name)
end
function IndustryType:get(industry_type, ref_name)
	industry_type.parent = self
	industry_type.id, industry_type.name = get_industry_type(ref_name)
	industry_type.ref_name = ref_name
	return industry_type
end
function IndustryType:add_input(self, good)
	add_input_to_industry_type(self.id, good.id)
end
function IndustryType:add_output(self, good)
	add_output_to_industry_type(self.id, good.id)
end
function IndustryType:requires_good(self, good, amount)
	add_req_good_to_industry_type(self.id, good.id, amount)
end

Nation = { id = 0, name = "", ref_name = "" }
function Nation:new(nation)
	nation.parent = self
	return nation
end
function Nation:register(self)
	self.id = add_nation(self.ref_name, self.name)
end
function Nation:get(nation, ref_name)
	nation.parent = self
	nation.id, nation.name = get_nation(ref_name)
	nation.ref_name = ref_name
	return nation
end
function Nation:set_capital(self, province)
	set_nation_capital(self.id, province.id)
end
function Nation:add_accepted_culture(self, culture)
	add_nation_accepted_culture(self.id, culture.id)
end
function Nation:get_policies(self)
	policies = Policies:new{}

	-- If someone knows a better way to do this please do a PR
	policies.treatment, policies.migration, policies.immigration, policies.censorship, policies.build_infrastructure, policies.build_factories, policies.national_id, policies.men_suffrage, policies.men_labour, policies.women_suffrage, policies.women_labour, policies.private_property, policies.companies_allowed, policies.public_education, policies.secular_education, policies.public_healthcare, policies.social_security, policies.slavery, policies.legislative_parliament, policies.executive_parliament, policies.constitutional, policies.foreign_trade, policies.import_tax, policies.export_tax, policies.domestic_import_tax, policies.domestic_export_tax, policies.poor_flat_tax, policies.med_flat_tax, policies.rich_flat_tax, policies.industry_tax, policies.military_spending, policies.free_supplies, policies.minimum_wage = get_nation_policies(self.id)
	return policies
end
function Nation:set_policies(self, policies)
	set_nation_policies(self.id, policies.treatment, policies.migration, policies.immigration, policies.censorship, policies.build_infrastructure, policies.build_factories, policies.national_id, policies.men_suffrage, policies.men_labour, policies.women_suffrage, policies.women_labour, policies.private_property, policies.companies_allowed, policies.public_education, policies.secular_education, policies.public_healthcare, policies.social_security, policies.slavery, policies.legislative_parliament, policies.executive_parliament, policies.constitutional, policies.foreign_trade, policies.import_tax, policies.export_tax, policies.domestic_import_tax, policies.domestic_export_tax, policies.poor_flat_tax, policies.med_flat_tax, policies.rich_flat_tax, policies.industry_tax, policies.military_spending, policies.free_supplies, policies.minimum_wage)
end
function Nation:set_ideology(self, ideology)
	set_nation_ideology(self.id, ideology.id)
end
function Nation:add_client_hint(self, ideology, alt_name, colour)
	add_nation_client_hint(self.id, ideology.id, alt_name, colour)
end

Province = { id = 0, name = "", ref_name = "", color = 0 }
function Province:new(province)
	province = province or {}
	setmetatable(province, self)
	province.__index = self
	return province
end
function Province:register(self)
	self.id = add_province(self.ref_name, self.color, self.name)
end
function Province:get(province, ref_name)
	province.parent = self
	province.id, province.name, province.color = get_province(ref_name)
	province.ref_name = ref_name
	return province
end
function Province:get_by_id(province, id)
	province.parent = self
	province.ref_name, province.name, province.color = get_province_by_id(id)
	province.id = id
	return province
end
function Province:add_industry(self, industry_type, company)
	add_province_industry(self.id, company.id, industry_type.id)
end
function Province:give_to(self, nation)
	give_province_to(self.id, nation.id)
end
function Province:get_owner(self)
	return Nation:get({}, get_province_owner(self.id))
end
function Province:get_controller(self)
	return Nation:get({}, get_province_controller(self.id))
end
function Province:get_neighbours_id(self)
	return get_province_neighbours(self.id)
end
-- Increments militancy for all POPs
function Province:multiply_militancy(self, factor)
	multiply_province_militancy_global(self.id, factor)
end
function Province:multiply_militancy_by_culture(self, culture, factor)
	multiply_province_militancy_by_culture(self.id, culture.id, factor)
end
function Province:multiply_militancy_by_religion(self, religion, factor)
	multiply_province_militancy_by_religion(self.id, religion.id, factor)
end
-- Increments consciousness for all POPs
function Province:multiply_consciousness_global(self, factor)
	multiply_province_consciousness_global(self.id, factor)
end
function Province:multiply_consciousness_by_culture(self, culture, factor)
	multiply_province_consciousness_by_culture(self.id, culture.id, factor)
end
function Province:multiply_consciousness_by_religion(self, religion, factor)
	multiply_province_consciousness_by_religion(self.id, religion.id, factor)
end
-- Adds a POP to the province
function Province:add_pop(self, pop_type, culture, religion, size, literacy)
	add_province_pop(self.id, pop_type.id, culture.id, religion.id, size, literacy)
end
-- Rename a province
function Province:rename(self, new_name)
	rename_province(self.id, new_name)
end
-- Adds a country to the nucleus list of a province
function Province:add_nucleus(self, nation)
	add_province_nucleus(self.id, nation.id)
end

Event = { ref_name = "", conditions_fn = "", event_fn = "", title = "", text = "" }
function Event:new(event)
	event.parent = self
	return event
end
function Event:register(self)
	self.id = add_event(self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text)
end
function Event:get(event, ref_name)
	event.parent = self
	event.id, event.conditions_fn, event.event_fn = get_event(ref_name)
	event.ref_name = ref_name
	return event
end
function Event:add_receivers(self, ...)
	local args = table.pack(...)
	for i = 1, args.n do
		args[i] = args[i].id
	end
	add_event_receivers(self.id, args.n, table.unpack(args))
end
function Event:add_descision(self, descision)
	add_descision(self.id, descision.ref_name, descision.name, descision.descision_fn, descision.effects)
end

Descision = { ref_name = "", descision_fn = "", name = "", effects = "" }
function Descision:new(descision)
	descision.parent = self
	return descision
end

PopType = { id = 0, ref_name = "", name = "" }
function PopType:new(pop_type)
	pop_type.parent = self
	return pop_type
end
function PopType:get(pop_type, ref_name)
	pop_type.parent = self
	pop_type.id, pop_type.name = get_pop_type(ref_name)
	pop_type.ref_name = ref_name
	return pop_type
end
function PopType:register(self)
	self.id = add_pop_type(self.ref_name, self.name)
end

Culture = { id = 0, ref_name = "", name = "" }
function Culture:new(culture)
	culture.parent = self
	return culture
end
function Culture:get(culture, ref_name)
	culture.parent = self
	culture.id, culture.name = get_culture(ref_name)
	culture.ref_name = ref_name
	return culture
end
function Culture:register(self)
	self.id = add_culture(self.ref_name, self.name)
end

Religion = { id = 0, ref_name = "", name = "" }
function Religion:new(religion)
	religion.parent = self
	return religion
end
function Religion:get(religion, ref_name)
	religion.parent = self
	religion.id, religion.name = get_religion(ref_name)
	religion.ref_name = ref_name
	return religion
end
function Religion:register(self)
	self.id = add_religion(self.ref_name, self.name)
end

BuildingType = {
	id = 0,
	ref_name = "",
	is_naval = false,
	is_build_land_units = false,
	is_build_naval_units = false,
	defense_bonus = 1.0,
}
function BuildingType:new(building_type)
	building_type.parent = self
	return building_type
end
function BuildingType:register(self)
	self.id = add_building_type(self.ref_name, self.is_naval, self.is_build_land_units, self.is_build_naval_units, self.defense_bonus)
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
function UnitType:new(unit_type)
	unit_type.parent = self
	return unit_type
end
function UnitType:get(unit_type, ref_name)
	unit_type.parent = self
	unit_type.id, unit_type.name, unit_type.attack, unit_type.defense, unit_type.health, unit_type.max_defensive_ticks, unit_type.position_defense = get_unit_type(ref_name)
	unit_type.ref_name = ref_name
	return unit_type
end
function UnitType:register(self)
	self.id = add_unit_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.max_defensive_ticks, self.position_defense)
end
function UnitType:requires_good(self, good, amount)
	add_req_good_unit_type(self.id, good.id, amount)
end

BoatType = { id = 0, ref_name = "", name = "", health = 100.0, defense = 1.0, attack = 1.0, capacity = 100 }
function BoatType:new(boat_type)
	boat_type.parent = self
	return boat_type
end
function BoatType:get(boat_type, ref_name)
	boat_type.parent = self
	boat_type.id, boat_type.name, boat_type.attack, boat_type.defense, boat_type.health, boat_type.capacity = get_boat_type(ref_name)
	boat_type.ref_name = ref_name
	return boat_type
end
function BoatType:register(self)
	self.id = add_boat_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.capacity)
end
function BoatType:requires_good(self, good, amount)
	add_req_good_boat_type(self.id, good.id, amount)
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

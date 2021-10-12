NationMod = {
	id = 0,
	name = "",
	industry_output = 1.0,
	industry_input = 1.0,
	workers_needed = 1.0,
	salary_paid = 1.0,
	delivery_cost = 1.0,
	literacy_learn = 1.0,
	reproduction = 1.0,
	death = 1.0,
	militancy = 1.0,
	consciousness = 1.0,
	life_needs_met = 1.0,
	everyday_needs_met = 1.0,
	luxury_needs_met = 1.0
}
function NationMod:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function NationMod:register()
	self.id = add_nation_mod(self.ref_name, self.name, self.industry_output, self.industry_input, self.workers_needed, self.salary_paid, self.delivery_cost, self.literacy_learn, self.reproduction, self.death, self.militancy, self.consciousness, self.life_needs_met, self.everyday_needs_met, self.luxury_needs_met)
end
function NationMod:get(ref_name)
	o = NationMod:new()
	o.id, o.name, o.industry_output, o.industry_input, o.workers_needed, o.salary_paid, o.delivery_cost, o.literacy_learn, o.reproduction, o.death, o.militancy, o.consciousness, o.life_needs_met, o.everyday_needs_met, o.luxury_needs_met = get_nation_mod(ref_name)
	o.ref_name = ref_name
	return o
end

Technology = {
	id = 0,
	ref_name = "",
	name = "",
	description = "",
	cost = 1.0,
	type = TECH_STRATEGIC
}
function Technology:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Technology:register()
	self.id = add_technology(self.ref_name, self.name, self.description, self.cost, self.type)
end
function Technology:get(o, ref_name)
	o = Technology:new()
	o.id, o.name, o.description, o.type = get_technology(ref_name)
	o.ref_name = ref_name
	return o
end
function Technology:requires_technology(o)
	add_req_tech_to_tech(self.id, o.id)
end

Invention = {
	id = 0,
	ref_name = "",
	name = "",
	description = ""
}
function Invention:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Invention:register()
	self.id = add_invention(self.ref_name, self.name, self.description)
end
function Invention:get(ref_name)
	o = Invention:new()
	o.id, o.name, o.description = get_invention(ref_name)
	o.ref_name = ref_name
	return o
end
function Invention:set_nation_modifier(self, mod)
	set_nation_mod_to_invention(self.id, mod.id)
end

Ideology = {
	id = 0,
	ref_name = "",
	name = "",
	check_policies_fn = ""
}
function Ideology:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Ideology:register()
	self.id = add_ideology(self.ref_name, self.name, self.check_policies_fn)
end
function Ideology:get(ref_name)
	o = Ideology:new()
	o.id, o.name = get_ideology(ref_name)
	o.ref_name = ref_name
	return o
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
function Policies:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

Good = {
	id = 0,
	name = "",
	ref_name = "",
	is_edible = false
}
function Good:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Good:register()
	self.id = add_good(self.ref_name, self.name, self.is_edible)
end
function Good:get(o, ref_name)
	o = Good:new()
	o.id, o.name = get_good(ref_name)
	o.ref_name = ref_name
	return o
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
function UnitTrait:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function UnitTrait:register()
	self.id = add_unit_trait(self.ref_name, self.supply_consumption_mod, self.speed_mod, self.max_health_mod, self.defense_mod, self.attack_mod)
end

Company = {
	id = 0,
	name = "",
	is_transport = false,
	is_retailer = false,
	is_industry = false,
	money = 0,
}
function Company:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Company:register()
	self.id = add_company(self.name, self.money, self.is_transport, self.is_retailer, self.is_industry)
end

-- Add a province to the list of operational provinces
function Company:add_province(province)
	add_op_province_to_company(self.id, province.id)
end

Nation = {
	id = 0,
	name = "",
	ref_name = ""
}
function Nation:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:register()
	self.id = add_nation(self.ref_name, self.name)
end
function Nation:get(ref_name)
	o = Nation:new()
	o.id, o.name = get_nation(ref_name)
	o.ref_name = ref_name
	return o
end
function Nation:set_capital(province)
	set_nation_capital(self.id, province.id)
end
function Nation:add_accepted_culture(culture)
	add_nation_accepted_culture(self.id, culture.id)
end
function Nation:get_policies()
	o = Policies:new()

	-- If someone knows a better way to do this please do a PR
	o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.national_id, o.men_suffrage, o.men_labour, o.women_suffrage, o.women_labour, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.minimum_wage = get_nation_policies(self.id)
	return o
end
function Nation:set_policies(o)
	set_nation_policies(self.id, o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.national_id, o.men_suffrage, o.men_labour, o.women_suffrage, o.women_labour, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.minimum_wage)
end
function Nation:set_ideology(ideology)
	set_nation_ideology(self.id, ideology.id)
end
function Nation:add_client_hint(ideology, alt_name, colour)
	add_nation_client_hint(self.id, ideology.id, alt_name, colour)
end
function Nation:get_owned_provinces()
	local table = get_provinces_owned_by_nation(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
function Nation:get_nuclei_provinces()
	local table = get_provinces_with_nucleus_by_nation(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
function Nation:is_owns_nuclei_from(other)
	local owned_table = Nation:get_owned_provinces(self)
	local nuclei_table = Nation:get_nuclei_provinces(other)

	-- Expected size from total counting
	local nucleuses_cont = 0
	for _ in pairs(nuclei_table) do nucleuses_cont = nucleuses_cont + 1 end
	
	local total_eq_cont = 0
	for k1, v1 in pairs(owned_table) do
		for k2, v2 in pairs(nuclei_table) do
			if v1.ref_name == v2.ref_name then
				total_eq_cont = total_eq_cont + 1
			end
		end
	end
	return (total_eq_cont == nucleuses_cont)
end

Province = {
	id = 0,
	name = "",
	ref_name = "",
	color = 0
}
function Province:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Province:register()
	self.id = add_province(self.ref_name, self.color, self.name)
end
function Province:get(ref_name)
	o = Province:new()
	o.id, o.name, o.color = get_province(ref_name)
	o.ref_name = ref_name
	return o
end
function Province:get_by_id(id)
	o = Province:new()
	o.ref_name, o.name, o.color = get_province_by_id(id)
	o.id = id
	return o
end
function Province:add_industry(industry_type, company)
	add_province_industry(self.id, company.id, industry_type.id)
end
function Province:give_to(nation)
	give_province_to(self.id, nation.id)
end
function Province:get_owner()
	return Nation:get(get_province_owner(self.id))
end
function Province:get_controller()
	return Nation:get(get_province_controller(self.id))
end
function Province:get_neighbours()
	local table = get_province_neighbours(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Province:get_by_id(v)
	end
	return new_table
end
-- Increments militancy for all POPs
function Province:multiply_militancy(factor)
	multiply_province_militancy_global(self.id, factor)
end
function Province:multiply_militancy_by_culture(culture, factor)
	multiply_province_militancy_by_culture(self.id, culture.id, factor)
end
function Province:multiply_militancy_by_religion(religion, factor)
	multiply_province_militancy_by_religion(self.id, religion.id, factor)
end
-- Increments consciousness for all POPs
function Province:multiply_consciousness_global(factor)
	multiply_province_consciousness_global(self.id, factor)
end
function Province:multiply_consciousness_by_culture(culture, factor)
	multiply_province_consciousness_by_culture(self.id, culture.id, factor)
end
function Province:multiply_consciousness_by_religion(religion, factor)
	multiply_province_consciousness_by_religion(self.id, religion.id, factor)
end
-- Adds a POP to the province
function Province:add_pop(pop_type, culture, religion, size, literacy)
	add_province_pop(self.id, pop_type.id, culture.id, religion.id, size, literacy)
end
-- Rename a province
function Province:rename(new_name)
	rename_province(self.id, new_name)
end
-- Adds a country to the nucleus list of a province
function Province:add_nucleus(nation)
	add_province_nucleus(self.id, nation.id)
end

Event = {
	ref_name = "",
	conditions_fn = "",
	event_fn = "",
	title = "",
	text = ""
}
function Event:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Event:register()
	self.id = add_event(self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text)
end
function Event:get(o, ref_name)
	o = Event:new()
	o.id, o.conditions_fn, o.event_fn = get_event(ref_name)
	o.ref_name = ref_name
	return o
end
function Event:add_receivers(...)
	local args = table.pack(...)
	for i = 1, args.n do
		args[i] = args[i].id
	end
	add_event_receivers(self.id, args.n, table.unpack(args))
end
function Event:add_descision(descision)
	add_descision(self.id, descision.ref_name, descision.name, descision.descision_fn, descision.effects)
end

Descision = {
	ref_name = "",
	descision_fn = "",
	name = "",
	effects = ""
}
function Descision:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

PopType = {
	id = 0,
	ref_name = "",
	name = "",
	social_value = 1.0,
	is_entrepreneur = false,
    is_slave = false,
    is_farmer = false,
    is_laborer = false
}
function PopType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function PopType:get(ref_name)
	o = PopType:new()
	o.id, o.name, o.social_value, o.is_entrepreneur, o.is_slave, o.is_farmer, o.is_laborer = get_pop_type(ref_name)
	o.ref_name = ref_name
	return o
end
function PopType:register()
	self.id = add_pop_type(self.ref_name, self.name, self.social_value, self.is_entrepreneur, self.is_slave, self.is_farmer, self.is_laborer)
end

Culture = {
	id = 0,
	ref_name = "",
	name = ""
}
function Culture:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Culture:get(ref_name)
	o = Culture:new()
	o.id, o.name = get_culture(ref_name)
	o.ref_name = ref_name
	return o
end
function Culture:register()
	self.id = add_culture(self.ref_name, self.name)
end

Religion = {
	id = 0,
	ref_name = "",
	name = ""
}
function Religion:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Religion:get(ref_name)
	o = Religion:new()
	o.id, o.name = get_religion(ref_name)
	o.ref_name = ref_name
	return o
end
function Religion:register()
	self.id = add_religion(self.ref_name, self.name)
end

BuildingType = {
	id = 0,
	name = "",
	ref_name = "",
	is_naval = false,
	is_build_land_units = false,
	is_build_naval_units = false,
	defense_bonus = 1.0,
	is_factory = false,
}
function BuildingType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function BuildingType:register()
	self.id = add_building_type(self.ref_name, self.name, self.is_naval, self.is_build_land_units, self.is_build_naval_units, self.defense_bonus, self.is_factory)
end
function BuildingType:add_input(good)
	add_input_to_industry_type(self.id, good.id)
end
function BuildingType:add_output(good)
	add_output_to_industry_type(self.id, good.id)
end
function BuildingType:requires_good(good, amount)
	add_req_good_to_industry_type(self.id, good.id, amount)
end

IndustryType = BuildingType
IndustryType.is_factory = true
function IndustryType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
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
	is_ground = false,
	is_naval = false
}
function UnitType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function UnitType:get( ref_name)
	o = UnitType:new()
	o.id, o.name, o.attack, o.defense, o.health, o.max_defensive_ticks, o.position_defense, o.is_ground, o.is_naval = get_unit_type(ref_name)
	o.ref_name = ref_name
	return o
end
function UnitType:register()
	self.id = add_unit_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.max_defensive_ticks, self.position_defense, self.is_ground, self.is_naval)
end
function UnitType:requires_good(good, amount)
	add_req_good_unit_type(self.id, good.id, amount)
end

BoatType = UnitType
BoatType.is_ground = false
BoatType.is_naval = true
function BoatType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

AirplaneType = UnitType
AirplaneType.is_ground = true
AirplaneType.is_naval = true
function AirplaneType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
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

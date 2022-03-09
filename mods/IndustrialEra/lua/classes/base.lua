-- Industrial_Era_mod - Base game files for Symphony of Empires
-- Copyright (C) 2021, Symphony of Empires contributors
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- ----------------------------------------------------------------------------
--	Name:
--  	base.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

NationMod = {
	id = 0,
	ref_name = "",
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
	con = 1.0,
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
	self.id = add_nation_mod(self.ref_name, self.name, self.industry_output, self.industry_input, self.workers_needed, self.salary_paid, self.delivery_cost, self.literacy_learn, self.reproduction, self.death, self.militancy, self.con, self.life_needs_met, self.everyday_needs_met, self.luxury_needs_met)
end
function NationMod:get(ref_name)
	o = NationMod:new()
	o.id, o.name, o.industry_output, o.industry_input, o.workers_needed, o.salary_paid, o.delivery_cost, o.literacy_learn, o.reproduction, o.death, o.militancy, o.con, o.life_needs_met, o.everyday_needs_met, o.luxury_needs_met = get_nation_mod(ref_name)
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
function Technology:set_nation_modifier(self, mod)
	set_nation_mod_to_invention(self.id, mod.id)
end

Ideology = {
	id = 0,
	ref_name = "",
	name = "",
	color = 0x00000000
}
function Ideology:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Ideology:get(ref_name)
	o = Ideology:new()
	o.id, o.name, o.color = get_ideology(ref_name)
	o.ref_name = ref_name
	return o
end
function Ideology:get_by_id(id)
	o = Ideology:new()
	o.ref_name, o.name, o.color = get_ideology_by_id(id)
	o.id = id
	return o
end
function Ideology:register()
	self.id = add_ideology(self.ref_name, self.name, self.color)
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
    min_wage = 1.0,
	min_sv_for_parliament = 0.0
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

Nation = {
	id = 0,
	name = "",
	ref_name = "",

	adjective = "",
	noun = "",
	combo_form = ""
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
function Nation:get_by_id(id)
	o = Nation:new()
	o.name, o.ref_name = get_nation_by_id(id)
	o.id = id
	return o
end
function Nation:set_capital(province)
	set_nation_capital(self.id, province.id)
end
function Nation:add_accepted_culture(culture)
	add_nation_accepted_culture(self.id, culture.id)
end
function Nation:add_accepted_religion(religion)
	add_nation_accepted_religion(self.id, religion.id)
end
function Nation:get_policies()
	o = Policies:new()

	-- If someone knows a better way to do this please do a PR
	o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.national_id, o.men_suffrage, o.men_labour, o.women_suffrage, o.women_labour, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.min_wage, o.min_sv_for_parliament = get_nation_policies(self.id)
	return o
end
function Nation:set_policies(o)
	set_nation_policies(self.id, o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.national_id, o.men_suffrage, o.men_labour, o.women_suffrage, o.women_labour, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.min_wage, o.min_sv_for_parliament)
end
function Nation:set_ideology(ideology)
	set_nation_ideology(self.id, ideology.id)
end
function Nation:add_client_hint(ideology, alt_name, color)
	add_nation_client_hint(self.id, ideology.id, alt_name, color)
end

function Nation:get_all()
	local table = get_all_nations()
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Nation:get_by_id(v)
	end
	return new_table
end

function Nation:get_friends()
	local table = self:get_all()
	local new_table = {}
	for k, v in pairs(table) do
		local relation = self:get_relation(Nation:get_by_id(v))
		if relation.relation > 50.0 then
			new_table[k] = Nation:get_by_id(v)
		end
	end
	return new_table
end
function Nation:is_friend(nation)
	local table = Nation:get_friends(self)
	for k, v in pairs(table) do
		if v.id == nation.id then
			return true
		end
	end
	return false
end

function Nation:get_enemies()
	local table = self:get_all()
	local new_table = {}
	for k, v in pairs(table) do
		local relation = self:get_relation(Nation:get_by_id(v))
		if relation.relation < -50.0 then
			new_table[k] = Nation:get_by_id(v)
		end
	end
	return new_table
end
function Nation:is_enemy(nation)
	local table = Nation:get_enemies(self)
	for k, v in pairs(table) do
		if v.id == nation.id then
			return true
		end
	end
	return false
end

function Nation:get_allies()
	local table = self:get_all()
	local new_table = {}
	for k, v in pairs(table) do
		local relation = self:get_relation(Nation:get_by_id(v))
		if relation.has_alliance == true then
			new_table[k] = Nation:get_by_id(v)
		end
	end
	return new_table
end
function Nation:is_ally(nation)
	return Nation:get_relation(nation).has_alliance
end

function Nation:get_war_enemies()
	local table = self:get_all()
	local new_table = {}
	for k, v in pairs(table) do
		local relation = self:get_relation(Nation:get_by_id(v))
		if relation.has_war == true then
			new_table[k] = Nation:get_by_id(v)
		end
	end
	return new_table
end
function Nation:is_war_enemy(nation)
	local table = Nation:get_war_enemies(self)
	for k, v in pairs(table) do
		if v.id == nation.id then
			return true
		end
	end
	return false
end

function Nation:get_embargoed()
	local table = self:get_all()
	local new_table = {}
	for k, v in pairs(table) do
		local relation = self:get_relation(Nation:get_by_id(v))
		if relation.has_embargo == true then
			new_table[k] = Nation:get_by_id(v)
		end
	end
	return new_table
end
function Nation:is_embargoed(nation)
	return Nation:get_relation(nation).has_embargo
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
	local nuclei_cont = 0
	for _ in pairs(nuclei_table) do nuclei_cont = nuclei_cont + 1 end
	
	local total_eq_cont = 0
	for k1, v1 in pairs(owned_table) do
		for k2, v2 in pairs(nuclei_table) do
			if v1.ref_name == v2.ref_name then
				total_eq_cont = total_eq_cont + 1
			end
		end
	end
	return (total_eq_cont == nuclei_cont)
end

NationRelation = {
	relation = 0.0,
	interest = 0.0,
	has_embargo = false,
    has_war = false,
    has_alliance = false,
    has_defensive_pact = false,
    has_truce = false,
    has_embassy = false,
    has_military_access = false,
    has_market_access = false,
    free_supplies = false
}
function NationRelation:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:get_relation(other)
	rel = NationRelation:new{} or {}
	rel.relation, rel.interest, rel.has_embargo, rel.has_war, rel.has_alliance, rel.has_defensive_pact, rel.has_truce, rel.has_embassy, rel.has_military_access, rel.has_market_access = get_nation_relation(self.id, other.id)
	return rel
end
function Nation:set_relation(other, rel)
	get_nation_relation(self.id, other.id, rel.relation, rel.interest, rel.has_embargo, rel.has_war, rel.has_alliance, rel.has_defensive_pact, rel.has_truce, rel.has_embassy, rel.has_military_access, rel.has_market_access)
end
function Nation:make_ally(other)
	rel = Nation:get_relation(self, other)
	rel.has_embargo = false
	rel.has_truce = false
	rel.has_war = false
	rel.has_alliance = true
	Nation:set_relation(self, other, rel)
	-- TODO: Clear up the war
end
function Nation:embargo(other)
	rel = Nation:get_relation(self, other)
	rel.has_embargo = true
	Nation:set_relation(self, other, rel)
end
function Nation:declare_unjustified_war(other)
	nation_declare_unjustified_war(self.id, other.id)
end

Pop = {
	id = 0,
	province_id = 0,
	size = 0,
	budget = 0,
	literacy = 0,
	life_needs_met = 0,
	everday_needs_met = 0,
	luxury_needs_met = 0,
	type = {},
	culture = {},
	religion = {},
	ideology = {},
	militancy = 0,
	con = 0,
}
function Pop:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Pop:get_ideology_approval(ideology)
	return get_province_pop_ideology_approval(self.province_id, self.id, ideology.id)
end
function Pop:set_ideology_approval(ideology, approval)
	set_province_pop_ideology_approval(self.province_id, self.id, ideology.id, approval)
end

Province = {
	id = 0,
	name = "",
	ref_name = "",
	color = 0,
	terrain = {}
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
	local terrain_id = 0
	o.id, o.name, o.color, terrain_id = get_province(ref_name)
	o.terrain = TerrainType:get(terrain_id)
	o.ref_name = ref_name
	return o
end
function Province:get_by_id(id)
	o = Province:new()
	local terrain_id = 0
	o.ref_name, o.name, o.color, terrain_id = get_province_by_id(id)
	o.terrain = TerrainType:get(terrain_id)
	o.id = id
	return o
end
function Province:update_building(building_type, level)
	update_province_building(self.id, building_type.id, level)
end
function Province:give_to(nation)
	give_province_to(self.id, nation.id)
end
function Province:give_and_relinquish_to(nation)
	give_hard_province_to(self.id, nation.id)
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
function Province:get_nuclei()
	local table = get_province_nuclei(self.id)
	local new_table = {}
	for k, v in pairs(table) do
		new_table[k] = Nation:get_by_id(v)
	end
	return new_table
end
-- function Province:__get_pop_size()
-- 	return get_province_pops_size(self.id)
-- end
function Province:get_pops()
	local n_pops = get_province_pops_size(self.id) - 1
	local new_table = {}
	for i = 0, n_pops do
		local tb = Pop:new()
		tb.size, tb.budget, tb.literacy, tb.life_needs_met, tb.everday_needs_met, tb.luxury_needs_met, tb.type, tb.culture, tb.religion, tb.ideology, tb.militancy, tb.con = get_province_pop(self.id, i)
		tb.type = PopType:get_by_id(tb.type)
		tb.culture = Culture:get_by_id(tb.culture)
		tb.religion = Religion:get_by_id(tb.religion)
		tb.ideology = Ideology:get_by_id(tb.ideology)
		tb.id = i
		tb.province_id = self.id
		new_table[i] = tb
	end
	return new_table
end
function Province:update_pop(pop)
	set_province_pop(self.id, pop.id, pop.size, pop.budget, pop.literacy, pop.life_needs_met, pop.everday_needs_met, pop.luxury_needs_met, pop.type.id, pop.culture.id, pop.religion.id, pop.militancy, pop.con)
end
function Province:update_pops(pop)
	-- TODO: Do important stuff
end

-- ============================================================================
-- DEPRECATED do not use!!!!
-- ============================================================================

-- Increments militancy for all POPs
function Province:multiply_militancy(factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		pop.militancy = pop.militancy * factor
		self:update_pop(pop)
	end
	self:update_pops()
end
function Province:multiply_militancy_by_culture(culture, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.culture.id == culture.id then
			pop.militancy = pop.militancy * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end
function Province:multiply_militancy_by_religion(religion, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.religion.id == religion.id then
			pop.militancy = pop.militancy * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end

-- Increments con for all POPs
function Province:multiply_con_global(factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		pop.con = pop.con * factor
		self:update_pop(pop)
	end
	self:update_pops()
end
function Province:multiply_con_by_culture(culture, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.culture.id == culture.id then
			pop.con = pop.con * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end
function Province:multiply_con_by_religion(religion, factor)
	local pops = self:get_pops()
	for k, pop in pairs(pops) do
		if pop.religion.id == religion.id then
			pop.con = pop.con * factor
			self:update_pop(pop)
		end
	end
	self:update_pops()
end
-- ============================================================================

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
	text = "",
	checked = false
}
function Event:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Event:register()
	self.id = add_event(self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:update()
	update_event(self.id, self.ref_name, self.conditions_fn, self.event_fn, self.title, self.text, self.checked)
end
function Event:get(ref_name)
	o = Event:new()
	o.id, o.conditions_fn, o.event_fn, o.title, o.text, o.checked = get_event(ref_name)
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
	is_burgeoise = false,
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
	o.id, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_farmer, o.is_laborer = get_pop_type(ref_name)
	o.ref_name = ref_name
	return o
end
function PopType:get_by_id(id)
	o = PopType:new()
	o.ref_name, o.name, o.social_value, o.is_burgeoise, o.is_slave, o.is_farmer, o.is_laborer = get_pop_type_by_id(id)
	o.id = id
	return o
end
function PopType:register()
	self.id = add_pop_type(self.ref_name, self.name, self.social_value, self.is_burgeoise, self.is_slave, self.is_farmer, self.is_laborer)
end

Culture = {
	id = 0,
	ref_name = "",
	name = "",
	adjective = "",
	noun = "",
	combo_form = "",
	color = 0x00000000
}
function Culture:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Culture:get(ref_name)
	o = Culture:new()
	o.id, o.name, o.color, o.adjective, o.noun, o.combo_form = get_culture(ref_name)
	o.ref_name = ref_name
	return o
end
function Culture:get_by_id(id)
	o = Culture:new()
	o.ref_name, o.name, o.color, o.adjective, o.noun, o.combo_form = get_culture_by_id(id)
	o.id = id
	return o
end
function Culture:register()
	self.id = add_culture(self.ref_name, self.name, self.color, self.adjective, self.noun, self.combo_form)
end

Religion = {
	id = 0,
	ref_name = "",
	name = "",
	color = 0x00000000
}
function Religion:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Religion:get(ref_name)
	o = Religion:new()
	o.id, o.name, o.color = get_religion(ref_name)
	o.ref_name = ref_name
	return o
end
function Religion:get_by_id(id)
	o = Religion:new()
	o.ref_name, o.name, o.color = get_religion_by_id(id)
	o.id = id
	return o
end
function Religion:register()
	self.id = add_religion(self.ref_name, self.name, self.color)
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
function BuildingType:get(ref_name)
	o = BuildingType:new()
	o.id, o.name, o.is_naval, o.is_build_land_units, o.is_build_naval_units, o.defense_bonus, o.is_factory = get_building_type(ref_name)
	o.ref_name = ref_name
	return o
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
function BuildingType:requires_tech(tech, amount)
	add_req_technology_to_industry_type(self.id, tech.id)
end

IndustryType = BuildingType
IndustryType.is_factory = true
function IndustryType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

TerrainType = {
	id = 0,
	name = "",
	ref_name = "",
	color = 0x000000,
	movement_penalty = 0.0,
	is_water_body = false
}
function TerrainType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function TerrainType:register()
	self.id = add_terrain_type(self.ref_name, self.name, self.color, self.movement_penalty, self.is_water_body)
end
function TerrainType:get(ref_name)
	o = TerrainType:new()
	o.id, o.name, o.color, o.movement_penalty, o.is_water_body = get_terrain_type(ref_name)
	o.ref_name = ref_name
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
	is_naval = false,
	speed = 1.0
}
function UnitType:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function UnitType:get( ref_name)
	o = UnitType:new()
	o.id, o.name, o.attack, o.defense, o.health, o.max_defensive_ticks, o.position_defense, o.is_ground, o.is_naval, o.speed = get_unit_type(ref_name)
	o.ref_name = ref_name
	return o
end
function UnitType:register()
	self.id = add_unit_type(self.ref_name, self.name, self.attack, self.defense, self.health, self.max_defensive_ticks, self.position_defense, self.is_ground, self.is_naval, self.speed)
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
	local color = 0
	color = color | (r << 16)
	color = color | (g << 8)
	color = color | (b << 0)
	return color
end

function is_empty(s)
	return (s == nil or s == '')
end

Language = {}

function Language:is_vowel(c)
	return (c == 'a' or c == 'e' or c == 'i' or c == 'o' or c == 'u')
end

function Language:conjugate_related_and(...)
	local args = table.pack(...)

	local str = ""
	for i = 1, args.n do
		-- CF not available, fallback to an adjective
		if(is_empty(args[i].combo_form)) then
			str = str .. args[i].adjective
		-- Combining form available (only possible if it's first)
		elseif i == 1 then
			str = str .. args[i].combo_form
		end

		-- Makes somewhat correct phrases like Franco-Prussian war, Russo-Chinesse-Roman war
		if i < (args.n - 1) then
			str = str .. "-"
		end
	end
end

-- [determiner (a/an)] [adjective (or noun as fallback)]
function Language:conjugate_indefinite_article(o)
	local str = ""

	if(is_empty(o.adjective)) then
		str = str .. o.noun
	else
		str = str .. o.adjective
	end

	if(Language:is_vowel(str[0])) then
		str = "an" .. str
	else
		str = "a" .. str
	end
end

print("loaded api.lua")

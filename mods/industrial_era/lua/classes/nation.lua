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
--  	nation.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

Nation = {
	id = 0,
	name = "",
	ref_name = "",

	adjective = "",
	noun = "",
	combo_form = ""
}
function Nation:new(o)
	local o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:register()
	self.id = add_nation(self.ref_name, self.name)
end
function Nation:get(ref_name)
	local o = Nation:new()
	o.id, o.name = get_nation(ref_name)
	o.ref_name = ref_name
	return o
end
function Nation:get_by_id(id)
	local o = Nation:new()
	o.name, o.ref_name = get_nation_by_id(id)
	o.id = id
	return o
end
function Nation:set_capital(province)
	set_nation_capital(self.id, province.id)
end
function Nation:add_accepted_language(language)
	add_nation_accepted_language(self.id, language.id)
end
function Nation:add_accepted_religion(religion)
	add_nation_accepted_religion(self.id, religion.id)
end
function Nation:get_policies()
	local o = Policies:new()
	-- If someone knows a better way to do this please do a PR
	o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.min_wage, o.min_sv_for_parliament = get_nation_policies(self.id)
	return o
end
function Nation:set_policies(o)
	set_nation_policies(self.id, o.treatment, o.migration, o.immigration, o.censorship, o.build_infrastructure, o.build_factories, o.private_property, o.companies_allowed, o.public_education, o.secular_education, o.public_healthcare, o.social_security, o.slavery, o.legislative_parliament, o.executive_parliament, o.constitutional, o.foreign_trade, o.import_tax, o.export_tax, o.domestic_import_tax, o.domestic_export_tax, o.poor_flat_tax, o.med_flat_tax, o.rich_flat_tax, o.industry_tax, o.military_spending, o.free_supplies, o.min_wage, o.min_sv_for_parliament)
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

--- Switches the player of nation A to nation B
function Nation:switch_soul(target)
	switch_nation_soul(self.id, target.id)
end

--- Declares a war to another nation without a cb
--- @param other Nation to declare war to
function Nation:declare_no_cb(other)
	nation_declare_war_no_cb(self.id, other.id)
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
    has_war = false
}
function NationRelation:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
function Nation:get_relation(other)
	rel = NationRelation:new{} or {}
	rel.relation, rel.has_war = get_nation_relation(self.id, other.id)
	return rel
end
function Nation:set_relation(other, rel)
	get_nation_relation(self.id, other.id, rel.relation)
end
function Nation:make_puppet(other)
	nation_make_puppet(self.id, other.id)
end
function Nation:declare_unjustified_war(other)
	nation_declare_unjustified_war(self.id, other.id)
end

Good = { id = 0, name = "", ref_name = "" }
function Good:create(good)
	good.parent = self
	return good
end
function Good:register(good)
	good.id = add_good(good.ref_name, good.name)
end
function Good:get(good, ref_name)
	good.parent = self
	good.id, good.ref_name, good.name = get_good(ref_name)
	return good
end

Company = { id = 0, name = "", is_transport = false, is_retailer = false, is_industry = false, money = 0, }
function Company:create(company)
	company.parent = self
	return company
end
function Company:register(company)
	company.id = add_company(company.name, company.money, company.is_transport, company.is_retailer, company.is_industry)
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
	add_input_to_industry_type(industry_type.ref_name, good.ref_name)
end
function IndustryType:add_output(industry_type, good)
	add_output_to_industry_type(industry_type.ref_name, good.ref_name)
end

Nation = { id = 0, name = "", ref_name = "", color = 0, default_flag = "", }
function Nation:create(nation)
	nation.parent = self
	return nation
end
function Nation:register(nation)
	nation.id = add_nation(nation.ref_name, nation.color, nation.default_flag, nation.name)
end
function Nation:get(nation, ref_name)
	nation.parent = self
	nation.id, nation.ref_name, nation.name, o.color = get_nation(ref_name)
	return nation
end

Province = { id = 0, name = "", ref_name = "", color = 0, }
function Province:create(province)
	province.parent = self
	return province
end
function Province:register(province)
	add_province(province.ref_name, province.color, province.name)
end
function Province:get(province, ref_name)
	province.parent = self
	province.id, province.ref_name, province.name, province.color = get_province(ref_name)
	return province
end
function Province:give_to(province, nation)
	give_province_to(province.id, nation.id)
end
function Province:add_pop(province, pop_type, culture, religion, size)
	province.id = add_province_pop(province.id, pop_type.id, culture.id, religion.id, size)
end

Event = { ref_name = "", conditions_fn = "", event_fn = "" }
function Event:create(event)
	event.parent = self
	return evento
end
function Event:register(event)
	add_event(event.ref_name, event.conditions_fn, event.event_fn)
end

PopType = { id = 0, ref_name = "", name = "", on_tick_fn = "" }
function PopType:create(pop_type)
	pop_type.parent = self
	return pop_type
end
function PopType:get(pop_type, ref_name)
	pop_type.id, pop_type.ref_name, pop_type.name = get_pop_type(ref_name)
end
function PopType:register(pop_type)
	pop_type.id = add_pop_type(pop_type.ref_name, pop_type.name, pop_type.on_tick_fn)
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

print("loaded api.lua")
Good = { id = 0, name = "default", ref_name = "default" }
function Good:create(o)
	o.parent = self
	return o
end
function Good:register(o)
	add_good(o.ref_name, o.name)
end
function Good:get(o, ref_name)
	o.parent = self
	o.id, o.ref_name, o.name = get_good(ref_name)
	return o
end

Company = { id = 0, name = "default", is_transport = false, is_retailer = false, is_industry = false, money = 0, }
function Company:create(o)
	o.parent = self
	return o
end
function Company:register(o)
	add_company(o.name, o.money, o.is_transport, o.is_retailer, o.is_industry)
end

IndustryType = { id = 0, name = "default", ref_name = "default" }
function IndustryType:create(o)
	o.parent = self
	return o
end
function IndustryType:register(o)
	add_industry_type(o.ref_name, o.name)
end
function IndustryType:get(o, ref_name)
	o.parent = self
	o.id, o.ref_name, o.name = get_industry_type(ref_name)
	return o
end
function IndustryType:add_input(o, p)
	add_input_to_industry_type(o.ref_name, p.ref_name)
end
function IndustryType:add_output(o, p)
	add_output_to_industry_type(o.ref_name, p.ref_name)
end

Nation = { id = 0, name = "default", ref_name = "default", color = 0, default_flag = "flags/japan.png", }
function Nation:create(o)
	o.parent = self
	return o
end
function Nation:register(o)
	add_nation(o.ref_name, o.color, o.default_flag, o.name)
end
function Nation:get(o, ref_name)
	o.parent = self
	o.id, o.ref_name, o.name, o.color = get_nation(ref_name)
	return o
end

Province = { id = 0, name = "default", ref_name = "default", color = 0, }
function Province:create(o)
	o.parent = self
	return o
end
function Province:register(o)
	add_province(o.ref_name, o.color, o.name)
end
function Province:get(o, ref_name)
	o.parent = self
	o.id, o.ref_name, o.color = get_province(ref_name)
	return o
end
function Province:give_to(p, n)
	give_province_to(p.id, n.id)
end

Event = { ref_name = "default", conditions_fn = "", event_fn = "" }
function Event:create()
	o.parent = self
	return o
end
function Event:register(o)
	add_event(o.ref_name, o.conditions_fn, o.event_fn)
end

print("loaded api.lua")
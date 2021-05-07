Nation = { name = "default", ref_name = "default", color = 0, default_flag = "data/flags/japan.png", }
function Nation:create(o)
	o.parent = self
	return o
end
function Nation:register(o)
	AddNation(o.ref_name, o.color, o.default_flag, o.name)
end

print("loaded nation.lua")
return Nation
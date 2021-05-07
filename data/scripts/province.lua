Province = { name = "default", ref_name = "default", color = 0, }
function Province:create(o)
	o.parent = self
	return o
end
function Province:register(o)
	AddProvince(o.ref_name, o.color, o.name)
end

print("loaded province.lua")
return Province
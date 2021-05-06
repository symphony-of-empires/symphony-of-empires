Good = { ref_name = "default" }
function Good:create(o)
	o.parent = self
	return o
end
function Good:register(o)
	AddGood(o.ref_name)
end

print("loaded good.lua")
return Good
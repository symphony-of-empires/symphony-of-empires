IndustryType = { ref_name = "default" }
function IndustryType:create(o)
	o.parent = self
	return o
end

function IndustryType:register(o)
	AddIndustryType(o.ref_name)
end

function IndustryType:add_input(o, p)
	AddInputToIndustryType(o.ref_name, p.ref_name)
end

function IndustryType:add_output(o, p)
	AddOutputToIndustryType(o.ref_name, p.ref_name)
end

print("loaded industry_type.lua")
return IndustryType
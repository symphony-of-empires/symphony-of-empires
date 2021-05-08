Company = { name = "default", is_transport = false, is_retailer = false, is_industry = false, money = 0, }
function Company:create(o)
	o.parent = self
	return o
end
function Company:register(o)
	AddCompany(o.name, o.money, o.is_transport, o.is_retailer, o.is_industry)
end

print("loaded company.lua")
return Company
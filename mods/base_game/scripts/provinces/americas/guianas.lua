--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: British Empire

province = Province:create{ ref_name = "essequibo", color = 0x99FFC8 }
province.name = _("Essequibo")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, british, protestant, 5000, 0.7)
Province:add_pop(province, farmer, british, protestant, 5000, 0.1)
Province:add_pop(province, soldier, british, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, british, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, british, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, british, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, british, protestant, 5000, 0.3)
Province:add_pop(province, laborer, british, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, british, protestant, 500, 0.9)
Province:add_nucleus(province, united_kingdom)
Province:give_to(province, united_kingdom)

province = Province:create{ ref_name = "demerara", color = 0xcc0a37 }
province.name = _("Demerara")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, british, protestant, 5000, 0.7)
Province:add_pop(province, farmer, british, protestant, 5000, 0.1)
Province:add_pop(province, soldier, british, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, british, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, british, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, british, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, british, protestant, 5000, 0.3)
Province:add_pop(province, laborer, british, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, british, protestant, 500, 0.9)
Province:add_nucleus(province, united_kingdom)
Province:give_to(province, united_kingdom)

--if taken by suriname rename it to corantijn
province = Province:create{ ref_name = "corentyne", color = 0x56ffa5 }
province.name = _("Corentyne")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, british, protestant, 5000, 0.7)
Province:add_pop(province, farmer, british, protestant, 5000, 0.1)
Province:add_pop(province, soldier, british, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, british, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, british, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, british, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, british, protestant, 5000, 0.3)
Province:add_pop(province, laborer, british, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, british, protestant, 500, 0.9)
Province:add_nucleus(province, united_kingdom)
Province:give_to(province, united_kingdom)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: Kingdom of the Netherlands

province = Province:create{ ref_name = "suriname", color = 0xFF42AD }
province.name = _("Suriname")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, german, protestant, 5000, 0.7)
Province:add_pop(province, farmer, german, protestant, 5000, 0.1)
Province:add_pop(province, soldier, german, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, german, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, german, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, german, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, german, protestant, 5000, 0.3)
Province:add_pop(province, laborer, german, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, german, protestant, 500, 0.9)
Province:add_pop(province, slave, german, protestant, 500, 0.5)
Province:add_nucleus(province, netherlands)
Province:give_to(province, netherlands)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: South America
-- Country: France

province = Province:create{ ref_name = "cayenne", color = 0x552804 }
province.name = _("Cayenne")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, german, protestant, 5000, 0.7)
Province:add_pop(province, farmer, german, protestant, 5000, 0.1)
Province:add_pop(province, soldier, german, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, german, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, german, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, german, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, german, protestant, 5000, 0.3)
Province:add_pop(province, laborer, german, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, german, protestant, 500, 0.9)
Province:add_pop(province, slave, german, protestant, 500, 0.5)
Province:add_nucleus(province, france)
Province:give_to(province, france)

-- if taken by suriname, rename it to Marowijne
province = Province:create{ ref_name = "maroni", color = 0xe7ffa5 }
province.name = _("Maroni")
Province:register(province)
Province:add_industry(province, wheat_farm, andes_transport)
Province:add_pop(province, artisan, german, protestant, 5000, 0.7)
Province:add_pop(province, farmer, german, protestant, 5000, 0.1)
Province:add_pop(province, soldier, german, protestant, 5000, 0.2)
Province:add_pop(province, craftsmen, german, protestant, 5000, 0.2)
Province:add_pop(province, bureaucrat, german, protestant, 5000, 0.7)
Province:add_pop(province, aristocrat, german, protestant, 5000, 0.2)
Province:add_pop(province, clergymen, german, protestant, 5000, 0.3)
Province:add_pop(province, laborer, german, protestant, 5000, 0.2)
Province:add_pop(province, entrepreneur, german, protestant, 500, 0.9)
Province:add_pop(province, slave, german, protestant, 500, 0.5)
Province:add_nucleus(province, france)
Province:add_nucleus(province, netherlands)
Province:give_to(province, france)

--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: Russian Empire
-- Change to Western Alaska if taken by the US or UK
province = Province:new{ ref_name = "zapadnaya_alyaska", color = rgb(214, 255, 215) }
province.name = _("Zapadnaya Alyaska")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Inner Alaska if taken by US or UK
province = Province:new{ ref_name = "vnutrennyaya_alyaska", color = rgb(212, 198, 255) }
province.name = _("Vnutrennyaya Alyaska")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Sitka if taken by US or UK
province = Province:new{ ref_name = "novoarkhangelsk", color = rgb(181, 232, 255) }
province.name = _("Novoarkhangelsk")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Kodiak if taken by US or UK
province = Province:new{ ref_name = "kad_yak", color = rgb(255, 173, 176) }
province.name = _("Kad'yak")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Big Diomede when take by the US or UK
province = Province:new{ ref_name = "bolshoy_diomida", color = rgb(255, 69, 66) }
province.name = _("Bolshoy Diomida")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

province = Province:new{ ref_name = "unalaska", color = rgb(211, 255, 213) }
province.name = _("Unalaska")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Little Diomede
province = Province:new{ ref_name = "malenkiy_diomid", color = rgb(137, 122, 255) }
province.name = _("Malenkiy Diomid")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Nunivak Island when US or UK gets it
province = Province:new{ ref_name = "nunivok", color = rgb(172, 155, 255) }
province.name = _("Nunivok")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Norton Sound when US or UK gets it
province = Province:new{ ref_name = "zaliv_norton", color = rgb(96, 255, 104) }
province.name = _("Zaliv Norton")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

province = Province:new{ ref_name = "alexandrovski", color = rgb(155, 211, 255) }
province.name = _("Alexandrovski")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

province = Province:new{ ref_name = "kenai", color = rgb(119, 248, 255) }
province.name = _("Kenai")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to North Chugach when taken by US or UK
province = Province:new{ ref_name = "severnyy_chugach", color = rgb(255, 99, 174) }
province.name = _("Severnyy Chugach")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Hichinbrook Island when taken by US or UK
province = Province:new{ ref_name = "ostrov_nuchek", color = rgb(218, 145, 255) }
province.name = _("Ostrov Nuchek")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Alaska Peninsula when taken by US or UK
province = Province:new{ ref_name = "poluostrov_alyaska", color = rgb(255, 204, 243) }
province.name = _("Poluostrov Alyaska")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

-- Change to Fort Ross when taken by US
-- California!
province = Province:new{ ref_name = "krepost_ross", color = 0x77ff87 }
province.name = _("Krepostʹ Ross")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

province = Province:new{ ref_name = "yakutat", color = rgb(187, 168, 255) }
province.name = _("Yakutat")
Province:register(province)
Province:add_industry(province, wheat_farm, royal_russia_transport)
Province:add_pop(province, artisan, russian, orthodox, 300, 0.7)
Province:add_pop(province, farmer, russian, orthodox, 2700, 0.6)
Province:add_pop(province, soldier, russian, orthodox, 800, 0.8)
Province:add_pop(province, craftsmen, russian, orthodox, 700, 0.5)
Province:add_pop(province, bureaucrat, russian, orthodox, 200, 0.7)
Province:add_pop(province, aristocrat, russian, orthodox, 200, 0.9)
Province:add_pop(province, clergymen, russian, orthodox, 500, 0.9)
Province:add_pop(province, laborer, russian, orthodox, 2700, 0.8)
Province:add_pop(province, entrepreneur, russian, orthodox, 100, 0.9)
Province:add_nucleus(province, russia)
Province:give_to(province, russia)

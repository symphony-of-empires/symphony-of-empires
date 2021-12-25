--------------------------------------------------------------
-- Continent: Americas
-- Subcontinent: North America
-- Country: USA

-- East united states
local usa_provinces = {
    { ref_name = "upper_coast", name = _("Upper Coast"), color = rgb(164, 221, 32) },
    { ref_name = "florida", name = _("Florida"), color = rgb(112, 166, 255) },
    { ref_name = "connecticut", name = _("Connecticut"), color = rgb(233, 135, 255) },
    { ref_name = "new_jersey", name = _("New Jersey"), color = rgb(255, 174, 168) },
    { ref_name = "rhode_island", name = _("Rhode Island"), color = rgb(123, 7, 255) },
    { ref_name = "new_york", name = _("New York"), color = rgb(188, 253, 255) },
    { ref_name = "manhattan", name = _("Manhattan"), color = rgb(159, 104, 255) },
    { ref_name = "massachusetts", name = _("Massachusetts"), color = rgb(99, 159, 255) },
    { ref_name = "new_hampshire", name = _("New Hampshire"), color = rgb(248, 48, 255) },
    { ref_name = "vermont", name = _("Vermont"), color = rgb(255, 78, 50) },
    { ref_name = "north_maine", name = _("North Maine"), color = rgb(145, 174, 255) },
    { ref_name = "maine", name = _("Maine"), color = rgb(255, 76, 127) },
    { ref_name = "delaware", name = _("Delaware"), color = rgb(163, 114, 255) },
    { ref_name = "maryland", name = _("Maryland"), color = rgb(165, 255, 237) },
    { ref_name = "pennsylvania", name = _("Pennsylvania"), color = rgb(255, 73, 167) },
    { ref_name = "virginia", name = _("Virginia"), color = rgb(255, 117, 121) },
    { ref_name = "north_carolina", name = _("North Carolina"), color = rgb(124, 176, 255) },
    { ref_name = "south_carolina", name = _("South Carolina"), color = rgb(255, 181, 231) },
    { ref_name = "west_virginia", name = _("West Virginia"), color = rgb(217, 158, 255) },
    { ref_name = "ohio", name = _("Ohio"), color = rgb(140, 212, 255) },
    { ref_name = "michigan", name = _("Michigan"), color = rgb(255, 45, 174) },
    { ref_name = "upper_peninsula", name = _("Upper Peninsula"), color = rgb(216, 150, 255) },
    { ref_name = "indiana", name = _("Indiana"), color = rgb(188, 255, 239) },
    { ref_name = "kentucky", name = _("Kentucky"), color = rgb(150, 127, 255) },
    { ref_name = "tennessee", name = _("Tennessee"), color = rgb(255, 76, 189) },
    { ref_name = "georgia", name = _("Georgia"), color = rgb(255, 251, 73) },
    { ref_name = "alabama", name = _("Alabama"), color = rgb(255, 124, 107) },
    { ref_name = "mississipi", name = _("Mississipi"), color = rgb(255, 102, 221) },
    { ref_name = "mobile", name = _("Mobile"), color = rgb(255, 79, 175) },
    { ref_name = "biloxi", name = _("Biloxi"), color = rgb(115, 28, 255) },
    { ref_name = "east_louisiana", name = _("East Louisiana"), color = rgb(199, 48, 255) },
    { ref_name = "louisiana", name = _("Louisiana"), color = rgb(91, 146, 255) },
}

for k, v in pairs(usa_provinces) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, american, protestant, 500, 0.7)
    province:add_pop(farmer, american, protestant, 500, 0.1)
    province:add_pop(soldier, american, protestant, 500, 0.2)
    province:add_pop(craftsmen, american, protestant, 500, 0.2)
    province:add_pop(bureaucrat, american, protestant, 500, 0.7)
    province:add_pop(burgeoise, american, protestant, 500, 0.2)
    province:add_pop(clergymen, american, protestant, 500, 0.3)
    province:add_pop(laborer, american, protestant, 500, 0.2)
    province:add_pop(burgeoise, american, protestant, 500, 0.9)
    province:add_pop(slave, american, protestant, 500, 0.5)

    province:add_nucleus(united_states)
    province:give_to(united_states)
    province:add_industry(wheat_farm, united_states)
end

-- Uncolonized part of the mainland USA should be added as a nucleus after "manifest destiny"
local uncolonized_us_provinces = {
    { ref_name = "arkansas", name = _("Arkansas"), color = rgb(123, 255, 22) },
    { ref_name = "missouri", name = _("Missouri"), color = rgb(223, 175, 255) },
    { ref_name = "iowa", name = _("Iowa"), color = rgb(255, 253, 209) },
    { ref_name = "illinois", name = _("Illinois"), color = rgb(255, 58, 130) },
    { ref_name = "winsconsin", name = _("Winsconsin"), color = rgb(255, 205, 155) },
    { ref_name = "minnesota", name = _("Minnesota"), color = rgb(168, 255, 201) },
    { ref_name = "north_dakota", name = _("North Dakota"), color = rgb(191, 255, 204) },
    { ref_name = "south_dakota", name = _("South Dakota"), color = rgb(160, 255, 250) },
    { ref_name = "nebraska", name = _("Nebraska"), color = rgb(255, 224, 239) },
    { ref_name = "grand_portage", name = _("Grand Portage"), color = rgb(255, 89, 166) },
    { ref_name = "kansas", name = _("Kansas"), color = rgb(255, 113, 73) },
    { ref_name = "kansas_indian_territory", name = _("Kansas Indian Territory"), color = rgb(221, 255, 242) },
    { ref_name = "oklahoma", name = _("Oklahoma"), color = rgb(132, 240, 255) },
    { ref_name = "north_oklahoma", name = _("North Oklahoma"), color = rgb(37, 120, 131) },
    { ref_name = "neutral_strip", name = _("Neutral Strip"), color = rgb(233, 147, 255) },
    { ref_name = "southwestern_colorado", name = _("Southwestern Colorado"), color = rgb(235, 158, 255) },
    { ref_name = "northwestern_colorado", name = _("Northwestern Colorado"), color = rgb(28, 164, 255) },
    { ref_name = "colorado", name = _("Colorado"), color = rgb(255, 40, 215) },
    { ref_name = "southern_wyoming", name = _("Southern Wyoming"), color = rgb(109, 182, 255) },
    { ref_name = "southwestern_wyoming", name = _("Southwestern Wyoming"), color = rgb(40, 90, 255) },
    { ref_name = "western_wyoming", name = _("Western Wyoming"), color = rgb(255, 58, 97) },
    { ref_name = "wyoming", name = _("Wyoming"), color = rgb(177, 135, 255) },
    { ref_name = "montana", name = _("Montana"), color = rgb(255, 188, 196) },
    { ref_name = "western_montana", name = _("Western Montana"), color = rgb(255, 165, 145) },
    { ref_name = "idaho", name = _("Idaho"), color = rgb(255, 163, 204) },
    { ref_name = "east_oregon", name = _("East Oregon"), color = rgb(89, 235, 255) },
    { ref_name = "west_oregon", name = _("West Oregon"), color = rgb(224, 103, 255) },
    { ref_name = "washington", name = _("Washington"), color = 0xe066ff },
}

for k, v in pairs(uncolonized_us_provinces) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, american, protestant, 500, 0.7)
    province:add_pop(farmer, american, protestant, 500, 0.1)
    province:add_pop(soldier, american, protestant, 500, 0.2)
    province:add_pop(craftsmen, american, protestant, 500, 0.2)
    province:add_pop(bureaucrat, american, protestant, 500, 0.7)
    province:add_pop(burgeoise, american, protestant, 500, 0.2)
    province:add_pop(clergymen, american, protestant, 500, 0.3)
    province:add_pop(laborer, american, protestant, 500, 0.2)
    province:add_pop(burgeoise, american, protestant, 500, 0.9)
    province:add_pop(slave, american, protestant, 500, 0.5)

    province:give_to(united_states)
    province:add_industry(wheat_farm, united_states)
end

-- Texas provinces (originally part of Mexico and then is conquered by the U.S on event)
local texas_provinces = {
    { ref_name = "northern_texas", name = _("Northern Texas"), color = rgb(255, 102, 137) },
    { ref_name = "eastern_texas", name = _("Eastern Texas"), color = rgb(94, 174, 255) },
    { ref_name = "el_paso", name = _("El Paso"), color = rgb(255, 165, 211) },
    { ref_name = "southern_texas", name = _("Southern Texas"), color = rgb(168, 202, 255) },
    { ref_name = "corpus_christi", name = _("Corpus Christi"), color = rgb(173, 255, 241) },
    { ref_name = "hill_country", name = _("Hill Country"), color = rgb(255, 225, 127) },
}

for k, v in pairs(texas_provinces) do
    province = Province:new(v)
    province:register()

    province:add_pop(artisan, american, protestant, 500, 0.7)
    province:add_pop(farmer, american, protestant, 500, 0.1)
    province:add_pop(soldier, american, protestant, 500, 0.2)
    province:add_pop(craftsmen, american, protestant, 500, 0.2)
    province:add_pop(bureaucrat, american, protestant, 500, 0.7)
    province:add_pop(burgeoise, american, protestant, 500, 0.2)
    province:add_pop(clergymen, american, protestant, 500, 0.3)
    province:add_pop(laborer, american, protestant, 500, 0.2)
    province:add_pop(burgeoise, american, protestant, 500, 0.9)

    province:add_pop(artisan, mexican, catholic, 500, 0.7)
    province:add_pop(farmer, mexican, catholic, 500, 0.1)
    province:add_pop(soldier, mexican, catholic, 500, 0.2)
    province:add_pop(craftsmen, mexican, catholic, 500, 0.2)
    province:add_pop(bureaucrat, mexican, catholic, 500, 0.7)
    province:add_pop(burgeoise, mexican, catholic, 500, 0.2)
    province:add_pop(clergymen, mexican, catholic, 500, 0.3)
    province:add_pop(laborer, mexican, catholic, 500, 0.2)
    province:add_pop(burgeoise, mexican, catholic, 500, 0.9)

    province:add_nucleus(mexico)
    province:add_nucleus(texas)
    province:add_nucleus(united_states)
    province:give_to(mexico)
    province:add_industry(wheat_farm, mexico)
end

province = Province:new{ ref_name = "nevada", color = rgb(157, 122, 255) }
province.name = _("Nevada")
province:register()
province:add_industry(wheat_farm, russia)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "utah", color = rgb(109, 225, 255) }
province.name = _("Utah")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "rocky_mountains", color = rgb(246, 165, 255) }
province.name = _("Rocky Mountains")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "northern_arizona", color = rgb(204, 173, 255) }
province.name = _("Northern Arizona")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "southern_arizona", color = rgb(255, 193, 208) }
province.name = _("Southern Arizona")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "east_new_mexico", color = rgb(109, 255, 242) }
province.name = _("East New Mexico")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

province = Province:new{ ref_name = "south_new_mexico ", color = rgb(45, 161, 255) }
province.name = _("South New Mexico ")
province:register()
province:add_industry(wheat_farm, united_states)
province:add_industry(bakery, united_states)
province:add_pop(artisan, american, protestant, 500, 0.7)
province:add_pop(farmer, american, protestant, 500, 0.1)
province:add_pop(soldier, american, protestant, 500, 0.2)
province:add_pop(craftsmen, american, protestant, 500, 0.2)
province:add_pop(bureaucrat, american, protestant, 500, 0.7)
province:add_pop(burgeoise, american, protestant, 500, 0.2)
province:add_pop(clergymen, american, protestant, 500, 0.3)
province:add_pop(laborer, american, protestant, 500, 0.2)
province:add_pop(burgeoise, american, protestant, 500, 0.9)
province:add_pop(slave, american, protestant, 500, 0.5)
province:add_nucleus(mexico)
province:add_nucleus(mexico)
province:give_to(mexico)

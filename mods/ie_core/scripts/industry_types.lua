-- Industry types

-- Primary sector industries (the most essential)
glass_factory = IndustryType:new{ ref_name = "glass_factory" }
glass_factory.name = _("Glass factory")
glass_factory:register()
-- glass factories can new glass from thin air
glass_factory:add_output(glass)

oil_refinery = IndustryType:new{ ref_name = "oil_refinery" }
oil_refinery.name = _("Oil refinery")
oil_refinery:register()
-- thin air oil, i think the air needs some democracy!
oil_refinery:add_output(oil)

lumberjack = IndustryType:new{ ref_name = "lumberjack" }
lumberjack.name = _("Lumberjack")
lumberjack:register()
-- same logic here
lumberjack:add_output(timber)

vineyard = IndustryType:new{ ref_name = "vineyard" }
vineyard.name = _("Vineyard")
vineyard:register()
vineyard:add_output(grapes)

wheat_farm = IndustryType:new{ ref_name = "wheat_farm" }
wheat_farm.name = _("Wheat farm")
wheat_farm:register()
wheat_farm:add_output(wheat)

rice_farm = IndustryType:new{ ref_name = "rice_farm" }
rice_farm.name = _("Rice farm")
rice_farm:register()
rice_farm:add_output(rice)

cotton_farm = IndustryType:new{ ref_name = "cotton_farm" }
cotton_farm.name = _("Cotton farm")
cotton_farm:register()
cotton_farm:add_output(cotton)

-- Mines can new stuff from thin air, but don't worry because this is
-- not abuse-ble
diamond_mine = IndustryType:new{ ref_name = "diamond_mine" }
diamond_mine.name = _("Diamond mine")
diamond_mine:register()
diamond_mine:add_output(diamond)

zinc_mine = IndustryType:new{ ref_name = "zinc_mine" }
zinc_mine.name = _("Zinc mine")
zinc_mine:register()
zinc_mine:add_output(zinc)

copper_mine = IndustryType:new{ ref_name = "copper_mine" }
copper_mine.name = _("Copper mine")
copper_mine:register()
copper_mine:add_output(copper)

iron_mine = IndustryType:new{ ref_name = "iron_mine" }
iron_mine.name = _("Iron mine")
iron_mine:register()
iron_mine:add_output(iron)

gold_mine = IndustryType:new{ ref_name = "gold_mine" }
gold_mine.name = _("Gold mine")
gold_mine:register()
gold_mine:add_output(gold)

sulfur_mine = IndustryType:new{ ref_name = "sulfur_mine" }
sulfur_mine.name = _("Sulfur mine")
sulfur_mine:register()
sulfur_mine:add_output(sulfur)

titanium_mine = IndustryType:new{ ref_name = "titanium_mine" }
titanium_mine.name = _("Titanium mine")
titanium_mine:register()
titanium_mine:add_output(titanium)

-- Secondary sector industries (now the fun begins)
chair_manufacturer = IndustryType:new{ ref_name = "chair_manufacturer" }
chair_manufacturer.name = _("Chair manufacturer")
chair_manufacturer:register()
chair_manufacturer:add_input(timber)
chair_manufacturer:add_output(chair)

bed_manufacturer = IndustryType:new{ ref_name = "bed_manufacturer" }
bed_manufacturer.name = _("Bed manufacturer")
bed_manufacturer:register()
bed_manufacturer:add_input(timber)
bed_manufacturer:add_output(bed)

carbon_destroyer = IndustryType:new{ ref_name = "carbon_destroyer" }
carbon_destroyer.name = _("Carbon destroyer")
carbon_destroyer:register()
carbon_destroyer:add_input(coal)
carbon_destroyer:add_output(charcoal)

gunpowder_mixer = IndustryType:new{ ref_name = "gunpowder_mixer" }
gunpowder_mixer.name = _("Gunpowder mixer")
gunpowder_mixer:register()
gunpowder_mixer:add_input(sulfur)
gunpowder_mixer:add_input(charcoal)
gunpowder_mixer:add_output(gunpowder)

stainless_steel_smelter = IndustryType:new{ ref_name = "stainless_steel_smelter" }
stainless_steel_smelter.name = _("Stainless steel smelter")
stainless_steel_smelter:register()
stainless_steel_smelter:add_input(iron)
stainless_steel_smelter:add_input(chromium)
stainless_steel_smelter:add_output(stainless_steel)

brass_smelter = IndustryType:new{ ref_name = "brass_smelter" }
brass_smelter.name = _("Brass smelter")
brass_smelter:register()
brass_smelter:add_input(copper)
brass_smelter:add_input(zinc)
brass_smelter:add_output(brass)

iron_smelter = IndustryType:new{ ref_name = "iron_smelter" }
iron_smelter.name = _("Iron smelter")
iron_smelter:register()
iron_smelter:add_input(iron)
iron_smelter:add_input(coal)
iron_smelter:add_output(steel)

musket_factory = IndustryType:new{ ref_name = "musket_factory" }
musket_factory.name = _("Musket factory")
musket_factory:register()
musket_factory:add_input(timber)
musket_factory:add_input(brass)
musket_factory:add_input(stainless_steel)
musket_factory:add_input(ammunition)
musket_factory:add_output(musket)

machinegun_factory = IndustryType:new{ ref_name = "machinegun_factory" }
machinegun_factory.name = _("Machinegun factory")
machinegun_factory:register()
machinegun_factory:add_input(brass)
machinegun_factory:add_input(stainless_steel)
machinegun_factory:add_input(ammunition)
machinegun_factory:add_input(coal)
machinegun_factory:add_input(musket)
machinegun_factory:add_output(machinegun)

ammunition_factory = IndustryType:new{ ref_name = "ammunition_factory" }
ammunition_factory.name = _("Ammunition factory")
ammunition_factory:register()
ammunition_factory:add_input(gunpowder)
ammunition_factory:add_input(coal)
ammunition_factory:add_output(ammunition)

sails_factory = IndustryType:new{ ref_name = "sails_factory" }
sails_factory.name = _("Sails factory")
sails_factory:register()
sails_factory:add_input(textile)
sails_factory:add_output(sail)

-- Food factories
bakery = IndustryType:new{ ref_name = "bakery" }
bakery.name = _("Bakery")
bakery:register()
bakery:add_input(wheat)
bakery:add_output(bread)
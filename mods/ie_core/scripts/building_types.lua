-- Outpost types
barracks = BuildingType:new{ ref_name = "barracks", is_build_land_units = true, defense_bonus = 1.05 }
barracks:register()

fort = BuildingType:new{ ref_name = "fort", defense_bonus = 1.45 }
fort:register()

seaport = BuildingType:new{ ref_name = "seaport", is_build_naval_units = true, defense_bonus = 1.05 }
seaport:register()

coast_cannons = BuildingType:new{ ref_name = "coast_cannons", defense_bonus = 1.01 }
coast_cannons:register()
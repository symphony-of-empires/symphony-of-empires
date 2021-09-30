-- Outpost types
local building_type = BuildingType:new{ ref_name = "barracks", is_build_land_units = true, defense_bonus = 1.05 }
BuildingType:register(building_type)

building_type = BuildingType:new{ ref_name = "fort", defense_bonus = 1.45 }
BuildingType:register(building_type)

building_type = BuildingType:new{ ref_name = "seaport", is_build_naval_units = true, defense_bonus = 1.05 }
BuildingType:register(building_type)

building_type = BuildingType:new{ ref_name = "coast_cannons", defense_bonus = 1.01 }
BuildingType:register(building_type)
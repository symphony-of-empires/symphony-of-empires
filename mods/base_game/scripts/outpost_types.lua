-- Outpost types
local outpost_type = OutpostType:create{ ref_name = "barracks", is_build_land_units = true, defense_bonus = 1.05 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "fort", defense_bonus = 1.45 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "seaport", is_build_naval_units = true, defense_bonus = 1.05 }
OutpostType:register(outpost_type)
outpost_type = OutpostType:create{ ref_name = "coast_cannons", defense_bonus = 1.01 }
OutpostType:register(outpost_type)
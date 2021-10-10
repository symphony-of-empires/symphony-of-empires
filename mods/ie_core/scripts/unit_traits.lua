-- Unit traits
-- TODO: Probably generals have them
fanatic = UnitTrait:new{ ref_name = "fanatic", attack_mod = 1.2 }
fanatic:register()

frugal = UnitTrait:new{ ref_name = "frugal", supply_consumption_mod = 0.7 }
frugal:register()

stupid = UnitTrait:new{ ref_name = "stupid", attack_mod = 1.5, defense_mod = 0.8, supply_consumption_mod = 1.4 }
stupid:register()

heroic = UnitTrait:new{ ref_name = "heroic", attack_mod = 0.7, defense_mod = 1.5 }
heroic:register()

iron_teeth = UnitTrait:new{ ref_name = "iron_teeth", supply_consumption_mod = 0.7 }
iron_teeth:register()

chargers = UnitTrait:new{ ref_name = "chargers", attack_mod = 2.0 }
chargers:register()

zealous = UnitTrait:new{ ref_name = "zealous", defense_mod = 2.0 }
zealous:register()

suicidal = UnitTrait:new{ ref_name = "suicidal", attack_mod = 5.0, defense_mod = 0.1, supply_consumption_mod = 5.0 }
suicidal:register()
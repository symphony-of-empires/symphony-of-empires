-- Unit traits
-- TODO: Probably generals have them
local trait = UnitTrait:create{ ref_name = "fanatic", attack_mod = 1.2 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "frugal", supply_consumption_mod = 0.7 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "stupid", attack_mod = 1.5, defense_mod = 0.8, supply_consumption_mod = 1.4 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "heroic", attack_mod = 0.7, defense_mod = 1.5 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "iron_teeth", supply_consumption_mod = 0.7 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "chargers", attack_mod = 2.0 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "zealous", defense_mod = 2.0 }
UnitTrait:register(trait)
trait = UnitTrait:create{ ref_name = "suicidal", attack_mod = 5.0, defense_mod = 0.1, supply_consumption_mod = 5.0 }
UnitTrait:register(trait)
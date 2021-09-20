-- Unit types
garrison = UnitType:new{ ref_name = "garrison", defense = 5.0, attack = 2.0, health = 100.0 }
garrison.name = _("Garrison")
UnitType:register(garrison)
UnitType:requires_good(garrison, ammunition, 200)
UnitType:requires_good(garrison, musket, 100)
infantry = UnitType:new{ ref_name = "infantry", defense = 5.0, attack = 2.0, health = 100.0 }
infantry.name = _("Infantry")
UnitType:register(infantry)
UnitType:requires_good(infantry, ammunition, 200)
UnitType:requires_good(infantry, musket, 100)
cavalry = UnitType:new{ ref_name = "cavalry", defense = 2.0, attack = 15.0, health = 50.0 }
cavalry.name = _("Cavalry")
UnitType:register(cavalry)
UnitType:requires_good(cavalry, ammunition, 200)
UnitType:requires_good(cavalry, musket, 100)
artillery = UnitType:new{ ref_name = "artillery", defense = 2.0, attack = 15.0, health = 50.0 }
artillery.name = _("Artillery")
UnitType:register(artillery)
UnitType:requires_good(artillery, ammunition, 500)
UnitType:requires_good(artillery, gunpowder, 500)
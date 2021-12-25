-- Unit types
militia = UnitType:new{ ref_name = "militia", defense = 1.5, attack = 0.5, health = 10.0, speed = 0.5 }
militia.name = _("Militia")
militia:register()
--militia:requires_good(ammunition, 200)
--militia:requires_good(musket, 100)
--militia:requires_good(wheat, 10)

garrison = UnitType:new{ ref_name = "garrison", defense = 3.0, attack = 1.0, health = 100.0, speed = 0.5 }
garrison.name = _("Garrison")
garrison:register()
garrison:requires_good(ammunition, 200)
garrison:requires_good(musket, 100)

infantry = UnitType:new{ ref_name = "infantry", defense = 5.0, attack = 2.0, health = 100.0, speed = 0.5 }
infantry.name = _("Infantry")
infantry:register()
infantry:requires_good(ammunition, 200)
infantry:requires_good(musket, 100)

cavalry = UnitType:new{ ref_name = "cavalry", defense = 2.0, attack = 15.0, health = 50.0, speed = 0.5 }
cavalry.name = _("Cavalry")
cavalry:register()
cavalry:requires_good(ammunition, 200)
cavalry:requires_good(musket, 100)

artillery = UnitType:new{ ref_name = "artillery", defense = 2.0, attack = 15.0, health = 50.0, speed = 0.5 }
artillery.name = _("Artillery")
artillery:register()
artillery:requires_good(ammunition, 500)
artillery:requires_good(gunpowder, 500)

-- Naval unit types
clipper = BoatType:new{ ref_name = "clipper", defense = 10.0, attack = 10.0, health = 100.0, capacity = 4000 }
clipper.name = _("Clipper")
clipper:register()
clipper:requires_good(timber, 800)
clipper:requires_good(sail, 20)
clipper:requires_good(gunpowder, 200)

barque = BoatType:new{ ref_name = "barque", defense = 15.0, attack = 10.0, health = 200.0, capacity = 400 }
barque.name = _("Barque")
barque:register()
barque:requires_good(timber, 800)
barque:requires_good(sail, 20)
barque:requires_good(gunpowder, 200)

sloop_of_war = BoatType:new{ ref_name = "sloop_of_war", defense = 40.0, attack = 20.0, health = 400.0, capacity = 400 }
sloop_of_war.name = _("Sloop of war")
sloop_of_war:register()
sloop_of_war:requires_good(timber, 1000)
sloop_of_war:requires_good(sail, 20)
sloop_of_war:requires_good(gunpowder, 400)

man_of_war = BoatType:new{ ref_name = "man_of_war", defense = 20.0, attack = 20.0, health = 500.0, capacity = 500 }
man_of_war.name = _("Man of war")
man_of_war:register()
man_of_war:requires_good(timber, 1500)
man_of_war:requires_good(sail, 30)
man_of_war:requires_good(gunpowder, 500)
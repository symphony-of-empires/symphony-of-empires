-- Naval unit types
clipper = BoatType:new{ ref_name = "clipper", defense = 10.0, attack = 10.0, health = 100.0, capacity = 4000 }
clipper.name = _("Clipper")
BoatType:register(clipper)
BoatType:requires_good(clipper, timber, 800)
BoatType:requires_good(clipper, sail, 20)
BoatType:requires_good(clipper, gunpowder, 200)
barque = BoatType:new{ ref_name = "barque", defense = 15.0, attack = 10.0, health = 200.0, capacity = 400 }
barque.name = _("Barque")
BoatType:register(barque)
BoatType:requires_good(barque, timber, 800)
BoatType:requires_good(barque, sail, 20)
BoatType:requires_good(barque, gunpowder, 200)
sloop_of_war = BoatType:new{ ref_name = "sloop_of_war", defense = 40.0, attack = 20.0, health = 400.0, capacity = 400 }
sloop_of_war.name = _("Sloop of war")
BoatType:register(sloop_of_war)
BoatType:requires_good(sloop_of_war, timber, 1000)
BoatType:requires_good(sloop_of_war, sail, 20)
BoatType:requires_good(sloop_of_war, gunpowder, 400)
man_of_war = BoatType:new{ ref_name = "man_of_war", defense = 20.0, attack = 20.0, health = 500.0, capacity = 500 }
man_of_war.name = _("Man of war")
BoatType:register(man_of_war)
BoatType:requires_good(man_of_war, timber, 1500)
BoatType:requires_good(man_of_war, sail, 30)
BoatType:requires_good(man_of_war, gunpowder, 500)
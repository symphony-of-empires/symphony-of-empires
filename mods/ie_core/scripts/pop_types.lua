-- Pop types
entrepreneur = PopType:new{
    name = _("Entrepreneur"),
    ref_name = "entrepreneur",
    social_value = 3.0,
    is_entrepreneur = true
}
PopType:register(entrepreneur)

artisan = PopType:new{
    name = _("Artisan"),
    ref_name = "artisan",
    social_value = 1.0
}
PopType:register(artisan)

bureaucrat = PopType:new{
    name = _("Bureaucrat"),
    ref_name = "bureaucrat",
    social_value = 2.5
}
PopType:register(bureaucrat)

aristocrat = PopType:new{
    name = _("Aristocrat"),
    ref_name = "aristocrat",
    social_value = 2.0
}
PopType:register(aristocrat)

clergymen = PopType:new{
    name = _("Clergymen"),
    ref_name = "clergymen",
    social_value = 2.5
}
PopType:register(clergymen)

soldier = PopType:new{
    name = _("Soldier"),
    ref_name = "soldier",
    social_value = 1.5
}
PopType:register(soldier)

farmer = PopType:new{
    name = _("Farmer"),
    ref_name = "farmer",
    social_value = 1.0,
    is_farmer = true
}
PopType:register(farmer)

laborer = PopType:new{
    name = _("Laborer"),
    ref_name = "laborer",
    social_value = 1.0,
    is_laborer = true
}
PopType:register(laborer)

slave = PopType:new{
    name = _("Slave"),
    ref_name = "slave",
    social_value = 0.1,
    is_slave = true
}
PopType:register(slave)

-- TODO: Remove these, because they are deprecated
officer = PopType:new{
    name = _("Officer"),
    ref_name = "officer",
    social_value = 1.5
}
PopType:register(officer)

craftsmen = PopType:new{
    name = _("Craftsmen"),
    ref_name = "craftsmen",
    social_value = 1.5
}
PopType:register(craftsmen)
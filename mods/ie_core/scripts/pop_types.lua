-- Pop types

burgeoise = PopType:new{
    name = _("Burgeoise"),
    ref_name = "burgeoise",
    social_value = 2.0,
    is_burgeoise = true
}
burgeoise:register()

artisan = PopType:new{
    name = _("Artisan"),
    ref_name = "artisan",
    social_value = 1.0
}
artisan:register()

bureaucrat = PopType:new{
    name = _("Bureaucrat"),
    ref_name = "bureaucrat",
    social_value = 2.5
}
bureaucrat:register()

clergymen = PopType:new{
    name = _("Clergymen"),
    ref_name = "clergymen",
    social_value = 2.5
}
clergymen:register()

soldier = PopType:new{
    name = _("Soldier"),
    ref_name = "soldier",
    social_value = 1.5
}
soldier:register()

farmer = PopType:new{
    name = _("Farmer"),
    ref_name = "farmer",
    social_value = 1.0,
    is_farmer = true
}
farmer:register()

laborer = PopType:new{
    name = _("Laborer"),
    ref_name = "laborer",
    social_value = 1.0,
    is_laborer = true
}
laborer:register()

slave = PopType:new{
    name = _("Slave"),
    ref_name = "slave",
    social_value = 0.1,
    is_slave = true
}
slave:register()

-- TODO: Remove these, because they are deprecated
officer = PopType:new{
    name = _("Officer"),
    ref_name = "officer",
    social_value = 1.5
}
officer:register()

craftsmen = PopType:new{
    name = _("Craftsmen"),
    ref_name = "craftsmen",
    social_value = 1.5
}
craftsmen:register()
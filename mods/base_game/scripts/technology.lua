voltaic_pile = Technology:new{
    ref_name = "voltaic_pile",
    name = _("Voltaic pile"),
    description = "An early battery",
    cost = 15.0
}
Technology:register(voltaic_pile)

combustion_engine = Technology:new{
    ref_name = "combustion_engine",
    name = "Combustion engine",
    description = "",
    cost = 50.0
}
Technology:register(combustion_engine)

steam_locomotive = Technology:new{
    ref_name = "steam_locomotive",
    name = "Steam locomotive",
    description = "",
    cost = 100.0
}
Technology:register(steam_locomotive)

assembly_line = Technology:new{
    ref_name = "assembly_line",
    name = "Assembly line",
    description = "",
    cost = 100.0
}
Technology:register(assembly_line)
Technology:requires_technology(assembly_line, steam_locomotive)
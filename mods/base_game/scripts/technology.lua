local technology = Technology:create{
    ref_name = "voltaic_pile",
    name = _("Voltaic pile"),
    description = "An early battery",
    cost = 15.0
}
Technology:register(technology)

technology = Technology:create{
    ref_name = "combustion_engine",
    name = "Combustion engine",
    description = "",
    cost = 50.0
}
Technology:register(technology)

technology = Technology:create{
    ref_name = "steam_locomotive",
    name = "Steam locomotive",
    description = "",
    cost = 100.0
}
Technology:register(technology)
najran = Nation:new{ ref_name = "najran", name = _("Najran") }
najran.adjective = _("Najrani")
najran:register()
najran:add_accepted_culture(arabic)
najran:add_client_hint(monarchy, _("Najran"), 0x808080)

nation = Nation:new{ ref_name = "zaidi_imamate", name = _("Zaidi Imamate") }
nation.adjective = _("Zaidi")
nation:register()
nation:add_accepted_culture(arabic)
nation:add_client_hint(monarchy, _("Zaidi Imamate"), 0x808080)

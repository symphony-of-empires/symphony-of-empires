
korea = Nation:new{ ref_name = "korea", name = _("Korea") }
korea.adjective = _("Korean")
korea:register()
korea:add_accepted_culture(belgian)
korea:add_client_hint(monarchy, _("Korea"), 0x808080)

taiwan = Nation:new{ ref_name = "taiwan", name = _("Taiwan") }
taiwan.adjective = _("Taiwanesse")
taiwan:register()
--taiwan:add_accepted_culture(chinesse)
taiwan:add_client_hint(socialism, _("Taiwan's People's Republic"), 0x808080)
taiwan:add_client_hint(communism, _("Taiwan"), 0x808080)

-- Nations
hawai_i = Nation:new{ ref_name = "hawai_i" }
hawai_i.name = _("Hawai'i")
hawai_i:register()
hawai_i:add_accepted_culture(polynesian)
hawai_i:add_client_hint(communism, _("Hawai'i"), 0x808000)

require('nations/africa')
require('nations/america')
require('nations/europe')
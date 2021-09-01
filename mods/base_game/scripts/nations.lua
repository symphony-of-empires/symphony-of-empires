-- Nations
hawai_i = Nation:create{ ref_name = "hawai_i" }
hawai_i.name = _("Hawai'i")
Nation:register(hawai_i)
Nation:add_accepted_culture(hawai_i, polynesian)

require('nations/africa')
require('nations/america')
require('nations/europe')
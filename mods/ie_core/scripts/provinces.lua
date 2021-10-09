-- Provinces
require('provinces/africa/northern')

require('provinces/americas/alaska')
require('provinces/americas/argentina')
require('provinces/americas/brazil_and_pantanal')
require('provinces/americas/canada')
require('provinces/americas/caribbean')
require('provinces/americas/central_america')
require('provinces/americas/chile')
require('provinces/americas/guianas')
require('provinces/americas/mexico')
require('provinces/americas/south_america')
require('provinces/americas/united_states')

require('provinces/europe/austria')
require('provinces/europe/balkans')
require('provinces/europe/baltics')
require('provinces/europe/bohemia')
require('provinces/europe/denmark')
require('provinces/europe/france')
require('provinces/europe/greece')
require('provinces/europe/portugal')
require('provinces/europe/scandinavia')
require('provinces/europe/spain')

require('provinces/oceania/hawaii')

require('provinces/unknown')

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Saxony (Germany)
province = Province:new{ ref_name = "dresden", color = 0x8cb2ff }
province.name = _("Dresden")
Province:register(province)
Province:add_pop(province, artisan, german, protestant, 5000, 0.9)
Province:add_pop(province, farmer, german, protestant, 40000, 0.8)
Province:add_pop(province, soldier, german, protestant, 8000, 0.8)
Province:add_pop(province, craftsmen, german, protestant, 5000, 0.9)
Province:add_pop(province, bureaucrat, german, protestant, 5000, 0.9)
Province:add_pop(province, aristocrat, german, protestant, 5000, 0.9)
Province:add_pop(province, clergymen, german, protestant, 5000, 0.9)
Province:add_pop(province, laborer, german, protestant, 50000, 0.8)
Province:add_pop(province, entrepreneur, german, protestant, 800, 0.9)
Province:add_nucleus(province, saxony)
Province:give_to(province, saxony)
Nation:set_capital(saxony, province)
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

require('provinces/asia/anatolia')

require('provinces/europe/austria')
require('provinces/europe/balkans')
require('provinces/europe/baltics')
require('provinces/europe/bohemia')
require('provinces/europe/britain')
require('provinces/europe/denmark')
require('provinces/europe/germany')
require('provinces/europe/netherlands')
require('provinces/europe/france')
require('provinces/europe/greece')
require('provinces/europe/portugal')
require('provinces/europe/russia')
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
province:register()
province:add_pop(artisan, german, protestant, 5000, 0.9)
province:add_pop(farmer, german, protestant, 40000, 0.8)
province:add_pop(soldier, german, protestant, 8000, 0.8)
province:add_pop(craftsmen, german, protestant, 5000, 0.9)
province:add_pop(bureaucrat, german, protestant, 5000, 0.9)
province:add_pop(aristocrat, german, protestant, 5000, 0.9)
province:add_pop(clergymen, german, protestant, 5000, 0.9)
province:add_pop(laborer, german, protestant, 50000, 0.8)
province:add_pop(entrepreneur, german, protestant, 800, 0.9)
province:add_nucleus(saxony)
province:give_to(saxony)
saxony:set_capital(province)
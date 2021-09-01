--------------------------------------------------------------------------------------------------------------------
usa_east_transport = Company:create{ name = "USA East Transport", money = 10000000, is_transport = true }
Company:register(usa_east_transport)
andes_transport = Company:create{ name = "Andes Transport", money = 10000000, is_transport = true }
Company:register(andes_transport)
mexico_transport = Company:create{ name = "Mexico Transport", money = 10000000, is_transport = true }
Company:register(mexico_transport)
royal_russia_transport = Company:create{ name = "Royal Russia Transport", money = 10000000, is_transport = true }
Company:register(mexico_transport)
--------------------------------------------------------------------------------------------------------------------

-- Provinces
require('provinces/africa/northern')
require('provinces/americas/alaska')
require('provinces/americas/argentina')
require('provinces/americas/brazil_and_pantanal')
require('provinces/americas/canada')
require('provinces/americas/caribbean')
require('provinces/americas/central_america')
require('provinces/americas/guianas')
require('provinces/americas/mexico')
require('provinces/americas/south_america')
require('provinces/americas/united_states')
require('provinces/europe/austria')
require('provinces/europe/bohemia')
require('provinces/europe/denmark')
require('provinces/europe/france')
require('provinces/europe/scandinavia')
require('provinces/oceania/hawaii')
require('provinces/unknown')

--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Saxony (Germany)
province = Province:create{ ref_name = "dresden", color = 0x8cb2ff }
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

-- THIS IS NOT PART OF THE FINAL MODDING API

-- Companies

add_op_province_to_company(0, "maryland")
add_op_province_to_company(0, "virginia")
add_op_province_to_company(0, "delaware")
add_op_province_to_company(0, "maine")
add_op_province_to_company(0, "pensylvania")
add_op_province_to_company(0, "vermont")
add_op_province_to_company(0, "new_hampshire")
add_op_province_to_company(0, "massachusetts")
add_op_province_to_company(0, "new_york")
add_op_province_to_company(0, "rhode_island")
add_op_province_to_company(0, "new_jersey")
add_op_province_to_company(0, "connecticut")
add_op_province_to_company(0, "florida")

add_op_province_to_company(1, "sergipe")
add_op_province_to_company(1, "alagoas")
add_op_province_to_company(1, "sao_francisco")
add_op_province_to_company(1, "pernambuco")
add_op_province_to_company(1, "ceara")
add_op_province_to_company(1, "mato_grosso")
add_op_province_to_company(1, "goias")

add_op_province_to_company(2, "durango")
add_op_province_to_company(2, "san_luis_potosi")
add_op_province_to_company(2, "guanajuato")
add_op_province_to_company(2, "michoacan")
add_op_province_to_company(2, "colima")
add_op_province_to_company(2, "mexico")
add_op_province_to_company(2, "puebla")
add_op_province_to_company(2, "queretaro")
add_op_province_to_company(2, "tlaxcala")
add_op_province_to_company(2, "chiapas")
add_op_province_to_company(2, "oaxaca")
add_op_province_to_company(2, "nuevo_mexico")
add_op_province_to_company(2, "sinaloa")
add_op_province_to_company(2, "jalisco")
add_op_province_to_company(2, "baja_california")
add_op_province_to_company(2, "alta_california")
add_op_province_to_company(2, "sonora")
add_op_province_to_company(2, "tamaulipas")
add_op_province_to_company(2, "nuevo_leon")
add_op_province_to_company(2, "veracruz")
add_op_province_to_company(2, "coahuila_y_texas")
add_op_province_to_company(2, "chihuahua")

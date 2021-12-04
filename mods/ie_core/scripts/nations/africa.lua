-----------------------------------------------------------------------
-- North Africa
-- Initial nations
tunis = Nation:new{ ref_name = "tunis" }
tunis.name = _("Tunis")
tunis.adjective = _("Tunisian")
tunis:register()
tunis:set_ideology(monarchy)
tunis:add_accepted_culture(arabic)
tunis:add_client_hint(monarchy, _("Tunis"), 0x808000)

algeirs = Nation:new{ ref_name = "algeirs" }
algeirs.name = _("Algeirs")
algeirs.adjective = _("Algerian")
algeirs:register()
algeirs:add_accepted_culture(arabic)
algeirs:add_client_hint(monarchy, _("Algeirs"), 0x808000)

abbyssina = Nation:new{ ref_name = "abbyssina" }
abbyssina.name = _("Abbyssina")
abbyssina:register()
abbyssina:add_client_hint(monarchy, _("Abbyssina"), 0x808000)

egypt = Nation:new{ ref_name = "egypt" }
egypt.name = _("Egypt")
egypt.adjective = _("Egyptians")
egypt:register()
egypt:add_client_hint(monarchy, _("Egypt"), 0x808000)

tripolitania = Nation:new{ ref_name = "tripolitania" }
tripolitania.name = _("Tripolitania")
tripolitania.adjective = _("Tripolitanians")
tripolitania:register()
tripolitania:add_client_hint(monarchy, _("Tripolitania"), 0x808000)

ashantee = Nation:new{ ref_name = "ashantee" }
ashantee.name = _("Ashantee")
ashantee:register()
ashantee:add_client_hint(monarchy, _("Ashantee"), 0x808000)
-- Formable nations
arabia = Nation:new{ ref_name = "arabia" }
arabia.name = _("Arabia")
arabia.adjective = "Arabic"
arabia.noun = "Arab"
arabia:register()
arabia:add_accepted_culture(arabic)
arabia:add_client_hint(monarchy, _("Arabia"), 0x808000)
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Western Africa
-- Initial nations
sokoto = Nation:new{ ref_name = "sokoto" }
sokoto.name = _("Sokoto")
sokoto:register()
sokoto:add_accepted_culture(arabic)
sokoto:add_client_hint(monarchy, _("Sokoto"), 0x808000)

wadai_empire = Nation:new{ ref_name = "wadai_empire" }
wadai_empire.name = _("Wadai Empire")
wadai_empire:register()
wadai_empire:add_client_hint(monarchy, _("Wadai Empire"), 0x808000)

bornu = Nation:new{ ref_name = "bornu" }
bornu.name = _("Bornu")
bornu:register()
bornu:add_client_hint(monarchy, _("Bornu"), 0x808000)

dahomey = Nation:new{ ref_name = "dahomey" }
dahomey.name = _("Dahomey")
dahomey:register()
dahomey:add_client_hint(monarchy, _("Dahomey"), 0x808000)
-----------------------------------------------------------------------
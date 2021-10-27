-- Terrain types
deepsea = TerrainType:new{ ref_name = "deepsea", name = _("Deep sea") }
deepsea:register()

sea = TerrainType:new{ ref_name = "sea", name = _("Sea"), color = 0x0000ff }
sea:register()

plains = TerrainType:new{ ref_name = "plains", name = _("Plains") }
plains:register()

forest = TerrainType:new{ ref_name = "forest", name = _("Forest") }
forest:register()

river = TerrainType:new{ ref_name = "river", name = _("River") }
river:register()

jungle = TerrainType:new{ ref_name = "jungle", name = _("Jungle"), color = 0x00ff00 }
jungle:register()

desert = TerrainType:new{ ref_name = "desert", name = _("Desert"), color = 0xfcff0f }
desert:register()

savanna = TerrainType:new{ ref_name = "savanna", name = _("Savanna"), color = 0xff970f }
savanna:register()

grasslands = TerrainType:new{ ref_name = "grasslands", name = _("Grasslands"), color = 0x346b1f }
grasslands:register()

taiga = TerrainType:new{ ref_name = "taiga", name = _("Taiga"), color = 0x47bf17 }
taiga:register()

tundra = TerrainType:new{ ref_name = "tundra", name = _("Tundra"), color = 0x00ffff }
tundra:register()

artic = TerrainType:new{ ref_name = "artic", name = _("Artic"), color = 0xffffff }
artic:register()

mountain = TerrainType:new{ ref_name = "mountain", name = _("Mountain") }
mountain:register()

-- Terrain types where farms can be created
tropical_fsoil = TerrainType:new{ ref_name = "tropical_fsoil", name = _("Tropical Soil") }
tropical_fsoil:register()

subtropical_fsoil = TerrainType:new{ ref_name = "subtropical_fsoil", name = _("Subtropical Soil") }
subtropical_fsoil:register()

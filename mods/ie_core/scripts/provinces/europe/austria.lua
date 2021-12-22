--------------------------------------------------------------
-- Continent: Europe
-- Subcontinent: Middle Europe
-- Country: Austrian Empire

local austria_provinces_list = {
    -- Austria
    { ref_name = "vorarlberg", name = _("Vorarlberg"), color = 0x5effb6 },
    { ref_name = "tyrol", name = _("Tyrol"), color = 0xff899f },
    { ref_name = "salzburg", name = _("Salzburg"), color = 0xff4c85 },
    { ref_name = "carinthia", name = _("Carinthia"), color = 0xd1ffff },
    { ref_name = "styria", name = _("Styria"), color = 0x77ffff },
    { ref_name = "upper_austria", name = _("Upper Austria"), color = 0xff6554 },
    { ref_name = "lower_austria", name = _("Lower Austria"), color = 0xfffd89 },
    { ref_name = "burgenland", name = _("Burgenland"), color = 0x7c92ff },

    -- Uncategorized
	{ ref_name = "province_1111ff00", name = _("Unknown"), color = 0x1111ff },
    { ref_name = "province_568efe00", name = _("Unknown"), color = 0x568efe },
    { ref_name = "province_ff91b900", name = _("Unknown"), color = 0xb991ff },
    { ref_name = "province_baffba00", name = _("Unknown"), color = 0xbaffba },
    { ref_name = "province_b2ff7700", name = _("Unknown"), color = 0x77ffb2 },
    { ref_name = "province_ff829200", name = _("Unknown"), color = 0x9282ff },
    { ref_name = "province_20465600", name = _("Unknown"), color = 0x564620 },
    { ref_name = "province_ff7f9f00", name = _("Unknown"), color = 0x9f7fff },
    { ref_name = "province_0bb900", name = _("Unknown"), color = 0xb90b00 },
    { ref_name = "province_ffe28e00", name = _("Unknown"), color = 0x8ee2ff },
    { ref_name = "province_ffb9b500", name = _("Unknown"), color = 0xb5b9ff },
    { ref_name = "province_b7e7ff00", name = _("Unknown"), color = 0xffe7b7 },
    { ref_name = "province_a538ff00", name = _("Unknown"), color = 0xff38a5 },
    { ref_name = "province_7038ff00", name = _("Unknown"), color = 0xff3870 },
    { ref_name = "province_ff5b9a00", name = _("Unknown"), color = 0x9a5bff },
    { ref_name = "province_ff93f600", name = _("Unknown"), color = 0xf693ff },
    { ref_name = "province_bacdff00", name = _("Unknown"), color = 0xffcdba },
    { ref_name = "province_f4ff6800", name = _("Unknown"), color = 0x68fff4 },
    { ref_name = "province_f244ff00", name = _("Unknown"), color = 0xff44f2 },
    { ref_name = "province_ff695b00", name = _("Unknown"), color = 0x5b69ff },
    { ref_name = "province_9d87ff00", name = _("Unknown"), color = 0xff879d },
    { ref_name = "province_d28eff00", name = _("Unknown"), color = 0xff8ed2 },
    { ref_name = "province_3affbd00", name = _("Unknown"), color = 0xbdff3a },
    { ref_name = "province_5954ff00", name = _("Unknown"), color = 0xff5459 },
    { ref_name = "province_47caff00", name = _("Unknown"), color = 0xffca47 },
    { ref_name = "province_2850ff00", name = _("Unknown"), color = 0xff5028 },
    { ref_name = "province_ff59d200", name = _("Unknown"), color = 0xd259ff },
    { ref_name = "province_8cffe600", name = _("Unknown"), color = 0xe6ff8c },
    { ref_name = "province_b970ff00", name = _("Unknown"), color = 0xff70b9 },
    { ref_name = "province_66ff9900", name = _("Unknown"), color = 0x99ff66 },
    { ref_name = "province_9dcccc00", name = _("Unknown"), color = 0xcccc9d },
    { ref_name = "province_6f47ff00", name = _("Unknown"), color = 0xff476f },
    { ref_name = "province_cc5bbb00", name = _("Unknown"), color = 0xbb5bcc },
    { ref_name = "province_24cc2700", name = _("Unknown"), color = 0x27cc24 },
    { ref_name = "province_90ccaf00", name = _("Unknown"), color = 0xafcc90 },
    { ref_name = "province_b1ffaa00", name = _("Unknown"), color = 0xaaffb1 },
    { ref_name = "province_fdffb700", name = _("Unknown"), color = 0xb7fffd },
    { ref_name = "province_46681d00", name = _("Unknown"), color = 0x1d6846 },
    { ref_name = "province_2e536600", name = _("Unknown"), color = 0x66532e },
    { ref_name = "province_4098f000", name = _("Unknown"), color = 0xf09840 },
    { ref_name = "province_60075d00", name = _("Unknown"), color = 0x5d0760 },
    { ref_name = "province_ff5e4c00", name = _("Unknown"), color = 0x4c5eff },
    { ref_name = "province_60d65000", name = _("Unknown"), color = 0x50d660 },
    { ref_name = "province_52664f00", name = _("Unknown"), color = 0x4f6652 },
    { ref_name = "province_ff569c00", name = _("Unknown"), color = 0x9c56ff },
    { ref_name = "province_77574900", name = _("Unknown"), color = 0x495777 },
    { ref_name = "province_96ce7700", name = _("Unknown"), color = 0x77ce96 },
    { ref_name = "province_cc7ec000", name = _("Unknown"), color = 0xc07ecc },
    { ref_name = "province_7b41cc00", name = _("Unknown"), color = 0xcc417b },
    { ref_name = "province_aacc9000", name = _("Unknown"), color = 0x90ccaa },
    { ref_name = "province_2d0e1400", name = _("Unknown"), color = 0x140e2d },
    { ref_name = "province_ad5bff00", name = _("Unknown"), color = 0xff5bad },
    { ref_name = "province_93ffc700", name = _("Unknown"), color = 0xc7ff93 },
    { ref_name = "province_ff7c6800", name = _("Unknown"), color = 0x687cff },
    { ref_name = "province_cc744b00", name = _("Unknown"), color = 0x4b74cc },
    { ref_name = "province_29336800", name = _("Unknown"), color = 0x683329 },
    { ref_name = "province_cc397d00", name = _("Unknown"), color = 0x7d39cc },
    { ref_name = "province_3f664200", name = _("Unknown"), color = 0x42663f },
    { ref_name = "province_15142d00", name = _("Unknown"), color = 0x2d1415 },
    { ref_name = "province_48666300", name = _("Unknown"), color = 0x636648 },
    { ref_name = "province_661e2600", name = _("Unknown"), color = 0x261e66 },
    { ref_name = "province_3e637700", name = _("Unknown"), color = 0x77633e },
    { ref_name = "province_28d100", name = _("Unknown"), color = 0xd12800 },
    { ref_name = "province_77202e00", name = _("Unknown"), color = 0x2e2077 },
    { ref_name = "province_66283400", name = _("Unknown"), color = 0x342866 },
    { ref_name = "province_1b661e00", name = _("Unknown"), color = 0x1e661b },
    { ref_name = "province_20000000", name = _("Unknown"), color = 0x20 },
    { ref_name = "province_3d186b00", name = _("Unknown"), color = 0x6b183d },
    { ref_name = "province_20d11500", name = _("Unknown"), color = 0x15d120 },
    { ref_name = "province_e0c72700", name = _("Unknown"), color = 0x27c7e0 },
    { ref_name = "province_c0ef6700", name = _("Unknown"), color = 0x67efc0 },
    { ref_name = "province_80a52200", name = _("Unknown"), color = 0x22a580 },
    { ref_name = "province_4091b500", name = _("Unknown"), color = 0xb59140 },
    { ref_name = "province_20f2cd00", name = _("Unknown"), color = 0xcdf220 },
    { ref_name = "province_ce2f5400", name = _("Unknown"), color = 0x542fce },
    { ref_name = "province_6d066500", name = _("Unknown"), color = 0x65066d },
    { ref_name = "province_9c7bce00", name = _("Unknown"), color = 0xce7b9c },
    { ref_name = "province_396b4100", name = _("Unknown"), color = 0x416b39 },
    { ref_name = "province_77523300", name = _("Unknown"), color = 0x335277 },
    { ref_name = "province_684a0e00", name = _("Unknown"), color = 0xe4a68 },
    { ref_name = "province_653f7700", name = _("Unknown"), color = 0x773f65 },
    { ref_name = "province_44697f00", name = _("Unknown"), color = 0x7f6944 },
    { ref_name = "province_5d7f5f00", name = _("Unknown"), color = 0x5f7f5d },
    { ref_name = "province_e172d00", name = _("Unknown"), color = 0x2d170e },
    { ref_name = "province_e007c00", name = _("Unknown"), color = 0x7c000e },
    { ref_name = "province_44ff4e00", name = _("Unknown"), color = 0x4eff44 },
    { ref_name = "province_6ba6ff00", name = _("Unknown"), color = 0xffa66b },
    { ref_name = "province_7a433700", name = _("Unknown"), color = 0x37437a },
    { ref_name = "province_7f306700", name = _("Unknown"), color = 0x67307f },
    { ref_name = "province_9b93ff00", name = _("Unknown"), color = 0xff939b },
    { ref_name = "province_ff583a00", name = _("Unknown"), color = 0x3a58ff },
    { ref_name = "province_ccff6000", name = _("Unknown"), color = 0x60ffcc },
    { ref_name = "province_60e2ff00", name = _("Unknown"), color = 0xffe260 },
    { ref_name = "province_a6ff9600", name = _("Unknown"), color = 0x96ffa6 },
    { ref_name = "province_15577000", name = _("Unknown"), color = 0x705715 },
    { ref_name = "province_6c705100", name = _("Unknown"), color = 0x51706c },
    { ref_name = "province_527a2900", name = _("Unknown"), color = 0x297a52 },
    { ref_name = "province_35537a00", name = _("Unknown"), color = 0x7a5335 },
    { ref_name = "province_7a235400", name = _("Unknown"), color = 0x54237a },
    { ref_name = "province_106d3800", name = _("Unknown"), color = 0x386d10 },
    { ref_name = "province_25137000", name = _("Unknown"), color = 0x701325 },
    { ref_name = "province_ffaaf300", name = _("Unknown"), color = 0xf3aaff },
    { ref_name = "province_ff7f9d00", name = _("Unknown"), color = 0x9d7fff },
    { ref_name = "province_4a3dff00", name = _("Unknown"), color = 0xff3d4a },
    { ref_name = "province_3419ff00", name = _("Unknown"), color = 0xff1934 },
    { ref_name = "province_ffb96300", name = _("Unknown"), color = 0x63b9ff },
    { ref_name = "province_701d7700", name = _("Unknown"), color = 0x771d70 },
    { ref_name = "province_5eff4c00", name = _("Unknown"), color = 0x4cff5e },
}

for k, v in pairs(austria_provinces_list) do
    province = Province:new(v)
    province:register()

    province:add_industry(wheat_farm, austria, andes_transport)
    province:add_pop(artisan, german, catholic, 5000, 0.9)
    province:add_pop(farmer, german, catholic, 40000, 0.8)
    province:add_pop(soldier, german, catholic, 8000, 0.8)
    province:add_pop(craftsmen, german, catholic, 5000, 0.9)
    province:add_pop(bureaucrat, german, catholic, 5000, 0.9)
    province:add_pop(aristocrat, german, catholic, 5000, 0.9)
    province:add_pop(clergymen, german, catholic, 5000, 0.9)
    province:add_pop(laborer, german, catholic, 50000, 0.8)
    province:add_pop(entrepreneur, german, catholic, 800, 0.9)
    province:add_nucleus(austria)
    province:give_to(austria)
    province:set_terrain(plains)
end

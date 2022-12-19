-- Industrial_Era_mod - Base game files for Symphony of Empires
-- Copyright (C) 2021, Symphony of Empires contributors
-- 
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- ----------------------------------------------------------------------------
--	Name:
--  	ideologies.lua
--
-- 	Abstract:
--      Does important stuff
-- ----------------------------------------------------------------------------

monarchy = Ideology:new{ ref_name = "monarchy", name = _("Monarchy"), color = 0xf2de24 }
monarchy:register()
monarchy:add_subideology(Subideology:new{
	ref_name = "absolute_monarchy",
	name = "Absolute monarchy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = -1.0,
})
monarchy:add_subideology(Subideology:new{
	ref_name = "constitutional_monarchy",
	name = "Constitutional monarchy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 0.5,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = -0.5,
})
monarchy:add_subideology(Subideology:new{
	ref_name = "constitutional_monarchy",
	name = "Constitutional monarchy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 0.5,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = -0.25,
})
monarchy:add_subideology(Subideology:new{
	ref_name = "parliamentary_monarchy",
	name = "Parliamentary monarchy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 0.5,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.25,
})
monarchy:add_subideology(Subideology:new{
	ref_name = "monarchist_slave_state",
	name = "Monarchist slave state",
	distributism = -1.0,
	mercantilist = 1.0,
	capitalism = 1.0,
	individualism = -1.0,
	state_power = 1.0,
	equalitarianism = -1.0,
	secular = 0.0,
	pluralism = -1.0,
})

republic = Ideology:new{ ref_name = "republic", name = _("Republic"), color = 0x4245f5 }
republic:register()
republic:add_subideology(Subideology:new{
	ref_name = "absolute_democracy",
	name = "Absolute democracy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = -1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 1.0,
})
republic:add_subideology(Subideology:new{
	ref_name = "parliamentary_democracy",
	name = "Parliamentary democracy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = -0.5,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.75,
})
republic:add_subideology(Subideology:new{
	ref_name = "representative_democracy",
	name = "Representative democracy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = -0.25,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.5,
})
republic:add_subideology(Subideology:new{
	ref_name = "oligarchic_democracy",
	name = "Oligarchic democracy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 1.0,
	individualism = 0.0,
	state_power = 0.5,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = -0.5,
})

communism = Ideology:new{ ref_name = "communism", name = _("Communism"), color = 0x800e04 }
communism:register()
communism:add_subideology(Subideology:new{
	ref_name = "???",
	name = "???",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 0.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
})

fascism = Ideology:new{ ref_name = "fascism", name = _("Fascism"), color = 0xcc1f6d }
fascism:register()
fascism:add_subideology(Subideology:new{
	ref_name = "absolute_dictatorship",
	name = "Absolute dictatorship",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = -1.0,
})
fascism:add_subideology(Subideology:new{
	ref_name = "fascism",
	name = "Fascism",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = -0.5,
	secular = 0.0,
	pluralism = -1.0,
})
fascism:add_subideology(Subideology:new{
	ref_name = "religious_fascism",
	name = "Religious fascism",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = 0.0,
	secular = 1.0,
	pluralism = -1.0,
})

socialism = Ideology:new{ ref_name = "socialism", name = _("Socialism"), color = 0xff1500 }
socialism:register()
socialism:add_subideology(Subideology:new{
	ref_name = "proletariat_dictatorship",
	name = "Proletariat dictatorship",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = -1.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 1.0,
})

tribal = Ideology:new{ ref_name = "tribal", name = _("Tribal"), color = 0x684242 }
tribal:register()
tribal:add_subideology(Subideology:new{
	ref_name = "hierachical_tribe",
	name = "Hierachical tribe",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = 1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
})
tribal:add_subideology(Subideology:new{
	ref_name = "nomadic_tribe",
	name = "Nomadic tribe",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 0.0,
	state_power = -1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
})

anarchy = Ideology:new{ ref_name = "anarchy", name = _("Anarchy"), color = 0x363533 }
anarchy:register()
anarchy:add_subideology(Subideology:new{
	ref_name = "anarcho_capitalism",
	name = "Anarcho capitalism",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 1.0,
	individualism = 1.0,
	state_power = -1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
})
anarchy:add_subideology(Subideology:new{
	ref_name = "anarcho_communism",
	name = "Anarcho communism",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = -1.0,
	individualism = 1.0,
	state_power = -1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 0.0,
})
anarchy:add_subideology(Subideology:new{
	ref_name = "anarcho_democracy",
	name = "Anarcho democracy",
	distributism = 0.0,
	mercantilist = 0.0,
	capitalism = 0.0,
	individualism = 1.0,
	state_power = -1.0,
	equalitarianism = 0.0,
	secular = 0.0,
	pluralism = 1.0,
})
anarchy:add_subideology(Subideology:new{
	ref_name = "anarchic_slave_state",
	name = "Anarchic slave state",
	distributism = -1.0,
	mercantilist = 1.0,
	capitalism = 1.0,
	individualism = -1.0,
	state_power = -1.0,
	equalitarianism = -1.0,
	secular = 0.0,
	pluralism = -1.0,
})

-- Good types
-- Farmable stuff
wheat = Good:new{ ref_name = "wheat" }
wheat.name = _("Wheat")
wheat.is_edible = true
wheat:register()
grapes = Good:new{ ref_name = "grapes" }
grapes.name = _("Grapes")
grapes.is_edible = true
grapes:register()
rice = Good:new{ ref_name = "rice" }
rice.name = _("Rice")
rice.is_edible = true
rice:register()
cotton = Good:new{ ref_name = "cotton" }
cotton.name = _("Cotton")
cotton:register()
coffee = Good:new{ ref_name = "coffee" }
coffee.name = _("Coffe bean")
coffee:register()
salt_grain = Good:new{ ref_name = "salt_grain" }
salt_grain.name = _("Salt grain")
salt_grain:register()
silk = Good:new{ ref_name = "silk" }
silk.name = _("Silk")
silk:register()

-- Industrial materials
fuel = Good:new{ ref_name = "fuel" }
fuel.name = _("Fuel")
fuel:register()
glass = Good:new{ ref_name = "glass" }
glass.name = _("Glass")
glass:register()
fabric = Good:new{ ref_name = "fabric" }
fabric.name = _("Fabric")
fabric:register()
dye = Good:new{ ref_name = "dye" }
dye.name = _("Dye")
dye:register()
textile = Good:new{ ref_name = "textile" }
textile.name = _("Textile")
textile:register()
plastic = Good:new{ ref_name = "plastic" }
plastic.name = _("Plastic")
plastic:register()
steel = Good:new{ ref_name = "steel" }
steel.name = _("Steel")
steel:register()
charcoal = Good:new{ ref_name = "charcoal" }
charcoal.name = _("Charcoal")
charcoal:register()

-- Military stuff
gunpowder = Good:new{ ref_name = "gunpowder" }
gunpowder.name = _("Gunpowder")
gunpowder:register()
glock = Good:new{ ref_name = "glock" }
glock.name = _("Glock")
glock:register()
cannon = Good:new{ ref_name = "cannon" }
cannon.name = _("Cannon")
cannon:register()
revolver = Good:new{ ref_name = "revolver" }
revolver.name = _("Revolver")
revolver:register()
musket = Good:new{ ref_name = "musket" }
musket.name = _("Musket")
musket:register()
rifle = Good:new{ ref_name = "rifle" }
rifle.name = _("Rifle")
rifle:register()
machinegun = Good:new{ ref_name = "machinegun" }
machinegun.name = _("Machinegun")
machinegun:register()
ammunition = Good:new{ ref_name = "ammunition" }
ammunition.name = _("Ammunition")
ammunition:register()
sail = Good:new{ ref_name = "sail" }
sail.name = _("Sail")
sail:register()

-- Minerals
copper = Good:new{ ref_name = "copper" }
copper.name = _("Copper")
copper:register()
silica = Good:new{ ref_name = "silica" }
silica.name = _("Silica")
silica:register()
uranium = Good:new{ ref_name = "uranium" }
uranium.name = _("Uranium")
uranium:register()
gold = Good:new{ ref_name = "gold" }
gold.name = _("Gold")
gold:register()
iron = Good:new{ ref_name = "iron" }
iron.name = _("Iron")
iron:register()
diamond = Good:new{ ref_name = "diamond" }
diamond.name = _("Diamond")
diamond:register()
titanium = Good:new{ ref_name = "titanium" }
titanium.name = _("Titanium")
titanium:register()
bronze = Good:new{ ref_name = "bronze" }
bronze.name = _("Bronze")
bronze:register()
silver = Good:new{ ref_name = "silver" }
silver.name = _("Silver")
silver:register()
platinum = Good:new{ ref_name = "platinum" }
platinum.name = _("Platinum")
platinum:register()
plutonium = Good:new{ ref_name = "plutonium" }
plutonium.name = _("Plutonium")
plutonium:register()
tin = Good:new{ ref_name = "tin" }
tin.name = _("Tin")
tin:register()
tungsten = Good:new{ ref_name = "tungsten" }
tungsten.name = _("Tungsten")
tungsten:register()
oil = Good:new{ ref_name = "oil" }
oil.name = _("Oil")
oil:register()
sand = Good:new{ ref_name = "sand" }
sand.name = _("Sand")
sand:register()
timber = Good:new{ ref_name = "timber" }
timber.name = _("Timber")
timber:register()
coal = Good:new{ ref_name = "coal" }
coal.name = _("Coal")
coal:register()
zinc = Good:new{ ref_name = "zinc" }
zinc.name = _("Zinc")
zinc:register()
copper = Good:new{ ref_name = "copper" }
copper.name = _("Copper")
copper:register()
chromium = Good:new{ ref_name = "chromium" }
chromium.name = _("Chromium")
chromium:register()
brass = Good:new{ ref_name = "brass" }
brass.name = _("Brass")
brass:register()
sulfur = Good:new{ ref_name = "sulfur" }
sulfur.name = _("Sulfur")
sulfur:register()
stainless_steel = Good:new{ ref_name = "stainless_steel" }
stainless_steel.name = _("Stainless steel")
stainless_steel:register()

-- Food
bread = Good:new{ ref_name = "bread" }
bread.name = _("Bread")
bread.is_edible = true
bread:register()
salt = Good:new{ ref_name = "salt" }
salt.name = _("Salt")
salt.is_edible = true
salt:register()
alcohol = Good:new{ ref_name = "alcohol" }
alcohol.name = _("Alcohol")
alcohol.is_edible = true
alcohol:register()

-- Commodities
window = Good:new{ ref_name = "window" }
window.name = _("Window")
window:register()
wine = Good:new{ ref_name = "wine" }
wine.name = _("Wine")
wine:register()
gasoline = Good:new{ ref_name = "gasoline" }
gasoline.name = _("Gasoline")
gasoline:register()
chair = Good:new{ ref_name = "chair" }
chair.name = _("Chair")
chair:register()
bed = Good:new{ ref_name = "bed" }
bed.name = _("Bed")
bed:register()
opium = Good:new{ ref_name = "opium" }
opium.name = _("Opium")
opium:register()
spices = Good:new{ ref_name = "spices" }
spices.name = _("Spices")
spices:register()
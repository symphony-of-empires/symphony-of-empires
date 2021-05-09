require("data/scripts/api")

local ns = Province:get({}, "amur")
local jap = Nation:get({}, "japan")
Province:give_to(ns, jap)

function ec_test()
    print("conditions!")
    return 0
end

function ef_test()
    print("checking!")
end

local japan_dies = Event:create{ ref_name = "japan_dies", conditions_fn = "ec_test", event_fn = "ef_test" }
Event:register(japan_dies)

print("loaded mod.lua")
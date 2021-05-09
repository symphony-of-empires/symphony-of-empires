require("data/scripts/api")

local ns = Province:get({}, "amur")
local jap = Nation:get({}, "japan")
Province:give_to(ns, jap)

print("loaded mod.lua")
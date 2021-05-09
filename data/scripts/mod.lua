require("data/scripts/api")

function ec_test()
	local year = get_year()
	local month = get_month()
	local day = get_day()

	if(year == 4 and month == 3 and day == 25) then
		return 1
	else
		print("japan survives... for now")
		return 0
	end
end

function ef_test()
	print("japan dies!")
	local russia = Nation:get({}, "russia")
	Province:give_to(Province:get({}, "tokyo"), russia)
	Province:give_to(Province:get({}, "niigata"), russia)
	Province:give_to(Province:get({}, "kyoto"), russia)
	Province:give_to(Province:get({}, "iwate"), russia)
	Province:give_to(Province:get({}, "east_north_korea"), russia)
	Province:give_to(Province:get({}, "west_north_korea"), russia)
	Province:give_to(Province:get({}, "east_south_korea"), russia)
	Province:give_to(Province:get({}, "west_south_korea"), russia)
	Province:give_to(Province:get({}, "souther_korea"), russia)
	Province:give_to(Province:get({}, "nagasaki"), russia)
	Province:give_to(Province:get({}, "central_sakhlin"), russia)
	Province:give_to(Province:get({}, "south_sakhlin"), russia)
	Province:give_to(Province:get({}, "paramushir"), russia)
	Province:give_to(Province:get({}, "raycoke"), russia)
	Province:give_to(Province:get({}, "iturup"), russia)
	Province:give_to(Province:get({}, "hokkaido"), russia)
	return 0
end

local japan_dies = Event:create{ ref_name = "japan_dies", conditions_fn = "ec_test", event_fn = "ef_test" }
Event:register(japan_dies)

print("loaded mod.lua")
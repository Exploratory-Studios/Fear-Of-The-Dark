local QuestFunctions = {} -- Make a global table.

function tobool(string)
	bool = false

	if(string == "true" or string == "1") then
		bool = true
	end

	return bool
end

function QuestFunctions.killEntity(entityID)
	while(waitForEvent("entityKilled")["ID"] ~= entityID) do
		-- Just watch each event. If one matches the ID, then we can exit.
	end
	return true
end

function QuestFunctions.killSpecificEntity(entityUUID)
	while(waitForEvent("entityKilled")["UUID"] ~= entityUUID) do
		-- Just watch each event. If one matches the UUID, then we can exit.
	end
	return true
end

function QuestFunctions.gather(quantity, itemID)
	gathered = false
	while(not gathered) do
		waitForEvent("itemGathered" .. itemID) -- Yields until something is picked up.
		if(inventoryContains(getPlayer(), itemID) >= quantity) then
			gathered = true
		end
	end
	return true
end

function QuestFunctions.protectEntityTime(entityUUID, time)
	while(time > 0) do
		waitForEvent("update")
		time = time - 1
		if(eventExists("entityKilled")["UUID"] == entityUUID) then -- TODO: Must create eventExists, which just yields and checks once (like waitForEvent with a 1 update timeout) (Y'know, maybe I should add a timeout to the waitForEvent)
			return false
		end
	end
	
	return true
end

function QuestFunctions.defendBlock(blockX, blockY, blockLayer)
	-- Similar to protectEntity, we need a timeout.
end

function QuestFunctions.activateFlag(flagID, flagVal)
	completed = (getFlag(flagID) == flagVal) -- make sure its not already set

	while(not completed) do
		local args = {}
		args = waitForEvent("setFlag")

		if(tonumber(args["flagID"]) == flagID) then
			if(tobool(args["flagValue"]) == flagVal) then
				completed = true
			end
		end
	end

	return true
end

--function QuestFunctions.deliver

function QuestFunctions.discover(structureID)
	while(waitForEvent("discoveredStructure")["ID"] ~= structureID) do

	end

	return true
end

function QuestFunctions.destroy(blockX, blockY, blockLayer)
	completed = false

	while(not completed) do
		args = waitForEvent("destroyedBlock")
		if(args["blockLayer"] == blockLayer) then
			if(args["blockX"] == blockX) then
				if(args["blockY"] == blockY) then
					completed = true
				end
			end
		end
	end

	return true
end

return QuestFunctions

local args = ...

-- id 11 is water, 0 is air (obviously)
local down = getBlockData(args["blockX"], args["blockY"]-1, 0)
--if(down["metadata"]["waterLevel"] == nil) then
--	print(": " .. down["blockX"] .. ", " .. down["blockY"])
--end
if(down["blockID"] == 11) then
	local level = down["metadata"]["waterLevel"]
	if(level < 1.0) then
		local maxTaken = args["metadata"]["waterLevel"]
		
		local taken = math.min(maxTaken, 1.0-level)

		args["metadata"]["waterLevel"] = maxTaken - taken
		down["metadata"]["waterLevel"] = level + taken

		setBlockMetaData(down["blockX"], down["blockY"], down["blockLayer"], down["metadata"])
		setBlockSize(down["blockX"], down["blockY"], down["blockLayer"], 1.0, down["metadata"]["waterLevel"])
	end
end

setBlockMetaData(args["blockX"], args["blockY"], args["blockLayer"], args["metadata"])
setBlockSize(args["blockX"], args["blockY"], args["blockLayer"], 1.0, args["metadata"]["waterLevel"])

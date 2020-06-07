local args = ...

itemID_blockID = {
	[1] = 1, -- dirt
	[2] = 6, -- torch
	[3] = 7, -- bright torch
	[4] = 0, -- anti-torch (unimplemented)
	[5] = 2, -- grass
	[6] = 10, -- wood
	[7] = 12, -- wood pole
	[8] = 13 -- wood sign
}

id = itemID_blockID[args["itemID"]]

print(id)

setBlock(id, args["blockX"], args["blockY"], 0, "")

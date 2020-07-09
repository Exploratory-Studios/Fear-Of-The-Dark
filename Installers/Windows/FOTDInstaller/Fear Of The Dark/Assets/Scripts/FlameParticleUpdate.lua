local args = ...

args["particleYVel"] = 0.02
args["particleXVel"] = args["particleXVel"] / 1.5
args["particleAlpha"] = args["particleLife"] * 255

args["particleY"] = args["particleY"] + args["particleYVel"]
args["particleX"] = args["particleX"] + args["particleXVel"]

return args

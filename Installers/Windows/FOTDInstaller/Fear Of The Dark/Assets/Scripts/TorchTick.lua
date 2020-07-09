local args = ...

x = args["blockX"] + .35
y = args["blockY"] + .75

xVel = (math.random()-0.5) / 8

createParticle(1, x, y, xVel, 0.1, .2)

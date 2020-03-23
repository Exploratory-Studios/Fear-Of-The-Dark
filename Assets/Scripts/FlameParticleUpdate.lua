particleYVel = 0.02
particleXVel = particleXVel / 1.5
particleAlpha = particleLife * 255

particleY = particleY + particleYVel
particleX = particleX + particleXVel

return particleX, particleY, particleXVel, particleYVel, particleLife, particleWidth, particleAlpha

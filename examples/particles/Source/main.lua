
local font = playdate.graphics.font.new('font/namco-1x')

local gfx = playdate.graphics


local particleCount = 20
local particles = particlelib.particles.new(particleCount)

local testRect = playdate.geometry.rect.new(170, 90, 60, 60)

local showInstructions = true

function playdate.update()

	particles:update()
	particles:draw()

	gfx.setColor(gfx.kColorWhite)
	gfx.drawRect(testRect)

	gfx.setFont(font)
	gfx.setFontTracking(-1)
	gfx.drawText("count: "..particleCount, 4, 2)

	local boxcount = particles:particleCountInRect(testRect.x, testRect.y, testRect.width, testRect.height)
	gfx.drawText("  box: "..boxcount, 4, 12)

	if showInstructions then
		gfx.drawText("Press UP to add more snowflakes, DOWN to remove", 10, 60)
	end

	gfx.setImageDrawMode(playdate.graphics.kDrawModeCopy)
	playdate.drawFPS(2, 224)

end


function playdate.upButtonDown()

	particleCount += 40
	particles:setNumberOfParticles(particleCount)
	showInstructions = false

end


function playdate.downButtonDown()

	particleCount -= 40
	if particleCount < 20 then particleCount = 20 end
	particles:setNumberOfParticles(particleCount)
	showInstructions = false

end

//
//  main.c
//  Extension
//
//  Original Created by Dan Messing on 5/01/18.
//  Refactored into a C++ example by MrBZapp on 9/16/23
//

#include <stdlib.h>
#include <cmath>
#include <vector>
#include <memory>

#include <pd_api.h>
#include <pdcpp/pdnewlib.h>

// First, give the library that will be included a name
constexpr char* PARTICLE_CLASS_NAME = "particlelib.particles";

// These static elements will be initialized in the eventHandler's `InitLua` event
static PlaydateAPI* pd = nullptr;
static LCDBitmap* flakes[4];

/*
* Each individual particle will draw a snowflake randomly selected from the
* array of flakes above
*/
class Particle
{
public:
    Particle()
        : x((rand() % 441) - 20.0f)
        , y((rand() % 241) - 240.0f)
        , w(19)
        , h(21)
        , speed(rand() % 4 + 1)
        , type(rand() % 4)
        , drift((float)((rand() % 5) - 2.0) / 10.0f)
    {}

    // Move the snowflake according to its movement properties
    void update()
    {
        drift += (float)(((rand() % 5) - 2.0f) / 10.0f);
        y += speed;
        x += drift;

        if (y > 240)	// if the particle is off the screen, move it back to the top
        {
            y = -22;
            x = (rand() % 441) - 20;
            drift = (float)((rand() % 5) - 2.0f) / 10.0f;
        }
    }

    // Paint the specified snowflake at its location
    void draw()
    {
        pd->graphics->setDrawMode(kDrawModeInverted);
        pd->graphics->drawBitmap(flakes[type], round(x), y, kBitmapUnflipped);
    }

    // Determine if the snowflake is inside a given box
    bool isInBounds(int boxX, int boxY, int boxW, int boxH)
    {
        return !(y >= boxY + boxH || y + h <= boxY || x >= boxX + boxW || x + w <= boxX);
    }

private:
    float x, drift;
    int y, w, h, speed, type;
};

/**
 * This class is essentially a helper wrapper around a std::vector. Each could
 * theoretically be done in the C functions passed to Lua as well, but this
 * class also helps demonstrate generally how one might consolidate logic,
 * isolate lua and the playdate API, and/or use std::unique_ptr for memory
 * management.
 */
class ParticlesLuaManager
{
public:
    explicit ParticlesLuaManager(int count)
        { setNumParticles(count); }

    void setNumParticles(int count) {m_Particles.resize(count); }
    void update() { for(auto& p : m_Particles) { p.update(); } }
    void draw() { for(auto& p : m_Particles) { p.draw(); } }

    int countInBounds(int x, int y, int w, int h)
    {
        int rv = 0;
        for(auto& p : m_Particles)
        {
            if (p.isInBounds(x, y, w, h))
                { rv++; }
        }
        return rv;
    }

private:
    std::vector<Particle> m_Particles;
};

// It's likely impossible, but using a std::unique_ptr here this ensures that
// if `newobject` is called unevenly with dealloc that we can't leak anything
std::unique_ptr<ParticlesLuaManager> particles;

// The remaining functions here are all bridge functions between things we're
// allowed to pass to lua and the C++ objects actually responsible for the
// game's operation.

static int particlelib_dealloc(lua_State* L)
{
    (void)L;  // Ignore me!
    particles.reset(nullptr);
    return 0;
}


static int particlelib_newobject(lua_State* L)
{
    (void)L; // Ignore me!
    int count = pd->lua->getArgInt(1);
    particles = std::make_unique<ParticlesLuaManager>(count);
    pd->lua->pushObject(particles.get(), PARTICLE_CLASS_NAME, 0);
    return 1;
}


static int particlelib_setNumberOfParticles(lua_State* L)
{
    (void)L;  // Ignore me!
    int count = pd->lua->getArgInt(2);
    particles->setNumParticles(count);
    return 0;
}


static int particlelib_particleCountInRect(lua_State* L)
{
    (void)L;  // Ignore me!

    int x = pd->lua->getArgInt(2);
    int y = pd->lua->getArgInt(3);
    int w = pd->lua->getArgInt(4);
    int h = pd->lua->getArgInt(5);
    //	int type = pd->lua->getArgInt(6);	// could also filter for type here if it was piped down

    auto result = particles->countInBounds(x, y, w, h);
    pd->lua->pushInt(result);
    return 1;
}


static int particlelib_update(lua_State* L)
{
    (void)L;
    particles->update();
    return 0;
}


static int particlelib_draw(lua_State* L)
{
    (void)L;
    pd->graphics->clear(kColorBlack);
    particles->draw();
    return 0;
}

static const lua_reg particlesLib[] =
{
    { "__gc",					particlelib_dealloc },
    { "new", 					particlelib_newobject },
    { "setNumberOfParticles", 	particlelib_setNumberOfParticles },
    { "particleCountInRect", 	particlelib_particleCountInRect },
    { "update",					particlelib_update },
    { "draw", 					particlelib_draw },
    { nullptr, nullptr }
};

/**
 * the `eventHandler` function is the entry point for all Playdate applications
 * and Lua extensions. It requires C-style linkage (no name mangling) so we
 * must wrap the entire thing in this `extern "C" {` block
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	(void)arg;
    eventHandler_pdnewlib(pd, event, arg);
    if (event == kEventInit)
    {

    }

	if (event == kEventInitLua)
	{
		pd = playdate;

		const char* err;

		if ( !pd->lua->registerClass(PARTICLE_CLASS_NAME, particlesLib, nullptr, 0, &err) )
			pd->system->logToConsole("%s:%i: registerClass failed, %s", __FILE__, __LINE__, err);

		srand(pd->system->getSecondsSinceEpoch(nullptr));

		// load particle images
		const char *outErr = nullptr;
		const char *path1 = "images/snowflake1";
		const char *path2 = "images/snowflake2";
		const char *path3 = "images/snowflake3";
		const char *path4 = "images/snowflake4";
		flakes[0] = pd->graphics->loadBitmap(path1, &outErr);
		flakes[1] = pd->graphics->loadBitmap(path2, &outErr);
		flakes[2] = pd->graphics->loadBitmap(path3, &outErr);
		flakes[3] = pd->graphics->loadBitmap(path4, &outErr);
	}

	return 0;
}

#ifdef __cplusplus
}
#endif

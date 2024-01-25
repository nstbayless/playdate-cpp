/**
 * Hello World CPP
 *
 * Reproduces the Hello World Example from the Playdate C SDK, but with C++
 * instead. A full game can be built from similar principles.
 *
 * MrBZapp 9/2023
 */
#include <memory>
#include <string>
#include <pdcpp/pdnewlib.h>


constexpr int TEXT_WIDTH = 86;
constexpr int TEXT_HEIGHT = 16;

/**
 * This Class contains the entire logic of the "game"
 */
class HelloWorld
{
public:
    explicit HelloWorld(PlaydateAPI* api)
        : pd(api)
        , fontpath("/System/Fonts/Asheville-Sans-14-Bold.pft")
        , x((400 - TEXT_WIDTH) / 2)
        , y((240 - TEXT_HEIGHT) / 2)
        , dx(1) , dy(2)
    {
        const char* err;
        font = pd->graphics->loadFont(fontpath.c_str(), &err);

        if (font == nullptr)
            pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath.c_str(), err);
    }

    void update()
    {
        pd->graphics->clear(kColorWhite);
        pd->graphics->setFont(font);
        pd->graphics->drawText("Hello C++!", strlen("Hello World!"), kASCIIEncoding, x, y);

        x += dx;
        y += dy;

        if ( x < 0 || x > LCD_COLUMNS - TEXT_WIDTH )
        {
            dx = -dx;
        }

        if ( y < 0 || y > LCD_ROWS - TEXT_HEIGHT )
        {
            dy = -dy;
        }

        pd->system->drawFPS(0,0);
    }

private:
    PlaydateAPI* pd;
    std::string fontpath;
    LCDFont* font;
    int x, y, dx, dy;
};

/**
 * You can use STL containers! Be careful with some stdlib objects which rely
 * on an OS though, those will cause your app to crash on launch.
 */
std::unique_ptr<HelloWorld> helloWorld;


/**
 * The Playdate API requires a C-style, or static function to be called as the
 * main update function. Here we use such a function to delegate execution to
 * our class.
 */
static int gameTick(void* userdata)
{
    helloWorld->update();
    return 1;
};


/**
 * the `eventHandler` function is the entry point for all Playdate applications
 * and Lua extensions. It requires C-style linkage (no name mangling) so we
 * must wrap the entire thing in this `extern "C" {` block
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the main event handler. Using the `Init` and `Terminate` events, we
 * allocate and free the `HelloWorld` handler accordingly.
 *
 * You only need this `_WINDLL` block if you want to run this on a simulator on
 * a windows machine, but for the sake of broad compatibility, we'll leave it
 * here.
 */
#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
    /*
     * This is required, otherwise linker errors abound
     */
    eventHandler_pdnewlib(pd, event, arg);

    // Initialization just creates our "game" object
    if (event == kEventInit)
    {
        pd->display->setRefreshRate(20);
        helloWorld = std::make_unique<HelloWorld>(pd);

        // and sets the tick function to be called on update to turn off the
        // typical 'Lua'-ness.
        pd->system->setUpdateCallback(gameTick, pd);
    }

    // Destroy the global state to prevent memory leaks
    if (event == kEventTerminate)
    {
        pd->system->logToConsole("Tearing down...");
        helloWorld = nullptr;
    }
    return 0;
}
#ifdef __cplusplus
}
#endif

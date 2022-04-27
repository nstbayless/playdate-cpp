#include <cstdlib>
#include "pdnewlib.h"
#include "stdio.h"

extern "C"
{
#include "pd_api.h"
}

volatile int order = 1;
int next2(void);
int next(void) {return order++;}


namespace
{
    int O_ST = order++;
    int O_STFN = next();
    int O_LINK = next2();
    int O_INITSEC = 0;
    int O_CONSTRUC = 0;
    int O_STATICCON = 0;
    int O_MAIN = 0;
}

__attribute__((constructor))
void T2()
{
    O_CONSTRUC = next();
}

struct A {
    A()
    {
        O_STATICCON = next();
    }
} _;

int _main();

int* a = (int*)malloc(sizeof(int));
int* b = new int;

extern void throw_exception(int);
int excepttest()
{
    throw_exception(1);
    return 2;
}

extern "C"
int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
    eventHandler_pdnewlib(playdate, event, arg);
    
    if (event == kEventInit)
    {
        O_MAIN = next2();
    }
    
    if (event == kEventInitLua)
    {
        #define print playdate->system->logToConsole
        #define printO(X) print(#X ": %d\n", X)
        print("LUA_INIT\n");
        printO(O_ST);
        printO(O_STFN);
        printO(O_LINK);
        printO(O_INITSEC);
        printO(O_CONSTRUC);
        printO(O_STATICCON);
        printO(O_MAIN);
        
        printf("printf test\n");
        
        #if TARGET_PLAYDATE
        print("exception test: %d", excepttest());
        #endif
        
        print("DONE\n");
    }
    
    return 0;
}
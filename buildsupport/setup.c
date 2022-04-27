
#include "pd_api.h"

typedef int (PDEventHandler)(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg);

extern PDEventHandler eventHandler;

static void* (*pdrealloc)(void* ptr, size_t size);

#if TARGET_PLAYDATE

typedef const void(*init_routine_t)(void);
extern init_routine_t __preinit_array_start, __preinit_array_end, __init_array_start, __init_array_end, __fini_array_start, __fini_array_end;
static PlaydateAPI* pd;

static void exec_array(init_routine_t* start, init_routine_t* end)
{
    while (start < end)
    {
        if (*start) (*start)();
        ++start;
    }
}

int eventHandlerShim(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if ( event == kEventInit )
	{
		pd = playdate;
		pdrealloc = playdate->system->realloc;
		exec_array(&__preinit_array_start, &__preinit_array_end);
		exec_array(&__init_array_start, &__init_array_end);
	}
	
	if (event == kEventTerminate)
	{
		int result = eventHandler(playdate, event, arg);
		exec_array(&__fini_array_start, &__fini_array_end);
		return result;
	}
	
	return eventHandler(playdate, event, arg);
}

// standard library functions
void* _malloc_r(struct _reent* _REENT, size_t nbytes) { return pdrealloc(NULL,nbytes); }
void* _realloc_r(struct _reent* _REENT, void* ptr, size_t nbytes) { return pdrealloc(ptr,nbytes); }
void _free_r(struct _reent* _REENT, void* ptr ) { pdrealloc(ptr,0); }

PDEventHandler* PD_eventHandler __attribute__((section(".capi_handler"))) = &eventHandlerShim;

extern uint32_t bssStart asm("__bss_start__");
uint32_t* _bss_start __attribute__((section(".bss_start"))) = &bssStart;

extern uint32_t bssEnd asm("__bss_end__");
uint32_t* _bss_end __attribute__((section(".bss_end"))) = &bssEnd;

#else

int eventHandlerShim(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if ( event == kEventInit )
		pdrealloc = playdate->system->realloc;
	
	return eventHandler(playdate, event, arg);
}

void* malloc(size_t nbytes) { return pdrealloc(NULL,nbytes); }
void* realloc(void* ptr, size_t nbytes) { return pdrealloc(ptr,nbytes); }
void  free(void* ptr ) { pdrealloc(ptr,0); }

#endif

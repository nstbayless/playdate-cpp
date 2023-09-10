//
// Created by MrBZapp on 9/9/2023.
//
#ifdef TARGET_PLAYDATE
#include <cstdlib>
void operator delete(void* obj, unsigned int size) { free(obj); }
void* operator new(unsigned int size) { return malloc(size); }
#endif

#ifndef __STDPD_H
#define __STDPD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pd_api.h"
int eventHandler_pdnewlib(PlaydateAPI*, PDSystemEvent event, uint32_t arg);

#ifdef __cplusplus
}
#endif

#endif
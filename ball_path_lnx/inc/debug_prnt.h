/* debug_prnt.h - header for debug printing module
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a global user defined macroses and function 
 * prototypes for the debugging.
 * */

/*
* INCLUDES
* */
#ifndef _DEBUG_PRNT
#define _DEBUG_PRNT

#include <stdio.h>
#include "ball_path.h"

/*
* DEFINES
* */
#if DEBUG_APP
#define APP_PRNT(level, args, ...) \
            printf ("%s:%d[%s()]" args "\n", \
            __FILE__,\
            __LINE__,\
            __FUNCTION__,\
            ##__VA_ARGS__)
#endif

#if !defined DEBUG_APP
#define APP_PRNT(args, ...) printf ("--->")
#endif

#ifdef DEBUG_APP
    #define DEBUG_PRNT(args, ...) APP_PRNT ("! ", args, ##__VA_ARGS__)
#else
    #define DEBUG_PRNT(args, ...)
#endif

/*
 * GLOBAL ROUTINES PROTOTYPES
 * */

/*
* name: print_bdmap_dbg - prints the debug info about the data map file
*                         and the modified map itself
* in: DMAP_INFO *
* out: 
* ret: void
* */
void print_bdmap_dbg (DMAP_INFO *dmap_info);/* print balls distriburion map 
                                               - debug routine */

/*
* name: print_dmap_prps_dbg - prints the debug info about the data map properties
* in: DMAP_PRPS *
* out: 
* ret: void
* */
void print_dmap_prps_dbg (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info, long x, long y);
#endif

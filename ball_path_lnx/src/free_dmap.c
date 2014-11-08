/* free_dmap.cpp - module for the free dmap routine
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains the realisation of the freeing the memory of dmap.
 * */

/*
* INCLUDES
* */
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "ball_path.h"
#include "debug_prnt.h"


/*
 * GLOBAL ROUTINES IMPLEMENTATIONS
 * */

/* name: free_dmap - free data map file memory
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: STATUS
 * */
STATUS free_dmap (DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    long cnt = 0;

    /* releasing the memory of dmap */
    if (NULL != dmap_info->dmap)
    {
        for (cnt = 0; cnt < dmap_info->row_num; cnt ++)
        {
            if (NULL != dmap_info->dmap[cnt])
            {
                free (dmap_info->dmap[cnt]);
            }
            else
            {
                DEBUG_PRNT ("cannot free data map memory - columns are NULL");
                status_ret = FAIL;
            }
        }
        free (dmap_info->dmap);
    }
    else
    {
        DEBUG_PRNT ("cannot free data map memory - rows are NULL");
        status_ret = FAIL;
    }

    return status_ret;
}

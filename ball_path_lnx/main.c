/* ball_path.cpp - main module of the ball_path test task
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a main() entry function.
 * */

/*
* INCLUDES
* */
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "proc_dmap_file.h"
#include "find_ball_path.h"
#include "free_dmap.h"
#include "debug_prnt.h"

/*
* GLOBAL DATA
* */

/*
 * GLOBAL ROUTINES IMPLEMENTATIONS
 * */

/*
* name: print_bdmap_dbg - prints the debug info about the data map file
*                         and the modified map itself
* in: DMAP_INFO *
* out: 
* ret: void
* */
void print_bdmap_dbg (DMAP_INFO *dmap_info)
{
    static int print_data = 0;
    static int print_map = 0;
    long row_cnt = 0;
    long col_cnt = 0;


    #ifdef DEBUG_DMAP_DATA
    /* print data only once */
    if (1 != print_data)
    {
        print_data = 1;

        printf ("\nsize: y=%ld (rows) and x=%ld (columns)\n"
                "coordinates: (x=%ld,y=%ld) [start point] and (x=%ld,y=%ld) [destination point]\n",
                dmap_info->row_num, dmap_info->col_num,
                dmap_info->initial_point.x, dmap_info->initial_point.y,
                dmap_info->final_point.x, dmap_info->final_point.y);

        /* print dmap */
        printf ("\nthe map is:\n");
    }
    #endif

    #ifndef DEBUG_DMAP
    /* print map any time */
    if (1 != print_map)
    #endif
    {
        print_map = 1;

        printf ("\n\n");
        for (row_cnt = 0; row_cnt < dmap_info->row_num; row_cnt ++)
        {
            for (col_cnt = 0; col_cnt < dmap_info->col_num; col_cnt ++)
            {
                printf ("%c", dmap_info->dmap[row_cnt][col_cnt]);
            }
            printf ("\n");
        }
        printf ("\n\n");
    }

    return;
}

/* name: print_bdmap_dbg - prints the debug info about the data map properties
 * in: DMAP_PRPS *
 * in: DMAP_INFO *
 * out: 
 * ret: void
 * */
void print_dmap_prps_dbg (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info, long x, long y)
{
    static int print_data = 0;

    long row_cnt = 0;
    long col_cnt = 0;


    #ifdef DEBUG_DMAP_PRPS
    if (1 != print_data)
    {
        print_data = 1;

        printf ("\nWALL value: %ld\n"
            "PATH value: %ld\n"
            "STRT value: %ld\n"
            "DSTN value: %ld\n",
            WALL_VALUE(dmap_info->col_num, dmap_info->row_num),
            PATH_VALUE(dmap_info->col_num, dmap_info->row_num),
            STRT_VALUE(dmap_info->col_num, dmap_info->row_num),
            DSTN_VALUE(dmap_info->col_num, dmap_info->row_num));

        printf ("\nthe map is:\n");
    }

    printf ("\nthe current X=%ld Y=%ld, and the point has value %ld\n", 
        x, dmap_info->row_num - y - 1,
        dmap_prps->xy_prps[/*dmap_info->row_num - y - 1*/y][x]);

    /* print properties map any time */
    for (row_cnt = 0; row_cnt < dmap_info->row_num; row_cnt ++)
    {
        for (col_cnt = 0; col_cnt < dmap_info->col_num; col_cnt ++)
        {
            printf ("%4ld", dmap_prps->xy_prps[row_cnt][col_cnt]);
        }
        printf ("\n");
    }
    printf ("\n\n");

    #endif /* #ifdef DEBUG_PRNT */

    return;
}




/*
* name: main - main function entry
* in: int
* in: char **
* out:
* ret: STATUS
* */
int main (int argc, char **argv)
{
    STATUS status_ret = OK;
    STATUS_FIND status_find = TRUE;
    DMAP_INFO dmap_info;
    int cnt = 0;


    memset (&dmap_info, 0x00, sizeof (DMAP_INFO));

    /* proceeding data map file */
    DEBUG_PRNT ("proceeding the data map file %s", argv[1]);
    if (OK != proc_dmap_file (argc, argv, &dmap_info))
    {
        DEBUG_PRNT ("cannot proceed data map file %s", argv[1]);
        printf ("cannot proceed data map file %s\n", argv[0]);
        status_ret = FAIL;
    }
    else
    {
        #ifdef DEBUG_DMAP
        print_bdmap_dbg (&dmap_info);
        #endif
    }
    
    /* searching for the shortest path */
    DEBUG_PRNT ("searching for the shortest ball path on the map");
    if (FAIL != status_ret)
    {
        if (TRUE != find_ball_path (&dmap_info))
        {
            DEBUG_PRNT ("cannot find ball path");
            status_find = FALSE;
            //status_ret = FAIL;
        }
        else
        {
            #ifdef DEBUG_DMAP
            print_bdmap_dbg (&dmap_info);
            #endif
        }
    }

    /* freeing dmap memory */
    DEBUG_PRNT ("freeing previously allocated memory for the balls distribution map");
    if (FAIL != status_ret)
    {
        if (OK != free_dmap (&dmap_info))
        {
            DEBUG_PRNT ("cannot free previously allocated balls distribution map");
            status_ret = FAIL;
        }
    }

    return status_ret;
}

/* find_ball_path.c - ball path search module
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a global user-defined data.
 * */

/*
* INCLUDES
* */
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "../inc/find_ball_path.h"
#include "../inc/debug_prnt.h"

/*
 * GLOBAL DATA
 * */
//static char tmp_chr = 0;
static char tmp_chr = START_CELL_MOD;
static char *way_symbls = NULL;
static long step_cnt = 0;

/*
 * LOCAL ROUTINES PROTOTYPES
 * */
static void _mark_strt (DMAP_INFO *dmap_info);
static void _mark_dstn (DMAP_INFO *dmap_info);
static void _mark_cells (DMAP_INFO *dmap_info);
static STATUS_FIND _path_fnder (DMAP_INFO *dmap_info, long x_strt, long y_strt);/* main realisation of the search algorithm */
static STATUS _path_fnd_dir (long x_strt, long y_strt);


static STATUS_FIND _path_fnder_alt (DMAP_INFO *dmap_info, DMAP_PRPS *dmap_prps/*, static long *ni, long nk*/);/* alternative realisation of the algorithm */
static STATUS _path_fnder_alt_init (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info);/* initialize data for the alternative search algorithm */
static STATUS _dmap_prps_creat (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info);/* create data map properties array of <y = number of rows> * <x = number of columns> size */
static STATUS _dmap_prps_dstry (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info);/* destroy the data map properties array of <y = number of rows> * <x = number of columns> size */
static STATUS _dmap_prnt_path_seq (DMAP_COOR_SEQ *dmap_coor_seq/*, long n_steps*/);/* prints the data about path and coordinates sequence */
static STATUS _dmap_cnvrt_path (DMAP_INFO *dmap_info, DMAP_COOR_SEQ *dmap_coor_seq/*, long n_steps*/);/*  */

/* name: find_ball_path - searching for the shortest path either with DFS or BFS
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: STATUS_FIND
 * */
STATUS_FIND find_ball_path (DMAP_INFO *dmap_info)
{
    STATUS_FIND find_status = TRUE;
    STATUS status_ret = OK;

    DMAP_PRPS dmap_prps;
    memset (&dmap_prps, 0x00, sizeof (DMAP_PRPS));



    /* marking all empty cells with dots '.' */
    _mark_cells (dmap_info);
    /* marking the starting point with 's' */
    _mark_strt (dmap_info);
    /* marking the destination point with 'd' */
    _mark_dstn (dmap_info);
    
    #ifdef DEBUG_DMAP
    /* prints debug info */
    print_bdmap_dbg (dmap_info);
    #endif


    #ifndef DEBUG_ALT

    if (NULL != (way_symbls = (char *)malloc (MAX_ITERS(dmap_info->col_num, dmap_info->row_num) * sizeof (char))))
    {
        memset (way_symbls, 0x00, MAX_ITERS(dmap_info->col_num, dmap_info->row_num) * sizeof (char));
    }
    else
    {
        printf ("cannot allocate memory for the symbols of the way\n");
        return FALSE;
    }
    
    /* DFS algorithm */
    find_status = _path_fnder (dmap_info,
                               dmap_info->initial_point.x,
                               dmap_info->initial_point.y);
    
    //if (FALSE == find_status)
    {
        _mark_strt (dmap_info);
    }

    print_bdmap_dbg (dmap_info);
    
    if (FALSE != find_status)
    {
        printf ("the way is: ");
        for (int cnt = step_cnt; cnt >0; cnt --)
        {
            printf ("%c ", way_symbls[cnt - 1]);
        }
        printf ("\n\n");
        printf ("the number of steps in the path is: %ld\n\n", step_cnt);
    }

    free (way_symbls);

    #else

    /* BFS algorithm */
    (void)_dmap_prps_creat (&dmap_prps, dmap_info);
    (void)_path_fnder_alt_init (&dmap_prps, dmap_info);
    
    /* convert the 'Y' coordinate */
    print_dmap_prps_dbg (&dmap_prps, dmap_info, dmap_info->initial_point.x,
        /*dmap_info->initial_point.y*/dmap_info->row_num - dmap_info->initial_point.y - 1);

    (void)_path_fnder_alt (dmap_info, &dmap_prps);

    (void)_dmap_prps_dstry (&dmap_prps, dmap_info);

    #endif

    if (FALSE != find_status)
    {
        //printf ("\nSUCCESS!\n");
    }
    else
    {
        //printf ("\nFAIL!\n");
        printf ("there is no path or destination busy\n\n");
    }

    return find_status;
}

/* name: _mark_strt - marks start point with a giving symbol
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: void
 * */
static void _mark_strt (DMAP_INFO *dmap_info)
{
    long row_convert = \
        dmap_info->row_num - dmap_info->initial_point.y - 1;
    long col_convert = \
        dmap_info->initial_point.x;
    
    dmap_info->dmap[row_convert][col_convert] = START_CELL_MOD;
    
    return;
}

/* name: _mark_dstn - marks destination point with a giving symbol
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: void
 * */
static void _mark_dstn (DMAP_INFO *dmap_info)
{
    long row_convert = \
        dmap_info->row_num - dmap_info->final_point.y - 1;
    long col_convert = \
        dmap_info->final_point.x;

    dmap_info->dmap[row_convert][col_convert] = DESTN_CELL_MOD;

    return;
}

/* name: _mark_cells - marks all empty cells as a path points with a giving symbol
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: void
 * */
static void _mark_cells (DMAP_INFO *dmap_info)
{
    long row_cnt = 0;
    long col_cnt = 0;

    for (row_cnt = 0; row_cnt < dmap_info->row_num; row_cnt ++)
    {
        for (col_cnt = 0; col_cnt < dmap_info->col_num; col_cnt ++)
        {
            switch (dmap_info->dmap[row_cnt][col_cnt])
            {
                case EMP_CELL:
                    dmap_info->dmap[row_cnt][col_cnt] = EMP_CELL_MOD;
                    break;

                case BSY_CELL:
                    dmap_info->dmap[row_cnt][col_cnt] = BSY_CELL_MOD;
                    break;
            }
        }
    }

    return;
}

/*
 * LOCAL ROUTINES IMPLEMENTATIONS
 * */

/*****************************************************/
/* DFS algorithm implementations - Deep-first search */
/*****************************************************/

/* name: _path_fnder - finds a path in a maze using Deep-first search
 * in: long
 * in: long
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: STATUS_FIND
 * */
static STATUS_FIND _path_fnder (DMAP_INFO *dmap_info, long x_strt, long y_strt)
{
    /* checking our boundaries */
    if ((x_strt < 0) || (x_strt > dmap_info->col_num - 1) || \
        (y_strt < 0) || (y_strt > dmap_info->row_num - 1))
    {
        return FALSE;
    }

    /* checking the destination first - if the distination point equal start point */
    if (dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] == DESTN_CELL_MOD)
    {
        dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] = FNL_CELL;
        return TRUE;
    }

    /* checking the destination second - if the dstination point is busy */
    if ((dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] != EMP_CELL_MOD) \
        && (dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] != START_CELL_MOD))
    {
        return FALSE;
    }


    /* marking our path with plus '+' */
    //dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] = /*'+'*/PATH_SYM;
    dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] = tmp_chr;

    #ifdef DEBUG_DMAP_STEPS
    /* just for debug */
    print_bdmap_dbg (dmap_info);
    #endif /* #ifdef DEBUG_DMAP_STEPS */
    
    tmp_chr = UP_DIRECT_SYM;
    /* UP: checking if we are moving UP */
    if (_path_fnder (dmap_info, x_strt, y_strt + 1) == TRUE)
    {
        way_symbls [step_cnt] = UP_DIRECT_SYM;
        step_cnt ++;

        return TRUE;
    }

    tmp_chr = RT_DIRECT_SYM;
    /* RIGHT: checking if we are moving RIGHT */
    if (_path_fnder (dmap_info, x_strt + 1, y_strt) == TRUE)
    {
        way_symbls [step_cnt] = RT_DIRECT_SYM;
        step_cnt ++;

        return TRUE;
    }

    tmp_chr = DW_DIRECT_SYM;
    /* DOWN: checking if we are moving DOWN */
    if (_path_fnder (dmap_info, x_strt, y_strt - 1) == TRUE)
    {
        way_symbls [step_cnt] = DW_DIRECT_SYM;
        step_cnt ++;

        return TRUE;
    }

    tmp_chr = LF_DIRECT_SYM;
    /* LEFT: checking if we are moving LEFT */
    if (_path_fnder (dmap_info, x_strt - 1, y_strt) == TRUE)
    {
        way_symbls [step_cnt] = LF_DIRECT_SYM;
        step_cnt ++;

        return TRUE;
    }

    /* unmarking our path backwards with dot '.' if obstacle is found */
    dmap_info->dmap[dmap_info->row_num - y_strt - 1][x_strt] = EMP_CELL_MOD;
    
    printf ("\n\n");

    return FALSE;
}

/* name: _path_fnd_dir - finds a first direction in a maze
 * in: long
 * in: long
 * ret: STATUS_FIND
 * */
static STATUS _path_fnd_dir (long x_strt, long y_strt)
{
    /* stubbed for a while */

    return OK;
}

/***************************************************************************************/
/* BFS algorithm implementations - Breadth-first search (Wave algorithm/Lee algorithm) */
/***************************************************************************************/

/* name: _path_fnder_alt - finds a shortest path in the maze using the wave algorithm
 * in: DMAP_INFO *
 * in: DMAP_PRPS *
 * out: DMAP_INFO *
 * out: DMAP_PRPS *
 * ret: STATUS_FIND
 * */
static STATUS_FIND _path_fnder_alt (DMAP_INFO *dmap_info, DMAP_PRPS *dmap_prps)
{
    long ni = 0;/* number of current iteration */
    long nk = MAX_ITERS(dmap_info->col_num, dmap_info->row_num);/* total max number of iterations - 
                                                                increased to absolute maximum which 
                                                                is multiplication of rows and columns */
    long n_steps = 0;
    long row_cnt = 0;
    long col_cnt = 0;
    long tmp_cmp = ni;/* initial value for the comparing variable */
    long x_tmp = dmap_info->initial_point.x;/* keep it the same cause there is no need in X coordinate convertation */
    long y_tmp = dmap_info->row_num - dmap_info->initial_point.y - 1;/* converting the Y coordinate */
    long wall = WALL_VALUE(dmap_info->col_num, dmap_info->row_num);
    long path = PATH_VALUE(dmap_info->col_num, dmap_info->row_num);
    BOOL search_on = TRUE;
    BOOL backwrd_point_found = FALSE;
    long next_cmp = 0;
    BOOL path_found = FALSE;

    DMAP_COOR_SEQ *dmap_coor_seq = NULL;
    dmap_coor_seq = (DMAP_COOR_SEQ *)malloc ((nk + 1) * sizeof (DMAP_COOR_SEQ));
    memset (dmap_coor_seq, 0x00, (nk + 1) * sizeof (DMAP_COOR_SEQ));

    DMAP_NUM_SEQ *dmap_num_seq = NULL;
    dmap_num_seq = (DMAP_NUM_SEQ *)malloc ((nk + 1) * sizeof (DMAP_NUM_SEQ));
    memset (dmap_num_seq, 0x00, (nk + 1) * sizeof (DMAP_NUM_SEQ));
    
    /* starting from the source point */
    dmap_prps->xy_prps[dmap_info->row_num - dmap_info->initial_point.y - 1][dmap_info->initial_point.x] = ni;
    dmap_prps->xy_prps[dmap_info->row_num - dmap_info->final_point.y - 1][dmap_info->final_point.x] = \
        STRT_VALUE(dmap_info->col_num, dmap_info->row_num);

    while (TRUE == search_on)
    {
        #ifdef DEBUG_DMAP_PRPS
        /* debug print of the data map properties */
        print_dmap_prps_dbg (dmap_prps, dmap_info, x_tmp, y_tmp/*, dmap_info->row_num - y_tmp - 1*/);
        printf ("reference point iteration: %ld\n\r", ni);
        #endif
        
        /* * * * * * * * * * * * * * * * * * * * * * * * * *
         * BUILDING A WAVE !!! & looking for the DSTN point
         * * * * * * * * * * * * * * * * * * * * * * * * * */
        for (row_cnt = 0; (row_cnt < dmap_info->col_num) && (FALSE != search_on); row_cnt ++)
        {
            for (col_cnt = 0; (col_cnt < dmap_info->col_num)/* && (FALSE != search_on)*/; col_cnt ++)
            {
                //printf ("dmap_prps->xy_prps[%d][%d] = %d\n\r", row_cnt, col_cnt, dmap_prps->xy_prps[row_cnt][col_cnt]);

                //searching for reference point on each iteration
                if (ni == dmap_prps->xy_prps[row_cnt][col_cnt])
                {
                    /* LEFT: checking boundaries */
                    if ((col_cnt - 1 >= 0)/* && (FALSE != search_on)*/)
                    {
                        if ((wall != dmap_prps->xy_prps[row_cnt][col_cnt - 1]) \
                            && (path == dmap_prps->xy_prps[row_cnt][col_cnt - 1]))
                        {
                            dmap_prps->xy_prps[row_cnt][col_cnt - 1] = ni + 1;
                        }

                        /* Looking for the DSTN point at the LEFT */
                        if (dmap_prps->xy_prps[row_cnt][col_cnt - 1] == STRT_VALUE(dmap_info->col_num, dmap_info->row_num))
                        {
                            printf ("FOUND: destination point is reached!!!\n\r");
                            //printf ("VALUE: %d\n\r", dmap_prps->xy_prps[row_cnt][col_cnt - 1]);
                            search_on = FALSE;
                            path_found = TRUE;

                            break;
                        }
                    }

                    /* !!! NEED TO CONVERT */
                    /* DOWN: checking boundaries */
                    if ((row_cnt - 1 >= 0)/* && (FALSE != search_on)*/)
                    {
                        if ((wall != dmap_prps->xy_prps[row_cnt - 1][col_cnt]) \
                            && (path == dmap_prps->xy_prps[row_cnt - 1][col_cnt]))
                        {
                            dmap_prps->xy_prps[row_cnt - 1][col_cnt] = ni + 1;
                        }

                        /* Looking for the DSTN point BELOW */
                        if (dmap_prps->xy_prps[row_cnt - 1][col_cnt] == STRT_VALUE(dmap_info->col_num, dmap_info->row_num))
                        {
                            printf ("FOUND: destination point is reached!!!\n\r");
                            //printf ("VALUE: %d\n\r", dmap_prps->xy_prps[row_cnt - 1][col_cnt]);
                            search_on = FALSE;
                            path_found = TRUE;

                            break;
                        }
                    }

                    /* RIGHT: checking boundaries */
                    if ((col_cnt + 1 < dmap_info->col_num) && (FALSE != search_on))
                    {
                        if ((wall != dmap_prps->xy_prps[row_cnt][col_cnt + 1]) \
                            && (path == dmap_prps->xy_prps[row_cnt][col_cnt + 1]))
                        {
                            dmap_prps->xy_prps[row_cnt][col_cnt + 1] = ni + 1;
                        }

                        /* Looking for the DSTN point at the RIGHT */
                        if (dmap_prps->xy_prps[row_cnt][col_cnt + 1] == STRT_VALUE(dmap_info->col_num, dmap_info->row_num))
                        {
                            printf ("FOUND: destination point is reached!!!\n\r");
                            //printf ("VALUE: %d\n\r", dmap_prps->xy_prps[row_cnt][col_cnt + 1]);
                            search_on = FALSE;
                            path_found = TRUE;

                            break;
                        }
                    }

                    /* !!! NO NEED TO CONVERT */
                    /* UP: checking boundaries */
                    if ((row_cnt + 1 < dmap_info->row_num) && (FALSE != search_on))
                    {
                        if ((wall != dmap_prps->xy_prps[row_cnt + 1][col_cnt]) \
                            && (path == dmap_prps->xy_prps[row_cnt + 1][col_cnt]))
                        {
                            dmap_prps->xy_prps[row_cnt + 1][col_cnt] = ni + 1;
                        }

                        /* Looking for the DSTN point ABOVE */
                        if (dmap_prps->xy_prps[row_cnt + 1][col_cnt] == STRT_VALUE(dmap_info->col_num, dmap_info->row_num))
                        {
                            printf ("FOUND: destination point is reached!!!\n\r");
                            //printf ("VALUE: %d\n\r", dmap_prps->xy_prps[row_cnt + 1][col_cnt]);
                            search_on = FALSE;
                            path_found = TRUE;

                            break;
                        }
                    }
                }
            }
        }

        if (FALSE != search_on)
        {
            ni ++;

            if (ni >= nk)
            {
                search_on = FALSE;
                /* NO path is found */
                path_found = FALSE;
                printf ("NO PATH IS FOUND!!!\n\r");
            }
        }
        else
        {
            printf ("Number of iterations: %ld\n\r", ni);
        }
    }

    if (FALSE != path_found)
    {
        search_on = TRUE;
    
        /* Init the sequence of search path coordinates */
        dmap_coor_seq[n_steps].x_coor = dmap_info->initial_point.x/*x_tmp*/;
        dmap_coor_seq[n_steps].y_coor = /*dmap_info->row_num - */dmap_info->initial_point.y/* - 1*//*y_tmp*/;
        dmap_coor_seq[n_steps].total_num = ni + 1;
        dmap_coor_seq[n_steps].num = 0;
    
        printf ("START COORDINATES INFO: <x=%ld, y=%ld>, step=%ld\n\r",
                dmap_coor_seq[n_steps].x_coor,
                dmap_coor_seq[n_steps].y_coor,
                dmap_coor_seq[n_steps].num);



        dmap_coor_seq[ni].x_coor = dmap_info->final_point.x/*x_tmp*/;
        dmap_coor_seq[ni].y_coor = /*dmap_info->row_num - */dmap_info->final_point.y/* - 1*//*y_tmp*/;
        dmap_coor_seq[ni].total_num = ni + 1;
    
        printf ("DESTINATION COORDINATES INFO: <x=%ld, y=%ld>, step=%ld\n\r",
                dmap_coor_seq[ni].x_coor,
                dmap_coor_seq[ni].y_coor,
                dmap_coor_seq[ni].total_num);

        /*
         * dmap_coor_seq - the container which will hold all the coordinates,
         * total number of steps (iterations) and character of the search direction
         */

        tmp_cmp = STRT_VALUE(dmap_info->col_num, dmap_info->row_num);//assigning STRT value

        /* Looking for the coordinates of the DSTN point to start searching path backwards */
        for (row_cnt = 0; (row_cnt < dmap_info->row_num) && (FALSE == backwrd_point_found); row_cnt ++)
        {
            for (col_cnt = 0; (col_cnt < dmap_info->col_num)/* && (FALSE == backwrd_point_found)*/; col_cnt ++)
            {
                //looking for the current number in the cell
                if (tmp_cmp == dmap_prps->xy_prps[row_cnt][col_cnt])
                {
                    /* do this once - the DSTN point is begginning of backward search */
                             if ((FALSE == backwrd_point_found) && \
                        (tmp_cmp == dmap_prps->xy_prps[row_cnt][col_cnt]))
                    {
                        x_tmp = col_cnt;
                        y_tmp = row_cnt;//do we need to convert Y coordinate?

                        tmp_cmp = dmap_coor_seq[0].total_num - 1;

                        next_cmp = tmp_cmp - 1;

                        n_steps = 0;

                        dmap_coor_seq[n_steps].x_coor = x_tmp;
                        dmap_coor_seq[n_steps].y_coor = y_tmp;
                        dmap_coor_seq[n_steps].srch_ch = DESTN_CELL_MOD;
                        dmap_coor_seq[n_steps].num = n_steps;

                        backwrd_point_found = TRUE;

                        break;
                    }
                }
            }
        }
    }
    

    /* searching coordinates by moving backward */
    while (TRUE == search_on)
    {
        #ifdef DEBUG_DMAP_PRPS
        /* debug print of the data map properties */
        print_dmap_prps_dbg (dmap_prps, dmap_info, x_tmp, y_tmp/*, dmap_info->row_num - y_tmp - 1*/);
        #endif

        /* DO NOT REVERT THE WAY OF LOOKING OVER THE ARRAY - MOVE FROM TOP TO BOTTOM! */
        /* searching for the shortest path and saving the current coordinates */
        /* Looking for the path by moving backward e.g. from the END to the START */

        /* DOWN: looking around for the bigger value */
        if (y_tmp + 1 < dmap_info->row_num)
        {
            if (tmp_cmp == dmap_prps->xy_prps[y_tmp + 1][x_tmp])
            {
                tmp_cmp = dmap_prps->xy_prps[y_tmp + 1][x_tmp];
                next_cmp = tmp_cmp - 1;
                tmp_cmp = next_cmp;

                /* counting steps */
                n_steps ++;

                /* saving coordinates */
                dmap_coor_seq[n_steps].x_coor = x_tmp;
                dmap_coor_seq[n_steps].y_coor = (y_tmp += 1);//convert the Y coordinate here!
                dmap_coor_seq[n_steps].num = n_steps;
                dmap_coor_seq[n_steps].srch_ch = UP_DIRECT_SYM;
            }
        }

        /* RIGHT: looking around for the bigger value */
        if (x_tmp + 1 < dmap_info->col_num)
        {
            if (tmp_cmp == dmap_prps->xy_prps[y_tmp][x_tmp + 1])
            {
                tmp_cmp = dmap_prps->xy_prps[y_tmp][x_tmp + 1];
                next_cmp = tmp_cmp - 1;
                tmp_cmp = next_cmp;

                /* counting steps */
                n_steps ++;

                /* saving coordinates */
                dmap_coor_seq[n_steps].x_coor = (x_tmp += 1);
                dmap_coor_seq[n_steps].y_coor = y_tmp;
                dmap_coor_seq[n_steps].num = n_steps;
                dmap_coor_seq[n_steps].srch_ch = LF_DIRECT_SYM;
            }
        }

        /* UP: checking boundaries */
        if (y_tmp > 0)
        {
            if (tmp_cmp == dmap_prps->xy_prps[y_tmp - 1][x_tmp])
            {
                tmp_cmp = dmap_prps->xy_prps[y_tmp - 1][x_tmp];
                next_cmp = tmp_cmp - 1;
                tmp_cmp = next_cmp;

                /* counting steps */
                n_steps ++;
                            
                /* saving coordinates */
                dmap_coor_seq[n_steps].x_coor = x_tmp;
                dmap_coor_seq[n_steps].y_coor = (y_tmp -= 1);
                dmap_coor_seq[n_steps].num = n_steps;
                dmap_coor_seq[n_steps].srch_ch = DW_DIRECT_SYM;
            }
        }

        /* LEFT: checking boundaries */
        if (x_tmp > 0)
        {
            if (tmp_cmp == dmap_prps->xy_prps[y_tmp][x_tmp - 1])
            {
                tmp_cmp = dmap_prps->xy_prps[y_tmp][x_tmp - 1];
                next_cmp = tmp_cmp - 1;
                tmp_cmp = next_cmp;

                /* counting steps */
                n_steps ++;
                            
                /* saving coordinates */
                dmap_coor_seq[n_steps].x_coor = (x_tmp -= 1);
                dmap_coor_seq[n_steps].y_coor = y_tmp;
                dmap_coor_seq[n_steps].num = n_steps;
                dmap_coor_seq[n_steps].srch_ch = RT_DIRECT_SYM;
            }
        }

        ni ++;

        if (DSTN_VALUE(0,0) == dmap_prps->xy_prps[y_tmp][x_tmp])
        {
            printf ("\nstop coordinates searching - SUCCEED to find the valid!!!\n\n");
            search_on = FALSE;
        }

        if (ni >= nk)
        {
            printf ("\nstop coordinates searching - FAILED to find the valid!!!\n\n");
            printf ("\nno path is found!\n\n");
            search_on = FALSE;
        }
    }

    if (FALSE != path_found)
    {
        (void)_dmap_prnt_path_seq (dmap_coor_seq);
        (void)_dmap_cnvrt_path (dmap_info, dmap_coor_seq);
    }

    /* TODO: free DMAP_COOR_SEQ and DMAP_NUM_SEQ memory */
    free (dmap_coor_seq);
    free (dmap_num_seq);

    return TRUE;
}

/* name: _path_fnder_alt_init - initializes the data which in use by the 
                                wave algorithm search routine
 * in: DMAP_INFO *
 * in: DMAP_PRPS *
 * out: DMAP_INFO *
 * out: DMAP_PRPS *
 * ret: STATUS
 * */
static STATUS _path_fnder_alt_init (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    long row_cnt = 0;
    long col_cnt = 0;

    long row_num = dmap_info->row_num;
    long col_num = dmap_info->col_num;


    for (row_cnt = 0; row_cnt < row_num; row_cnt ++)
    {
        for (col_cnt = 0; col_cnt < col_num; col_cnt++)
        {
            switch (dmap_info->dmap[row_cnt][col_cnt])
            {
                case BSY_CELL_MOD:/* if point is wall or out of border */
                    dmap_prps->xy_prps[row_cnt][col_cnt] = \
                        WALL_VALUE(col_num,row_num);
                    break;

                case EMP_CELL_MOD:/* if point is part of the path */
                    dmap_prps->xy_prps[row_cnt][col_cnt] = \
                        PATH_VALUE(col_num,row_num);
                    break;

                case START_CELL_MOD:/* if point is a starting point */
                    dmap_prps->xy_prps[row_cnt][col_cnt] = \
                        STRT_VALUE(col_num,row_num);
                    break;

                #if 0
                case DESTN_CELL_MOD:/* if point is a destination point */
                    dmap_prps->xy_prps[row_cnt][col_cnt] = \
                        DSTN_VALUE(col_num,row_num);
                    break;
                #endif
            }
        }
    }

    return status_ret;
}

/* name: _dmap_prps_creat - creates a data map properties for the wave algorithm search routine
 * in: DMAP_INFO *
 * in: DMAP_PRPS *
 * out: DMAP_INFO *
 * out: DMAP_PRPS *
 * ret: STATUS
 * */
static STATUS _dmap_prps_creat (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    long cnt = 0;



    if (NULL != (dmap_prps->xy_prps = (long **)malloc (dmap_info->row_num * sizeof (long *))))
    {
        memset (dmap_prps->xy_prps, 0x00, dmap_info->row_num * sizeof (long *));

        for (cnt = 0; cnt < dmap_info->row_num; cnt ++)
        {
            if (NULL != (dmap_prps->xy_prps[cnt] = (long *)malloc (dmap_info->col_num * sizeof (long))))
            {
                memset (dmap_prps->xy_prps[cnt], 0x00, dmap_info->col_num * sizeof (long));
                continue;
            }
            else
            {
                DEBUG_PRNT ("cannot allocate memory for the data map points properties - columns");
                status_ret = FAIL;
                break;
            }
        }
    }
    else
    {
        DEBUG_PRNT ("cannot allocate memory for the data map points properties - rows");
        status_ret = FAIL;
    }
        
    return status_ret;
}

/* name: _dmap_prps_dstry - destroys a data map properties for the wave algorithm search routine
 * in: DMAP_INFO *
 * in: DMAP_PRPS *
 * out: DMAP_INFO *
 * out: DMAP_PRPS *
 * ret: STATUS
 * */
static STATUS _dmap_prps_dstry (DMAP_PRPS *dmap_prps, DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    long cnt = 0;

    if (NULL != dmap_prps->xy_prps)
    {
        for (cnt = 0; cnt < dmap_info->row_num; cnt ++)
        {
            if (NULL != dmap_prps->xy_prps[cnt])
            {
                free (dmap_prps->xy_prps[cnt]);
            }
            else
            {
                DEBUG_PRNT ("cannot free data map points properties - columns are NULL");
                status_ret = FAIL;
            }
        }
        free (dmap_prps->xy_prps);
    }
    else
    {
        DEBUG_PRNT ("cannot free data map points properties - rows are NULL");
        status_ret = FAIL;
    }

    return status_ret;
}

static STATUS _dmap_prnt_path_seq (DMAP_COOR_SEQ *dmap_coor_seq/*, long n_steps*/)
{
    //(void)dmap_coor_seq;

    if (NULL == dmap_coor_seq/* || 0 == n_steps*/)
    {
        return FAIL;
    }

    long n_steps = dmap_coor_seq[0].total_num;

    #ifdef DEBUG_COOR_SEQ
    printf ("\n\nthe way coordinates sequence is:\n\n");
    for (int cnt = 0; cnt < n_steps; cnt ++)
    {
        printf ("STEP=%ld X=%ld Y=%ld\n",
                cnt,
                dmap_coor_seq[n_steps - cnt].x_coor,
                dmap_coor_seq[n_steps - cnt].y_coor);
    }
    #endif

    printf ("\nPATH: s -> ");
    for (int cnt = 0; cnt < n_steps; cnt ++)
    {
        printf ("%c -> ", dmap_coor_seq[n_steps - cnt].srch_ch);
    }
    printf ("d\n\n");

    return OK;
}

STATUS _dmap_cnvrt_path (DMAP_INFO *dmap_info, DMAP_COOR_SEQ *dmap_coor_seq)
{
    if (NULL == dmap_info || NULL == dmap_coor_seq)
    {
        return FAIL;
    }

    long n_steps = dmap_coor_seq[0].total_num;
    
    /* converting path - begin from the cnt=1 to save the 's' marked cell on the map */
    for (int cnt = 1; cnt < n_steps; cnt ++)
    {
        dmap_info->dmap[dmap_coor_seq[n_steps - cnt].y_coor][dmap_coor_seq[n_steps - cnt].x_coor] \
            = PATH_SYM;
    }

    /* mark the STRT point */


    /* mark the DSTN point */


    //print_bdmap_dbg (dmap_info);

    return OK;
}

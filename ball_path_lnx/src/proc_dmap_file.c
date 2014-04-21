/* subrtns.cpp - subroutines module for main module of the ball_path test task
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a several subroutines entry function.
 * */

/*
* INCLUDES
* */
//#include "io.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "../inc/proc_dmap_file.h"
#include "../inc/ball_path.h"
#include "../inc/debug_prnt.h"

/*
 * IMPORTED GLOBAL DATA
 * */

/*
 * LOCAL ROUTINES PROTOTYPES
 * */
static STATUS_FILE _get_dmap_file (char **argv, FILE **fstream, int *dmap_file_s);
static STATUS_FILE _close_dmap_file (FILE **fstream);/* close data map file */
static STATUS _get_ndim (long *dim_num, FILE **fstream); /* get the dimensions */
static STATUS _get_point (FILE **fstream, COOR_POINT *fpoint, int dmap_file_s); /* get the (x,y) coordinates */
static STATUS _bld_dmap (FILE **fstream, DMAP_INFO *dmap_info);/* build the balls distribution map */
static STATUS _check_rslts (DMAP_INFO *dmap_info);/* check the valid results */

/*
 * GLOBAL ROUTINES PROTOTYPES
 * */

/*
* name: proc_dmap_file - proceeds input txt data map file
* in: int
* in: char *
* in: DMAP_INFO *
* out: DMAP_INFO *
* ret: STATUS
* */
STATUS proc_dmap_file (int argc, char **argv, DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    STATUS_FILE status_file = OPEN_SUCCESS;
    FILE *fstream = NULL;
    int dmap_file_s = 0;
    
    /* FIXME: Temporary debugging hack */
    #ifdef DEBUG_INPUT
    //argv[1] = "c:\\Users\\Ivan.Starodubtsev\\Desktop\\map.txt";
    //argc = 2;

    //argv[1] = MAP_FILE_PATH;
	argv[1] = MAP_FILE_PATH_LOCAL;
    argc = 2;

    #endif

    /* checking the input parameters */
    if ((2 != argc) || (NULL == argv[1]) || (NULL == dmap_info))
    {
        #if 1
        printf ("the invalid number of input arguments or bad input data\n"
                "usage: %s <filename>\n"
                "where the <filename> is full filename "
                "path to the txt which contains all input data\n", argv[0]);
        #else
        DEBUG_PRNT ("the invalid number of input arguments or bad input data\n"
                "usage: %s <filename>\n"
                "where the <filename> is full filename "
                "path to the txt which contains all input data\n", argv[0]);
        #endif
        status_ret = FAIL;
    }
    else
    {
        DEBUG_PRNT ("opening and reading txt input file %s", argv[1]);
    }

    /* getting the data map file */
    if (FAIL != status_ret)
    {
        status_file = _get_dmap_file (argv, &fstream, &dmap_file_s);
        
        if (OPEN_FAIL != status_file)
        {
            DEBUG_PRNT ("data map file size is: %d", dmap_file_s);
        }
        else
        {
            status_ret = FAIL;
        }
    }
    else
    {
        status_file = OPEN_FAIL;
    }

    /* reading number of rows */
    if (FAIL != status_ret)
    {
        status_ret = _get_ndim (&(dmap_info->row_num), &fstream);

        if (FAIL != status_ret)
        {
            DEBUG_PRNT ("number of rows is: %d", dmap_info->row_num);
        }
    }

    /* reading number of columns */
    if (FAIL != status_ret)
    {
        status_ret = _get_ndim (&(dmap_info->col_num), &fstream);

        if (FAIL != status_ret)
        {
            DEBUG_PRNT ("number of columns is: %d", dmap_info->col_num);
        }

    }

    /* getting the initial points */
    if (FAIL != status_ret)
    {
        status_ret = _get_point (&fstream, &(dmap_info->initial_point), dmap_file_s);

        if (FAIL != status_ret)
        {
            DEBUG_PRNT ("the initial point coordinates are: (x=%d, y=%d)",
                dmap_info->initial_point.x,
                dmap_info->initial_point.y);
        }
    }
    
    /* getting the final points */
    if (FAIL != status_ret)
    {
        status_ret = _get_point (&fstream, &(dmap_info->final_point), dmap_file_s);

        if (FAIL != status_ret)
        {
            DEBUG_PRNT ("the final point coordinates are: (x=%d, y=%d)",
                dmap_info->final_point.x,
                dmap_info->final_point.y);

        }
    }

    /* check the valid results */
    if (FAIL != status_ret)
    {
        status_ret = _check_rslts (dmap_info);
    }

    if (FAIL != status_ret)
    {
        status_ret = _bld_dmap (&fstream, dmap_info);

        if (FAIL != status_ret)
        {
            DEBUG_PRNT ("cannot build the map of balls distribution");
        }
    }

    /* closing data map file */
    if ((OPEN_FAIL != status_file) || (FAIL != status_ret))
    {
        status_file = _close_dmap_file (&fstream);
    }

    if ((FAIL == status_ret) || (OPEN_FAIL == status_file))
    {
        printf (EXIT);
    }

    return status_ret;
}


/*
 * LOCAL ROUTINES IMPLEMENTATIONS
 * */

/* name: _get_dmap_file - opens and analyzing the data map file
 * in: FILE **
 * in: int
 * out: COOR_POINT *
 * ret: STATUS
 * */
static STATUS_FILE _get_dmap_file (char **argv, FILE **fstream, int *dmap_file_s)
{
    STATUS_FILE status_ret = OPEN_SUCCESS;

    #if 0
    errno_t errnou = 0;
    struct _stat stat_buffer;
    #endif
    
    struct stat stat_buffer;
    int fstat = 0;


    #if 0
    memset (&stat_buffer, 0x00, sizeof (struct _stat));
    #endif

    memset (&stat_buffer, 0x00, sizeof (struct stat));

    if (NULL != argv)
    {
        #if 0
        errnou = _set_fmode(_O_BINARY);

        if (0 != errnou)
        {
            DEBUG_PRNT ("could not set the fmode for the data map file");
            status_ret = OPEN_FAIL;
        }

        /* getting the status of the  */
        if (FAIL != status_ret)
        #endif
        {
            #if 0
            fstat = _stat (argv [1], &stat_buffer);
            #endif

            fstat = stat (argv [1], &stat_buffer);
            
            if (0 == fstat)
            {
                DEBUG_PRNT ("successfully got the txt input file status %s", argv[1]);
                *dmap_file_s = stat_buffer.st_size;
            }
            else
            {
                DEBUG_PRNT ("cannot get the txt input file status %s", argv[1]);
                status_ret = OPEN_FAIL;
            }
        }

        /* opening txt input data file */
        if (FAIL != status_ret)
        {
            *fstream = fopen (argv [1], "r+b");

            if (NULL != *fstream)
            {
                DEBUG_PRNT ("successfully opened txt input file %s", argv[1]);
            }
            else
            {
                DEBUG_PRNT ("cannot open the txt input file %s", argv[1]);
                status_ret = OPEN_FAIL;
            }
        }

        /* seeking to the beginning of the file */
        if (0 != fseek (*fstream, 0L, SEEK_SET))
        {
            DEBUG_PRNT ("cannot set the buffer to the beginning of the file");
        }
    }
    else
    {
        DEBUG_PRNT ("bad input map data file");
        status_ret = OPEN_FAIL;
    }

    return status_ret;
}

/* name: _get_ndim - gets the next number of dimension from the data map file
 * in: FILE **
 * out: long *
 * ret: STATUS
 * */
static STATUS _get_ndim (long *dim_num, FILE **fstream)
{
    STATUS status_ret = OK;



    fscanf (*fstream, "%ld", dim_num);

    if (0 > *dim_num)
    {
        DEBUG_PRNT ("number of dimension is negative: %ld", *dim_num);
        status_ret = FAIL;
    }

    return status_ret;
}

/* name: _get_point - gets the point of the next coordinates from the data map file
 * in: FILE **
 * in: int
 * out: COOR_POINT *
 * ret: STATUS
 * */
static STATUS _get_point (FILE **fstream, COOR_POINT *point, int dmap_file_s)
{
    STATUS status_ret = OK;
    char *point_str = NULL;
    char *cur_point = NULL;
    int strl = 0;
    int cnt = 0;
    int _cnt = 0;



    memset (point, 0x00, sizeof (COOR_POINT));
    
    if (NULL != (point_str = (char *)malloc (dmap_file_s * sizeof (char))))
    {
        memset (point_str, 0x00, dmap_file_s * sizeof (char));
        fscanf (*fstream, "%32s", point_str);/* Trouble with free() after fscanf() was called! */
        strl = strlen (point_str);

        /* converting the string (x,y) into the separate coordinates */
        for (cnt = 0; cnt < strl; cnt ++)
        {
            switch (point_str[cnt])
            {
                case '(':
                    
                    if (0 != isdigit (point_str [cnt + 1]))
                    {
                        if (NULL == (cur_point = (char *)malloc (strl * sizeof (char))))
                        {
                            DEBUG_PRNT ("cannot allocate memory to parse the 'x' coordinate");
                            status_ret = FAIL;
                            break;
                        }

                        memset (cur_point, 0x00, strl * sizeof (char));

                        for (_cnt = 0 ; ; _cnt ++)
                        {
                            cur_point [_cnt] = point_str[cnt + _cnt + 1];

                            if (',' == point_str [cnt + _cnt + 2])
                            {
                                point->x = atoi (cur_point);                              
                                free (cur_point);
                                break;
                            }
                        }

                    }

                    break;

                case ',':

                    if (0 != isdigit (point_str [cnt + 1]))
                    {
                        if (NULL == (cur_point = (char *)malloc (strl * sizeof (char))))
                        {
                            DEBUG_PRNT ("cannot allocate memory to parse the 'y' coordinate");
                            status_ret = FAIL;
                            break;
                        }

                        memset (cur_point, 0x00, strl * sizeof (char));

                        for (_cnt = 0 ; ; _cnt ++)
                        {
                            cur_point [_cnt] = point_str[cnt + _cnt + 1];

                            if (')' == point_str [cnt + _cnt + 2])
                            {
                                point->y = atoi (cur_point);
                                free (cur_point);
                                break;
                            }
                        }
                    }

                    break;

                case ')':
                    break;
            }
        }

        free (point_str);
    }
    else
    {
        DEBUG_PRNT ("cannot allocate the memory for the final point coordinates string");
    }

    return status_ret;
}

/* name: _bld_dmap - reads and builds the balls distribution from the data map file
 * in: FILE **
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: STATUS
 * */
static STATUS _bld_dmap (FILE **fstream, DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;
    int cnt = 0;
    int cur_strl = 0;
    long cnt_row = 0;
    long cnt_col = 0;
    int _cnt = 0;
    char *str_tmp = NULL;
    char ch_tmp = 0;

    

    /* allocating memory for the rows */
    if (NULL != (dmap_info->dmap = (char **)malloc (dmap_info->row_num * sizeof (char *))))
    {
        memset (dmap_info->dmap, 0x00, dmap_info->row_num * sizeof (char *));

        for (cnt = 0; cnt < dmap_info->row_num; cnt ++)
        {
            /* allocating memory for the columns + 1 item to close the string and preventing 
               appearing the garbage at the end */
            if (NULL != (dmap_info->dmap[cnt] = (char *)malloc ((dmap_info->col_num + 1) * sizeof (char))))
            {
                memset (dmap_info->dmap[cnt], 0x00, (dmap_info->col_num + 1) * sizeof (char));
                dmap_info->dmap[cnt][dmap_info->col_num] = '\0';
                continue;
            }
            else
            {
                DEBUG_PRNT ("cannot allocate the memory for the balls distribution map - columns");
                status_ret = FAIL;
                break;
            }
        }
    }
    else
    {
        DEBUG_PRNT ("cannot allocate the memory for the balls distribution map - rows");
    }


    if (FAIL != status_ret)
    {
        DEBUG_PRNT ("the balls distribution map is:");

        if (NULL != (str_tmp = (char *)malloc ((dmap_info->col_num + 1) * sizeof (char))))
        {
            memset (str_tmp, 0x00, (dmap_info->col_num + 1) * sizeof (char));
            str_tmp [dmap_info->col_num] = '\0';

            /* reading the lines from the data map file */
            for (cnt_row = 0; (cnt_row < dmap_info->row_num); cnt_row ++)
            {
                for (cnt_col = 0; (cnt_col < dmap_info->col_num) && (FAIL != status_ret); )
                {
                    ch_tmp = fgetc (*fstream);

                    switch (ch_tmp)
                    {
                        /* fall through */
                        case CR:
                        case LF:
                            /* checking the validness of the balls distribution map */
                            if ((cnt_col != 0) && (cnt_col != dmap_info->col_num))
                            {
                                DEBUG_PRNT ("invalid balls distribution map");
                                status_ret = FAIL;
                            }
                            break;
                            
                        /* tearing out chars and spaces */
                        default:
                            str_tmp [cnt_col] = ch_tmp;
                            cnt_col ++;
                            break;
                    }
                }

                if (FAIL != status_ret)
                {
                    if (!strncpy (dmap_info->dmap [cnt_row], str_tmp, dmap_info->col_num))
                    {
                        DEBUG_PRNT ("cannot copy strings to build the map");
                        status_ret = FAIL;
                    }
                }
                else
                {
                    break;
                }

            }
        
            free (str_tmp);
        }
    }

    return status_ret;
}

/* name: _close_dmap_file - closes the data map file
 * in: FILE **
 * out: 
 * ret: STATUS
 * */
static STATUS_FILE _close_dmap_file (FILE **fstream)
{
    STATUS_FILE status_ret = OPEN_SUCCESS;



    if (0 == fclose (*fstream))
    {
        DEBUG_PRNT ("successfully closed data map file");
    }
    else
    {
        DEBUG_PRNT ("failed to close data map file");
        status_ret = OPEN_FAIL;
    }
    
    return status_ret;
}

/* name: _check_rslts - check the processed data results
 * in: DMAP_INFO *
 * out: DMAP_INFO *
 * ret: STATUS
 * */
static STATUS _check_rslts (DMAP_INFO *dmap_info)
{
    STATUS status_ret = OK;

    if ((dmap_info->initial_point.x > dmap_info->col_num) \
        || (dmap_info->initial_point.y > dmap_info->row_num) \
        || (dmap_info->initial_point.x < 0) \
        || (dmap_info->initial_point.y < 0))
    {
        DEBUG_PRNT ("bad initial point coordinates");
        printf ("bad initial point coordinates - out of map\n");
        status_ret = FAIL;
    }

    if ((dmap_info->final_point.x > dmap_info->col_num) \
        || (dmap_info->final_point.y > dmap_info->row_num) \
        || (dmap_info->final_point.x < 0) \
        || (dmap_info->final_point.y < 0))
    {
        DEBUG_PRNT ("bad final point coordinates");
        printf ("bad final point coordinates - out of map\n");
        status_ret = FAIL;
    }

    if (FAIL != status_ret)
    {
        DEBUG_PRNT ("the results are valid");
    }

    return status_ret;
}

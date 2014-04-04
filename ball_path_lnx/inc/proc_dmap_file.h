/* subrtns.h - subroutines module for main module of the ball_path test task
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a prototypes of the some subroutine functions.
 * */

/*
* INCLUDES
* */
#include "ball_path.h"

/*
 * PROTOTYPES
 * */

/* name: proc_dmap_file - processes data map file
 * in: int
 * in: DMAP_INFO *
 * out: char **
 * out: DMAP_INFO *
 * ret: STATUS
 * */
STATUS proc_dmap_file (int argc, char **argv, DMAP_INFO *dmap_info);
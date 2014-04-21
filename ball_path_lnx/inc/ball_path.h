/* ball_path.h - header for main module of the ball_path test task
 *
 * ball_path - test task from the samsung ukraine r&d center
 *
 * contains a global user-defined data.
 * */

/*
* INCLUDES
* */

/*
* DEFINES
* */
#ifndef BALL_PATH
#define BALL_PATH

//#define DEBUG_APP
#define DEBUG_DMAP
#undef DEBUG_DMAP_DATA
#undef DEBUG_DMAP_STEPS
#undef DEBUG_DMAP_PRPS
#undef DEBUG_INPUT
#define DEBUG_ALT
#undef DEBUG_COOR_SEQ



#define EXIT                    ("\n\nexiting...\n")

//#define EMP_CELL                (' ') /* empty cell indicator */
#define EMP_CELL                ('.') /* empty cell indicator - MODIFIED */
#define BSY_CELL                ('o') /* busy cell indicator */
#define FNL_CELL                ('F') /* destination point final mark */

#define EMP_CELL_MOD            ('.')
#define BSY_CELL_MOD            ('O')

#define START_CELL_MOD          ('s') /* start cell - user defined */
#define DESTN_CELL_MOD          ('d') /* destination cell - user defined */

#define UP_DIRECT_STR           ('U') //(" U ")
#define DW_DIRECT_STR           ('D') //(" D ")
#define LF_DIRECT_STR           ('L') //(" L ")
#define RT_DIRECT_STR           ('R') //(" R ")

#define PATH_SYM                ('+')

#define UP_DIRECT_SYM           ('U')
#define DW_DIRECT_SYM           ('D')
#define LF_DIRECT_SYM           ('L')
#define RT_DIRECT_SYM           ('R')


#define CR                      (0x0D)
#define LF                      (0x0A)


#define WALL_VALUE(cols,rows)       ((cols) * (rows) + 2)/* if a point is a wall or out of border */
#define PATH_VALUE(cols,rows)       ((cols) * (rows) + 1)/* if a point is a part of a path */
#define STRT_VALUE(cols,rows)       ((cols) * (rows))/* if a point is a starting point */
#define DSTN_VALUE(cols,rows)       (0)/* if apoint is destination point */

#define MAX_ITERS(cols,rows)        ((cols) * (rows))/* max number of possible iterations */

#define CONVERT_POINT_COOR(pnt,dim)	((dim) - (pnt) - 1)/* macro converts point in two dimensional array to <x,y> 
										 representation when the x=0,y=0 is at bottom of left corner */

#if 0
#define MAX_ITERS(cols,rows)        (cols*(rows/2) + cols)/* max number of possible iterations */
#endif

#define MAP_FILE_PATH				"C:\\Users\\ivan.starodubtsev\\Documents\\Visual Studio 2010\\map.txt"

#define MAP_FILE_PATH_LOCAL			"map.txt"

/*
* TYPEDEFS
* */
// Commont status data type
typedef enum _STATUS {
    OK = 0,
    FAIL = 1
} STATUS;

// Data map file open status data type
typedef enum _STATUS_FILE {
    OPEN_SUCCESS = 0,
    OPEN_FAIL
} STATUS_FILE;

// Point coordinates data type
typedef struct _COOR_POINT {
    long x;
    long y;
} COOR_POINT;

// Total info about data map file data type
typedef struct _DMAP_INFO {
    long row_num;/* number of rows */
    long col_num;/* number of columns */
    COOR_POINT initial_point;/* coordinates of the initial point */
    COOR_POINT final_point;/* coordinates of the final point */
    char **dmap;/* balls distribution map */
} DMAP_INFO;

// Status of path searching
typedef enum _STATUS_FIND {
    FALSE = 0,
    TRUE
} STATUS_FIND, BOOL;

// Data map properties
typedef struct _DMAP_PRPS {
    #if 0
    long x;/* column coordinate - no conversion */
    long y;/* row coordinate - to convert calc next: (<number of rows> - <y start point> - 1) */
    long xy_prps;/* numeric property of the current point coordinate */
    #endif
    long **xy_prps;
} DMAP_PRPS;

// Data map coordinates sequence
typedef struct _DMAP_COOR_SEQ {
    long x_coor;
    long y_coor;

	long num;

	char srch_ch;

	long total_num;
} DMAP_COOR_SEQ;

// Number sequence of path
typedef struct _DMAP_NUM_SEQ {
    long num;
} DMAP_NUM_SEQ;
#endif
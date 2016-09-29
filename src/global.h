#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include <cstring>
#include <string>
#include <vector>
#include <exception>

#include <iostream>

#define REL_GT(x, y, d)   ((d) ? (x) >  (y) : (x) <  (y))
#define REL_LT(x, y, d)   ((d) ? (x) <  (y) : (x) >  (y))
#define REL_LE(x, y, d)   ((d) ? (x) <= (y) : (x) >= (y))
#define REL_GE(x, y, d)   ((d) ? (x) >= (y) : (x) <= (y))
#define REL_INC(x, d)     ((d) ? (x++) : (x--))
#define REL_DEC(x, d)     ((d) ? (x--) : (x++))
#define REL_LO_IDX(c, d)  ((d) ? 0 : c->size - 1)
#define REL_HI_IDX(c, d)  ((d) ? c->size - 1 : 0)
#define REL_NEXT(a, i, d) ((d) ? (a)[i+1] : (a)[i-1])
#define REL_ADD(a, b, d)  ((d) ? (a) + (b) : (a) - (b))
#define REL_SUB(a, b, d)  ((d) ? (a) - (b) : (a) + (b))

#define LINE_BUFFER_SIZE 512
#define NAME_BUFFER_SIZE 128

// A value of 0 or 1 with is added to the starts and stops of all printed intervals
class Offsets
{
public:
    static int in_start;
    static int in_stop;
    static int out_start;
    static int out_stop;
};

typedef enum direction { LO = 0, HI = 1 } Direction;

typedef enum genome_idx { QUERY = 0, TARGET = 1 } Genome_idx;

typedef enum corner
{
    PREV_START = 0,   // previous element as ordered by start
    NEXT_START = 1,   // next element as ordered by start
    PREV_STOP  = 2,   // previous element as ordered by stop
    NEXT_STOP  = 3    // next element as ordered by stop
} Corner;

#endif

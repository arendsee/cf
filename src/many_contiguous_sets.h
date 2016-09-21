#ifndef __MANY_CONTIGUOUS_SETS_H__
#define __MANY_CONTIGUOUS_SETS_H__

#include "global.h"
#include "contiguous_set.h"
#include "interval_set.h"

/** A containter for ContiguousSets */
class ManyContiguousSets : public IntervalSet<ContiguousSet>
{
public:
    void link_contiguous_blocks(
        Block*  front,
        long    k,
        size_t& setid
    );
};

#endif

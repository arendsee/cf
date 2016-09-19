#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "global.h"
#include "interval.h"
#include "linked_interval.h"

// forward declaration
class ContiguousSet;

class Block : public LinkedInterval<Block>, public Interval<Block>
{
private:
    static void unlink(Block* blk, int u, int d);
    static void move_b_to_a(Block* a, Block* b, int u, int d);
    static void replace_edge(Block* a, Block* b, int u, int d);
    static void dissolve_edge(Block* blk, int u, int d);
    static void merge_block_a_into_b_edge_(Block* a, Block* b, int i);

public:
    Block*         cnr[2]; // adjacent block in contiguous set
    ContiguousSet* cset;   // contiguous set id

    Block();
    Block(
        long    start,
        long    stop,
        double  score,
        char    strand,
        Contig* parent,
        size_t  linkid
    );
    ~Block();

    /** A clean TAB-delimited output suitable for giving to the user */
    void print();

    /** Transform b into (a U b), relink as needed
     *
     * After calling this function, a is removed from the datastructure and
     * connections to it are redirected.
     *
     * WARNING: this function only relinks Block->cor connections
     *
     * @param a Block to be merged (and deleted)
     * @param b Block to hold the final union of a and b
     */
    static void merge_block_a_into_b(Block* a, Block* b);
};

#endif

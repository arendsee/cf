#include "many_blocks.h"

Block* ManyBlocks::front()
{
    return cor[0];
}

Block* ManyBlocks::terminus(size_t i)
{
    Block* blk = (i < 4) ? cor[i] : NULL;
    return blk;
}

Block* ManyBlocks::back()
{
    return cor[1];
}

bool ManyBlocks::empty()
{
    return cor[0] == NULL;
}

// TODO: set up a constant time alternative
size_t ManyBlocks::size()
{
    size_t N;
    for(Block* blk = front(); blk != NULL; blk = blk->next()) {
        N++;
    }
    return N;
}

void ManyBlocks::clear()
{
    inv.clear();
    delete tree;
}

void ManyBlocks::link_block_corners()
{
    size_t N = inv.size();

    // forward sort by stop
    sort(2);
    for (size_t i = 0; i < N; i++) {
        inv[i]->cor[2] = (i == 0)     ? NULL : inv[i - 1];
        inv[i]->cor[3] = (i == N - 1) ? NULL : inv[i + 1];
    }
    // forward sort by start
    sort(0);
    for (size_t i = 0; i < N; i++) {
        inv[i]->cor[0] = (i == 0)     ? NULL : inv[i - 1];
        inv[i]->cor[1] = (i == N - 1) ? NULL : inv[i + 1];
    }
}

void ManyBlocks::set_overlap_group()
{

    // Holds current overlapping group id
    size_t grpid = 1;
    // Needed for determining overlaps and thus setids
    long maximum_stop = 0;
    // The stop position of the current interval
    long this_stop = 0;

    maximum_stop = 0;
    // Loop through each Block in the linked list
    for (Block* blk = front(); blk != NULL; blk = blk->next()) {
        this_stop = blk->pos[1];
        // If the start is greater than the maximum stop, then the block is in
        // a new adjacency group. For this to work, Contig->block must be
        // sorted by start. This sort is performed in build_tree.
        if (blk->pos[0] > maximum_stop) {
            grpid++;
        }
        if (this_stop > maximum_stop) {
            maximum_stop = this_stop;
        }
        blk->grpid = grpid;
    }
}


void ManyBlocks::link_adjacent_blocks_directed(Direction d)
{
    // In diagrams:
    // <--- indicates a hi block
    // ---> indicates a lo block
    // All diagrams and comments relative to the d==HI direction

    if (cor[0] == NULL || cor[1] == NULL || cor[2] == NULL || cor[3] == NULL) {
        fprintf(stderr, "Contig head must be set before link_adjacent_blocks is called\n");
        // fprintf(stderr, "genome=(%s) contig=(%s)\n", parent->parent->name.c_str(), parent->name.c_str());
        exit(EXIT_FAILURE);
    }

    // Transformed indices for Block->cor and Contig->cor
    int idx_a = (!d * 2) + !d; // - 0 previous/first element by start
    int idx_b = (!d * 2) +  d; // - 1 next/last element by start
    int idx_c = ( d * 2) + !d; // - 2 previous/first element by stop
    int idx_d = ( d * 2) +  d; // - 3 next/last element by stop

    Block *lo, *hi;

    lo = cor[idx_c]; // first element by stop
    hi = cor[idx_a]; // first element by start

    while (hi != NULL) {

        //       --->
        // <---
        // OR
        // --->
        //   <---
        // This should should occur only at the beginning
        if (REL_LE(hi->pos[!d], lo->pos[d], d)) {
            hi->adj[!d] = NULL;
            hi = hi->cor[idx_b];
        }

        //  lo     next
        // ---->  ---->
        //               <---
        // If next is closer, and not overlapping the hi, increment lo
        // You increment lo until it is adjacent to the current hi
        else if (REL_LT(lo->cor[idx_d]->pos[d], hi->pos[!d], d)) {
            lo = lo->cor[idx_d];
        }

        // --->
        //      <---
        // The current lo is next to, and not overlapping, current hi
        else {
            hi->adj[!d] = lo;
            hi = hi->cor[idx_b];
        }
    }
}

void ManyBlocks::link_adjacent_blocks()
{
    link_adjacent_blocks_directed(HI);
    link_adjacent_blocks_directed(LO);
}

void ManyBlocks::merge_overlaps()
{
    Block *lo, *hi;

    // iterate through all blocks
    for (lo = front(); lo != NULL; lo = lo->next()) {
        // look ahead to find all doubly-overlapping blocks
        for (hi = lo->next(); hi != NULL; hi = hi->next()) {
            if (! hi->overlap(lo)) {
                break;
            }
            if (hi->over->overlap(lo->over) && hi->over->parent == lo->over->parent) {
                Block::merge_block_a_into_b(hi, lo);
                hi = lo;
            }
        }
    }
}

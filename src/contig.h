#ifndef __CONTIG_H__
#define __CONTIG_H__

#include "global.h"
#include "search_interval.h"
#include "many_blocks.h"
#include "many_contiguous_sets.h"
#include "feature.h"

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <list>
#include <set>

class Contig {
public:
    ManyBlocks block;
    ManyContiguousSets cset;
    Feature feat;

    Contig();

    Contig(
        const char* t_genome_name,
        const char* t_contig_name,
        long t_length=DEFAULT_CONTIG_LENGTH
    );

    ~Contig();

    void set_length(long length);

    /** Print target regions from a given query */
    void find_search_intervals(Feature& feat);

    /** Print target regions from a given query */
    std::vector<SearchInterval> list_search_intervals(Feature& feat);

    /** Write blocks overlapping intervals in intfile
     *
     * Prints the following TAB-delimited columns to STDOUT:
     * - query entry name, this should be unique for input interval
     * - target contig name
     * - target start position
     * - target stop position
     */
    void map(Feature& feat);

    /** Count blocks overlapping intervals in intfile
     *
     * Prints the input sequence name and count to STDOUT in TAB-delimited format.
     */
    void count(Feature& feat);
};

#endif

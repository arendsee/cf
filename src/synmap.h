#ifndef __SYNMAP_H__
#define __SYNMAP_H__

#include "global.h"
#include "bound.h"
#include "genome.h"
#include "linked_interval.hpp"
#include "feature.h"

#include <iterator>
#include <list>
#include <Rcpp.h>


/** A pair of syntenically linked Genome objects  */
class Synmap
{
private:

    Genome* genome[2] = { nullptr, nullptr };
    FILE* synfile     = nullptr;
    FILE* tclfile     = nullptr;
    FILE* qclfile     = nullptr;
    int swap          = 0;
    long k            = 0;
    double r          = 0.001;
    char trans        = 'i';

    // loads synfile and calls the below functions in proper order
    void load_blocks();

    // wrappers for Genome functions
    void link_blocks();

    /** Checks invariants - dies if anything goes wrong */
    void validate();

public:

    Synmap(
        FILE* synfile,
        FILE* tclfile,
        FILE* qclfile,
        bool swap,
        int k,
        double r,
        char trans
    );

    ~Synmap();

    Contig* get_contig(size_t gid, char* contig_name);

    Rcpp::DataFrame as_data_frame();

    /** Reads a GFF file and calls the appropriate command on each line */
    bool process_gff(FILE* intfile, Command cmd);

    void filter(FILE* hitfile);

};

#endif

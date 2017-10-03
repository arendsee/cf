#ifndef __SYNMAP_H__
#define __SYNMAP_H__

#include "global.h"
#include "bound.h"
#include "genome.h"
#include "linked_interval.h"
#include "feature.h"
#include "types.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <list>
#include <array>
#include <Rcpp.h>


/** A pair of syntenically linked Genome objects  */
class Synmap
{
private:
    Genome* genome[2] = { nullptr, nullptr };
    std::string synfile;
    std::string tclfile;
    std::string qclfile;
    int     swap      = 0;
    long    k         = 0;
    double  r         = 0.001;
    char    trans     = 'i';

    // The {{ is needed to workaround a bug in old g++ compilers
    std::array<int,4> offsets = {{1,1,1,1}};

    // utility function for loading GFF files
    std::vector<Feature> gff2features(std::string fh);

    void build_synmap();

    // loads synfile and calls the below functions in proper order
    void load_blocks();

    // wrappers for Genome functions
    void link_blocks();

    /** Checks invariants - dies if anything goes wrong */
    void validate();

public:
    Synmap(
        std::string  synfile,
        std::string  tclfile,
        std::string  qclfile,
        bool   swap,
        int    k,
        double r,
        char   trans,
        std::vector<int> offsets
    );

    ~Synmap();

    Contig* get_contig(size_t gid, const char* contig_name);

    Rcpp::DataFrame as_data_frame();

    Rcpp::DataFrame count(std::string intfile);

    Rcpp::DataFrame map(std::string intfile);

    Rcpp::DataFrame search(std::string intfile);

    Rcpp::CharacterVector filter(std::string hitfile);

};

#endif

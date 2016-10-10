[![Build Status](https://travis-ci.org/arendsee/synder.svg?branch=master)](https://travis-ci.org/arendsee/synder)

# Synder

    Map query intervals to target search spaces using a synteny map


# Installation

``` bash
make
make test
```

The synder executable is independent, so the binary can simply be moved to some
directory in your path (e.g. `mv synder $HOME/bin`).


# Getting help

`synder` is composed of several subcommands, each with a specific function and
set of legal parameters. For a toplevel overview of the available commands, run
`synder` with no arguments. Help messages for each subcommand can be accessed
by calling without arguments, e.g. `synder search` or `synder map`.


# Subcommands

## Search

This is the primary function of `synder`. A set of intervals in one genome (the
query) is mapped to a set of intervals in another genome (the target). The
input intervals may fall between query-side syntenic intervals, in which case,
the search interval also will fall inbetween target syntenic intervals.

The output is a table with the following fields:
 1.  query interval name (e.g. AT1G20300)
 2.  query chromosome name
 3.  query start position
 4.  query stop position
 5.  target chromosome name
 6.  search interval start position on target chromsome
 7.  search interval stop position on target chromsome
 8.  search interval strand ('+' / '-')
 9.  score
 10. contiguous set id
 11. lower flag
     - 0 lower bound is inside a syntenic interval
     - 1 lower bound is between intervals in a contiguous set
     - 2 lower bound does not overlap the contiguous set
     - 3 lower bound is beyond any syntenic interval (near end of scaffold)
 12. upper flag - see lower flag
 13. between flag - 0 if query overlaps a syntenic interval, 1 otherwise


## Filter

The filter function takes two syntenic maps and finds the congruent links.
Given two syntenic maps, A and B, the query-side intervals in A are mapped to
target side search intervals using the syntenic map B. Then the target-side
intervals in A that overlap the predicted search intervals are printed.

The most obvious usage case takes as input the results of BLASTing a sequence
against a genome. Often such searches will have many hits in a swooping e-value
gradient. The highest scoring hit may not be the orthologous one (for example,
a weakly similar, but long hit may outscore the nearly identical, but
truncated, true ortholog). `synder filter` will find the hits that are
concordant with genomic context, reducing possibly thousands of hits to only
a few.

## Map

Find all query-side syntenic blocks that overlap the input interval. Then map
these blocks to the target-side and print the result. If an input interval
overlaps no query bock, the flanks are printed.

The output will have the columns:o 
 1. input interval name (e.g. AT1G01010)
 2. target contig name (e.g. Chr1)
 3. target start position
 4. target stop position
 5. missing flag, 0 if input overlaps no block, 1 otherwise

## Count

`synder count` is like `synder map` except it counts the number of overlaps,
rather than printing them. The output is a TAB-delimited list of sequence names
and counts. For example:

```
$ synder count -i a.gff -s a-b.syn
a       0
b       1
c       4
```

## Dump

Builds the internal synteny datastructure and prints the results.

For example, given

```
$ cat que-tar.syn
que   100    200    tar   1100   1200   100   +
que   1100   1800   tar   1500   1900   100   +
que   1400   1700   tar   1600   2000   100   +
que   1200   1600   tar   1700   2100   100   +
que   1300   1900   tar   1800   2200   100   +
$ synder dump -s que-tar.syn -x p
que     100     200     tar     1100    1200    101.000000      +       0
que     1100    1900    tar     1500    2200    700.084964      +       0
```

This function is mostly useful for debugging. In the above example, `synder
dump` shows that blocks 2-5 are merged (since they are doubly-overlapping) and
shows the results of the score transformations. It is this dumped synteny map
that would have been used in any `synder filter` or `synder search` operations.

Also note the addition of a 9th column. This column specifies that contiguous
set. In this case, all blocks, after merging, are in the same set.

The output of `synder dump` may also be useful for plotting search intervals in
context.


# Definitions

 * query genome - the reference genome of input intervals

 * target genome - the genome to which input intervals are mapped

 * synteny map - a set of query/target interval pairs inferred to be syntenic
 
 * block - a single pair of intervals from the synteny map

 * contig - a chromosome, scaffold, contig (`synder` doesn't distinguish between them)

 * interval adjacency - two intervals are adjacent if they are on the same
   contig and no other interval is fully contained between them

 * block adjacency - two blocks are adjacent if 1) the intervals are adjacent on
   both the query and target sides and 2) both have the same sign

 * query context - all blocks that overlap or are adjacent to the query interval

 * contiguous set - a set where block *i* is adjacent to block *i+1*

 * search intervals - a set of intervals on the target genome where the
   ortholog of the query interval is expected to be (the ortholog search space)

# Algorithm

Execution order for the `synder search` command
 1.  load synteny map
 2.  transform scores
 3.  merge doubly-overlapping blocks
 4.  determine contiguous sets
 5.  find query-side overlapping and flanking blocks for each input sequence
 6.  map to overlapping contiguous sets
 7.  calculate score for input sequence relative to each contiguous set
 8.  calculate search interval relative to each contiguous set

## 1. Load Synteny Map

The input synteny map must have the following columns:

 1. qseqid - query contig id (e.g. Chr1)
 2. qstart - query interval start
 3. qstop  - query interval stop
 4. sseqid - target contig id
 5. sstart - target interval start
 6. sstop  - target interval stop
 7. score  - score of the syntenic match
 8. strand - relative orientation

## 2. Transform scores

Internally, scores for each block are assumed to be additive. However, there
are many possible forms of input scores. Low numbers may represent good matches
(e.g. e-value) or high numbers (e.g. bit scores). Scores may be additive
(bitscores) or averaged (percent similarity).

For this reason, the user must specify a transform for the score column (column
7 of synteny map). This transform can be one of the following:

 * `S' = S`           -- default, no transformation)
 * `S' = L * S`       -- transform from score densities
 * `S' = L * S / 100` -- transform from percent identity
 * `S' = -log(S)`     -- transform from e-values or p-values

Where S is input score, S' is the transformed score, and L interval length

## 3. Merge doubly-overlapping blocks

If two blocks overlap on both the query and target side, they should be merged.
In a perfect synteny map, this would never occur. But in practice, it is
common, and convolutes the formation of contiguous sets.

For example the following syntenic map

```
que 100 200 tar 100 200 100 +
que 300 400 tar 300 400 100 +
que 310 390 tar 310 390 100 +
que 500 600 tar 500 600 100 +
```

would be separated into two contiguous sets with the bounds (100, 400) and
(310, 600).

Now if a input intervals at (que, 250, 450) is searched, two search intervals
with the same bounds will obtain: (200, 500) and (200, 500). Each will be
flagged as unbound (e.g. an interval edge is between contiguous sets).

However, if the second and third blocks are merged, we obtain a single search
interval flagged as bound on both ends.

In practice, many repetitive regions of the genome can be massively
doubly-overlapping, resulting in many redundant search intervals. This throws
of statistics and wastes time searching extra space.

To avoid such problems, `synder` merges any blocks that overlap on both the
query and target sides.

The bounds of the merged blocks are simply the union.

But the scores also need to be merged. Originally, I used the equation:

 da (la - lo) + db (lb - lo) + lo (da + db) / 2            E1

Where
 - *da* and *db* are the score densities of blocks *a* and *b*
 - *la*, *lb* and *lo* are the lengths of *a*, *b*, and their overlap

E1 is problematic for two reasons. First, it doesn't really make sense
to average the overlap scores. Second, iterative pairwise averaging is
assymetric, giving higher weight to the blocks merged later.

I replaced this approach with taking the max of the overlap scores:

 da (la - lo) + db (lb - lo) + lo * max(da, db)            E2

This fixes the first problem.

The second problem is a bit trickier, since it would require tracking
sub-intervals. It would be a pain to implement and probably would have
little effect on any real dataset. So I am content with an imperfect
solution for now.


## 4. Determine contiguous sets

**OUT-OF-DATE**

Build an interval adjacency matrix for the query context intervals and for the
target context intervals. AND them together to get a block adjacency matrix.
From this matrix, extract paths of adjacent blocks. Synder will merge any
blocks that overlap on both genomes, this ensures there is a unique path
through the adjacency matrix.

## 5. Find overlapping/flanking blocks

**INCOMPLETE**

## 6. Reduce to overlapping sets

**INCOMPLETE**

## 7. Calculate scores relative to contiguous sets

**INCOMPLETE**

## 8. Calculate search interval relative to contiguous set

**INCOMPLETE**

 1. Find input interval, *i*, context in the query genome. This context consists of
    all query intervals *Sq* that either overlap or flank the input.

 2. From *Sq* determine which contiguous sets ,*Sc*, the query overlaps or borders.

 3. For each contiguous set, *c*, in *Sc*, classify each bound of *i* as:

    - **anchored** - if overlaps a member of *c*
    - **bound** - if is inbetween two members of *c*
    - **unbound** - if is more extreme than any member of the set, but is inbetween two contiguous sets
    - **extreme** - No entr is found

 
# TODO list

 - [x] Add strand awareness to contiguity rules (so all contiguous sets are elements on the same strand)
 - [x] Determine direction of SI based on strand
 - [x] Snap search space boundaries for to nearest block on target side
 - [x] write tests for 3rd gen cases 1-3
 - [x] debug contiguous set builder for cases 1-3
 - [x] implement contiguous\_set structures
 - [x] write tests for case 4
 - [x] debug contiguous set builder for case 4
 - [x] write tests for dedicated double-overlapper test
 - [x] implement double-overlapper merging
 - [x] add score transforms to positive additive
 - [x] write search interval score to output
 - [x] write contiguous set id to output
 - [x] write test code for merge scores
 - [ ] write test code for search interval scores
 - [x] test against fagin
 - [x] refactor to c++
 - [x] clean up IO
 - [x]  - replace getopt
 - [x]  - incorporate subcommands
 - [x]  - allow reading of GFF files with string sequence names
 - [ ]  - improve input file type checking, fail on misformatted files
 - [ ]  - extract name from GFF 9th column, i.e `s/.*ID=([^;]+).*/\1/`.
 - [x]  - if we get an argument that is not in the subcommands list, should die
 - [x] directly parse synteny files, no database Bash script
 - [x] implement filter
 - [ ] write tests for filter
 - [x] reimplement dump blocks
 - [x] add quiet mode to runtests.sh
 - [ ] add score to filter
 - [ ] add print statements to Contig, ManyContiguousSet, ManyBlocks, etc
 - [ ] implement assembly checking
 - [ ] update README documentation
 - [ ] update Doxygen documentation
 - [ ] make Github wiki
 - [ ] make Github pages site

# Theoretical stuff

I do not know a good way to do these, nor am I certain of their value.

 - [ ] implement target side scoring
 - [ ] implement score thresholding
 - [ ] implement contiguous set scoring

#include "search_interval.h"

SearchInterval::SearchInterval(
    const std::array<Block*,2>& t_ends,
    Feature* t_feat,
    bool t_inbetween,
    double r
)
    : m_feat(t_feat),
      m_inbetween(t_inbetween),
      m_bnds(t_ends)
{
    m_score = calculate_score(m_bnds[0], r);

    reduce_side(LO);
    reduce_side(HI);

    m_inverted = m_bnds[0]->over->strand == '-';

    get_si_bound(LO);
    get_si_bound(HI);
}

SearchInterval::~SearchInterval() { }


bool SearchInterval::feature_overlap(Feature* other)
{
    bool same_contig = other->parent_name == m_bnds[0]->over->parent->name;
    bool search_interval_overlap = overlap(other);
    return search_interval_overlap && same_contig;
}

void SearchInterval::reduce_side(const Direction d){
    while(m_bnds[d]->cnr[!d] != nullptr && REL_GT(m_bnds[d]->cnr[!d]->pos[d], m_feat->pos[d], d)){
        m_bnds[d] = m_bnds[d]->cnr[!d];
    }
}

void SearchInterval::add_row(SIType& stype) {
    stype.add_row(
                                        // Output column ids:
         m_feat->name,                  //  1
         m_feat->parent_name,           //  2
         m_feat->start(),               //  3
         m_feat->stop(),                //  4
         m_bnds[0]->over->parent->name, //  5
         start(),                       //  6
         stop(),                        //  7
         m_bnds[0]->over->strand,       //  8
         m_score,                       //  9
         m_bnds[0]->cset->id,           // 10
         m_flag[0],                     // 11
         m_flag[1],                     // 12
         m_inbetween                    // 13
    );
}

void SearchInterval::get_si_bound(const Direction d)
{

    // Invert orientation mapping to target if search interval is inverted
    Direction vd = (Direction) (m_inverted ? !d : d);
    // See contiguous.h
    int flag = 0;
    // non-zero to ease debugging
    long bound = 444444;

    long q = m_feat->pos[d];

    const auto& set_bounds = m_bnds[0]->cset->pos;

    // All diagrams are shown for the d=HI case, take the mirror image fr d=LO.
    //
    // KEY:
    // |x--  --y| - bounds of the contiguous block; start==x, stop==y
    // a========b - a syntenic block with start == a and stop == b
    //   <---q    - the query interval, with stop == q (start doesn't matter)
    // a==b--c==d - query bounding blocks in the contiguous set
    // [===]      - a non-bounding block in the same contiguous set
    //  ...  F=== - nearest non-adjacent block ***ON THE TARGET***, F=start
    //      ^     - search interval bound
    //
    // Possible snap positions (relative to query)
    //   |x...[===]-----a=======b-----c=======d-----[===]...y|  ...  F===
    //                 ^        ^    ^        ^    ^                ^

    // Positions of a, b, c, and d (as shown above)
    long pnt_a = m_bnds[!d]->pos[!d];
    long pnt_b = m_bnds[!d]->pos[ d];
    long pnt_c = m_bnds[ d]->pos[!d];
    long pnt_d = m_bnds[ d]->pos[ d];


    // This may occur when there is only one element in the ContiguousSet
    //          |x-----y|
    //       ...a=======b
    //         ^
    //   <---q
    // q < x
    if(REL_LT(q, set_bounds[!d], d)) {
        bound = m_bnds[!d]->over->pos[!vd];
        flag = UNBOUND;
    }

    //   |x---[===]-------a=======b...y|   ...    F===
    //                   ^
    //              --q
    // q < a
    else if(REL_LT(q, pnt_a, d)) {
        bound = m_bnds[!d]->over->pos[!vd];
        flag = BOUND;
    }

    //   |x...a=======b...y|   ...    F===
    //                ^
    //        <---q
    // q < b
    else if(REL_LE(q, pnt_b, d)) {
        bound = m_bnds[!d]->over->pos[vd];
        flag = ANCHORED;
    }

    //   |x...a=======b-------c=======d...y|  ...  F===
    //                       ^
    //                  <--q
    // q < c && q > b
    //   (q > b test required since m_bnds[LO] can equal m_bnds[HI])
    else if(REL_LT(q, pnt_c, d) && REL_GT(q, pnt_b, d)) {
        bound = m_bnds[d]->over->pos[!vd];
        flag = BOUND;
    }

    //   |x...a=======b-------c=======d...y|  ...  F===
    //                                ^
    //             <--------------q
    // q < d
    else if(REL_LE(q, pnt_d, d)) {
        bound = m_bnds[d]->over->pos[vd];
        flag = ANCHORED;
    }

    //   |x...a=======b-------c=======d-------[===]...y|  ...  F===
    //                                       ^
    //              <----------------------q
    // q < y, (which implies there is a node after d)
    else if(REL_LE(q, set_bounds[d], d)) {
        bound = m_bnds[d]->cnr[d]->over->pos[!vd];
        flag = BOUND;
    }

    // If none of the above, the bound beyond anything in the contiguous set
    // In this case, the hi and lo Contiguous nodes will be the same
    else {
        // Get nearest non-overlapping sequence
        Block* downstream_blk = m_bnds[d]->over->corner_adj(vd);

        // adjacent block on TARGET side exists
        //    |x...--a=======b|
        //    |x...--c=======d|  ...  F===
        //                           ^
        //                    <---q
        if(downstream_blk != nullptr) {
            flag = UNBOUND;
            bound = downstream_blk->pos[!vd];
        }
        //    |x...--a=======b|
        //    |x...--c=======d|  ...  THE_END
        //                    <---q
        // query is further out than ANYTHING in the synteny map
        else {
            bound = vd ? m_bnds[d]->over->parent->parent_length - 1 : 0;
            flag = m_bnds[d]->over->pos[vd] == bound ? EXTREME : BEYOND;
        }
    }

    // 0 if (((-) and d==0, looking for last)  OR
    //       ((+) and d==1, looking for first))
    // 1 otherwise
    size_t i  = m_inverted ^ d;
    m_flag[i] = flag;
    pos[i]  = bound;
}


// Arguments:
//  near - the distance from the block to the expected near end of the query
//  far  - the distance from the block to the expected far end of the query
//      
//                      Q                Q
//         T       b1       b2   |  b1       b2       T
//      a1   a2    |=========|   |  |=========|    a1   a2
//      |=====|    |             |                 |=====|
//      |<-------->| far         |       near |<-->|
//            |<-->| near        |        far |<-------->|
double SearchInterval::flank_area(long near, long far, double r)
{

    // If far <= 0, this means there is no interval to score in this direction
    if(far > 0) {
        // Adjust the near boundary, if needed, for example:
        //             b1        b2
        //             |=========|
        //      a1       |   a2
        //      |============|
        //      |<------>|     far
        //               |<->| near (negative value)
        //               |     snap near to relative 0
        near = near > 0 ? near : 0;

        // the weight falls exponentially with distance from the query, e.g.
        // $$ \int_{near}^{far} exp(-kx) dx $$
        // which evaluates to the following:
        double area
            // This a bit of a hack. But I want users to be able to set r to 0.
            = (r = 0)
            ? far - near + 1 // The limit as r goes to 0
            : (1 / r) * ( exp(-1 * r * near) - exp(-1 * r * far) );
    }
    return 0;
}

double SearchInterval::calculate_score(Block* b, double r)
{

    double score = 0;

    if(b == nullptr)
        return score;

    // rewind
    while(b->cnr[0] != nullptr) {
        b = b->cnr[0];
    }

    Feature* a  = m_feat;
    long     a1 = a->start();

    for(; b != nullptr ; b = b->cnr[1]) {
        long b1 = b->start();
        long b2 = b->stop();

        //               a1        a2
        //      b1  b2   |=========|    query interval
        //      |===|    |              syntenic interval
        //      |---|....|              interval to score
        // near difference := i1 = a1 - b2
        // far difference  := i2 = a1 - b1
        // i1 and i2 may be negative, if query is not in the above position
        double weighted_length = flank_area(a1 - b2, a1 - b1, r) +

                          //    a1        a2
                          //    |=========|    b1  b2     query interval
                          //              |....|---|      syntenic interval
                          //                   |===|      interval to score
                          // near difference := i1 = b1 - a2
                          // far difference  := i2 = b1 - a1
                          flank_area(b1 - a1, b2 - a1, r) +

                          //         a1        a2
                          //         |=========|         query interval
                          //             |=====|=====|   syntenic interval
                          //             b1    |     b1
                          //             |-----|         overlapping interval
                          //             i1    i2
                          a->overlap_length(b);

        // NOTE: I am kind of adding length to area here, but it actually
        // works. `_flank_area` returns the area of a segment of the base
        // exponentional (i.e. where f(0) = 1). Multiplying this base
        // exponential area by the syntenic link score, gives the final score
        // for the non-overlapping segment. In the same way, multiplying the
        // overlapping segment length by the score, gives the overlapping
        // segmental score.

        long actual_length = b->pos[1] - b->pos[0] + 1;

        score += b->score * weighted_length / actual_length;
    }
    return score;
}

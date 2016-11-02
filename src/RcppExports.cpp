// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// c_logical_strand
Rcpp::IntegerVector c_logical_strand(Rcpp::CharacterVector cv);
RcppExport SEXP synder_c_logical_strand(SEXP cvSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type cv(cvSEXP);
    rcpp_result_gen = Rcpp::wrap(c_logical_strand(cv));
    return rcpp_result_gen;
END_RCPP
}
// c_dump
Rcpp::DataFrame c_dump(std::string filename, bool swap, char trans);
RcppExport SEXP synder_c_dump(SEXP filenameSEXP, SEXP swapSEXP, SEXP transSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type filename(filenameSEXP);
    Rcpp::traits::input_parameter< bool >::type swap(swapSEXP);
    Rcpp::traits::input_parameter< char >::type trans(transSEXP);
    rcpp_result_gen = Rcpp::wrap(c_dump(filename, swap, trans));
    return rcpp_result_gen;
END_RCPP
}
// c_search
Rcpp::DataFrame c_search(std::string synfilename, std::string gfffilename, std::string tclfilename, std::string qclfilename, bool swap, int k, double r, char trans);
RcppExport SEXP synder_c_search(SEXP synfilenameSEXP, SEXP gfffilenameSEXP, SEXP tclfilenameSEXP, SEXP qclfilenameSEXP, SEXP swapSEXP, SEXP kSEXP, SEXP rSEXP, SEXP transSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type gfffilename(gfffilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type tclfilename(tclfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type qclfilename(qclfilenameSEXP);
    Rcpp::traits::input_parameter< bool >::type swap(swapSEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< double >::type r(rSEXP);
    Rcpp::traits::input_parameter< char >::type trans(transSEXP);
    rcpp_result_gen = Rcpp::wrap(c_search(synfilename, gfffilename, tclfilename, qclfilename, swap, k, r, trans));
    return rcpp_result_gen;
END_RCPP
}
// c_filter
Rcpp::CharacterVector c_filter(std::string synfilename, std::string intfilename, bool swap, int k, double r, char trans);
RcppExport SEXP synder_c_filter(SEXP synfilenameSEXP, SEXP intfilenameSEXP, SEXP swapSEXP, SEXP kSEXP, SEXP rSEXP, SEXP transSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type intfilename(intfilenameSEXP);
    Rcpp::traits::input_parameter< bool >::type swap(swapSEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< double >::type r(rSEXP);
    Rcpp::traits::input_parameter< char >::type trans(transSEXP);
    rcpp_result_gen = Rcpp::wrap(c_filter(synfilename, intfilename, swap, k, r, trans));
    return rcpp_result_gen;
END_RCPP
}
// c_map
Rcpp::DataFrame c_map(std::string synfilename, std::string gfffilename, bool swap);
RcppExport SEXP synder_c_map(SEXP synfilenameSEXP, SEXP gfffilenameSEXP, SEXP swapSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type gfffilename(gfffilenameSEXP);
    Rcpp::traits::input_parameter< bool >::type swap(swapSEXP);
    rcpp_result_gen = Rcpp::wrap(c_map(synfilename, gfffilename, swap));
    return rcpp_result_gen;
END_RCPP
}
// c_count
Rcpp::DataFrame c_count(std::string synfilename, std::string gfffilename, bool swap);
RcppExport SEXP synder_c_count(SEXP synfilenameSEXP, SEXP gfffilenameSEXP, SEXP swapSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type gfffilename(gfffilenameSEXP);
    Rcpp::traits::input_parameter< bool >::type swap(swapSEXP);
    rcpp_result_gen = Rcpp::wrap(c_count(synfilename, gfffilename, swap));
    return rcpp_result_gen;
END_RCPP
}

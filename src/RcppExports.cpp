// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// dump
void dump(std::string filename);
RcppExport SEXP synder_dump(SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type filename(filenameSEXP);
    dump(filename);
    return R_NilValue;
END_RCPP
}
// search
void search(std::string synfilename, std::string intfilename);
RcppExport SEXP synder_search(SEXP synfilenameSEXP, SEXP intfilenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type intfilename(intfilenameSEXP);
    search(synfilename, intfilename);
    return R_NilValue;
END_RCPP
}
// filter
void filter(std::string synfilename, std::string intfilename);
RcppExport SEXP synder_filter(SEXP synfilenameSEXP, SEXP intfilenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type intfilename(intfilenameSEXP);
    filter(synfilename, intfilename);
    return R_NilValue;
END_RCPP
}
// map
void map(std::string synfilename, std::string intfilename);
RcppExport SEXP synder_map(SEXP synfilenameSEXP, SEXP intfilenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type intfilename(intfilenameSEXP);
    map(synfilename, intfilename);
    return R_NilValue;
END_RCPP
}
// count
void count(std::string synfilename, std::string intfilename);
RcppExport SEXP synder_count(SEXP synfilenameSEXP, SEXP intfilenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type synfilename(synfilenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type intfilename(intfilenameSEXP);
    count(synfilename, intfilename);
    return R_NilValue;
END_RCPP
}

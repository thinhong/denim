#' discreteModel
#'
#' My cool package
#'
#' Imports
#' @useDynLib discreteModel, .registration = TRUE
#' @export simcm runSim gamma exponential values print.Distribution
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

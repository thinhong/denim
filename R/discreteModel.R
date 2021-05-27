#' discreteModel
#'
#' My cool package
#'
#' Imports
#' @useDynLib discreteModel, .registration = TRUE
#' @export simcm runSim gamma weibull exponential values print.Distribution newFullModel
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

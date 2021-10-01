#' discreteModel
#'
#' My cool package
#'
#' Imports
#' @useDynLib discreteModel, .registration = TRUE
#' @export simcm runSim gamma weibull exponential frequency mathExpression values transitionProb
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

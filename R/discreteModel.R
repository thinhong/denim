#' discreteModel
#'
#' My cool package
#'
#' Imports
#' @useDynLib discreteModel, .registration = TRUE
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

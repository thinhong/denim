#' denim
#'
#' Simulate deterministic model
#'
#' Imports
#' @useDynLib denim, .registration = TRUE
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

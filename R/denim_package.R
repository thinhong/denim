#' denim
#'
#' Simulate deterministic discrete time model
#'
#' Imports
#' @useDynLib denim, .registration = TRUE
#' @import Rcpp
"_PACKAGE"

Rcpp::loadModule(module = "simcm", TRUE)

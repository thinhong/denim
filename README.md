
<!-- README.md is generated from README.Rmd. Please edit that file -->

# denim <img src="man/figures/logo.svg" align="right" width="120" />

<!-- badges: start -->

[![Project Status: Active – The project has reached a stable, usable
state and is being actively
developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
[![R-CMD-check](https://github.com/thinhong/denim/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/thinhong/denim/actions/workflows/R-CMD-check.yaml)
[![Codecov test
coverage](https://codecov.io/gh/thinhong/denim/branch/master/graph/badge.svg)](https://app.codecov.io/gh/thinhong/denim?branch=master)
<!-- badges: end -->

An R package for building and simulating deterministic compartmental models with memory.

## Installation

You can install denim from CRAN with:

``` r
install.packages("denim")
```

Or install the development version of denim from
[GitHub](https://github.com/) with:

``` r
# install.packages("devtools")
devtools::install_github("thinhong/denim")
```

## Example

This is a basic example to illustrate the specification of a simple SIR
model, which contains three compartments susceptible (S), infected (I)
and recovered (R). The recovery probabilities of infected individuals
are gamma distributed in this example:

``` r
library(denim)

transitions <- denim_dsl({
  S -> I = beta * S * I / N
  I -> R = d_gamma(rate = 1/3, shape = 2)
})

parameters <- c(
  beta = 1.2,
  N = 1000
)

initialValues <- c(
  S = 999, 
  I = 1, 
  R = 0
)

simulationDuration <- 20
timeStep <- 0.01

mod <- sim(transitions = transitions, initialValues = initialValues, 
           parameters = parameters, simulationDuration = simulationDuration, 
           timeStep = timeStep)
```

The output is a data frame with 4 columns: `Time`, `S`, `I` and `R`

``` r
head(mod)
#>   Time        S        I            R
#> 1 0.00 999.0000 1.000000 0.000000e+00
#> 2 0.01 998.9880 1.011982 5.543225e-06
#> 3 0.02 998.9759 1.024097 2.219016e-05
#> 4 0.03 998.9636 1.036346 5.000038e-05
#> 5 0.04 998.9512 1.048730 8.903457e-05
#> 6 0.05 998.9386 1.061252 1.393545e-04
```

We can plot the output with:

``` r
plot(mod, ylim = c(1, 1000))
```

![](man/figures/README-example-plot-1.png)<!-- -->

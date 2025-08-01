% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/distribution.R
\name{nonparametric}
\alias{nonparametric}
\title{Nonparametric distribution transition}
\usage{
nonparametric(x, dist_init = FALSE)
}
\arguments{
\item{x}{a vector of values}

\item{dist_init}{whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment))}
}
\value{
a Transition object for simulator
}
\description{
Convert a vector of frequencies, percentages... into a distribution
}
\examples{
transitions <- list("S->I"=nonparametric( c(0.1, 0.2, 0.5, 0.2) ))
transitions <- denim_dsl({S->I=nonparametric( c(0.1, 0.2, 0.5, 0.2) )})
# you can also define a model parameter for the distribution
transitions <- denim_dsl({S->I=nonparametric( dwelltime_dist )})
}

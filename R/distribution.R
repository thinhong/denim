# Constructors for distributions

#' Discrete gamma distribution
#' 
#' @param rate rate parameter of a gamma distribution
#' @param shape shape parameter of a gamma distribution
#' @param dist_init whether to distribute initial value across subcompartments following this distribution.
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("S -> I" = d_gamma(rate = 1, shape = 5))
#' @export
d_gamma <- function(rate, shape, dist_init = FALSE) {
  distr <- list(
    distribution = "gamma",
    rate = rate,
    shape = shape, 
    dist_init = as.numeric(dist_init)
    )
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Discrete Weibull distribution
#' 
#' @param scale scale parameter of a Weibull distribution
#' @param shape shape parameter of a Weibull distribution
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_weibull(0.6, 2))
#' @export
d_weibull <- function(scale, shape, dist_init = FALSE) {
  distr <- list(
    distribution = "weibull",
    scale = scale,
    shape = shape,
    dist_init = as.numeric(dist_init)
    )
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' Discrete exponential distribution
#' 
#' @param rate rate parameter of an exponential distribution
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_exponential(0.3))
#' 
#'
#' @export
d_exponential <- function(rate, dist_init = FALSE) {
  distr <- list(
    distribution = "exponential",
    rate = rate,
    dist_init = as.numeric(dist_init)
  )
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' Discrete log-normal distribution
#' 
#' @param mu location parameter or the ln mean
#' @param sigma scale parameter or ln standard deviation
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_lognormal(3, 0.6))
#' @export
d_lognormal <- function(mu, sigma, dist_init = FALSE) {
  distr <- list(
    distribution = "lognormal",
    mu = mu,
    sigma = sigma,
    dist_init = as.numeric(dist_init)
    )
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' Mathematical expression
#'
#' @param expr User defined mathematial expression. he expression will be processed by 
#' muparser library which offers a wide variety of operators. Visit 
#' muparser website (https://beltoforion.de/en/muparser/features.php) 
#' to see full list of available operators.
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("S->I"=mathexpr("beta*S/N"))
#' # definition for parameters in the expression required
#' params <- c(N = 1000, beta = 0.3)
#' @export
mathexpr <- function(expr) {
  distr <- list(
    distribution = "mathExpression",
    expression = expr)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Fixed transition
#' 
#' Define a fixed number of individuals of the left compartment transit to the 
#' right compartment at every time step
#'
#' @param x number of individuals who move from one compartment to another
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("S->I" = constant(10))
#' @export
constant <- function(x) {
  distr <- list(
    distribution = "constant",
    constant = x)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Transition probability
#' 
#' A fixed percentage of the left compartment transit to the right compartment 
#' at every time step
#' 
#' @param x a float number between 0 to 1
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("S->I"=transprob(0.8))
#' @export
transprob <- function(x) {
  distr <- list(
    distribution = "transitionProb",
    transitionProb = x)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Nonparametric distribution
#' 
#' Convert a vector of frequencies, percentages... into a distribution
#' 
#' @param ... a vector of values
#' @return a Distribution object for simulator
#' 
#' @examples
#' transitions <- list("S->I"=nonparametric(0.1, 0.2, 0.5, 0.2))
#' @export
nonparametric <- function(...) {
  distr <- list(
    distribution = "nonparametric",
    waitingTime = c(...))
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Multinomial
#' 
#' Define a set of probabilities of transition from one compartment to multiple
#' compartments
#' ```
#' "I -> R" = d_gamma(1/3, 2),
#' "I -> D" = d_lognormal(2, 0.5)
#' ```
#' is equal to
#' ```
#' "0.5 * I -> R" = d_gamma(1/3, 2),
#' "0.5 * I -> D" = d_lognormal(2, 0.5)
#' ```
#'
#' @param ... a vector of probabilities. Vector is automatically rescaled to sum to 1. 
#' @return a Distribution object for simulator
#'
#' @export
multinomial <- function(...) {
  distr <- list(
    distribution = "multinomial",
    probabilities = c(...))
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' @export
print.Distribution <- function(x, ...) {
  # Print the name of this distribution
  if (x$distribution %in% c("gamma", "weibull", "exponential")) {
    cat("Discretized", x$distribution, "distribution\n")
    if (x$distribution == "weibull") {
      cat("Scale = ", x$scale, ", Shape = ", x$shape, sep = "")
    } 
    else if (x$distribution == "gamma"){
      cat("Rate = ", x$rate, ", Shape = ", x$shape, sep = "")
    }
    else if (x$distribution == "exponential") {
      cat("Rate = ", x$rate, sep = "")
    }
    else if (x$distribution == "lognormal") {
      cat("Mu = ", x$mu, ", Sigma = ", x$sigma, sep = "")
    }
  } 
  else if (x$distribution == "mathExpression") {
    cat("Math expression: ")
    cat(x$expression, sep = "")
  } 
  else if (x$distribution == "constant") {
    cat("Constant: ")
    cat(x$constant, sep = "")
  } 
  else if (x$distribution == "transitionProb") {
    cat("Transition probability: ")
    cat(x$transitionProb, sep = "")
  } 
  else if (x$distribution == "nonparametric") {
    cat("Waiting time values: ")
    wt <- utils::head(x$waitingTime, 5)
    wt <- paste0(wt, collapse = ", ")
    cat(wt, sep = "")
    if (length(x$waitingTime) > 5) cat("...")
  } 
  else if (x$distribution == "multinomial") {
    cat("Probabilities: ")
    wt <- utils::head(x$probabilities, 5)
    wt <- paste0(wt, collapse = ", ")
    cat(wt, sep = "")
    if (length(x$probabilities) > 5) cat("...")
  }
  cat("\n")
  invisible(x)
}

# Distribution object to json
#
# Input is a distribution object of a compartment, not the full vector/list 
# of distributions
# distribution: a list with elements $name, $rate / $scale / $shape...
# return a json object that match format {"distribution": "weibull", "scale": 2, "shape": 5}
distributionToJson <- function(distribution) {
  contents <- c()
  if (distribution$distribution == "nonparametric") {
    dn <- newJsonKeyPair(key = "distribution", value = "nonparametric")
    wt <- newJsonKeyPair(key = "waitingTime", value = newJsonArray(distribution$waitingTime))
    contents <- c(dn, wt)
  } 
  else if (distribution$distribution == "multinomial") {
    dn <- newJsonKeyPair(key = "distribution", value = "multinomial")
    wt <- newJsonKeyPair(key = "probabilities", value = newJsonArray(distribution$probabilities))
    contents <- c(dn, wt)
  }
  else {
    for (i in 1:length(distribution)) {
      key <- names(distribution)[i]
      val <- distribution[[i]]
      kp <- newJsonKeyPair(key = key, value = val)
      contents <- c(contents, kp)
    }
  }
  newJsonObject(contents, inline = TRUE)
}

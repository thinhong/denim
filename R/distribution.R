# Constructors for distributions

#' Discrete gamma distribution
#' 
#' @param scale scale parameter of a gamma distribution
#' @param shape shape parameter of a gamma distribution
#'
#' @export
d_gamma <- function(scale, shape) {
  distr <- list(
    distribution = "gamma",
    scale = scale,
    shape = shape)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Discrete Weibull distribution
#' 
#' @param scale scale parameter of a Weibull distribution
#' @param shape shape parameter of a Weibull distribution
#'
#' @export
d_weibull <- function(scale, shape) {
  distr <- list(
    distribution = "weibull",
    scale = scale,
    shape = shape)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' Discrete exponential distribution
#' 
#' @param rate rate parameter of an exponential distribution
#'
#' @export
d_exponential <- function(rate) {
  distr <- list(
    distribution = "exponential",
    rate = rate)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}

#' Discrete log-normal distribution
#' 
#' @param mu location parameter or the ln mean
#' @param sigma scale parameter or ln standard deviation
#'
#' @export
d_lognormal <- function(mu, sigma) {
  distr <- list(
    distribution = "lognormal",
    mu = mu,
    sigma = sigma)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


# Mathematical expression
# 
# User-defined mathematical expression. The expression will be processed by 
# muparser library which offers a wide variety of operators. Visit 
# muparser website (https://beltoforion.de/en/muparser/features.php) 
# to see full list of available operators.
mathexpr <- function(expr) {
  distr <- list(
    distribution = "mathExpression",
    expression = expr)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


# Constant
# 
# Define a fixed number of individuals of the left compartment transit to the 
# right compartment at every time step
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
#'
#' @export
transprob <- function(x) {
  distr <- list(
    distribution = "transitionProb",
    transitionProb = x)
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


#' Nonparametric
#' 
#' Convert a vector of frequencies, percentages... into a distribution
#' 
#' @param ... a vector of values
#'
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
#' "I -> R, D" = multinomial(0.9, 0.1),
#' "I -> R" = d_gamma(3, 2),
#' "I -> D" = d_lognormal(2, 0.5)
#' ```
#' is equal to
#' ```
#' "0.9 * I -> R" = d_gamma(3, 2),
#' "0.1 * I -> D" = d_lognormal(2, 0.5)
#' ```
#'
#' @param ... a vector of probabilities, must add up to 1
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
    if (x$distribution %in% c("gamma", "weibull")) {
      cat("Scale = ", x$scale, ", Shape = ", x$shape, sep = "")
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
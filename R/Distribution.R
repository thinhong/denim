# Constructors
## Gamma distribution
#' @export
gamma <- function(scale, shape) {
  distr <- list()
  
  distr$distribution <- "gamma"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Weibull distribution
#' @export
weibull <- function(scale, shape) {
  distr <- list()
  
  distr$distribution <- "weibull"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Exponential distribution
#' @export
exponential <- function(rate) {
  distr <- list()
  
  distr$distribution <- "exponential"
  distr$rate <- rate
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Log-normal distribution
#' @export
lognormal <- function(mu, sigma) {
  distr <- list()
  
  distr$distribution <- "lognormal"
  distr$mu <- mu
  distr$sigma <- sigma
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Math expression
#' @export
mathExpression <- function(expression) {
  distr <- list()
  
  distr$distribution <- "mathExpression"
  distr$expression <- deparse(substitute(expression))
  distr$expression <- gsub("\\\"", "", distr$expression)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Constant
#' @export
constant <- function(x) {
  distr <- list()
  
  distr$distribution <- "constant"
  distr$constant <- x
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Transition probability
#' @export
transitionProb <- function(x) {
  distr <- list()
  
  distr$distribution <- "transitionProb"
  distr$transitionProb <- x
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Input with raw values
#' @export
nonparametric <- function(...) {
  distr <- list()
  
  distr$distribution <- "nonparametric"
  distr$waitingTime <- c(...)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Multinomial
#' @export
multinomial <- function(...) {
  distr <- list()
  
  distr$distribution <- "multinomial"
  distr$probabilities <- c(...)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

print.Distribution <- function(x) {
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
    cat(x$frequency, sep = "")
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

#' Distribution object to json
#'
#' Input is a distribution object of a compartment, not the full vector/list 
#' of distributions
#' @param distribution a list with elements $name, $rate / $scale / $shape...
#'
#' @return a json object that match format {"distribution": "weibull", "scale": 2, "shape": 5}
#' @export
#'
#' @examples
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
  obj <- newJsonObject(contents, inline = TRUE)
  return(obj)
}


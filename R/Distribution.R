# Constructors
## Gamma distribution
gamma <- function(scale, shape) {
  distr <- list()
  
  distr$distribution <- "gamma"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Weibull distribution
weibull <- function(scale, shape) {
  distr <- list()
  
  distr$distribution <- "weibull"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Exponential distribution
exponential <- function(rate) {
  distr <- list()
  
  distr$distribution <- "exponential"
  distr$rate <- rate
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Math expression
mathExpression <- function(expression) {
  distr <- list()
  
  distr$distribution <- "mathExpression"
  distr$expression <- deparse(substitute(expression))
  distr$expression <- gsub("\\\"", "", distr$expression)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

frequency <- function(x) {
  distr <- list()
  
  distr$distribution <- "frequency"
  distr$frequency <- x
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

transitionProb <- function(x) {
  distr <- list()
  
  distr$distribution <- "transitionProb"
  distr$transitionProb <- x
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Input with raw values
values <- function(...) {
  distr <- list()
  
  distr$distribution <- "values"
  distr$waitingTime <- c(...)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

print.Distribution <- function(x) {
  # Print the name of this distribution
  if (x$distribution %in% c("gamma", "weibull", "exponential")) {
    cat("Discretized", x$distribution, "distribution\n")
    if (x$distribution %in% c("gamma", "weibull")) {
      cat("Scale = ", x$scale, ", Shape = ", x$shape, sep = "")
    } else if (x$distribution == "exponential") {
      cat("Rate = ", x$rate, sep = "")
    }
  } else if (x$distribution == "mathExpression") {
    cat("Math expression: ")
    cat(x$expression, sep = "")
  } else if (x$distribution == "frequency") {
    cat("Frequency: ")
    cat(x$frequency, sep = "")
  } else if (x$distribution == "transitionProb") {
    cat("Transition probability: ")
    cat(x$transitionProb, sep = "")
  } else if (x$distribution == "values") {
    cat("Raw values: ")
    wt <- head(x$waitingTime, 5)
    wt <- paste0(wt, collapse = ", ")
    cat(wt, sep = "")
    if (length(x$waitingTime) > 5) cat("...")
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
  if (distribution$distribution == "values") {
    dn <- newJsonKeyPair(key = "distribution", value = "values")
    wt <- newJsonKeyPair(key = "waitingTime", value = newJsonArray(distribution$waitingTime))
    contents <- c(dn, wt)
  } else {
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


# Constructor
## Gamma distribution
gamma <- function(scale, shape) {
  distr <- list()
  
  distr$name <- "gamma"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- "Distribution"
  return(distr)
}

## Weilbull distribution
weibull <- function(scale, shape) {
  distr <- list()
  
  distr$name <- "weibull"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- "Distribution"
  return(distr)
}

## Exponential distribution
exponential <- function(rate) {
  distr <- list()
  
  distr$name <- "exponential"
  distr$rate <- rate
  
  class(distr) <- "Distribution"
  return(distr)
}

## Input with raw values
values <- function(...) {
  distr <- list()
  
  distr$name <- "custom"
  distr$values <- c(...)
  
  class(distr) <- "Distribution"
  return(distr)
}

# Generic print method to print out what we want the users to see
print.Distribution <- function(x) {
  # Print the name of this distribution
  if (x$name %in% c("gamma", "exponential")) {
    cat("Discretized", x$name, "distribution\n")
  }
  # Print other parameters
  if (x$name == "gamma") {
    cat("Scale = ", x$scale, ", Shape = ", x$shape, sep = "")
    
  } else if (x$name == "exponential") {
    cat("Rate = ", x$rate, sep = "")
    
  } else if (x$name =="custom") {
    cat("Raw values")
    for (i in head(x$values, 5)) {
      cat(" ", i, sep = "")
    }
    if (length(x$values) > 5) cat("...")
  }
  cat("\n")
  invisible(x)
}

# Constructors
## Gamma distribution
gamma <- function(scale, shape) {
  distr <- list()
  
  distr$name <- "gamma"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Weibull distribution
weibull <- function(scale, shape) {
  distr <- list()
  
  distr$name <- "weibull"
  distr$scale <- scale
  distr$shape <- shape
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Exponential distribution
exponential <- function(rate) {
  distr <- list()
  
  distr$name <- "exponential"
  distr$rate <- rate
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

## Input with raw values
values <- function(...) {
  distr <- list()
  
  distr$name <- "custom"
  distr$values <- c(...)
  
  class(distr) <- c("Distribution", class(distr))
  return(distr)
}

# Tidy up distribution by comparing with initialValues
tidyDistribution <- function(initialValues, distributions) {
  distr <- list()
  cnInitVal <- names(initialValues)
  cnDist <- unique(sapply(names(distributions), function(x) strsplit(x, "\\.")[[1]][[1]]))
  for (compName in cnInitVal) {
    if (compName %in% cnDist) {
      for (i in 1:length(distributions)) {
        cName <- unlist(strsplit(names(distributions)[i], "\\."))[1]
        pName <- unlist(strsplit(names(distributions)[i], "\\."))[2]
        if (cName == compName) {
          distr[[compName]][[pName]] <- distributions[[i]]
        }
      }
    } else {
      distr[[compName]][["name"]] <- "none"
    }
  }
  return(distr)
}

print.Distribution <- function(x) {
  # Print the name of this distribution
  if (x$name %in% c("gamma", "exponential", "weibull")) {
    cat("Discretized", x$name, "distribution\n")
  }
  # Print other parameters
  if (x$name %in% c("gamma", "weibull")) {
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

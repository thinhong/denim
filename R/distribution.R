# Constructors for distributions
evaluate_par <- function(par){
  
  if(is.symbol(par)) {
    par <- as.character(par)
  } else{
    par <- as.numeric(eval(par))
  }
  
  par
}


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
  # capture expression for evaluation
  rate <- substitute(rate)
  shape <- substitute(shape)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  rate <- evaluate_par(rate)
  shape <- evaluate_par(shape)
  
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
  # capture expression for evaluation
  scale <- substitute(scale)
  shape <- substitute(shape)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  scale <- evaluate_par(scale)
  shape <- evaluate_par(shape)
  
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
  # capture expression for evaluation
  rate <- substitute(rate)
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  rate <- evaluate_par(rate)

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
  # capture expression for evaluation
  mu <- substitute(mu)
  sigma <- substitute(sigma)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  mu <- evaluate_par(mu)
  sigma <- evaluate_par(sigma)
  
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


#Fixed transition
constant <- function(x) {
  # capture expression for evaluation
  x <- substitute(x)

  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  x <- evaluate_par(x)

  distr <- list(
    distribution = "constant",
    constant = x)

  class(distr) <- c("Distribution", class(distr))
  distr
}


# transprob <- function(x) {
#   distr <- list(
#     distribution = "transitionProb",
#     transitionProb = x)
# 
#   class(distr) <- c("Distribution", class(distr))
#   distr
# }


#' Nonparametric distribution
#' 
#' Convert a vector of frequencies, percentages... into a distribution
#' 
#' @param ... a vector of values
#' @return a Distribution object for simulator
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment))
#' 
#' @examples
#' transitions <- list("S->I"=nonparametric(0.1, 0.2, 0.5, 0.2))
#' @export
nonparametric <- function(..., dist_init = FALSE) {
  x <- substitute(c(...))

  # remove "c" 
  x <- as.character(x)[-1]
  
  # try parsing ... 
  failed_parse <- FALSE
  try_eval <- sapply(parse(text = x), \(expr){
    tryCatch(
      eval(expr),
      error = \(e) {
        failed_parse <<- TRUE
      }
    )
  })
  x <- if(!failed_parse) {
    try_eval
  } else{
    x
  }

  # if ... is a parameter and more than 1 parameter is given -> throw error
  if(all(is.character(x)) && length(x) > 1){
    stop("nonparametric only takes 1 parameter as input")
  }
  
  distr <- list(
    distribution = "nonparametric",
    waitingTime = x,
    dist_init = as.numeric(dist_init))
  
  class(distr) <- c("Distribution", class(distr))
  distr
}


# multinomial <- function(...) {
#   distr <- list(
#     distribution = "multinomial",
#     probabilities = c(...))
#   
#   class(distr) <- c("Distribution", class(distr))
#   distr
# }


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
    kp <- newJsonKeyPair(key = "dist_init", value = distribution$dist_init)
    contents <- c(dn, wt, kp)
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

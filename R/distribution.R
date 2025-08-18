# Constructors for distributions
evaluate_par <- function(par){
  
  if(is.symbol(par) | is.character(par)) {
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
#' @return a Transition object for simulator
#' 
#' @examples
#' transitions <- list("S -> I" = d_gamma(rate = 1, shape = 5))
#' transitions_dsl <- denim_dsl({S -> I = d_gamma(rate = 1, shape = 5)})
#' # define model parameters as distributional parameters
#' transitions_dsl <- denim_dsl({S -> I = d_gamma(rate = i_rate, shape = i_shape)})
#' @export
d_gamma <- function(rate, shape, dist_init = FALSE) {
  # capture expression for evaluation
  rate <- substitute(rate)
  shape <- substitute(shape)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  rate <- evaluate_par(rate)
  shape <- evaluate_par(shape)
  
  distr <- list(
    transition = "gamma",
    rate = rate,
    shape = shape, 
    dist_init = as.numeric(dist_init)
    )
  class(distr) <- c("Transition", class(distr))
  distr
}


#' Discrete Weibull distribution
#' 
#' @param scale scale parameter of a Weibull distribution
#' @param shape shape parameter of a Weibull distribution
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Transition object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_weibull(0.6, 2))
#' transitions <- denim_dsl({ I -> D = d_weibull(0.6, 2) })
#' @export
d_weibull <- function(scale, shape, dist_init = FALSE) {
  # capture expression for evaluation
  scale <- substitute(scale)
  shape <- substitute(shape)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  scale <- evaluate_par(scale)
  shape <- evaluate_par(shape)
  
  distr <- list(
    transition = "weibull",
    scale = scale,
    shape = shape,
    dist_init = as.numeric(dist_init)
    )
  
  class(distr) <- c("Transition", class(distr))
  distr
}

#' Discrete exponential distribution
#' 
#' @param rate rate parameter of an exponential distribution
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Transition object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_exponential(0.3))
#' transitions <- denim_dsl({I -> D = d_exponential(0.3)})
#' @export
d_exponential <- function(rate, dist_init = FALSE) {
  # capture expression for evaluation
  rate <- substitute(rate)
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  rate <- evaluate_par(rate)

  distr <- list(
    transition = "exponential",
    rate = rate,
    dist_init = as.numeric(dist_init)
  )
  
  class(distr) <- c("Transition", class(distr))
  distr
}

#' Discrete log-normal distribution
#' 
#' @param mu location parameter or the ln mean
#' @param sigma scale parameter or ln standard deviation
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment)
#' @return a Transition object for simulator
#' 
#' @examples
#' transitions <- list("I -> D" = d_lognormal(3, 0.6))
#' transitions <- denim_dsl({I -> D = d_lognormal(3, 0.6)})
#' @export
d_lognormal <- function(mu, sigma, dist_init = FALSE) {
  # capture expression for evaluation
  mu <- substitute(mu)
  sigma <- substitute(sigma)
  
  # check whether it is a fixed value (numeric) or a model parameter (string or expression)
  mu <- evaluate_par(mu)
  sigma <- evaluate_par(sigma)
  
  distr <- list(
    transition = "lognormal",
    mu = mu,
    sigma = sigma,
    dist_init = as.numeric(dist_init)
    )
  
  class(distr) <- c("Transition", class(distr))
  distr
}

#' Mathematical expression
#'
#' @param expr User defined mathematial expression. he expression will be processed by 
#' muparser library which offers a wide variety of operators. Visit 
#' muparser website (https://beltoforion.de/en/muparser/features.php) 
#' to see full list of available operators.
#' @return a Transition object for simulator
#' 
#' @examples
#' transitions <- list("S->I"="beta*S/N")
#' transitions <- denim_dsl({S->I=beta*S/N})
#' # definition for parameters in the expression required
#' params <- c(N = 1000, beta = 0.3)
#' 
#' @keywords internal
#' @export
mathexpr <- function(expr) {
  # change in API, timeStep is now automatically multiplied internally
  pattern <- "\\* *timeStep($| )*"
  if(grepl(pattern, expr)){
    warning("denim now automatically multiply timeStep internally, `* timeStep` will be remove")
    expr <- gsub(pattern, "", expr)
  }
  
  distr <- list(
    transition = "mathExpression",
    expression = expr)
  
  class(distr) <- c("Transition", class(distr))
  distr
}


#Fixed transition
constant <- function(x) {
  distr <- list(
    transition = "constant",
    constant = x)

  class(distr) <- c("Transition", class(distr))
  distr
}


# transprob <- function(x) {
#   distr <- list(
#     distribution = "transitionProb",
#     transitionProb = x)
# 
#   class(distr) <- c("Transition", class(distr))
#   distr
# }


#' Nonparametric distribution transition
#' 
#' Convert a vector of frequencies, percentages... into a distribution
#' 
#' @param x a vector of values
#' @return a Transition object for simulator
#' @param dist_init whether to distribute initial value across subcompartments following this distribution. (default to FALSE, meaning init value is always in the first compartment))
#' 
#' @examples
#' transitions <- list("S->I"=nonparametric( c(0.1, 0.2, 0.5, 0.2) ))
#' transitions <- denim_dsl({S->I=nonparametric( c(0.1, 0.2, 0.5, 0.2) )})
#' # you can also define a model parameter for the distribution
#' transitions <- denim_dsl({S->I=nonparametric( dwelltime_dist )})
#' @export
nonparametric <- function(x, dist_init = FALSE) {
  # x <- substitute(c(...))
  # 
  # # remove "c" 
  # x <- as.character(x)[-1]
  # 
  # # try parsing ... 
  # failed_parse <- FALSE
  # try_eval <- sapply(parse(text = x), \(expr){
  #   tryCatch(
  #     eval(expr),
  #     error = \(e) {
  #       failed_parse <<- TRUE
  #     }
  #   )
  # })
  # x <- if(!failed_parse) {
  #   try_eval
  # } else{
  #   x
  # }
  # 
  # # if ... is a parameter and more than 1 parameter is given -> throw error
  # if(all(is.character(x)) && length(x) > 1){
  #   stop("nonparametric only takes 1 parameter as input")
  # }
  # 
  
  x <- substitute(x)
  x <- evaluate_par(x)
  distr <- list(
    transition = "nonparametric",
    waitingTime = x,
    dist_init = as.numeric(dist_init))
  
  class(distr) <- c("Transition", class(distr))
  distr
}


# multinomial <- function(...) {
#   distr <- list(
#     distribution = "multinomial",
#     probabilities = c(...))
#   
#   class(distr) <- c("Transition", class(distr))
#   distr
# }


#' @export
print.Transition <- function(x, ...) {
  # Print the name of this distribution
  if (x$transition %in% c("gamma", "weibull", "exponential")) {
    cat("Discretized", x$transition, "distribution\n")
    if (x$transition == "weibull") {
      cat("Scale = ", x$scale, ", Shape = ", x$shape, sep = "")
    } 
    else if (x$transition == "gamma"){
      cat("Rate = ", x$rate, ", Shape = ", x$shape, sep = "")
    }
    else if (x$transition == "exponential") {
      cat("Rate = ", x$rate, sep = "")
    }
    else if (x$transition == "lognormal") {
      cat("Mu = ", x$mu, ", Sigma = ", x$sigma, sep = "")
    }
  } 
  else if (x$transition == "mathExpression") {
    cat("Math expression: ")
    cat(x$expression, sep = "")
  } 
  else if (x$transition == "constant") {
    cat("Constant: ")
    cat(x$constant, sep = "")
  } 
  else if (x$transition == "transitionProb") {
    cat("Transition probability: ")
    cat(x$transitionProb, sep = "")
  } 
  else if (x$transition == "nonparametric") {
    cat("Waiting time values: ")
    wt <- utils::head(x$waitingTime, 5)
    wt <- paste0(wt, collapse = ", ")
    cat(wt, sep = "")
    if (length(x$waitingTime) > 5) cat("...")
  } 
  else if (x$transition == "multinomial") {
    cat("Probabilities: ")
    wt <- utils::head(x$probabilities, 5)
    wt <- paste0(wt, collapse = ", ")
    cat(wt, sep = "")
    if (length(x$probabilities) > 5) cat("...")
  }
  cat("\n")
  invisible(x)
}

# Transition object to json
#
# Input is a transition object of a compartment, not the full vector/list 
# of transitions
# transition: a list with elements $name, $rate / $scale / $shape...
# return a json object that match format {"transition": "weibull", "scale": 2, "shape": 5}
transitionToJson <- function(transition) {
  contents <- c()
  if (transition$transition == "nonparametric") {
    dn <- newJsonKeyPair(key = "transition", value = "nonparametric")
    wt <- newJsonKeyPair(key = "waitingTime", value = newJsonArray(transition$waitingTime))
    kp <- newJsonKeyPair(key = "dist_init", value = transition$dist_init)
    contents <- c(dn, wt, kp)
  } 
  else if (transition$transition == "multinomial") {
    dn <- newJsonKeyPair(key = "transition", value = "multinomial")
    wt <- newJsonKeyPair(key = "probabilities", value = newJsonArray(transition$probabilities))
    contents <- c(dn, wt)
  }
  else {
    for (i in 1:length(transition)) {
      key <- names(transition)[i]
      val <- transition[[i]]
      kp <- newJsonKeyPair(key = key, value = val)
      contents <- c(contents, kp)
    }
  }
  newJsonObject(contents, inline = TRUE)
}

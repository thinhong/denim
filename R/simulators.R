checkInitsTransitions <- function(initialValues, transitions) {
  # Compartment name in initialValues
  initNames <- names(initialValues)
  # Compartment name in transitions
  the_names <- names(transitions)
  transNames <- vector("list", length(the_names))
  for (i in seq_along(the_names)) {
    # # Remove white space " ", any number, "*" symbol
    transName <- gsub(" ", "", the_names[i])
    tempNames <- unlist(strsplit(transName, "->|\\*")[[1]])
    if (length(tempNames) == 3) {
      tempNames <- tempNames[-2]
    }
    transNames[i] <- tempNames
  }
  transNames <- unique(transNames)
  
  i_t <- setdiff(initNames, transNames)
  if (length(i_t)) {
    mestext <- paste0("Compartment(s) ", paste0(i_t, collapse = ", "), " not existed in your transitions list")
    stop(mestext, call. = FALSE)
  }
  t_i <- setdiff(transNames, initNames)
  if (length(t_i)) {
    mestext <- paste0("Compartment(s) ", paste0(t_i, collapse = ", "), " not initialized")
    stop(mestext, call. = FALSE)
  }
}


#' Simulator for deterministic discrete time model with memory
#' 
#' Simulation function that call the C++ simulator
#'
#' @param transitions a list of transitions follows this format `"transition" = distribution()`
#' @param initialValues a vector contains the initial values of all compartments defined 
#' in the **transitions**, follows this format `compartment_name = initial_value`
#' @param parameters a vector contains values of any parameters that are not compartments, 
#' usually parameters used in `mathexp()` functions
#' @param simulationDuration duration of time to be simulate
#' @param timeStep set the output time interval. For example, if `simulationDuration = 10` 
#' means 10 days and `timeStep = 0.1`, the output will display results for each 0.1 daily interval
#' @param errorTolerance set the threshold so that a cumulative distribution function 
#' can be rounded to 1. For example, if we want a cumulative probability of 0.999 to 
#' be rounded as 1, we set `errorTolerance = 0.001` (1 - 0.999 = 0.001). Default is 0.001
#'
#' @return a data.frame with class `denim` that can be plotted with a `plot()` method
#' @export
#'
#' @examples 
#' transitions <- list(
#'    "S -> I" = "beta * S * I / N",
#'    "I -> R" = d_gamma(3, 2)
#' )
#' 
#' initialValues <- c(
#'    S = 999, 
#'    I = 1, 
#'    R = 0
#' )
#' 
#' parameters <- c(
#'    beta = 0.012,
#'    N = 1000
#' )
#' 
#' simulationDuration <- 30
#' timeStep <- 0.01
#' 
#' mod <- sim(transitions = transitions, 
#'            initialValues = initialValues, 
#'            parameters = parameters, 
#'            simulationDuration = simulationDuration, 
#'            timeStep = timeStep)
sim <- function(transitions, initialValues, parameters, 
                simulationDuration, timeStep = 1, errorTolerance = .001) {
  
  # First check their inputs
  # checkInitsTransitions(initialValues, transitions)
  
  # Generate model object
  mod <- newModel(simulationDuration, errorTolerance, initialValues, parameters, transitions, timeStep)
  modJson <- modelToJson(mod)
  # cat(fmodJson) # for debug
  
  # Parse the json to C++ with function simcm (simulating compartmental model)
  df <- simcm(modJson)

  class(df) <- c("denim", class(df))
  df
}

#' @export
plot.denim <- function(x, ...) {
  
  # Set color codes and compartment names
  col_codes <- viridisLite::viridis(ncol(x) - 1)
  comp_names <- colnames(x)[-1]
  
  # Plot the first compartment
  cmd1 <- paste0("with(x, {
  plot(Time, ", comp_names[1], ", type = \"l\", lwd = 3, col = \"", col_codes[1], 
                 "\", xlab = \"Time\", ylab = \"Number of people\")\n")
  
  # Add lines of the other compartments
  cmd2 <- ""
  for (i in 2:(ncol(x) - 1)) {
    cmd2 <- paste0(cmd2, "lines(Time, ", comp_names[i], 
                   ", col = \"", col_codes[i], "\", lwd = 3)\n")
  }
  
  # Add legend
  cmd3 <- paste0("legend(\"right\", c(" , 
                 paste0("\"", comp_names, collapse = ", ", "\""), 
                 "), col = col_codes, lty = 1, lwd = 3, bty = \"n\")")
  
  cmd <- paste0(cmd1, "\n", cmd2, "})", "\n", cmd3)
  
  eval(parse(text = cmd))
}
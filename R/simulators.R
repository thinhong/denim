checkInitsTransitions <- function(initialValues, transitions) {
  # Compartment name in initialValues
  initNames <- names(initialValues)
  # Compartment name in transitions
  transNames <- c()
  for (transName in names(transitions)) {
    # # Remove white space " ", any number, "*" symbol
    transName <- gsub(" ", "", transName)
    tempNames <- unlist(strsplit(transName, "->|\\*")[[1]])
    if (length(tempNames) == 3) {
      tempNames <- tempNames[-2]
    }
    transNames <- append(transNames, tempNames)
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
#'    "S -> I" = mathexpr(beta * S * I / N),
#'    "I -> R" = gamma(3, 2)
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
                simulationDuration, timeStep = 1, errorTolerance = 0.001) {
  
  # First check their inputs
  # checkInitsTransitions(initialValues, transitions)
  
  # Generate model object
  mod <- newModel(simulationDuration, errorTolerance, initialValues, parameters, transitions, timeStep)
  modJson <- modelToJson(mod)
  # cat(fmodJson) # for debug
  
  # Parse the json to C++ with function simcm (simulating compartmental model)
  df <- simcm(modJson)

  class(df) <- c("denim", class(df))
  return(df)
}

#' @export
plot.denim <- function(x, ...) {
  cols <- colnames(x)[-1]
  df_plot <- stats::reshape(x, varying = cols, v.names = "Value", 
                            times = cols, timevar = "Compartment", 
                            direction = "long")
  p <- ggplot2::ggplot(df_plot, ggplot2::aes(x = df_plot$Time, y = df_plot$Value, col = df_plot$Compartment)) + 
    ggplot2::geom_line(size = 1.1) + ggplot2::theme_light() + 
    ggplot2::labs(x = "Time", y = "Value", color = "Compartment")
  
  return(p)
}
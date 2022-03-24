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

#' @export
runSim <- function(transitions, initialValues, parameters, 
                   simulationDuration, timeStep = 1, errorTolerance = 0.001) {
  
  # First check their inputs
  # checkInitsTransitions(initialValues, transitions)
  
  # Generate model object
  mod <- newModel(simulationDuration, errorTolerance, initialValues, parameters, transitions, timeStep)
  modJson <- modelToJson(mod)
  # cat(fmodJson) # for debug
  
  # Parse the json to C++ with function simcm (simulating compartmental model)
  df <- simcm(modJson)

  class(df) <- c("discretedf", class(df))
  return(df)
}

#' Title
#'
#' @param df
#'
#' @return
#' @export
#'
#' @examples
plot.discretedf <- function(x, ...) {
  cols <- colnames(x)[-1]
  df_plot <- stats::reshape(x, varying = cols, v.names = "Value", 
                            times = cols, timevar = "Compartment", 
                            direction = "long")
  p <- ggplot2::ggplot(df_plot, ggplot2::aes(x = df_plot$Time, y = df_plot$Value, col = df_plot$Compartment)) + 
    ggplot2::geom_line(size = 1.1) + ggplot2::theme_light() + 
    ggplot2::labs(x = "Time", y = "Value", color = "Compartment")
  
  return(p)
}
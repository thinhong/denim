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

runSim <- function(daysFollowUp, errorTolerance, initialValues, 
                   parameters, transitions, timeStep = 1) {
  
  # First check their inputs
  checkInitsTransitions(initialValues, transitions)
  
  # Generate model object
  mod <- newModel(daysFollowUp, errorTolerance, initialValues, parameters, transitions, timeStep)
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
plot.discretedf <- function(df) {
  cols <- colnames(df)[-1]
  df_plot <- reshape(df, varying = cols, v.names = "Value", 
                     times = cols, timevar = "Compartment",
                     direction = "long")
  p <- ggplot2::ggplot(df_plot, ggplot2::aes(x = Time, y = Value, col = Compartment)) + 
      ggplot2::geom_line(size = 1.1) + ggplot2::theme_light() + ggplot2::labs(color = "Compartment")
  
  return(p)
}
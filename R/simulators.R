runSim <- function(daysFollowUp, errorTolerance, initialValues, 
                   parameters, transitions, timeStep = 1) {
  
  # Generate full model object
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
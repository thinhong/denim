runSim <- function(daysFollowUp, errorTolerance, timeStep, transmissionRate,
                   infectiousComps, contacts = NULL, transitions, 
                   initialValues, distributions) {
  
  # If user only has 1 type of contact and doesn't want to write it as a list
  # We will turn their contacts input into a list for them
  if (!is.list(contacts) && !is.null(contacts)) {
    contacts <- list(contacts)
  }
  
  # First, run all validators
  if (!is.null(contacts)) {
    checkContactMatrices(contacts)
    initValsVsContacts(initialValues, contacts)
  }
  
  # Generate full model object
  fmod <- newFullModel(daysFollowUp, errorTolerance, timeStep, transmissionRate,
                       infectiousComps, contacts, transitions, 
                       initialValues, distributions)
  fmodJson <- fullModelToJson(fmod)
  # cat(fmodJson) # for debug
  
  # Parse the json to C++ with function simcm (simulating compartmental model)
  df <- simcm(fmodJson)
  
  # When users use simple SIR model without contact, modelName = "" therefore
  # colnames will be S_, I_, R_, this code is to erase the "_" character
  if (length(unlist(strsplit(colnames(df)[[2]], "_"))) == 1) {
    colnames(df)[-1] <- gsub("_", "", colnames(df)[-1])
  }
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
                     times = cols, timevar = "FullCompartment",
                     direction = "long")
  n_group <- length(unlist(strsplit(df_plot$FullCompartment[1], "_"))) - 1
  if (n_group == 0) {
    df_plot$FullCompartment <- factor(df_plot$FullCompartment)
    p <- ggplot2::ggplot(df_plot, ggplot2::aes(x = Time, y = Value, col = FullCompartment)) + 
      ggplot2::geom_line(size = 1.1) + ggplot2::theme_light()
  } else if (n_group >= 1) {
    # Erase all strings after the first _ to get compartment name
    df_plot$Compartment <- gsub("_.*", "", df_plot$FullCompartment)
    # Then erase the first string element before the first _ to get the group
    df_plot$Group <- sub(".*?_", "", df_plot$FullCompartment)
    df_plot$Compartment <- factor(df_plot$Compartment)
    df_plot$Group <- factor(df_plot$Group)
    p <- ggplot2::ggplot(df_plot, ggplot2::aes(x = Time, y = Value, col = Compartment)) + 
      ggplot2::geom_line(size = 1.1) + ggplot2::theme_light() + ggplot2::facet_wrap(~ Group)
  }
  
  return(p)
}
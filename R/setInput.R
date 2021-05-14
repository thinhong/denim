#' @export
createContact <- function(contactType, contactClasses, contactRates) {
  d <- list(contactType = contactType, contactClasses = contactClasses, contactRates = contactRates)
  d <- toJSON(d, pretty = TRUE, auto_unbox = TRUE)
  return(d)
}

#' @export
setModelStructure <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

#' @export
setInfectiousComps <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

#' @export
setContactAssumption <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

#' @export
createDistribution <- function(name, ...) {
  d <- list(name = name, ...)
  return(d)
}

#' @export
createCompartment <- function(name, distribution, initialValue) {
  elements_list = list(name = name, distribution = distribution, initialValue = initialValue)
  return(elements_list)
}

#' @export
allCompartments <- function(modelName, transmissionRate, ...) {
  a <- list(modelName = list(modelName), transmissionRate = transmissionRate, compartments = list(...))
  a <- toJSON(a, pretty = TRUE, auto_unbox = TRUE)
  return(a)
}

#' Full JSON input format
#' 
#' Generate a full JSON input format
#' @param daysFollowUp number of time step to be simulated
#' @param errorTolerance if the cumulative probability exceed 1 - errorTolerance
#' it will be rounded to 1.0, default = 0.01
#' @param timeStep how much the time interval should be binned, default = 1 (not binning); 
#' adjust this to derive continuous-time model
#' @param modelStructure define model structure, ex: "S -> I", "I -> R"
#' @param infectiousComps name of the infectious compartment, ex: "I"
#' @param contactAssumption to be deprecated soon
#' @param contacts created by createContact()
#' @param models vector contains all model objects
#'
#' @export
allModels <- function(daysFollowUp, errorTolerance = 0.01, timeStep = 1, modelStructure, infectiousComps, contactAssumption, contacts, models) {
  contactJson <- makeJsonElement("contacts", contacts)
  modelsJson <- makeJsonElement("models", models)
  a <- paste0("{\n\"daysFollowUp\": ", daysFollowUp, ",\n",
              "\"errorTolerance\": ", errorTolerance, ",\n",
              "\"timeStep\": ", timeStep, ",\n",
              "\"modelStructure\": ", modelStructure, ",\n",
              "\"infectiousComps\": ", infectiousComps, ",\n",
              "\"contactAssumption\": ", contactAssumption, ",\n",
              contactJson, ",\n", 
              modelsJson,
              "\n}")
  return(a)
}

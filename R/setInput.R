#' Create contact manually
#' 
#' @param contactType type of the contact, example "age" for age group 0-14, 15-64...
#' @param contactClasses a vector contains all categories of this contact
#' @param contactRates contact rates between pair of categories
#'
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

#' Set name of infectious compartment
#' @param ... name of the infectious compartment
#'
#' @export
setInfectiousComps <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

#' Define distribution
#' 
#' @param name name of the distribution, could be "exponential", "gamma", "weibull",
#' "custom", or "transitionProb"
#' @details parameters for each distribution type are:
#' \itemize{
#'  \item{"exponential": }{rate}
#'  \item{"gamma": }{scale, shape}
#'  \item{"weibull": }{scale, shape}
#'  \item{"custom": }{a vector of percentages}
#'  \item{"transitionProb": }{a constant}
#' }
#'
#' @export
createDistribution <- function(name, ...) {
  d <- list(name = name, ...)
  return(d)
}

#' Define compartment
#' @param name name of this compartment, ex: "S", "I", "R"
#' @param distribution distribution of this compartment, defined by createDistribution()
#' @param initialValue number of people in this compartment at day 0
#'
#' @export
createCompartment <- function(name, distribution, initialValue) {
  elements_list = list(name = name, distribution = distribution, initialValue = initialValue)
  return(elements_list)
}

#' Gathering all compartments into a model
#'
#' @param modelName name of model
#' @param transmissionRate transmission rate, use to compute force of infection
#' @param ... all compartments objects created by createCompartment()
#'
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
#' @param contacts created by createContact()
#' @param models vector contains all model objects
#'
#' @export
allModels <- function(daysFollowUp, errorTolerance = 0.01, timeStep = 1, modelStructure, infectiousComps, contacts, models) {
  contactJson <- makeJsonElement("contacts", contacts)
  modelsJson <- makeJsonElement("models", models)
  a <- paste0("{\n",
              "\"daysFollowUp\": ", daysFollowUp, ",\n",
              "\"errorTolerance\": ", errorTolerance, ",\n",
              "\"timeStep\": ", timeStep, ",\n",
              "\"modelStructure\": ", modelStructure, ",\n",
              "\"infectiousComps\": ", infectiousComps, ",\n",
              contactJson, ",\n", 
              modelsJson,
              "\n}")
  return(a)
}

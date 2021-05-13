
createContact <- function(contactType, contactClasses, contactRates) {
  d <- list(contactType = contactType, contactClasses = contactClasses, contactRates = contactRates)
  d <- toJSON(d, pretty = TRUE, auto_unbox = TRUE)
  return(d)
}

setModelStructure <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

setInfectiousComps <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

setContactAssumption <- function(...) {
  d <- c(...)
  d <- toJSON(d)
  return(d)
}

createDistribution <- function(name, ...) {
  d <- list(name = name, ...)
  return(d)
}

createCompartment <- function(name, distribution, initialValue) {
  elements_list = list(name = name, distribution = distribution, initialValue = initialValue)
  return(elements_list)
}

allCompartments <- function(modelName, transmissionRate, ...) {
  a <- list(modelName = list(modelName), transmissionRate = transmissionRate, compartments = list(...))
  a <- toJSON(a, pretty = TRUE, auto_unbox = TRUE)
  return(a)
}

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

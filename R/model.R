# Constructor
newModel <- function(simulationDuration, errorTolerance, initialValues, 
                     parameters, transitions, timeStep = 1) {
  mod <- list(
    simulationDuration = simulationDuration,
    errorTolerance     = errorTolerance,
    timeStep           = timeStep,
    initialValues      = initialValues,
    parameters         = parameters,
    transitions        = transitions)
  
  class(mod) <- c("Model", class(mod))
  mod
}

# Model to json
modelToJson <- function(mod) {
  initialValues <- mod$initialValues
  parameters    <- mod$parameters
  transitions   <- mod$transitions
  
  ivKeyPairs <- newJsonKeyPair(names(initialValues), initialValues)
  pmKeyPairs <- newJsonKeyPair(names(parameters), parameters)
  
  nb <- length(transitions)
  distr <- vector("list", nb)
  for (i in 1:nb) {
    distr[i] <- newJsonNestedObject(
      names(transitions)[i],
      distributionToJson(transitions[[i]]), inline = TRUE)
  }
  
  newJsonObject(
    newJsonKeyPair("simulationDuration", mod$simulationDuration),
    newJsonKeyPair("errorTolerance"    , mod$errorTolerance),
    newJsonKeyPair("timeStep"          , mod$timeStep),
    newJsonNestedObject("initialValues", newJsonObject(ivKeyPairs, inline = TRUE)),
    newJsonNestedObject("parameters"   , newJsonObject(pmKeyPairs, inline = TRUE)),
    newJsonNestedObject("transitions"  , newJsonObject(distr)))
}
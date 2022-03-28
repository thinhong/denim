# Constructor
newModel <- function(simulationDuration, errorTolerance, initialValues, 
                     parameters, transitions, timeStep = 1) {
  mod <- list()
  
  mod$simulationDuration <- simulationDuration
  mod$errorTolerance <- errorTolerance
  mod$timeStep <- timeStep
  mod$initialValues <- initialValues
  mod$parameters <- parameters
  mod$transitions <- transitions
  
  class(mod) <- "Model"
  return(mod)
}

# Model to json
modelToJson <- function(mod) {
  dfu <- newJsonKeyPair("simulationDuration", mod$simulationDuration)
  et <- newJsonKeyPair("errorTolerance", mod$errorTolerance)
  ts <- newJsonKeyPair("timeStep", mod$timeStep)
  ivKeyPairs <- newJsonKeyPair(names(mod$initialValues), mod$initialValues)
  iv <- newJsonNestedObject("initialValues", newJsonObject(ivKeyPairs, inline = T))
  pmKeyPairs <- newJsonKeyPair(names(mod$parameters), mod$parameters)
  pm <- newJsonNestedObject("parameters", newJsonObject(pmKeyPairs, inline = T))
  distr <- c()
  for (i in 1:length(mod$transitions)) {
    distrName <- names(mod$transitions)[i]
    distrParams <- distributionToJson(mod$transitions[[i]])
    distr <- c(distr, newJsonNestedObject(distrName, distrParams, inline = TRUE))
  }
  tr <- newJsonNestedObject("transitions", newJsonObject(distr))
  contents <- newJsonObject(dfu, et, ts, iv, pm, tr)
  return(contents)
}
# Constructor
newModel <- function(modelName, transmissionRate, compartments) {
  mod <- list()
  
  modelNameVector <- unlist(strsplit(modelName, "\\."))
  mod$modelName <- modelNameVector
  mod$transmissionRate <- transmissionRate
  mod$compartments <- compartments
  
  class(mod) <- "Model"
  return(mod)
}
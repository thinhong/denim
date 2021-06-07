newFullModel <- function(daysFollowUp, errorTolerance, timeStep, transmissionRate,
                         infectiousComps, contacts = NULL, transitions, 
                         initialValues, distributions) {
  fmod <- list()
  
  fmod$daysFollowUp <- daysFollowUp
  fmod$errorTolerance <- errorTolerance
  fmod$timeStep <- timeStep
  fmod$infectiousComps <- infectiousComps
  if (!is.null(contacts)) {
    fmod$contacts <- newContact(contacts)
  }
  fmod$transitions <- transitions
  
  mods <- list()
  if (class(distributions[[1]]) == "list") {
    for (modelName in names(initialValues)) {
      comps <- list()
      # Tidy up the distribution elements of this model
      distributions[[modelName]] <- tidyDistribution(initialValues[[modelName]], distributions[[modelName]])
      for (compName in names(initialValues[[modelName]])) {
        comps[[compName]] <- newCompartment(compartmentName = compName,
                                            distribution = distributions[[modelName]][[compName]],
                                            initialValue = initialValues[[modelName]][[compName]])
      }
      mods[[modelName]] <- newModel(modelName = modelName,
                                    transmissionRate = transmissionRate,
                                    compartments = comps)
    }
  } else {
    # There is only one model in this case
    modelName <- ""
    comps <- list()
    # Tidy up the distribution of this model
    distributions <- tidyDistribution(initialValues, distributions)
    for (compName in names(initialValues)) {
      comps[[compName]] <- newCompartment(compartmentName = compName, 
                                          distribution = distributions[[compName]],
                                          initialValue = initialValues[[compName]])
    }
    mods[[modelName]] <- newModel(modelName = modelName, 
                                  transmissionRate = transmissionRate,
                                  compartments = comps)
  }
  
  fmod$models <- mods
  
  class(fmod) <- "FullModel"
  return(fmod)
}


# Check the matching compartment names between definitions of initialValues and distributions
initValDistrHelper <- function(initialValues, distributions) {
  # First, compare the models
  d_distr <- setdiff(names(distributions), names(initialValues))
  d_initVal <- setdiff(names(initialValues), names(distributions))
  
  # Two attributes need to have the same number of models
  if (length(d_distr) > 0 || length(d_initVal) > 0) {
    stop(
      if (length(d_distr) > 1) {
        paste0("Element ", paste(d_distr, collapse = ", "), " in distributions are not found in initialValues")
      } else if (length(d_distr) == 1) {
        paste0("Element ", d_distr, " in distributions is not found in initialValues")
      } else if (length(d_initVal) > 1) {
        paste0("Element ", paste(d_initVal, collapse = ", "), " in initialValues are not found in distributions")
      } else if (length(d_initVal) == 1) {
        paste0("Element ", d_initVal, " in initialValues is not found in distributions")
      },
      call. = FALSE
    )
  }
}

tidyDistribution <- function(initialValues, distributions) {
  distr <- list()
  cnInitVal <- names(initialValues)
  cnDist <- unique(sapply(names(distributions), function(x) strsplit(x, "\\.")[[1]][[1]]))
  for (compName in cnInitVal) {
    if (compName %in% cnDist) {
      for (i in 1:length(distributions)) {
        cName <- unlist(strsplit(names(distributions)[i], "\\."))[1]
        pName <- unlist(strsplit(names(distributions)[i], "\\."))[2]
        if (cName == compName) {
          distr[[compName]][[pName]] <- distributions[[i]]
        }
      }
    } else {
      distr[[compName]][["name"]] <- "none"
    }
  }
  return(distr)
}

inputHelper <- function(transitions, contactGroups = NULL) {
  # Handle user's need
  contactGrid <- expand.grid(contactGroups, stringsAsFactors = FALSE)
  contactGrid$pasted <- apply(contactGrid, 1, paste, collapse = "_")
  
  transitionsTrimmed <- gsub(" |\t", "", transitions)
  compartments <- unique(unlist(strsplit(transitionsTrimmed, "->")))
  allCompartments <- c()
  modelGroups <- list()
  for (contact in contactGrid$pasted) {
    compartment <- paste(compartments, contact, sep = "_")
    allCompartments <- append(allCompartments, compartment)
    modelGroups[[contact]] <- append(modelGroups[[contact]], compartment)
  }
  
  # Display correct input format
  cat("transitions <- c(\n")
  for (transition in transitions) {
    cat("  ", transition, sep = "")
    if (transition != transitions[length(transitions)]) {
      cat(",\n")
    } else {
      cat("\n")
    }
  }
  cat(")\n\n")
  cat("initialValues <- list(\n")
  for (compartment in allCompartments) {
    cat("  ", compartment, sep = "")
    if (compartment != allCompartments[length(allCompartments)]) {
      cat(" = ,\n")
    } else {
      cat(" = \n")
    }
  }
  cat(")\n\n")
  cat("distributions <- list(\n")
  for (compartment in allCompartments) {
    cat("  ", compartment, sep = "")
    if (compartment != allCompartments[length(allCompartments)]) {
      cat(" = ,\n")
    } else {
      cat(" = \n")
    }
  }
  cat(")\n\n")
  cat("modelElements <- list(\n")
  for (group in names(modelGroups)) {
    cat("  m_", group, " = c(", paste(modelGroups[[group]], collapse = ", "), sep = "")
    if (group != names(modelGroups)[length(modelGroups)]) {
      cat("),\n")
    } else {
      cat(")\n")
    }
  }
  cat(")\n")
}

runSim <- function(daysFollowUp, errorTolerance, timeStep, transmissionRate,
                   infectiousComps, contacts = NULL, transitions, 
                   initialValues, distributions) {
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
  return(df)
}
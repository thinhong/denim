# A helper function to display initialValues and transitions for inputHelper
displayModelGroups <- function(modelGroups) {
  for (i in 1:length(modelGroups)) {
    modelName <- names(modelGroups)[i]
    cat("  ", modelName, " = c(", sep = "")
    for (compartment in modelGroups[[i]]) {
      cat(compartment)
      if (compartment != modelGroups[[i]][length(modelGroups[[i]])]) {
        cat(" = , ")
      } else {
        cat(" = )")
      }
    }
    if (modelName != names(modelGroups)[length(modelGroups)]) {
      cat(",\n")
    } else {
      cat("\n")
    }
  }
}

inputHelper <- function(transitions, contacts = NULL) {
  # First, check that the contact matrices are correct
  checkContactMatrices(contacts)
  
  contactGroups <- lapply(contacts, rownames)
  contactGrid <- expand.grid(contactGroups, stringsAsFactors = FALSE)
  contactGrid$pasted <- apply(contactGrid, 1, paste, collapse = ".")
  
  # Sort these name for better format
  contactGrid <- contactGrid[order(contactGrid$pasted),]
  
  transitionsTrimmed <- gsub(" |\t", "", transitions)
  compartments <- unique(unlist(strsplit(transitionsTrimmed, "->")))
  modelGroups <- list()
  for (contact in contactGrid$pasted) {
    modelGroups[[contact]] <- compartments
  }
  
  # Display correct input format
  ## Initial values and transitions are based on modelGroups
  cat("# Place these codes under the transitions and contacts you have written\n")
  cat("# Set up the initial values for all compartments\n")
  cat("initialValues <- list(\n")
  displayModelGroups(modelGroups)
  cat(")\n\n")
  
  cat("# Set up the transitions for all compartments, any compartment without transition can be deleted\n")
  cat("transitions <- list(\n")
  displayModelGroups(modelGroups)
  cat(")\n\n")
  
  cat("# Insert all parameters here, please use ?runSim for more details\n")
  cat("fmod <- runSim(daysFollowUp = , errorTolerance = , timeStep = , 
               transmissionRate = , infectiousComps = , 
               contacts = , transitions = ,
               initialValues = initialValues, transitions = transitions)")
}

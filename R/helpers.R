# Generate correct input format for users
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

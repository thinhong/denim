# Check that row names and column names of all contact matrices are matched
checkContactMatrices <- function(contacts) {
  for (contact in contacts) {
    if (any(rownames(contact) != colnames(contact))) {
      stop("Please check your contact matrix\n", "Row names ", 
           paste(rownames(contact), collapse = ", "), 
           " and column names ", paste(colnames(contact), collapse = ", "), " are not matched")
    }
  }
}

# Check that model names in initialValues are match with names in contacts
initValsVsContacts <- function(initialValues, contacts) {
  # Get names of all contacts
  contactGroups <- lapply(contacts, rownames)
  # Make contactGrid = all combinations of contact names
  contactGrid <- expand.grid(contactGroups, stringsAsFactors = FALSE)
  # Extract each combination to list elements
  contactGrid <- t(contactGrid)
  modelNames <- list()
  for (i in 1:ncol(contactGrid)) {
    modelNames[[i]] <- contactGrid[, i]
  }
  
  # Check initialValues
  initValNames <- strsplit(names(initialValues), "\\.")
  for (i in 1:length(initValNames)) {
    iname <- initValNames[[i]]
    counter <- 0
    # Screen across all names in modelNames
    for (mname in modelNames) {
      # If the initialName match any name in modelNames, increase the counter
      if (all(sort(iname) == sort(mname))) {
        counter <- counter + 1
      }
    }
    # Finally, if the initialName did not match any name in modelNames, display warning
    if (counter == 0) {
      stop(names(initialValues)[i], " in initial values does not match any names defined in the contact matrices",
           call. = FALSE)
    }
  }
}
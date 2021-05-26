# Constructor
newCompartment <- function(compartmentName, distribution, initialValue) {
  comp <- list()
  
  comp$compartmentName <- compartmentName
  comp$distribution <- distribution
  comp$initialValue <- initialValue
  
  class(comp) <- "Compartment"
  return(comp)
}

# Generic print method to print out what we want the users to see
print.Compartment <- function(x) {
  cat("Compartment: ", x$compartmentName, ", Initial value = ", x$initialValue, "\n", sep = "")
  print(x$distribution)
  invisible(x)
}
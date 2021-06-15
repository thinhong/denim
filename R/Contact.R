# Create new Contact object from a contact matrix
contactFromMatrix <- function(contactType, contactMatrix) {
  ct <- list()
  
  ind <- which(upper.tri(contactMatrix, diag = TRUE), arr.ind = TRUE)
  df <- data.frame(row = dimnames(contactMatrix)[[1]][ind[,1]],
                   col = dimnames(contactMatrix)[[2]][ind[,2]],
                   val = contactMatrix[ind])
  df$contact <- paste0(df$row, " <-> ", df$col, ": ", df$val)
  
  ct$contactType <- contactType
  ct$contactClasses <- unique(colnames(contactMatrix))
  ct$contactRates <- df$contact
  
  class(ct) <- "Contact"
  return(ct)
}

# Constructor
newContact <- function(contacts) {
  ## If user doesn't define the name of type of contacts, we will also define it for them
  if (is.null(names(contacts))) {
    names(contacts) <- seq(1, length(contacts))
  }
  
  act <- list()
  for (cname in names(contacts)) {
    act[[cname]] <- contactFromMatrix(contactType = cname, contactMatrix = contacts[[cname]])
  }
  
  class(act) <- "allContacts"
  return(act)
}

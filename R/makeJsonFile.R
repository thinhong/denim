#' @export
getCompsFromModelStructure <- function(modelStructure) {
  comps <- c()
  for (i in 1:length(modelStructure)) {
    comps <- append(comps, unlist(strsplit(modelStructure[i], "->")))
  }
  comps <- unique(comps)
  return(comps)
}

#' Make contact element from a contact matrix
#' 
#' @param contactMatrix contact matrix dataframe, with column names and row
#' names are the classes
#' @param contactType type of the contact, example "age" for age group 0-14, 15-64...
#'
#' @export
getContactMatrix <- function(contactMatrix, contactType) {
  ind <- which(upper.tri(contactMatrix, diag = TRUE), arr.ind = TRUE)
  df <- data.frame(row = dimnames(contactMatrix)[[1]][ind[,1]],
                   col = dimnames(contactMatrix)[[2]][ind[,2]],
                   val = contactMatrix[ind])
  df$contact <- paste0(df$row, " <-> ", df$col, ": ", df$val)
  contactClasses <- unique(colnames(contactMatrix))
  return(createContact(contactType = contactType, 
                       contactClasses = contactClasses, 
                       contactRates = df$contact))
}


getCompsFromModelStructure <- function(modelStructure) {
  comps <- c()
  for (i in 1:length(modelStructure)) {
    comps <- append(comps, unlist(strsplit(modelStructure[i], "->")))
  }
  comps <- unique(comps)
  return(comps)
}

# ms <- c("S->I", "I->R")
# getCompsFromModelStructure(ms)

# Generate a JSON file
# makeJSONInput <- function(filePath, modelStructure, contactList) {
#   comps <- getCompsFromModelStructure(modelStructure = modelStructure)
# }

# modelStructure <- c("S->I", "I->R")
# contactList <- list(age = c("0_14", "15_64", "65"), location = c("HCM", "HN"))

# Contact JSON
# contactJSONList <- list()
# for (i in 1:length(contactList)) {
#   comb_df <- data.frame(combinations(length(contactList[[i]]), 2, contactList[[i]], repeats = TRUE))
#   comb_df$contactFormat <- paste0(comb_df[,1], " <-> ", comb_df[,2])
#   contactJSONList[[i]] <- createContact(names(contactList)[i], contactList[[i]], comb_df$contactFormat)
# }

# cat(makeJSONElement("contacts", contactJSONList))
# 
# comps <- getCompsFromModelStructure(modelStructure = modelStructure)
# expand.grid(contactList[[1]], contactList[[1]])


# Case 1: we have a contact matrix
# M <- read.csv("/home/thinh/Dropbox/oucru/rcpp/testRcpp/age.csv",
#               header = TRUE, check.names = FALSE, row.names = 1)

getPairContactFromMatrix <- function(contactMatrix) {
  ind <- which(upper.tri(contactMatrix, diag = TRUE), arr.ind = TRUE)
  df <- data.frame(row = dimnames(contactMatrix)[[1]][ind[,1]],
                   col = dimnames(contactMatrix)[[2]][ind[,2]],
                   val = contactMatrix[ind])
  df$contact <- paste0(df$row, " <-> ", df$col, ": ", df$val)
  return(df$contact)
}

# getPairContactFromMatrix(M)

# Case 2: generate pair of contact and 

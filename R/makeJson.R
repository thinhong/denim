#' @export
makeJsonElement <- function(label, elementList) {
  output <- paste0("\"", label, "\": [\n")
  for (i in 1:length(elementList)) {
    output <- paste0(output, elementList[i])
    if (i < length(elementList)) {
      output <- paste0(output, ", \n")
    }
  }
  output <- paste0(output, "\n]")
  return(output)
}
# Key pair is "key": value
newJsonKeyPair <- function(key, value) {
  # Check if character is string then automatically add ""
  if (is.character(value) && !grepl("\\[", value)) {
    value <- paste0("\"", value, "\"")
  }
  paste0("\"", key, "\": ", value)
}

# Json array is [ content_1, content_2 ]
newJsonArray <- function(...) {
  contents <- c(...)
  js <- "["
  for (i in 1:length(contents)) {
    if (i != length(contents)) {
      js <- paste0(js, contents[i], ", ")
    } else {
      js <- paste0(js, contents[i], "]")
    }
  }
  js
}

# Json object is { content_1, content_2 }
newJsonObject <- function(..., inline = FALSE) {
  contents <- c(...)
  if (inline == FALSE) {
    js <- "{\n"
    for (i in 1:length(contents)) {
      if (i != length(contents)) {
        js <- paste0(js, "  ", contents[i], ",\n")
      } else {
        js <- paste0(js, "  ", contents[i], "\n")
      }
    }
  } else {
    js <- "{"
    
    for (i in 1:length(contents)) {
      if (i != length(contents)) {
        js <- paste0(js, contents[i], ", ")
      } else {
        js <- paste0(js, contents[i], "")
      }
    }
  }
  paste0(js, "}")
}

# Nested array is "key": [ array ]
newJsonNestedArray <- function(key, array, quotation = FALSE) {
  arrayContent <- ""
  # Add "" to each element if this is an array of string
  if (quotation == TRUE) {
    arrayContent <- paste0("\"", paste(array, collapse = "\", \""))
    arrayContent <- paste0(arrayContent, "\"")
  } else {
    # If not an array of string, just paste all
    arrayContent <- paste(array, collapse = ", ")
  }
  paste0("\"", key, "\": [", arrayContent, "]")
}

# Nested object is "key": { object } with object created by function newJsonObject()
newJsonNestedObject <- function(key, object, inline = FALSE) {
  if (inline == TRUE) {
    object <- gsub("\n", "", object)
    object <- gsub("  ", "", object)
  }
  paste0("\"", key, "\": ", object)
}

# Key pair is "key": value
newJsonKeyPair <- function(key, value) {
  # Check if character is string then automatically add ""
  if (is.character(value) && !grepl("\\[", value)) {
    value <- paste0("\"", value, "\"")
  }
  js <- paste0("\"", key, "\": ", value)
  return(js)
}

# Json array is [ content_1, content_2 ]
newJsonArray <- function(...) {
  contents <- c(...)
  js <- "["
  for (content in contents) {
    if (content != contents[length(contents)]) {
      js <- paste0(js, content, ", ")
    } else {
      js <- paste0(js, content, "]")
    }
  }
  return(js)
}

# Json object is { content_1, content_2 }
newJsonObject <- function(..., inline = FALSE) {
  contents <- c(...)
  if (inline == FALSE) {
    js <- "{\n"
    for (content in contents) {
      if (content != contents[length(contents)]) {
        js <- paste0(js, "  ", content, ",\n")
      } else {
        js <- paste0(js, "  ", content, "\n")
      }
    }
  } else {
    js <- "{"
    for (content in contents) {
      if (content != contents[length(contents)]) {
        js <- paste0(js, content, ", ")
      } else {
        js <- paste0(js, content, "")
      }
    }
  }
  js <- paste0(js, "}")
  return(js)
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
  js <- paste0("\"", key, "\": [", arrayContent, "]")
  return(js)
}

# Nested object is "key": { object } with object created by function newJsonObject()
newJsonNestedObject <- function(key, object, inline = FALSE) {
  if (inline == TRUE) {
    object <- gsub("\n", "", object)
    object <- gsub("  ", "", object)
  }
  js <- paste0("\"", key, "\": ", object)
  return(js)
}

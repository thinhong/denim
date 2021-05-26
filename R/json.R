# Key pair is "key": value
newJsonKeyPair <- function(key, value) {
  # Check if character is string then automatically add ""
  if (is.character(value)) value <- paste0("\"", value, "\"")
  js <- paste0("\"", key, "\": ", value)
  return(js)
}

# Json array is [ content_1, content_2 ]
newJsonArray <- function(...) {
  contents <- c(...)
  js <- "["
  for (content in contents) {
    if (content != contents[length(contents)]) {
      js <- paste0(js, "  ", content, ",\n")
    } else {
      js <- paste0(js, "  ", content, "\n")
    }
  }
  js <- paste0(js, "]")
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

# Nested object is "key": { object }
newJsonNestedObject <- function(key, object) {
  js <- paste0("\"", key, "\": {", object, "}")
  return(js)
}

# Json object is { content_1, content_2 }
newJsonObject <- function(...) {
  contents <- c(...)
  js <- "{\n"
  for (content in contents) {
    if (content != contents[length(contents)]) {
      js <- paste0(js, "  ", content, ",\n")
    } else {
      js <- paste0(js, "  ", content, "\n")
    }
  }
  js <- paste0(js, "}")
  return(js)
}

# Contacts to json
contactsToJson <- function(contacts) {
  contents <- "\n"
  for (i in 1:length(contacts)) {
    contact <- contacts[[i]]
    ctType <- newJsonKeyPair("contactType", contact$contactType)
    ctClasses <- newJsonNestedArray("contactClasses", contact$contactClasses, quotation = TRUE)
    ctRates <- newJsonNestedArray("contactRates", contact$contactRates, quotation = TRUE)
    contents <- paste0(contents, newJsonObject(ctType, ctClasses, ctRates))
    if (i < length(contacts)) {
      contents <- paste0(contents, ",\n")
    } else {
      contents <- paste0(contents, "\n")
    }
  }
  return(contents)
}

# Compartments to json
compartmentsToJson <- function(compartments) {
  contents <- "\n"
  for (i in 1:length(compartments)) {
    compartment <- compartments[[i]]
    cName <- newJsonKeyPair("name", compartment$compartmentName)
    cDistr <- ""
    if (is.null(compartment$distribution)) {
      cDistr <- "\"name\": \"transitionProb\", \"transitionProb\": 0.0"
    } else {
      for (j in 1:length(compartment$distribution)) {
        cDistr <- paste0(cDistr, newJsonKeyPair(names(compartment$distribution)[j], 
                                                compartment$distribution[[j]]))
        if (j < length(compartment$distribution)) {
          cDistr <- paste0(cDistr, ", ")
        }
      }
    }
    cDistr <- newJsonNestedObject("distribution", cDistr)
    cInitVal <- newJsonKeyPair("initialValue", compartment$initialValue)
    contents <- paste0(contents, newJsonObject(cName, cDistr, cInitVal))
    if (i < length(compartments)) {
      contents <- paste0(contents, ",\n")
    } else {
      contents <- paste0(contents, "\n")
    }
  }
  return(contents)
}

# Models to json
modelsToJson <- function(models) {
  contents <- "\n"
  for (i in 1:length(models)) {
    model <- models[[i]]
    mdName <- newJsonNestedArray("modelName", model$modelName, quotation = TRUE)
    mdTR <- newJsonKeyPair("transmissionRate", model$transmissionRate)
    mdComps <- newJsonNestedArray("compartments", compartmentsToJson(model$compartments))
    contents <- paste0(contents, newJsonObject(mdName, mdTR, mdComps))
    if (i < length(models)) {
      contents <- paste0(contents, ",\n")
    } else {
      contents <- paste0(contents, "\n")
    }
  }
  return(contents)
}

# Full model to json
fullModelToJson <- function(fullModel) {
  dfu <- newJsonKeyPair("daysFollowUp", fullModel$daysFollowUp)
  et <- newJsonKeyPair("errorTolerance", fullModel$errorTolerance)
  ts <- newJsonKeyPair("timeStep", fullModel$timeStep)
  ms <- newJsonNestedArray("modelStructure", fullModel$transitions, quotation = TRUE)
  ic <- newJsonNestedArray("infectiousComps", fullModel$infectiousComps, quotation = TRUE)
  ct <- newJsonNestedArray("contacts", contactsToJson(fullModel$contacts))
  md <- newJsonNestedArray("models", modelsToJson(fullModel$models))
  contents <- paste0(newJsonObject(dfu, et, ts, ms, ic, ct, md))
  return(contents)
}
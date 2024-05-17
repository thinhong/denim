# Constructor
newModel <- function(simulationDuration, errorTolerance, initialValues, 
                     parameters=NULL, transitions, timeStep = 1) {
  
  has_math_dist <- FALSE
  
  # Convert string in transitions into math expression and number to constant
  for (i in 1:length(transitions)) {
    if (is.character(transitions[[i]]) & length(transitions[[i]]) == 1) {
      transitions[[i]] <- mathexpr(transitions[[i]])
    }
    if (is.numeric(transitions[[i]]) & length(transitions[[i]]) == 1) {
      transitions[[i]] <- constant(transitions[[i]])
    }
    
    # if transition -> get parameters from Distribution object
    if( (class(transitions[[i]])=="Distribution")[[1]] ){
      if((transitions[[i]]$distribution) == "nonparametric" | 
               (transitions[[i]]$distribution) == "constant"|
               (transitions[[i]]$distribution) == "multinomial"){
        # dont do anything to non parametric model
        next
      }else if((transitions[[i]]$distribution) != "mathExpression"){
        parameters <- c(parameters, transitions[[i]][-1])
      }else{
        has_math_dist <- TRUE
      }
    }
  }
  
  # print(parameters) for debugging
  
  if(has_math_dist & is.null(parameters)){
    stop("Parameters must be defined for Math Expression Distribution")
  }
  
  mod <- list(
    simulationDuration = simulationDuration,
    errorTolerance     = errorTolerance,
    timeStep           = timeStep,
    initialValues      = initialValues,
    parameters         = parameters,
    transitions        = transitions)
  
  class(mod) <- c("Model", class(mod))
  mod
}

# Model to json
modelToJson <- function(mod) {
  initialValues <- mod$initialValues
  parameters    <- mod$parameters
  transitions   <- mod$transitions
  
  ivKeyPairs <- newJsonKeyPair(names(initialValues), initialValues)
  pmKeyPairs <- newJsonKeyPair(names(parameters), parameters)
  
  nb <- length(transitions)
  distr <- vector("list", nb)
  for (i in 1:nb) {
    distr[i] <- newJsonNestedObject(
      names(transitions)[i],
      distributionToJson(transitions[[i]]), inline = TRUE)
  }
  
  newJsonObject(
    newJsonKeyPair("simulationDuration", mod$simulationDuration),
    newJsonKeyPair("errorTolerance"    , mod$errorTolerance),
    newJsonKeyPair("timeStep"          , mod$timeStep),
    newJsonNestedObject("initialValues", newJsonObject(ivKeyPairs, inline = TRUE)),
    newJsonNestedObject("parameters"   , newJsonObject(pmKeyPairs, inline = TRUE)),
    newJsonNestedObject("transitions"  , newJsonObject(distr)))
}
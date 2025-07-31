# Constructor
#' @import glue
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
    
    if( (class(transitions[[i]])=="Transition")[[1]] ){
      if((transitions[[i]]$transition) == "constant"|
               (transitions[[i]]$transition) == "multinomial"){
        next
      }else if((transitions[[i]]$transition) == "nonparametric"){
        # check whether the distribution 
        # parameters are numeric or string, if it is a string (i.e. model input)
        # make sure that value is provided as one of the parameters
        if(is.character(transitions[[i]]$waitingTime)){
          par_val <- transitions[[i]]$waitingTime
          
          if(!(transitions[[i]]$waitingTime %in% names(parameters))){
            stop(glue::glue("Value for {par_val} of transition {names(transitions)[i]} must provided as one of the model parameters"))
          }
          transitions[[i]]$waitingTime <- parameters[[par_val]]
          # remove waiting dist from parameters list
          parameters[[par_val]] <- NULL
        }
      }else if((transitions[[i]]$transition) != "mathExpression"){
        # check whether the transition 
        # parameters are numeric or string, if it is a string (i.e. model input)
        # make sure that value is provided as one of the parameters
        sapply(names(transitions[[i]][-1]), \(par_name){
          par_val <- transitions[[i]][[par_name]]
          
          if(is.character(par_val)){
            if(!(par_val %in% names(parameters))){
              stop(glue::glue("Value for {par_val} of transition {names(transitions)[i]} must provided as one of the model parameters"))
            }
            transitions[[i]][[par_name]] <<- parameters[[par_val]]
          }
        })
        
        # also get parameters from Transition objects
        parameters <- c(parameters, transitions[[i]][-1])
      }else{
        has_math_dist <- TRUE
      }
    }
  }
  
  # print(parameters) for debugging
  
  if(has_math_dist & is.null(parameters)){
    stop("Parameters must be defined for Math Expression Transition")
  }
  
  # remove dist_init due to down stream problem
  parameters[!(names(parameters) == "dist_init")]
  
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
      transitionToJson(transitions[[i]]), inline = TRUE)
  }
  
  # add handlers when transitions doesn't involve any parameters
  if( length(parameters > 0)){
    newJsonObject(
      newJsonKeyPair("simulationDuration", mod$simulationDuration),
      newJsonKeyPair("errorTolerance"    , mod$errorTolerance),
      newJsonKeyPair("timeStep"          , mod$timeStep),
      newJsonNestedObject("initialValues", newJsonObject(ivKeyPairs, inline = TRUE)),
      newJsonNestedObject("parameters"   , newJsonObject(pmKeyPairs, inline = TRUE)),
      newJsonNestedObject("transitions"  , newJsonObject(distr)))
  }else{
    newJsonObject(
      newJsonKeyPair("simulationDuration", mod$simulationDuration),
      newJsonKeyPair("errorTolerance"    , mod$errorTolerance),
      newJsonKeyPair("timeStep"          , mod$timeStep),
      newJsonNestedObject("initialValues", newJsonObject(ivKeyPairs, inline = TRUE)),
      newJsonNestedObject("transitions"  , newJsonObject(distr)))
  }
  
}


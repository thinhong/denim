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
      distributionToJson(transitions[[i]]), inline = TRUE)
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

# NOTE: experimental 
# TODO: test across various test case
# denim_transitions <- function(...) {
#   exprs <- enexprs(...)
#   transitions <- list()
#   
#   for (expr in exprs) {
#     # Each DSL argument should be an assignment (i.e. a call to `=`)
#     if (!is_call(expr, "=")) {
#       stop("Each argument must be an assignment using '='.")
#     }
#     
#     # Extract lhs and rhs of the assignment
#     lhs_expr <- expr[[2]]
#     rhs_expr <- expr[[3]]
#     
#     # Convert the LHS to a string (e.g., "S -> I" or "36 * I -> R")
#     lhs_str <- paste(deparse(lhs_expr), collapse = " ")
#     
#     # Check whether rhs is mathematical formula or call to d_* function
#     # If the rhs is a math formula, deparse it into a string.
#     # Otherwise (e.g., a call to d_gamma or d_exponential) leave it as an expression.
#     rhs_val <- if (is.call(rhs_expr)) {
#       fn <- as.character(rhs_expr[[1]])
#       if (grepl("^d_", fn) | grepl("^nonparametric", fn)) {
#         # If the call is to a function starting with "d_", leave it as an expression.
#         rhs_expr
#       } else {
#         # Otherwise, deparse the expression to convert it to a string.
#         paste(deparse(rhs_expr), collapse = " ")
#       }
#     } else {
#       # For non-call objects (names, numbers), deparse them to a string.
#       paste(deparse(rhs_expr), collapse = " ")
#     }
#     
#     transitions[[lhs_str]] <- rhs_val
#   }
#   
#   class(transitions) <- "denim_transition"
# }
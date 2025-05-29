#' Define transitions using denim's domain-specific language (DSL)
#' 
#' This function parses model transitions defined in denim's DSL syntax
#'
#' @param x - an expression written in denim's DSL syntax. Each line should be a transition written in the 
#' format `compartment -> out_compartment = expression` where expression can be either a math expression
#' or one of denim's built-in dwell time distribution function
#'
#' @return denim_transition object
#' @import rlang
#' @export
#'
#' @examples
#' transitions <- denim_transitions({
#'   S -> I = beta * (I/N) * S * timeStep
#'   I -> R = d_gamma(rate = 1/4, shape = 3)
#' })
denim_transitions <- function(x) {
  # Capture the expression as a quoted expression
  expr <- substitute(x)
  # Convert the expression into a list of individual expressions
  expr_list <- as.list(expr)[-1]
  
  transitions <- list()
  
  # helper function to flip the -> assigment which is non-standard in R
  flip_assignment <- function(expr) {
    # Split the string at "<-"
    comps <- strsplit(expr, "<-")[[1]]
    # Trim whitespace from both sides
    comps <- trimws(comps)
    
    # Check that the expression is valid
    if (length(comps) != 2) {
      stop("Transition must be of the form 'comp_A -> comp_B' or 'weight*comp_A -> comp_B'")
    }
    
    # Rearrange the parts and return
    paste(comps[2], "->", comps[1])
  }
  
  for (expr in expr_list) {
    # Each DSL argument should be an assignment (i.e. a call to `=`)
    if (!is_call(expr, "=")) {
      stop("Each argument must be an assignment using '='.")
    }
    
    expr <- as.character(expr)
    
    # Extract lhs and rhs of the assignment
    lhs_expr <- expr[2]
    rhs_expr <- expr[3]
    
    # Convert the LHS to a string (e.g., "S -> I" or "36 * I -> R")
    # lhs_str <- paste(deparse(lhs_expr), collapse = " ")
    lhs_str <- flip_assignment(lhs_expr)
    
    # Check whether rhs is mathematical formula or call to d_* function
    # If the rhs is a math formula, deparse it into a string.
    # Otherwise (e.g., a call to d_gamma or d_exponential) leave it as an expression.
    rhs_val <- if (grepl("^d_", rhs_expr) | grepl("^nonparametric", rhs_expr)) {
      # If the call is to a function starting with "d_", leave it as an expression.
      rhs_expr <- eval(parse_expr(rhs_expr))
    } else {
      # Otherwise, deparse the expression to convert it to a string.
      rhs_expr
    }
    
    transitions[[lhs_str]] <- rhs_val
  }
  
  class(transitions) <- "denim_transition"
  
  transitions
}

test_that("Compare denim and deSolve", {
    # --- Transition def for denim
    transitions <- list(
    "S -> I" = d_exponential(0.2),
    "I -> R" = d_gamma(1/3, 2)
    )
    parameters <- c(rate = 0.2, gamma_rate = 1/3, shape=2) 
    initialValues <- c(S = 999, I = 1, I1 = 1, I2=0, R=0)

    # --- Transition def for deSolve
    transition_func <- function(t, state, param){
    with(as.list( c(state, param) ), {
        dS = -rate*S
        dI1 = rate*S - gamma_rate*I1
        dI2 = gamma_rate*I1 - gamma_rate*I2
        dI =  dI1 + dI2
        dR = gamma_rate*I2
        list(c(dS, dI, dI1, dI2, dR))
    })
    }

    # --- Timestep definition
    simulationDuration <- 20 
    timestep <- 0.01 # use small timestep to compare with deSolve which has continuous timestep

    # --- Simulate using both package
    mod <- sim(transitions = transitions, initialValues = initialValues, parameters = parameters, simulationDuration = simulationDuration, timeStep = timestep)
    ode_mod <- deSolve::ode(y = initialValues, times = seq(0, simulationDuration, timestep), parms = parameters, func = transition_func)


    ode_mod <- as.data.frame(ode_mod)

    tolerance <- 0.001
    # compare proportion of S, I, R
    abs_diff <- round(abs(mod[, c("S", "I", "R")]/1000 - ode_mod[, c("S", "I", "R")]/1000), 3)
    expect_true(all(abs_diff <= tolerance))
    
    # test plot function does not throw error
    expect_no_error(plot(mod))
})


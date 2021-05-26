# discreteModel

## Installation
Use `install_github()` to install this package
```
remotes::install_github("thinhong/discreteModel")
```

## Example
This script will simulate a SIR model of 2 locations: Ho Chi Minh and Ha Noi
```
library(discreteModel)

# Make a contact matrix among locations
M_location <- matrix(c(0.85, 0.1, 0.1, 0.95), nrow = 2, ncol = 2, 
                     dimnames = list(c("HCM", "HN"), c("HCM", "HN")))

contacts <- list(
  location = M_location
)

transitions <- c(
  "S -> I", 
  "I -> R"
)

# Use list to be consistent with the distributions below
initialValues <- list(
  HCM = list(
    S = 999,
    I = 1,
    R = 0),
  HN = list(
    S = 699,
    I = 0,
    R = 0)
)

# Use list because R doesn't allow to create NULL values in a vector
distributions <- list(
  HCM = list(
    S = NULL,
    I = exponential(rate = 1.5),
    R = NULL),
  HN = list(
    S = NULL,
    I = gamma(shape = 5, scale = 2),
    R = NULL
  )
)

fmod <- runSim(daysFollowUp = 5000, errorTolerance = 0.01, timeStep = 0.01, 
               transmissionRate = 1.5, infectiousComps = c("I"), 
               contacts = contacts, transitions = transitions,
               initialValues = initialValues, distributions = distributions)
```

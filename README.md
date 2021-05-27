# discreteModel

## Installation
Use `install_github()` to install this package
```
remotes::install_github("thinhong/discreteModel")
```

## Example
### Simple SIR model without contact
Here we have to define:
* `transitions`: structure of the model or transitions among compartments, for instance S -> I -> R would be defined by `"S -> I"`, `"I -> R"` (note the `""` symbol, this will be read as strings), white space can be ignored here (i.e we can write `"S->I"`)
* `initialValues`: initial values of compartments
* `distributions`: distributions of compartments, currently `exponential(rate)`, `gamma(scale, shape)`, and `weibull(scale, shape)` are available
```
library(discreteModel)

transitions <- c(
  "S -> I", 
  "I -> R"
)

# Use list to be consistent with the distributions below
initialValues <- list(
  S = 999,
  I = 1,
  R = 0
)

# Use list because R does not allow to create NULL values in a vector
distributions <- list(
  S = NULL,
  I = exponential(rate = 1.5),
  R = NULL
)

fmod <- runSim(daysFollowUp = 5000, errorTolerance = 0.01, timeStep = 0.01, 
               transmissionRate = 1.5, infectiousComps = c("I"), 
               transitions = transitions,
               initialValues = initialValues, distributions = distributions)
```

### Contact between locations
We now add a parameter contacts, which is a list of contact matrices.

`initialValues` and `distributions` will be defined as a list of locations (noted that we have to use list instead of a vector because R does not allow to create NULL values in a vector).

This script will simulate a SIR model of 2 locations: Ho Chi Minh and Ha Noi.
```
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

distributions <- list(
  HCM = list(
    S = NULL,
    I = gamma(scale = 2, shape = 5),
    R = NULL),
  HN = list(
    S = NULL,
    I = weibull(scale = 2, shape = 5),
    R = NULL
  )
)

fmod <- runSim(daysFollowUp = 5000, errorTolerance = 0.01, timeStep = 0.01, 
               transmissionRate = 1.5, infectiousComps = c("I"), 
               contacts = contacts, transitions = transitions,
               initialValues = initialValues, distributions = distributions)
```

# discreteModel

## Installation
Install `devtools`  and use `install_github()` to install this package
```
install.packages("devtools")
library(devtools)
install_github("thinhong/discreteModel")
```

## Example
This script will simulate a SIR model of 2 locations Ho Chi Minh and Ha Noi
```
library(discreteModel)

# Model
ms <- setModelStructure("S->I", "I->R")
ic <- setInfectiousComps("I")
ca <- setContactAssumption("location")
contacts <- createContact("location", c("HCM", "HN"), c("HCM <-> HCM: 0.85", "HCM <-> HN: 0.1", "HN <-> HN: 0.95"))

S_HCM <- createCompartment(name = "S", 
                           distribution = createDistribution("transitionProb", transitionProb = 0.0), 
                           initialValue = 1000)
I_HCM <- createCompartment(name = "I", 
                           distribution = createDistribution("exponential", rate = 0.3), 
                           initialValue = 1)
R_HCM <- createCompartment(name = "R", 
                           distribution = createDistribution("transitionProb", transitionProb = 0.0), 
                           initialValue = 0)
S_HN <- createCompartment(name = "S", 
                          distribution = createDistribution("transitionProb", transitionProb = 0.0), 
                          initialValue = 699)
I_HN <- createCompartment(name = "I", 
                          distribution = createDistribution("gamma", shape = 3, scale = 2), 
                          initialValue = 1)
R_HN <- createCompartment(name = "R", 
                          distribution = createDistribution("transitionProb", transitionProb = 0.0), 
                          initialValue = 0)

HCM <- allCompartments(modelName = c("HCM"), transmissionRate = 1.5, S_HCM, I_HCM, R_HCM)
HN <- allCompartments(modelName = c("HN"), transmissionRate = 1.5, S_HN, I_HN, R_HN)

exportJSON <- allModels(daysFollowUp = 50002, timeStep = 0.01, modelStructure = ms,
              infectiousComps = ic, contactAssumption = ca,
              contacts = contacts, models = c(HCM, HN))

df <- simulate(exportJSON)

```

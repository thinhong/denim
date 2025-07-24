# Implementation for IONISE algorithm
# Source code: https://github.com/Mathbiomed/IONISE
# only include functions for SEIR model simulation 
library(invgamma)

# ========= Helper functions ============
conv_two_pdfs <- function(tmax, theta1, theta2, dist_type){
  if(tmax == 0){
    conv_val = 0
    return(conv_val)
  }else{
    xspan = seq(from = 0, to = tmax, by = tmax/1000)
    conv_val = numer_int(xspan, eval_density(xspan, theta = theta1, dist_type = dist_type, density_type = "pdf") * eval_density(max(xspan) - xspan, theta = theta2, dist_type = dist_type, density_type = "pdf"))
  }
  return(conv_val)
}

eval_density <- function(x, theta, dist_type, density_type){
  if (density_type == "pdf"){
    if (dist_type == "exp"){
      density_val = dexp(x, rate = 1/theta[2])
    }else if (dist_type == "gamma"){
      density_val = dgamma(x, shape = theta[1], scale = theta[2])
    }else if (dist_type == "invgamma"){
      zero_pos = which(x==0)
      density_val = dinvgamma(x, shape = theta[1], rate = theta[2])
      density_val[zero_pos] = 0
    }else if (dist_type == "lognormal"){
      density_val = dlnorm(x, meanlog = theta[1], sdlog = theta[2])
    }else if (dist_type == "weibull"){
      density_val = dweibull(x, shape = theta[1], scale = theta[2])
    }else{
      warning("The type of distribution must be one of c(exp, gamma, invgamma, lognormal, weibull)!")
      return(NA)
    }
  }else if (density_type == "cdf"){
    if (dist_type == "exp"){
      density_val = pexp(x, rate = 1/theta[2])
    }else if (dist_type == "gamma"){
      density_val = pgamma(x, shape = theta[1], scale = theta[2])
    }else if (dist_type == "invgamma"){
      density_val = pinvgamma(x, shape = theta[1], rate = theta[2])
    }else if (dist_type == "lognormal"){
      density_val = plnorm(x, meanlog = theta[1], sdlog = theta[2])
    }else if (dist_type == "weibull"){
      density_val = pweibull(x, shape = theta[1], scale = theta[2])
    }else{
      warning("The type of distribution must be one of c(exp, gamma, invgamma, lognormal, weibull)!")
      return(NA)
    }
  }else{
    warning("The type of density must be one of c(pdf, cdf)!")
    return(NA)
  }
  return(density_val)
}


numer_int <- function(xspan, yval){
  # xspan has to be evenly spaced.
  if(length(xspan) == 1){
    intval0 = 0
  }else{
    h = xspan[2] - xspan[1]
    # if the number of data points is even then it uses the trapezoidal rule.
    if(length(xspan)%%2 == 0){
      intval0 = 0
      for(jj in 2:length(xspan)){
        intval0 = intval0 + (xspan[jj] - xspan[jj-1]) * (yval[jj]+yval[jj-1])/2
      }
    }else{ # if the number of data points is odd then it uses Simpson's rule.
      intval0 = 0
      for(jj in seq(from=2,by=2,to=length(xspan))){
        intval0 = intval0 + h * (yval[jj-1] + 4*yval[jj] + yval[jj+1])/3
      }
    }
  }
  return(intval0)
}

diff_midpoint_extension <- function(x, t_interv){
  # x must be an evenly-spaced time series data.
  if(length(x) == 1){
    return(0)
  }else if(length(x) == 2){
    return(rep(diff(x), 2))
  }else{
    diff_x = diff(x)/t_interv
    dL = length(diff_x)
    deriv_x = rep(NA, length(x))
    deriv_x[2:dL] = (diff_x[1:(dL-1)] + diff_x[2:dL])/2
    # handle start and end points
    deriv_x[1] = 2*diff_x[1] - deriv_x[2]
    deriv_x[dL+1] = 2*diff_x[dL] - deriv_x[dL]
    return(deriv_x)
  }
}

mean_trajectory_SEIR_dist_input <- function(timespan, theta, y_init, dist_type){
  beta.param = theta[1]
  alpha1 = theta[2]
  beta1 = theta[3]
  alpha2 = theta[4]
  beta2 = theta[5]
  
  Ntot = sum(y_init)
  
  h = timespan[2] - timespan[1]
  St = rep(NA, length(timespan))
  Et = rep(NA, length(timespan))
  It = rep(NA, length(timespan))
  Rt = rep(NA, length(timespan))
  conv_val = rep(NA, length(timespan))
  
  # hs = rep(NA, length(timespan))
  # hs_tilde = rep(NA, length(timespan))
  St[1] = y_init[1]
  Et[1] = y_init[2]
  It[1] = y_init[3]
  Rt[1] = y_init[4]
  
  for(ii in 1:length(timespan)){
    conv_val[ii] = conv_two_pdfs(tmax = timespan[ii], theta1 = c(alpha1, beta1), theta2 = c(alpha2, beta2), dist_type = dist_type)
  }
  
  for(ii in 1:(length(timespan)-1)){
    
    term1 = beta.param * St[ii] * It[ii] / Ntot
    # Compute inflow to E at time t in terms of S(0) and I(0)
    term2 = beta.param * St[1] * It[1] / Ntot * eval_density(x = timespan[ii], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "cdf") + 
      numer_int(timespan[1:ii], beta.param * diff_midpoint_extension(St[1:ii] * It[1:ii], h) / Ntot * eval_density(x = max(timespan[1:ii]) - timespan[1:ii], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "cdf")) + 
      Et[1] * eval_density(x = timespan[ii], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "pdf")
    # Compute inflow to I at time t in terms of S(0) and I(0)
    term3 = numer_int(timespan[1:ii], beta.param * St[1:ii] * It[1:ii] / Ntot * conv_val[ii:1]) +  It[1] * eval_density(x = timespan[ii], theta = c(alpha2, beta2), dist_type = dist_type, density_type = "pdf") + Et[1] * conv_val[ii]
    
    term1 = max(term1, 0) # adjusted to avoid negative values 
    term2 = max(term2, 0) # adjusted to avoid negative values 
    term3 = max(term3, 0) # adjusted to avoid negative values 
    
    S_increm = - term1
    E_increm = term1 - term2
    I_increm = term2 - term3
    R_increm = term3
    
    St_tilde = St[ii] + h*S_increm
    Et_tilde = Et[ii] + h*E_increm
    It_tilde = It[ii] + h*I_increm
    Rt_tilde = Rt[ii] + h*R_increm
    
    
    term1_tilde = beta.param * St_tilde * It_tilde / Ntot
    # Compute outflow to E at time t in terms of S(0) and I(0)
    term2_tilde = beta.param * St[1] * It[1] / Ntot * eval_density(x = timespan[ii+1], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "cdf") + 
      numer_int(timespan[1:(ii+1)], beta.param * diff_midpoint_extension(c(St[1:ii], St_tilde) * c(It[1:ii], It_tilde), h) / Ntot * eval_density(x = max(timespan[1:(ii+1)]) - timespan[1:(ii+1)], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "cdf")) + 
      Et[1] * eval_density(x = timespan[ii+1], theta = c(alpha1, beta1), dist_type = dist_type, density_type = "pdf")
    # Compute outflow to E at time t in terms of S(0) and I(0)
    term3_tilde = numer_int(timespan[1:(ii+1)], beta.param * c(St[1:ii], St_tilde) * c(It[1:ii], It_tilde) / Ntot * conv_val[(ii+1):1])  + It[1] * eval_density(x = timespan[ii+1], theta = c(alpha2, beta2), dist_type = dist_type, density_type = "pdf") + Et[1] * conv_val[ii+1]
    
    term1_tilde = max(term1_tilde, 0) # adjusted to avoid negative values 
    term2_tilde = max(term2_tilde, 0) # adjusted to avoid negative values 
    term3_tilde = max(term3_tilde, 0) # adjusted to avoid negative values 
    
    S_increm_tilde = - term1_tilde
    E_increm_tilde = term1_tilde - term2_tilde
    I_increm_tilde = term2_tilde - term3_tilde
    R_increm_tilde = term3_tilde
    
    St[ii+1] = St[ii] + h/2 * (S_increm + S_increm_tilde)
    Et[ii+1] = Et[ii] + h/2 * (E_increm + E_increm_tilde)
    It[ii+1] = It[ii] + h/2 * (I_increm + I_increm_tilde)
    Rt[ii+1] = Rt[ii] + h/2 * (R_increm + R_increm_tilde)
    
  }
  return(list("St" = St, "Et" = Et, "It" = It, "Rt" = Rt))
}
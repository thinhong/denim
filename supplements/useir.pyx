# implementation from https://github.com/jjgomezcadenas/useirn/blob/master/nb/uSEIR.ipynb
from scipy.stats import gamma
from scipy.stats import expon
from scipy.stats import poisson
import numpy as np
cimport numpy as np
import pandas as pd

cdef compute_eps(str pde_sampling ):
    if pde_sampling == 'Fine':
        eps = 0.01
        dim = 18000
    else:
        eps =  0.1 
        dim = 1800
    return eps, dim
    
cdef compute_gamma_pde(int ne, double t_shape, double t_scale, double eps):
    
    cdef double [:] pdE = np.zeros(ne, dtype=np.double) 
    cdef double cd1 = 0
    cdef double cd2
    cdef int i
       
    for i in range(ne):
        cd2    = gamma.cdf(i*eps, a=t_shape, scale=t_scale)
        pdE[i] = cd2 - cd1
        cd1    = cd2

    return pdE

cdef compute_expon_pde(int ne, double t_scale, double eps):
    
    cdef double [:] pdE = np.zeros(ne, dtype=np.double) 
    cdef double cd1 = 0
    cdef double cd2
    cdef int i
       
    for i in range(ne):
        cd2    = expon.cdf(i*eps, scale=t_scale)
        pdE[i] = cd2 - cd1
        cd1    = cd2

    return pdE

cdef compute_poisson_pde(int ne, double mu, double eps):
    
    cdef double [:] pdE = np.zeros(ne, dtype=np.double) 
    cdef double cd1 = 0
    cdef double cd2
    cdef int i
       
    for i in range(ne):
        cd2    = poisson.cdf(i*eps, mu)
        pdE[i] = cd2 - cd1
        cd1    = cd2

    return pdE

cpdef compute_pde(str dist,
                  double ti_shape,  
                  double ti_scale, 
                  double tr_shape,  
                  double tr_scale,
                  str    pde_sampling = 'Fine'):
    
    kdist =['gamma', 'expon', 'poisson']
    
    cdef double eps
    cdef int    dim
    cdef int nE
    cdef int nI
    cdef double [:] pdE
    cdef double [:] pdI
    
    eps, dim         = compute_eps(pde_sampling)
    cdef double tol  = 0.9999
    
    if dist == 'gamma':
        nE = int(gamma.ppf(tol, a=ti_shape, scale=ti_scale) / eps)
        nI = int(gamma.ppf(tol, a=tr_shape, scale=tr_scale) / eps)
        pdE = compute_gamma_pde(nE, ti_shape, ti_scale, eps)
        pdI = compute_gamma_pde(nI, tr_shape, tr_scale, eps)
        ti = float(gamma.stats(a = ti_shape, scale = ti_scale, moments='m'))
        tr = float(gamma.stats(a = tr_shape, scale =  tr_scale, moments='m'))
        
    elif dist == 'expon':
        nE = int(expon.ppf(tol, scale=ti_scale) / eps)
        nI = int(expon.ppf(tol, scale=tr_scale) / eps)
        pdE = compute_expon_pde(nE, ti_scale, eps)
        pdI = compute_expon_pde(nI, tr_scale, eps)
        ti = float(expon.stats(scale=ti_scale, moments='m'))
        tr = float(expon.stats(scale=tr_scale, moments='m'))

    elif dist == 'poisson':
        nE = int(poisson.ppf(tol, mu=ti_shape) / eps)
        nI = int(poisson.ppf(tol, mu=tr_shape) / eps)
        pdE = compute_poisson_pde(nE, ti_shape, eps)
        pdI = compute_poisson_pde(nI, tr_shape, eps)
        ti = float(poisson.stats(mu=ti_shape, moments='m'))
        tr = float(poisson.stats(mu=ti_shape, moments='m'))
    else:
        print(f' Distribution {dist} not yet implemented please use one in {kdist}')
        return 0
      
    print(f' Function compute_pde with sampling = {pde_sampling}, time epsilon = {eps}')
    print(f' statistical distribution = {dist}, ti = {ti}, tr = {tr}')
    print(f' number of exposed compartments = {nE}, infected compartments = {nI}')
    print(f'len pdE = {len(pdE)}, max(pdE) ={np.max(pdE)} len pdi = {len(pdI)} max(pdI) ={np.max(pdI)}')
    
    return nE, pdE, nI, pdI, ti, tr
    
cpdef csolve_uSeir(str dist            = 'gamma', 
                   double ti_shape     = 5.5,  
                   double ti_scale     = 1, 
                   double tr_shape     = 6.5,  
                   double tr_scale     = 1,
                   str    pde_sampling = 'Fine',
                   double R0           = 3.5):
    """
    Function csolve_uSeir solves numerically the uSEIR equations (Hernandez et al, Preprint)
    following the numerical recipe by A. Ramos ( https://gitlab.ift.uam-csic.es/alberto/useir)
    
    THE BEER-WARE LICENSE:
    P. Hernandez, C. Pena, J.J. Gomez Cadenas wrote these files. As long
    as you retain this notice you can do whatever you want with this
    stuff. If we meet some day, and you think this stuff is worth it, you
    can buy us a beer in return.
    
    Arguments:
    dist       : either gamma, expon or poisson. Other distributions can easily be added.
    shape and scale parameters:
               for gamma you need to specify both --ti_shape, ti_scale, tr_shape, tr_scale
               for expon you need to specify only ti_scale, tr_scale (which correspond to the average ti, tr)
               for poisson you need to specify only ti_shape, tr_shape (which correspond to mu for the average ti, tr)
    
    The default sampling is Fine, which takes a epsilon time of 0.01. For faster execution specify Coarse 
    """

    cdef double N       = 1e+6,
    cdef double Smin    = 1e-10, 
    cdef double Emin    = 1e-10,
    cdef double nmax    = 21000
    cdef double eps
    cdef double pn
    cdef double prob

    cdef int    dim
    cdef int nE 
    cdef int nI
    
    cdef double [:] pdE
    cdef double [:] pdI
    
    nE, pdE, nI, pdI, ti, tr = compute_pde(dist, ti_shape, ti_scale, tr_shape, tr_scale, pde_sampling)

    cdef double [:] I   = np.zeros(nI, dtype=np.double)
    cdef double [:] E   = np.zeros(nE, dtype=np.double)
    E[0] = 1 / N


    cdef double S   = 1 - 1/N
    cdef double R   = 0
    cdef double sI  = 0
    
    prob = R0 / tr 
    eps, dim = compute_eps(pde_sampling)
    pn = prob * eps
    
    print(f'prob = {prob}, pn = {pn}')
    
    cdef np.ndarray[np.float64_t, ndim=1] TT = np.zeros(dim, dtype=np.double)
    cdef np.ndarray[np.float64_t, ndim=1] SS = np.zeros(dim, dtype=np.double)
    cdef np.ndarray[np.float64_t, ndim=1] EE = np.zeros(dim, dtype=np.double)
    cdef np.ndarray[np.float64_t, ndim=1] II = np.zeros(dim, dtype=np.double)
    cdef np.ndarray[np.float64_t, ndim=1] RR = np.zeros(dim, dtype=np.double)
    
    cdef int n    = 0
    cdef int end
    cdef int k
    while True:
        
        R += I[0]
        
        end = nI - 1
        for k in range(end):
            I[k] = I[k+1] + pdI[k] * E[0]
        I[end] = pdI[end] * E[0]
        
        #print(I)

        end = nE - 1
        for k in range(end):
            E[k] = E[k+1] + pn * pdE[k] * sI * S
        E[end]   = pn * pdE[end] * sI * S

        #print(E)
        
        S  = S - pn * sI * S
        sI = np.sum(I)
        sE = np.sum(E)
        
        TT[n] = n * eps
        SS[n] = S
        EE[n] = sE
        II[n] = sI
        RR[n] = R
        
        
        #print(f't = {n*eps} S = {S} E ={sE} I ={sI} R = {R}')
        n+=1
        if (sE < Smin and sI < Emin) or n > dim -1:
            break
    
    return pd.DataFrame({'t': TT.astype(float), 'S': SS.astype(float),'E' : EE.astype(float), 'I' : II.astype(float), 'R': RR.astype(float)})
    #return TT.astype(float), SS.astype(float), EE.astype(float), II.astype(float), RR.astype(float)
    #return TT, SS, EE, II, RR

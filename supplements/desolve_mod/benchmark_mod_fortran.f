c file benchmark_mod_fortran.f

            subroutine initmod(odeparms)
              external odeparms
              double precision parms(6)
              common /myparms/parms
              
              call odeparms(6, parms)
            return
            end
            
            subroutine derivs(neq, t, y, ydot, yout, ip)
              double precision t, y, ydot, R0, scale_I, shape_I, scale_R
              double precision shape_R, N
              integer neq, ip(*)
              dimension y(8), ydot(8), yout(*)
              
              common /myparms/R0,scale_I,shape_I,scale_R,shape_R,N
            
              double precision tr, rate_I, rate_R
            
              tr = scale_R * shape_R
            
              rate_I = 1.0d0 / scale_I
              rate_R = 1.0d0 / scale_R
            
              ydot(1) = - (R0 / tr) * y(1) * y(7) / N
              ydot(2) = (R0 / tr) * y(1) * y(7) / N - rate_I * y(2)
              ydot(3) = rate_I * y(2) - rate_I * y(3)
              ydot(4) = ydot(2) + ydot(3)
              ydot(5) = rate_I * y(3) - rate_R * y(5)
              ydot(6) = rate_R * y(5) - rate_R * y(6)
              ydot(7) = ydot(5) + ydot(6)
              ydot(8) = rate_R * y(6)
            
            return
            end
c end of file benchmark_mod_fortran.f

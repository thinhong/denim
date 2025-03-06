# Code for getting run time in parallel 
from useir_python import solve_uSeir
import time
import concurrent.futures
import pickle
from statistics import mean

python_runs = []

def get_python_runtime(n):
  start = time.time()
  df = solve_uSeir(ti_shape     = 2,
                     ti_scale     = 4,
                     tr_shape     = 2,
                     tr_scale     = 3,
                     R0           = 3.5)
  end = time.time()
  return end - start
  
if __name__ == '__main__':
  # multithread instead for quicker result
  with concurrent.futures.ProcessPoolExecutor(max_workers=8) as executor:
      python_runs = list(executor.map(get_python_runtime, range(50)))
      
  with open("python_runs.pkl", 'wb') as f:
      pickle.dump(python_runs, f)
  
  print(python_runs)

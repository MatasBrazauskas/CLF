Benchmark 1: ./cmake-build-debug/clf -f data/10M.log -n 5 -r 4000000
  Time (mean ± σ):      1.533 s ±  0.014 s    [User: 1.403 s, System: 0.130 s]
  Range (min … max):    1.514 s …  1.554 s    10 runs

calls to allocation functions: 39 (15/s)
temporary memory allocations: 26 (10/s)
peak heap memory consumption: 671.16M
peak RSS (including heaptrack overhead): 1.62G
total memory leaked: 1.02K

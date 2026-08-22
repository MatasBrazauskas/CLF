Benchmark 1: ./cmake-build-debug/clf -f data/10M.log -n 5 -e 0.0001 -d 0.0001
  Time (mean ± σ):      9.446 s ±  0.080 s    [User: 9.421 s, System: 0.021 s]
  Range (min … max):    9.361 s …  9.627 s    10 runs

calls to allocation functions: 44 (4/s)
temporary memory allocations: 27 (2/s)
peak heap memory consumption: 15.81M
peak RSS (including heaptrack overhead): 967.45M
total memory leaked: 1.02K

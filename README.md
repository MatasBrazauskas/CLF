Benchmark 1: ./cmake-build-debug/clf -f data/10M.log -n 5 -e 0.0001 -d 0.0001
  Time (mean ± σ):      3.431 s ±  0.126 s    [User: 34.396 s, System: 0.169 s]
  Range (min … max):    3.236 s …  3.680 s    10 runs

calls to allocation functions: 397 (113/s)
temporary memory allocations: 27 (7/s)
peak heap memory consumption: 220.41M
peak RSS (including heaptrack overhead): 1.17G
total memory leaked: 2.37K

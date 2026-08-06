Custom FNV hashing function:
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/100k -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/100k -n 5
  Time (mean ± σ):      78.9 ms ±   3.8 ms    [User: 73.9 ms, System: 4.8 ms]
  Range (min … max):    71.7 ms …  88.9 ms    37 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/1M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/1M -n 5
  Time (mean ± σ):      1.251 s ±  0.016 s    [User: 1.215 s, System: 0.034 s]
  Range (min … max):    1.232 s …  1.287 s    10 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/10M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5
  Time (mean ± σ):     15.285 s ±  0.524 s    [User: 14.952 s, System: 0.294 s]
  Range (min … max):   14.752 s … 16.436 s    10 runs

A5 hashing function:
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/100k -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/100k -n 5
  Time (mean ± σ):      74.9 ms ±   6.7 ms    [User: 70.0 ms, System: 4.7 ms]
  Range (min … max):    67.5 ms … 100.9 ms    41 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/1M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/1M -n 5
  Time (mean ± σ):      1.207 s ±  0.010 s    [User: 1.174 s, System: 0.030 s]
  Range (min … max):    1.195 s …  1.226 s    10 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/10M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5
  Time (mean ± σ):     14.457 s ±  0.048 s    [User: 14.142 s, System: 0.285 s]
  Range (min … max):   14.388 s … 14.544 s    10 runs

Growth factor of 0.7:
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/100k -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/100k -n 5
  Time (mean ± σ):      73.1 ms ±   5.4 ms    [User: 67.8 ms, System: 5.1 ms]
  Range (min … max):    66.5 ms …  93.9 ms    41 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/1M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/1M -n 5
  Time (mean ± σ):      1.076 s ±  0.009 s    [User: 1.041 s, System: 0.034 s]
  Range (min … max):    1.061 s …  1.094 s    10 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/10M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5
  Time (mean ± σ):     12.965 s ±  0.033 s    [User: 12.650 s, System: 0.290 s]
  Range (min … max):   12.921 s … 13.024 s    10 runs

Growth factor of 0.5:
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/100k -n 5'; hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/1M -n 5'; hyperfine --warmup 3 -- './cmake-build-debug/clf -f data/10M -n 5'
Benchmark 1: ./cmake-build-debug/clf -f data/100k -n 5
  Time (mean ± σ):      72.6 ms ±   4.1 ms    [User: 67.3 ms, System: 5.1 ms]
  Range (min … max):    65.4 ms …  85.7 ms    42 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/1M -n 5
  Time (mean ± σ):     996.1 ms ±   9.1 ms    [User: 959.8 ms, System: 34.2 ms]
  Range (min … max):   983.6 ms … 1014.3 ms    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5
  Time (mean ± σ):     12.364 s ±  0.410 s    [User: 11.953 s, System: 0.322 s]
  Range (min … max):   12.107 s … 13.436 s    10 runs


Prediction about hash maps size:
(Growth factor 0.7)
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 1000000
  Time (mean ± σ):     12.734 s ±  0.545 s    [User: 12.267 s, System: 0.315 s]
  Range (min … max):   12.390 s … 14.169 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 2000000
  Time (mean ± σ):     11.836 s ±  0.127 s    [User: 11.532 s, System: 0.294 s]
  Range (min … max):   11.697 s … 12.053 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 5000000
  Time (mean ± σ):     10.852 s ±  0.397 s    [User: 10.527 s, System: 0.314 s]
  Range (min … max):   10.446 s … 11.815 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 10000000
  Time (mean ± σ):     19.404 s ±  3.394 s    [User: 18.631 s, System: 0.714 s]
  Range (min … max):   10.063 s … 21.558 s    10 runs

(Growth factor 0.5)
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 1000000
  Time (mean ± σ):     12.005 s ±  0.698 s    [User: 11.553 s, System: 0.331 s]
  Range (min … max):   11.589 s … 13.899 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 2000000
  Time (mean ± σ):     12.020 s ±  0.608 s    [User: 11.678 s, System: 0.328 s]
  Range (min … max):   11.291 s … 12.952 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 5000000
  Time (mean ± σ):     10.675 s ±  0.534 s    [User: 10.329 s, System: 0.332 s]
  Range (min … max):   10.239 s … 12.057 s    10 runs
 
Benchmark 1: ./cmake-build-debug/clf -f data/10M -n 5 -r 10000000
  Time (mean ± σ):      9.955 s ±  0.147 s    [User: 9.519 s, System: 0.422 s]
  Range (min … max):    9.826 s … 10.292 s    10 runs

(Growth factor of 0.5)
Reserve 10M elements:
total runtime: 12.69s.
calls to allocation functions: 8001340 (630771/s)
temporary memory allocations: 26 (2/s)
peak heap memory consumption: 888.69M
peak RSS (including heaptrack overhead): 1.97G
total memory leaked: 1.02K

Reserve 5M elements:
total runtime: 12.38s.
calls to allocation functions: 8001340 (646207/s)
temporary memory allocations: 26 (2/s)
peak heap memory consumption: 632.60M
peak RSS (including heaptrack overhead): 1.71G
total memory leaked: 1.02K

Reserve 2M elements:
total runtime: 14.21s.
calls to allocation functions: 8001342 (562959/s)
temporary memory allocations: 26 (1/s)
peak heap memory consumption: 547.15M
peak RSS (including heaptrack overhead): 1.63G
total memory leaked: 1.02K

(Growth factor of 0.7)
Reserve 10M elements:
total runtime: 25.73s.
calls to allocation functions: 8001340 (310997/s)
temporary memory allocations: 26 (1/s)
peak heap memory consumption: 752.42M
peak RSS (including heaptrack overhead): 1.83G
total memory leaked: 1.02K

Reserve 5M elements:
total runtime: 26.41s.
calls to allocation functions: 8001340 (302908/s)
temporary memory allocations: 26 (0/s)
peak heap memory consumption: 565.50M
peak RSS (including heaptrack overhead): 1.64G
total memory leaked: 1.02K

Reserve 2M elements:
total runtime: 28.23s.
calls to allocation functions: 8001342 (283474/s)
temporary memory allocations: 26 (0/s)
peak heap memory consumption: 503.13M
peak RSS (including heaptrack overhead): 1.58G
total memory leaked: 1.02K

Reserve 1M elements:
total runtime: 31.13s.
calls to allocation functions: 8001344 (257005/s)
temporary memory allocations: 26 (0/s)
peak heap memory consumption: 491.19M
peak RSS (including heaptrack overhead): 1.57G
total memory leaked: 1.02K


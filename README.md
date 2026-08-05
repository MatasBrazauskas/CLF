matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/100k'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/100k
  Time (mean ± σ):     198.1 ms ±   4.2 ms    [User: 191.4 ms, System: 6.2 ms]
  Range (min … max):   191.0 ms … 206.1 ms    15 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/1M'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/1M
  Time (mean ± σ):      2.471 s ±  0.022 s    [User: 2.422 s, System: 0.047 s]
  Range (min … max):    2.446 s …  2.515 s    10 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/10M'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/10M
  Time (mean ± σ):     28.176 s ±  1.123 s    [User: 27.677 s, System: 0.477 s]
  Range (min … max):   26.804 s … 30.654 s    10 runs



total runtime: 0.45s.
calls to allocation functions: 760292 (1678348/s)
temporary memory allocations: 36116 (79726/s)
peak heap memory consumption: 5.89M
peak RSS (including heaptrack overhead): 12.41M
total memory leaked: 1.02K

total runtime: 4.29s.
calls to allocation functions: 7600505 (1770029/s)
temporary memory allocations: 361693 (84232/s)
peak heap memory consumption: 62.72M
peak RSS (including heaptrack overhead): 80.81M
total memory leaked: 1.02K

total runtime: 45.70s.
calls to allocation functions: 76002800 (1663117/s)
temporary memory allocations: 3619337 (79199/s)
peak heap memory consumption: 607.69M
peak RSS (including heaptrack overhead): 740.98M
total memory leaked: 1.02K


 Performance counter stats for './cmake-build-debug/clf -f data/100k -n 5':

                12      context-switches                 #     63.2 cs/sec  cs_per_second     
                 1      cpu-migrations                   #      5.3 migrations/sec  migrations_per_second
             1,959      page-faults                      #  10311.0 faults/sec  page_faults_per_second
            189.99 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
           503,977      cpu_core/branch-misses/          #      0.1 %  branch_miss_rate         (99.45%)
       390,803,199      cpu_core/branches/               #   2057.0 M/sec  branch_frequency     (99.45%)
       805,679,657      cpu_core/cpu-cycles/             #      4.2 GHz  cycles_frequency       (99.45%)
     2,465,407,208      cpu_core/instructions/           #      3.1 instructions  insn_per_cycle  (99.45%)
         2,156,211      cpu_atom/branch-misses/          #      9.5 %  branch_miss_rate         (0.02%)
        99,921,084      cpu_atom/branches/               #    525.9 M/sec  branch_frequency     (0.55%)
       587,181,820      cpu_atom/cpu-cycles/             #      3.1 GHz  cycles_frequency       (0.55%)
       552,695,329      cpu_atom/instructions/           #      0.9 instructions  insn_per_cycle  (0.55%)
             TopdownL1 (cpu_core)                        #      3.6 %  tma_bad_speculation    
                                                         #     19.6 %  tma_frontend_bound       (99.45%)
                                                         #     21.6 %  tma_backend_bound      
                                                         #     55.3 %  tma_retiring             (99.45%)
             TopdownL1 (cpu_atom)                        #     14.0 %  tma_bad_speculation      (0.53%)
                                                         #     45.9 %  tma_backend_bound        (0.53%)
                                                         #     17.0 %  tma_frontend_bound       (0.01%)
                                                         #      nan %  tma_retiring             (0.00%)

       0.191296197 seconds time elapsed

       0.184171000 seconds user
       0.007006000 seconds sys

 Performance counter stats for './cmake-build-debug/clf -f data/1M -n 5':

               143      context-switches                 #     58.8 cs/sec  cs_per_second     
                 4      cpu-migrations                   #      1.6 migrations/sec  migrations_per_second
            21,195      page-faults                      #   8716.2 faults/sec  page_faults_per_second
          2,431.68 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
         5,345,129      cpu_core/branch-misses/          #      0.1 %  branch_miss_rate         (99.67%)
     3,907,242,796      cpu_core/branches/               #   1606.8 M/sec  branch_frequency     (99.67%)
    10,408,202,858      cpu_core/cpu-cycles/             #      4.3 GHz  cycles_frequency       (99.67%)
    24,667,117,752      cpu_core/instructions/           #      2.4 instructions  insn_per_cycle  (99.67%)
         4,586,695      cpu_atom/branch-misses/          #      0.2 %  branch_miss_rate         (0.17%)
     1,770,052,535      cpu_atom/branches/               #    727.9 M/sec  branch_frequency     (0.17%)
     7,362,015,741      cpu_atom/cpu-cycles/             #      3.0 GHz  cycles_frequency       (0.17%)
    14,532,252,573      cpu_atom/instructions/           #      1.9 instructions  insn_per_cycle  (0.17%)
             TopdownL1 (cpu_core)                        #      3.5 %  tma_bad_speculation    
                                                         #     16.6 %  tma_frontend_bound       (99.67%)
                                                         #     37.0 %  tma_backend_bound      
                                                         #     42.9 %  tma_retiring             (99.67%)
             TopdownL1 (cpu_atom)                        #     16.8 %  tma_bad_speculation      (0.29%)
                                                         #     31.7 %  tma_backend_bound        (0.33%)
                                                         #     12.6 %  tma_frontend_bound       (0.29%)
                                                         #     39.2 %  tma_retiring             (0.25%)

       2.438679230 seconds time elapsed

       2.400362000 seconds user
       0.035975000 seconds sys

 Performance counter stats for './cmake-build-debug/clf -f data/10M -n 5':

               867      context-switches                 #     32.7 cs/sec  cs_per_second     
                59      cpu-migrations                   #      2.2 migrations/sec  migrations_per_second
           202,289      page-faults                      #   7639.2 faults/sec  page_faults_per_second
         26,480.23 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
        64,791,930      cpu_core/branch-misses/          #      0.2 %  branch_miss_rate         (99.16%)
    39,072,717,494      cpu_core/branches/               #   1475.5 M/sec  branch_frequency     (99.16%)
   113,468,110,625      cpu_core/cpu-cycles/             #      4.3 GHz  cycles_frequency       (99.16%)
   246,694,582,330      cpu_core/instructions/           #      2.2 instructions  insn_per_cycle  (99.16%)
        39,009,621      cpu_atom/branch-misses/          #      0.2 %  branch_miss_rate         (0.52%)
    21,699,895,646      cpu_atom/branches/               #    819.5 M/sec  branch_frequency     (0.52%)
    76,776,416,169      cpu_atom/cpu-cycles/             #      2.9 GHz  cycles_frequency       (0.53%)
   136,448,512,017      cpu_atom/instructions/           #      1.8 instructions  insn_per_cycle  (0.54%)
             TopdownL1 (cpu_core)                        #      3.6 %  tma_bad_speculation    
                                                         #     14.4 %  tma_frontend_bound       (99.16%)
                                                         #     43.3 %  tma_backend_bound      
                                                         #     38.6 %  tma_retiring             (99.16%)
             TopdownL1 (cpu_atom)                        #     17.2 %  tma_bad_speculation      (0.54%)
                                                         #     40.9 %  tma_backend_bound        (0.61%)
                                                         #      9.6 %  tma_frontend_bound       (0.61%)
                                                         #     32.7 %  tma_retiring             (0.61%)

      26.541709772 seconds time elapsed

      26.072683000 seconds user
       0.450890000 seconds sys


|--42.15%--parseLine(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
|--27.82%--StatHandler::analyzeLine(std::optional<LineInfo>)
|--1.84%--StatHandler::retrieveStats(unsigned long)

|--38.09%--parseLine(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
|--30.85%--StatHandler::analyzeLine(std::optional<LineInfo>)

|--45.04%--StatHandler::analyzeLine(std::optional<LineInfo>)
|--35.55%--parseLine(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)

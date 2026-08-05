matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/100k'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/100k
  Time (mean ± σ):      71.5 ms ±   4.2 ms    [User: 67.1 ms, System: 4.2 ms]
  Range (min … max):    64.6 ms …  88.5 ms    44 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/1M'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/1M
  Time (mean ± σ):      1.198 s ±  0.030 s    [User: 1.167 s, System: 0.029 s]
  Range (min … max):    1.166 s …  1.243 s    10 runs
 
matas@matas-ThinkBook-16-G7-IML:~/Repos/clf$ hyperfine --warmup 3 './cmake-build-debug/clf -n 5 -f data/10M'
Benchmark 1: ./cmake-build-debug/clf -n 5 -f data/10M
  Time (mean ± σ):     14.838 s ±  0.387 s    [User: 14.524 s, System: 0.280 s]
  Range (min … max):   14.265 s … 15.175 s    10 runs


total runtime: 0.16s.
calls to allocation functions: 80163 (491797/s)
temporary memory allocations: 26 (159/s)
peak heap memory consumption: 4.59M
peak RSS (including heaptrack overhead): 20.10M
total memory leaked: 1.02K

total runtime: 1.64s.
calls to allocation functions: 800553 (486954/s)
temporary memory allocations: 26 (15/s)
peak heap memory consumption: 49.91M
peak RSS (including heaptrack overhead): 160.71M
total memory leaked: 1.02K

total runtime: 16.48s.
calls to allocation functions: 8001384 (485550/s)
temporary memory allocations: 26 (1/s)
peak heap memory consumption: 479.66M
peak RSS (including heaptrack overhead): 1.56G
total memory leaked: 1.02K


 Performance counter stats for './cmake-build-debug/clf -f data/100k -n 5':

                 7      context-switches                 #    103.2 cs/sec  cs_per_second     
                 6      cpu-migrations                   #     88.4 migrations/sec  migrations_per_second
             1,783      page-faults                      #  26276.2 faults/sec  page_faults_per_second
             67.86 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
           427,524      cpu_core/branch-misses/          #      0.4 %  branch_miss_rate         (94.52%)
       113,756,254      cpu_core/branches/               #   1676.4 M/sec  branch_frequency     (94.52%)
       290,463,532      cpu_core/cpu-cycles/             #      4.3 GHz  cycles_frequency       (94.52%)
       769,960,332      cpu_core/instructions/           #      2.7 instructions  insn_per_cycle  (94.52%)
           189,907      cpu_atom/branch-misses/          #      0.8 %  branch_miss_rate         (0.69%)
        20,840,838      cpu_atom/branches/               #    307.1 M/sec  branch_frequency     (1.02%)
        91,744,405      cpu_atom/cpu-cycles/             #      1.4 GHz  cycles_frequency       (2.49%)
       165,021,602      cpu_atom/instructions/           #      1.7 instructions  insn_per_cycle  (3.96%)
             TopdownL1 (cpu_core)                        #      7.9 %  tma_bad_speculation    
                                                         #     13.3 %  tma_frontend_bound       (94.52%)
                                                         #     31.4 %  tma_backend_bound      
                                                         #     47.5 %  tma_retiring             (94.52%)
             TopdownL1 (cpu_atom)                        #     18.5 %  tma_bad_speculation      (4.86%)
                                                         #     40.0 %  tma_backend_bound        (4.86%)
                                                         #      9.6 %  tma_frontend_bound       (4.46%)
                                                         #     35.3 %  tma_retiring             (2.99%)

       0.069246586 seconds time elapsed

       0.065223000 seconds user
       0.004013000 seconds sys

 Performance counter stats for './cmake-build-debug/clf -f data/1M -n 5':

                33      context-switches                 #     27.6 cs/sec  cs_per_second     
                 2      cpu-migrations                   #      1.7 migrations/sec  migrations_per_second
            19,485      page-faults                      #  16274.0 faults/sec  page_faults_per_second
          1,197.31 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
         4,238,650      cpu_core/branch-misses/          #      0.4 %  branch_miss_rate         (99.83%)
     1,102,465,913      cpu_core/branches/               #    920.8 M/sec  branch_frequency     (99.83%)
     5,114,266,049      cpu_core/cpu-cycles/             #      4.3 GHz  cycles_frequency       (99.83%)
     7,470,202,116      cpu_core/instructions/           #      1.5 instructions  insn_per_cycle  (99.83%)
        17,229,642      cpu_atom/branch-misses/          #      3.2 %  branch_miss_rate         (0.08%)
       709,533,565      cpu_atom/branches/               #    592.6 M/sec  branch_frequency     (0.17%)
     3,320,113,789      cpu_atom/cpu-cycles/             #      2.8 GHz  cycles_frequency       (0.17%)
     4,413,232,898      cpu_atom/instructions/           #      1.3 instructions  insn_per_cycle  (0.17%)
             TopdownL1 (cpu_core)                        #      5.1 %  tma_bad_speculation    
                                                         #      7.3 %  tma_frontend_bound       (99.83%)
                                                         #     61.2 %  tma_backend_bound      
                                                         #     26.3 %  tma_retiring             (99.83%)
             TopdownL1 (cpu_atom)                        #     25.7 %  tma_bad_speculation      (0.08%)
                                                         #     34.1 %  tma_backend_bound        (0.08%)
                                                         #     34.8 %  tma_frontend_bound       (0.00%)
                                                         #      nan %  tma_retiring             (0.00%)

       1.202729291 seconds time elapsed

       1.168526000 seconds user
       0.032986000 seconds sys

 Performance counter stats for './cmake-build-debug/clf -f data/10M -n 5':

               660      context-switches                 #     46.7 cs/sec  cs_per_second     
                30      cpu-migrations                   #      2.1 migrations/sec  migrations_per_second
           185,466      page-faults                      #  13126.6 faults/sec  page_faults_per_second
         14,129.04 msec task-clock                       #      1.0 CPUs  CPUs_utilized       
        41,642,014      cpu_core/branch-misses/          #      0.4 %  branch_miss_rate         (99.82%)
    10,956,746,748      cpu_core/branches/               #    775.5 M/sec  branch_frequency     (99.82%)
    60,550,005,414      cpu_core/cpu-cycles/             #      4.3 GHz  cycles_frequency       (99.82%)
    74,232,998,681      cpu_core/instructions/           #      1.2 instructions  insn_per_cycle  (99.82%)
        39,544,678      cpu_atom/branch-misses/          #      0.5 %  branch_miss_rate         (0.10%)
     8,052,945,781      cpu_atom/branches/               #    570.0 M/sec  branch_frequency     (0.11%)
    37,220,748,726      cpu_atom/cpu-cycles/             #      2.6 GHz  cycles_frequency       (0.11%)
    50,195,427,792      cpu_atom/instructions/           #      1.4 instructions  insn_per_cycle  (0.12%)
             TopdownL1 (cpu_core)                        #      5.3 %  tma_bad_speculation    
                                                         #      7.5 %  tma_frontend_bound       (99.82%)
                                                         #     65.0 %  tma_backend_bound      
                                                         #     22.2 %  tma_retiring             (99.82%)
             TopdownL1 (cpu_atom)                        #     14.9 %  tma_bad_speculation      (0.13%)
                                                         #     55.8 %  tma_backend_bound        (0.14%)
                                                         #      8.8 %  tma_frontend_bound       (0.13%)
                                                         #     26.7 %  tma_retiring             (0.12%)

      14.172484914 seconds time elapsed

      13.908935000 seconds user
       0.242963000 seconds sys


|--62.61%--StatHandler::analyzeLine(std::optional<LineInfo>)
|--17.22%--parseLine(std::basic_string_view<char, std::char_traits<char> >)
|--3.09%--StatHandler::retrieveStats(unsigned long)

|--80.46%--StatHandler::analyzeLine(std::optional<LineInfo>)

|--78.39%--StatHandler::analyzeLine(std::optional<LineInfo>)
|--9.07%--parseLine(std::basic_string_view<char, std::char_traits<char> >)
|--0.66%--StatHandler::retrieveStats(unsigned long)

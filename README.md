1. Time information

hyperfine --warmup 3 ./cmake-build-debug/clf -f data/100k -n 5
hyperfine --warmup 3 ./cmake-build-debug/clf -f data/1M -n 5
hyperfine --warmup 3 ./cmake-build-debug/clf -f data/10M -n 5

2. Memory information

heaptrack -- ./cmake-build-debug/clf -n 5 -f data/100k
heaptrack -- ./cmake-build-debug/clf -n 5 -f data/1M
heaptrack -- ./cmake-build-debug/clf -n 5 -f data/10M

heaptrack_print heaptrack.clf.892641.zst

3. CPU utilization information

sudo perf stat -- ./cmake-build-debug/clf -f data/100k -n 5
sudo perf stat -- ./cmake-build-debug/clf -f data/1M -n 5
sudo perf stat -- ./cmake-build-debug/clf -f data/10M -n 5

sudo perf record -g -- ./cmake-build-debug/clf -f data/100k -n 5
sudo perf record -g -- ./cmake-build-debug/clf -f data/1M -n 5
sudo perf record -g -- ./cmake-build-debug/clf -f data/10M -n 5

perf report --stdio

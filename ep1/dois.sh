#!/bin/bash

for file in trace100; do
for i in $(seq 25 30); do
#./ep1 1 "$file" "saida";
./ep1 1 "$file" "saida_fcfs_""$file""_""$i"
./ep1 2 "$file" "saida_srtn_""$file""_""$i"
./ep1 3 "$file" "saida_rr_""$file""_""$i"
done
done

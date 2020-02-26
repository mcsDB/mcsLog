MAX_MEMORY=$(($(free | awk -F: '/Mem/ { print $2}' | awk '{print $NF}') * 1024))
SOCKETS=$(lscpu | awk -F: '/^Socket\(s\)/ { print $2}')
CORES_PER_SOCKET=$(lscpu | awk -F: '/^Core\(s\) per socket/ { print $2}')
THREADS_PER_CORE=$(lscpu | awk -F: '/^Thread\(s\) per core/ { print $2}')
MAX_THREADS=$(($SOCKETS * $CORES_PER_SOCKET * $THREADS_PER_CORE + 0))
PMEMDIR=/mnt/pmemdir
KiloByte=$((1024))
MegaByte=$((1024 * 1024))
GigaByte=$((1024 * 1024 * 1024))
fileNumber=0
threads=$MAX_THREADS
valueRange=($((1 * $KiloByte)) $((2 * $KiloByte)) $((4 * $KiloByte)) $((8 *$KiloByte)) $((16 * $KiloByte)) $((64 * $KiloByte)) $((256 * $KiloByte)) $MegaByte $((4 * $MegaByte)))
total_data=$(($1 * $GigaByte))
# Iterating on number of threads
while [ $threads -le $MAX_THREADS ]
do
  for nt in 0 1
  do
    # Iterating on reasonable value_sizes
    # for value_size in ${valueRange[@]}
    for value_size in 256
    do
      iterations=$(($total_data/$(($value_size * $threads))))
      echo "#define LOG_PATH \"$PMEMDIR/log\"" >> macros$fileNumber.hpp
      echo "#define NUM_THREADS $threads" >> macros$fileNumber.hpp
      echo "#define VAL_SIZE ${value_size}LL" >> macros$fileNumber.hpp
      echo "#define ITERATIONS ${iterations}LL" >> macros$fileNumber.hpp
      echo "#define LOG_SIZE ${total_data}LL" >> macros$fileNumber.hpp
      echo "// b - buffer, s - size, v - value" >> macros$fileNumber.hpp 
      echo "#define REPEAT(b, s, v) memset(b, v, s)" >> macros$fileNumber.hpp
      echo "#define IS_TEMPORAL $nt" >> macros$fileNumber.hpp
      fileNumber=$(($fileNumber + 1))
    done
    threads=$(($threads * 2))
  done
done

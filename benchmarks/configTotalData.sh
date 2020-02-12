MAX_MEMORY=$(($(free | awk -F: '/Mem/ { print $2}' | awk '{print $NF}') * 1024))
SOCKETS=$(lscpu | awk -F: '/^Socket\(s\)/ { print $2}')
CORES_PER_SOCKET=$(lscpu | awk -F: '/^Core\(s\) per socket/ { print $2}')
THREADS_PER_CORE=$(lscpu | awk -F: '/^Thread\(s\) per core/ { print $2}')
MAX_THREADS=$(($SOCKETS * $CORES_PER_SOCKET * $THREADS_PER_CORE + 0))
GigaByte=$((1024 * 1024 * 1024))
fileNumber=0
threads=1

total_data=$(($1 * $GigaByte))
while [ $threads -le $MAX_THREADS ]
do
  for value_size in 64 128 256 512 1024 2048 4096
  do
    iterations=$(($total_data/$(($value_size * $threads))))
    echo "#define LOG_PATH \"log.out\"" >> macros$fileNumber.hpp
    echo "#define NUM_THREADS $threads" >> macros$fileNumber.hpp
    echo "#define VAL_SIZE ${value_size}LL" >> macros$fileNumber.hpp
    echo "#define ITERATIONS ${iterations}LL" >> macros$fileNumber.hpp
    echo "#define LOG_SIZE ${total_data}LL" >> macros$fileNumber.hpp
    echo "// b - buffer, s - size, v - value" >> macros$fileNumber.hpp 
    echo "#define REPEAT(b, s, v) memset(b, v, s)" >> macros$fileNumber.hpp
    fileNumber=$(($fileNumber + 1))
  done
  threads=$(($threads * 2))
done

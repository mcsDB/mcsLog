NUM_CONFIGS=$(ls macro* | wc -l)
OUTPUT_FILE=$1
PMEMDIR="/mnt/pmemdir"
if [ "$#" != 1 ]; then
  echo "Please run the script with one argument: output filename"
  exit 1
fi

config=0
fileName="multi-loggers.cpp"

while [ $config -le $(($NUM_CONFIGS - 1)) ]
do
  echo "Running config: $config"
  sync; echo 1 > /proc/sys/vm/drop_caches
  sync; echo 2 > /proc/sys/vm/drop_caches
  sync; echo 3 > /proc/sys/vm/drop_caches
  cp macros$config.hpp ../utils/macros.hpp
  g++ ../test/$fileName ../src/log.cpp -lpthread -O3
  rm $PMEMDIR/log*.out
  ./a.out >> $OUTPUT_FILE
  config=$(($config + 1))
done

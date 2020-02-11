NUM_CONFIGS=$(ls macro* | wc -l)
OUTPUT_FILE=$1
if [ "$#" != 1 ]; then
  echo "Please run the script with one argument: output filename"
  exit 1
fi

config=0
while [ $config -le $NUM_CONFIGS ]
do
  for fileName in "multi-writers.cpp" "multi-loggers.cpp"
  do
    cp macros$config.hpp ../utils/macros.hpp
    g++ ../test/$fileName ../src/log.cpp -lpthread -O3
    rm *log.out
    ./a.out >> $OUTPUT_FILE
  done
  config=$(($config + 1))
done

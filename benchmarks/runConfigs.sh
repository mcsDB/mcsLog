NUM_CONFIGS=$(ls macro* | wc -l)
OUTPUT_FILE=$1
if [ "$#" != 1 ]; then
  echo "Please run the script with one argument: output filename"
  exit 1
fi

config=0
fileName="multi-loggers.cpp"

while [ $config -le $NUM_CONFIGS ]
do
    cp macros$config.hpp ../utils/macros.hpp
    g++ ../test/$fileName ../src/log.cpp -lpthread -O3
    rm *log.out
    ./a.out >> $OUTPUT_FILE
  config=$(($config + 1))
done

#!/bin/bash

echo 'compiling main...'
gcc -o ./build/PCA main.c logger.h logger.c config.h config.c random.h random.c transaction.h transaction.c shared_buffer.h shared_buffer.c -lrt
echo 'compiling producer...'
gcc -o ./build/producer producer.h producer.c logger.h logger.c random.h random.c transaction.h transaction.c shared_buffer.h shared_buffer.c -lrt
echo 'compiling consumer...'
gcc -o ./build/consumer consumer.h consumer.c logger.h logger.c random.h random.c transaction.h transaction.c shared_buffer.h shared_buffer.c -lrt
echo 'compiling controller...'
gcc -o ./build/controller controller.c logger.h logger.c random.h random.c transaction.h transaction.c shared_buffer.h shared_buffer.c -lrt
echo "preparing conf anf log files..."
cp pca.conf ./build/pca.conf
cp pca.log ./build/pca.log
echo 'done.'

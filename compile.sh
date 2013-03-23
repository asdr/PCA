#!/bin/bash

echo 'compiling main...'
gcc -o PCA main.c logger.h logger.c config.h config.c random.h random.c shared_buffer.h shared_buffer.c -lrt -pthread
echo 'compiling producer...'
gcc -o producer producer.h producer.c logger.h logger.c random.h random.c shared_buffer.h shared_buffer.c -lrt -pthread
echo 'compiling consumer...'
gcc -o consumer consumer.h consumer.c logger.h logger.c random.h random.c shared_buffer.h shared_buffer.c -lrt -pthread
echo 'compiling controller...'
gcc -o controller controller.c logger.h logger.c random.h random.c shared_buffer.h shared_buffer.c -lrt -pthread
echo 'done.'

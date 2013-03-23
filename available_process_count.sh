#!/bin/bash

max_count=`ulimit -u`
running_count=`ps ux|wc -l`
let "count = $max_count - $running_count"
echo $count

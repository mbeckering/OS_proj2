#!/bin/bash
# 
# File:   parse.bash
# Author: Michael Beckering
#
# Created on Feb 20, 2018, 3:14:27 PM

echo "Log Analysis"
echo 'File chosen: ' $1

echo "Total Sleep Time:"
stotal=$(awk -F " " '$2 ~ /Sleep/ {sleeptotal+=$3} END {print sleeptotal}' $1)
echo "$stotal"

echo "Number of times slept:"
stimes=$(awk -F " " '$2 ~ /Sleep/ {sleeptimes+=1} END {print (sleeptimes)}' $1)
echo "$stimes"

echo "Average Sleep Time:"
echo $((stotal / stimes))

echo "Total Buffer Checks:"
awk -F " " '$2 ~ /Check/ {readtotal+=1} END {print readtotal}' $1

echo "Total Buffer Reads(consumer only):"
awk -F " " '$2 ~ /Read/ {readtotal+=1} END {print readtotal}' $1

echo "Total Buffer Writes(producer only):"
awk -F " " '$2 ~ /Write/ {readtotal+=1} END {print readtotal}' $1
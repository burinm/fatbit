#!/bin/bash
#Find the frequency of letters in prose
for i in {a..z}; do echo -n "$i "; fgrep -i -o $i $1 | wc -l; done | sort -r -n -k 2

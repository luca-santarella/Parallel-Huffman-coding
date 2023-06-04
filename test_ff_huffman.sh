#! /bin/bash

for i in {1..10}
do
	./ff_huffman.out $1 $2 -v
done | awk '
NR%5==1{SUM_READ += $3;}
NR%5==2{SUM_COUNT += $3;} 
NR%5==3{SUM_HUF_ENC += $3;} 
NR%5==4{SUM_ASCII_ENC += $3;} 
NR%5==0{SUM_WRITE += $3;}  
END {
	print "avg read: " SUM_READ/10
	print "avg counting: " SUM_COUNT/10
	print "avg huf encoding: " SUM_HUF_ENC/10
	print "avg ascii encoding: " SUM_ASCII_ENC/10
	print "avg writing: " SUM_WRITE/10
}
'
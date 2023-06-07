#! /bin/bash

for i in {1..10}
do
	./seq_huffman.out $1 -v
done | awk '
NR%7==1{SUM_READ += $3;}
NR%7==2{SUM_COUNT += $3;} 
NR%7==3{SUM_HUF_ENC += $3;} 
NR%7==4{SUM_ASCII_ENC += $3;} 
NR%7==5{SUM_WRITE += $3;}  
NR%7==6{SUM_TOTAL += $3;}
NR%7==0{SUM_TOTALNOIO += $3;}
END {
	print "avg read: " SUM_READ/10
	print "avg counting: " SUM_COUNT/10
	print "avg huf encoding: " SUM_HUF_ENC/10
	print "avg ascii encoding: " SUM_ASCII_ENC/10
	print "avg writing: " SUM_WRITE/10
	print "avg total: " SUM_TOTAL/10
	print "avg total (no IO): " SUM_TOTALNOIO/10
}
'
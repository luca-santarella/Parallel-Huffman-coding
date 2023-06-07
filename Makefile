seq: test_ff test_th test_seq ff threads seq_huffman.cpp
	g++ -O3 seq_huffman.cpp -o seq_huffman.out

threads: th_huffman.cpp
	g++ -I . -O3 -pthread th_huffman.cpp -o th_huffman.out

ff: ff_huffman.cpp ASCIIEncUtils.hpp hufEncUtils.hpp countUtils.hpp
	g++ -I . -O3 -pthread ff_huffman.cpp -o ff_huffman.out

test_seq: seq_huffman.out
	./test_seq_huffman.sh file_10M.txt

test_th: th_huffman.out
	for((i=1;i<=64;i*=2)); do echo $i; ./test_th_huffman.sh $i file_10M.txt; done

test_ff: ff_huffman.cpp ASCIIEncUtils.hpp hufEncUtils.hpp countUtils.hpp
	for((i=1;i<=64;i*=2)); do echo $i; ./test_ff_huffman.sh $i file_10M.txt; done

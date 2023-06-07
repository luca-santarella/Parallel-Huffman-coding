seq: ff threads seq_huffman.cpp
	g++ -O3 seq_huffman.cpp -o seq_huffman.out

threads: th_huffman.cpp
	g++ -I . -O3 -pthread th_huffman.cpp -o th_huffman.out

ff: ff_huffman.cpp ASCIIEncUtils.hpp hufEncUtils.hpp countUtils.hpp
	g++ -I . -O3 -pthread ff_huffman.cpp -o ff_huffman.out

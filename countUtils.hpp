#define SIZE 128 //# of possible chars in ASCII

std::mutex countLock;
std::string strFile;
//vector used to store # occurrences of the 128 possible characters
std::vector<int> freqs(SIZE,0);

typedef struct countTask {
  int start; 
  int stop; 
  std::vector<int> partialFreqs;
} CTASK; 

class countEmitter : public ff::ff_monode_t<CTASK> {
private: 
    int nw; 
public:
    countEmitter(int nw):nw(nw) {}

    CTASK * svc(CTASK *) {
       for(int i=0; i<nw; i++) {
            int n = strFile.size();
            int delta = n/nw;
            int start = i*delta;    
            int stop;               
            if(i==nw-1) 
                stop = n;
            else
                stop = i*delta + delta; 
            auto t = new CTASK{start,stop};
            ff_send_out(t);
       }
       return(EOS);
    }
};

class countCollector : public ff::ff_node_t<CTASK> {
private: 
    CTASK * tt; 

public: 
    CTASK * svc(CTASK * t) {
        countLock.lock();
        for(int i=0; i<SIZE; i++)
            freqs[i] += t->partialFreqs[i];
        countLock.unlock();     
        free(t);
        return(GO_ON);
    }

};

int ASCIIToDec(char c) {
    return static_cast<int>(c);
}

CTASK *  countWorker(CTASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    t->partialFreqs.resize(SIZE, 0);
    for(int i=start; i<stop; i++) {         
      t->partialFreqs[ASCIIToDec(strFile[i])]++;
    }
    return t;
}
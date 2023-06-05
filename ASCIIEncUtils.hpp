std::mutex encASCIILock;
std::string ASCIIEncStr;
std::vector<std::string> partialASCIIEncStrs;
std::string hufEncodedStr;

typedef struct ASCIIEncTask {
  int start; 
  int stop; 
  int id;
  std::string partialEncodedStr;
} ATASK; 

class ASCIIEncEmitter : public ff::ff_monode_t<ATASK> {
private: 
    int nw; 
public:
    ASCIIEncEmitter(int nw):nw(nw) {}

    ATASK * svc(ATASK *) {
       for(int i=0; i<nw; i++) {
            int n = hufEncodedStr.size();
            int delta = n/nw;
            //make sure that delta is a multiple of 8
            if(delta % 8 != 0)
            {
                int bits = delta % 8;
                bits = 8 - bits;
                delta += bits;
            }
            int start = i*delta;    
            int stop;               
            if(i==nw-1) 
                stop = n;
            else
                stop = i*delta + delta; 
            auto t = new ATASK{start,stop,i};
            ff_send_out(t);
       }
       return(EOS);
    }
};

class ASCIIEncCollector : public ff::ff_node_t<ATASK> {
private: 
    ATASK * tt; 

public: 
    ATASK * svc(ATASK * t) {
        encASCIILock.lock();
            partialASCIIEncStrs[t->id] = t->partialEncodedStr;
        encASCIILock.unlock();     
        free(t);
        return(GO_ON);
    }

};

char convertToASCII(std::string binaryString)
{
    int decimalValue = 0;
    for (char bit : binaryString) 
        decimalValue = (decimalValue << 1) + (bit - '0');

    return static_cast<char>(decimalValue);
}

ATASK *  ASCIIWorker(ATASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    for(int i=start; i<stop; i+=8)
        t->partialEncodedStr += convertToASCII(hufEncodedStr.substr(i, 8));
    return t;
}


std::string ASCIIEncStr;
std::vector<std::string> partialASCIIEncStrs;
std::string hufEncodedStr;

typedef struct encTask {
  int start; 
  int stop; 
  int id;
  //partial result of the encoding of the string, when
  //scanning the original string the encoding is accumulated in this string
  std::string partialEncodedStr;
} ENCTASK; 

class ASCIIEncEmitter : public ff::ff_monode_t<ENCTASK> {
private: 
    int nw; 
public:
    ASCIIEncEmitter(int nw):nw(nw) {}

    ENCTASK * svc(ENCTASK *) {
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
            auto t = new ENCTASK{start,stop,i};
            ff_send_out(t);
       }
       return(EOS);
    }
};

class ASCIIEncCollector : public ff::ff_node_t<ENCTASK> {
private: 
    ENCTASK * tt; 

public: 
    ENCTASK * svc(ENCTASK * t) {
        partialASCIIEncStrs[t->id] = t->partialEncodedStr;
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

ENCTASK *  ASCIIWorker(ENCTASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    for(int i=start; i<stop; i+=8)
        t->partialEncodedStr += convertToASCII(hufEncodedStr.substr(i, 8));
    return t;
}


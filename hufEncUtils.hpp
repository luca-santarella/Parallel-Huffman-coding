std::mutex hufLock;
//map <char, huffman code>
std::unordered_map<char, std::string> codes;	

std::vector<std::string> partialHufEncStrs;

typedef struct hufEncTask {
  int start; 
  int stop; 
  int id;
  std::string partialEncodedStr;
} HTASK; 

class hufEncEmitter : public ff::ff_monode_t<HTASK> {
private: 
    int nw; 
public:
    hufEncEmitter(int nw):nw(nw) {}

    HTASK * svc(HTASK *) {
       for(int i=0; i<nw; i++) {
            int n = strFile.size();
            int delta = n/nw;
            int start = i*delta;    
            int stop;               
            if(i==nw-1) 
                stop = n;
            else
                stop = i*delta + delta; 
            auto t = new HTASK{start,stop,i};
            ff_send_out(t);
       }
       return(EOS);
    }
};

class hufEncCollector : public ff::ff_node_t<HTASK> {
private: 
    HTASK * tt; 

public: 
    HTASK * svc(HTASK * t) {
        hufLock.lock();
            partialHufEncStrs[t->id] = t->partialEncodedStr;
        hufLock.unlock();     
        free(t);
        return(GO_ON);
    }

};

HTASK *  hufWorker(HTASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    for(int i=start; i < stop; i++)
        t->partialEncodedStr += codes[strFile[i]];
    return t;
}

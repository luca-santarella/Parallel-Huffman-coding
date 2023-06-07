//map <char, huffman code>
std::unordered_map<char, std::string> codes;	

//vector of nw strings which will be concatenated to get final res
std::vector<std::string> partialHufEncStrs;

class hufEncEmitter : public ff::ff_monode_t<ENCTASK> {
private: 
    int nw; 
public:
    hufEncEmitter(int nw):nw(nw) {}

    ENCTASK * svc(ENCTASK *) {
       for(int i=0; i<nw; i++) {
            int n = strFile.size();
            int delta = n/nw;
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

class hufEncCollector : public ff::ff_node_t<ENCTASK> {
private: 
    ENCTASK * tt; 

public: 
    ENCTASK * svc(ENCTASK * t) {
        partialHufEncStrs[t->id] = t->partialEncodedStr;
        free(t);
        return(GO_ON);
    }

};

ENCTASK *  hufWorker(ENCTASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    //actual encoding char by char using the map<char, huf code> 'codes'
    for(int i=start; i < stop; i++)
        t->partialEncodedStr += codes[strFile[i]];
    return t;
}

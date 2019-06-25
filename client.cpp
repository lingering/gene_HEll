#include <cassert>

#include "networking.h"
#include "aux.h"
#include "gene.h"
using namespace std;
using namespace boost::asio;
using namespace seal;

int main(){
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    vector<uint64_t> gene;
    cout<<"[+]opening gene info.\twaiting....."<<endl;
    
        pFile = fopen("Normal.tsv","rb");
     if (pFile == NULL)
        {
            cout<<"open file error, exiting...";
            return 0;
        }
    
       
    
    fseek(pFile,0,SEEK_END);
    lSize=ftell(pFile);
    rewind(pFile);

    buffer = (char*)malloc(sizeof(char)*lSize);
    if(buffer==NULL){
        cout<<"[-]malloc error!"<<endl;
        return 0;
    }
    result = fread(buffer,1,lSize,pFile);
    if(result!=lSize){
        cout<<"[-]Reading your gene data failed, please contact the manager..exiting..."<<endl;
        return 0;
    }
    fclose(pFile);
    cout<<"[+]done read gene data, size:"<<lSize<<endl;

    size_t gene_size;
    size_t poly_modulus_degree=8192;
    unsigned short port=9999;
    io_context context;
    ip::tcp::socket socket(context);
    ip::tcp::resolver resolver(context);
    connect(socket,resolver.resolve("localhost","9999",resolver.numeric_service));
    Networking net(socket);
    cout <<"[+]connected to localhost at "<<port<<" \n[+]waiting for hello msg"<<endl;
    net.recv_command();
    
    size_t send_size = sizeof(NET_MAGIC_HELLO);
    cout <<"[+]recevied hello msg. prepare send size:"<<send_size<<endl;
    cout<<"[+]generating cryptographic parameter."<<endl;
    
    /* std::chrono::high_resolution_clock::time_point time_start, time_end;
	std::chrono::microseconds time_pram_gen_sum;
	std::chrono::microseconds time_calc_sum;
	time_start = std::chrono::high_resolution_clock::now();*/
    GeneParams params(poly_modulus_degree);
       
    //GeneSender Client_param(params);

    cout <<"[+]sending hello, size,seeds,pk,relin_keys"<<endl;
    net.set_seal_context(params.context);
    net.send_command();
    cout<<"[#]hello back success"<<endl;
    //net.write_uint32(gene_size);
    //net.write_uint64s(params.seeds);
    
    net.write_public_key(params.pub_key);
    net.write_relin_key(params.relin_key);
    //process gene data
    cout << "[+]encrypting gene_data:"<<endl;
    vector<double> input;
	vector<double> refer;
    double curr_point = 0;

    for (size_t i = 0; i < poly_modulus_degree; i++, curr_point += 0.1)
    {
        input.push_back(curr_point);
    }

    //cout << "Input vector: size:"<<input.size() << endl;
    //print_vector(input, 3, 7);
    vector<Ciphertext> enc_gene_data_;
    vector<double> slice;
    size_t step=poly_modulus_degree/2;
    size_t i=0;
    while(i<input.size()/step){
        if((input.end()-(input.begin()+(i+1)*step))<step)
            slice.assign(input.begin()+i*step,input.end());

        else{
        slice.assign(input.begin()+i*step,input.begin()+(i+1)*step);
        }
        enc_gene_data_.push_back(params.encrypted_genedata(slice));
        slice.clear();
        i+=1;
        
    }
    ;
    net.write_enc_geneRNAs(enc_gene_data_);
    cout<<"[+s]"<<i<<endl;
    free(buffer);
    return 0;
}
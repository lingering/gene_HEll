#include <cstdint>
#include <iostream>
#include <boost/asio.hpp>
#include "networking.h"
#include "gene.h"
#include "aux.h"
using namespace std;
using namespace boost::asio;
using namespace seal;
#define threhold 150
int main(){
    vector<float_t> inp;
    size_t iput_size;
    unsigned short port=9999;
    size_t ploy_modulus_degree =8192;
    //inital network interface
    io_context iocontext;
    ip::tcp::acceptor acceptor(iocontext);
    ip::tcp::endpoint endp(ip::tcp::v4(),port);
    acceptor.open(endp.protocol());
    acceptor.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endp);
    acceptor.listen();
    cout << "listening"<<endl;
    ip::tcp::socket socket(iocontext);
    acceptor.accept(socket);
    Networking net(socket);
    GeneParams param(ploy_modulus_degree,ckks);
    SecretKey secret_key;
    net.set_seal_context(param.context);

    cout<<"[+]connection established,sending hello to the client"<<endl;
    net.send_command();
    cout<<"[+]waiting for hello back.."<<endl;
    net.recv_command();
    //cout << "waiting for set size"<<endl;
    //size_t server_size=net.read_uint32();
   
    
    cout<<"[+]receiving user's public key"<<endl;
    PublicKey recv_pk;
    net.read_public_key(recv_pk);
    
    cout << "[+]waitint for relinerize key"<<endl;
    RelinKeys recv_relin_key;
    net.read_relin_key(recv_relin_key);

    cout<< "[+]waiting for encrypted RNA expressing "<<endl;
    vector<Ciphertext> recv_RNA;
    net.read_enc_geneRNAs(recv_RNA);
    cout<<recv_RNA[0].size()<<endl;
    cout<<"[+]received User data"<<endl;
    
    //evaluate distance
    Evaluator evaluator(param.context);
    Encryptor encryptor(param.context,recv_pk);
    
    //CKKSEncoder encoder(param.context);
    vector<double> refer;
    double curr_point = 0;

    for (size_t i = 0; i < ploy_modulus_degree; i++, curr_point += 0.01)
    {
        refer.push_back(curr_point);
    }

    print_vector(refer);
    vector<Ciphertext> enc_refer;
    vector<double> slice;
    size_t step=ploy_modulus_degree/2;
    size_t i=0;
    while(i<refer.size()/step){
        if((refer.end()-(refer.begin()+(i+1)*step))<step)
            slice.assign(refer.begin()+i*step,refer.end());

        else{
        slice.assign(refer.begin()+i*step,refer.begin()+(i+1)*step);
        }
        enc_refer.push_back(param.encrypted_genedata(slice,encryptor));
        evaluator.sub(enc_refer[i],recv_RNA[i],enc_refer[i]);
        evaluator.square_inplace(enc_refer[i]);
        slice.clear();
        
        i+=1;
        
    }
    net.write_enc_geneRNAs(enc_refer);
    //processing encrypted distance that are send back
    net.read_public_key(recv_pk);
    Ciphertext final_distance;
    net.read_enc_geneRNA(final_distance);
    vector<uint32_t> recv_distance;
    GeneParams params(ploy_modulus_degree,bfv);
    Evaluator eval(params.context);
    Plaintext plain_dis(to_string(threhold));
    Encryptor thre_encryptor(params.context,recv_pk);
    Ciphertext enc_threhold;
    thre_encryptor.encrypt(plain_dis,enc_threhold);
    evaluator.sub_inplace(final_distance,enc_threhold);

    net.write_enc_geneRNA(final_distance);
    return 0;
}
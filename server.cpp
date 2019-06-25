#include <cstdint>
#include <iostream>
#include <boost/asio.hpp>
#include "networking.h"
#include "gene.h"
using namespace std;
using namespace boost::asio;
using namespace seal;

int main(){
    vector<float_t> inp;
    size_t iput_size;
    unsigned short port=9999;
    size_t ploy_modulus_degree =8192;

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
    GeneParams param(ploy_modulus_degree);
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
    cout<<"recv"<<endl;
    return 0;
}
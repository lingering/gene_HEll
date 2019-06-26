#include "gene.h"
#include "seal/seal.h"
#include <cassert>
#include <iostream>
#include "gene_random.h"
#include "aux.h"
GeneParams::GeneParams(size_t poly_modulus_degree)
    : receiver_size(receiver_size),
      sender_size(sender_size),
      input_bits(input_bits),
      poly_modulus_degree_(poly_modulus_degree)
      
      
{
    assert((poly_modulus_degree_ == 8192) || (poly_modulus_degree_ == 16384));
    
if (gene_scheme){
    EncryptionParameters parms(scheme_type::BFV);
    parms.set_poly_modulus_degree(poly_modulus_degree_);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(512);
    context = SEALContext::Create(parms);
    
    }
else{
    EncryptionParameters parms(scheme_type::CKKS);
    parms.set_poly_modulus_degree(poly_modulus_degree_);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, { 60, 40, 40, 60 }));
    context = SEALContext::Create(parms);
    print_parameters(context);
    KeyGenerator keygen(context);
    pub_key=keygen.public_key();
    relin_key=keygen.relin_keys();
    sec_key=keygen.secret_key();

}
    
    

    // it must be possible to cuckoo hash the receiver's set into the buckets
    //assert(receiver_size <= (1ull << bucket_count_log()));
    // all of the receiver's buckets must fit into one batched ciphertext
}

void GeneParams::generate_seeds() {
    seeds.clear();
    auto random_factory = UniformRandomGeneratorFactory::default_factory();
    auto random = random_factory->create();

    for (size_t i = 0; i < 3; i++) {
        seeds.push_back(random_bits(random, 64));
    }
}
size_t GeneParams::bucket_count_log() {
    switch (poly_modulus_degree_) {
        case 8192: return 13;
        case 16384: return 14;
        default: assert(0);
    }
    return 0;
}
GeneSender::GeneSender(GeneParams &params)
    :params(params),
    keygen(params.context),
    public_key_(keygen.public_key()),
    secret_key_(keygen.secret_key()),
    relin_key_(keygen.relin_keys())
{        
}
PublicKey& GeneSender::public_key(){
    return public_key_;
}
RelinKeys&  GeneSender::relin_keys(){
    return relin_key_;
    //return keygen.relin_keys();
}
SecretKey& GeneSender::secret_key(){
    return secret_key_;
}

Ciphertext GeneParams::encrypted_genedata(vector<double_t> &gene_data,Encryptor &encryptor){
    //assert(gene_data.size()==params.receiver_size);
    //Encryptor encryptor(context,pub_key);
    Plaintext plaintext;
    Ciphertext ciphertext;
    vector<double> partial_gene;
    
    if (gene_scheme)
    {
        vector<uint64_t> gene_data(gene_data.begin(),gene_data.end());
        BatchEncoder encoder(context);
        size_t slot_count = encoder.slot_count();
        encoder.encode(gene_data,plaintext);
        encryptor.encrypt(plaintext,ciphertext);
        
    }
    else{
        CKKSEncoder encoder(context);
        
        size_t slot_count = encoder.slot_count();
        //cout<<"[x]what?"<<endl;
        double scale = pow(2.0, 40);
        //cout<<"[xx]what?"<<endl;
        encoder.encode(gene_data,scale,plaintext);
        //cout<<"[xxxx]what?"<<endl;

        encryptor.encrypt(plaintext,ciphertext);
        return ciphertext;
    }
}
vector<double> GeneParams::decrypt_data(Ciphertext &ciphertex,Decryptor &decryptor,CKKSEncoder &encoder){
    Plaintext plain;
    decryptor.decrypt(ciphertex,plain);
    vector<double> result;
    encoder.decode(plain,result);
    return result;
}

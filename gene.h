
#pragma once 
#include <vector>
#include <optional>
#include "seal/seal.h"
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace seal;
#define gene_scheme 0//1 for "BFV" 0 for "CKKS"
typedef pair<size_t, size_t> bucket_slot;

class GeneParams
{
public:
    GeneParams(size_t poly_modulus_degree);
    // you *must* call either generate_seeds or set_seeds.
    void generate_seeds();
    void set_seeds(vector<uint64_t> &seeds_ext);

    uint64_t plain_modulus();
    size_t hash_functions();
    size_t bucket_count_log();
    size_t sender_bucket_capacity();
    size_t sender_partition_count();
    size_t window_size();
    Ciphertext encrypted_genedata(vector<double_t> &gene_data);

    void set_sender_partition_count(size_t new_value);


    size_t receiver_size;
    size_t sender_size;
    size_t input_bits;
    shared_ptr<SEALContext> context;
    vector<uint64_t> seeds;
    PublicKey pub_key;
    RelinKeys relin_key;
    SecretKey sec_key;

private:
    size_t poly_modulus_degree_;
    size_t sender_partition_count_;
};
class GeneSender
{
public:
    GeneSender(GeneParams &params);
    //vector<Ciphertext> encrypt_inputs(vector<uint64_t> &inputs, vector<bucket_slot> &buckets);
    //vector<size_t> decrypt_matches(vector<Ciphertext> &encrypted_matches);
    //vector<pair<size_t, uint64_t>> decrypt_labeled_matches(vector<Ciphertext> &encrypted_matches);
    PublicKey& public_key();
    vector<Ciphertext> encrypted_genedata(vector<double_t> &gene_data);
    RelinKeys& relin_keys();
    SecretKey& secret_key();
private:
    GeneParams &params;
    KeyGenerator keygen;
    PublicKey public_key_;
    SecretKey secret_key_;
    RelinKeys relin_key_;
};

class GeneReceiver
{
public:
    GeneReceiver(GeneParams &params);
    

private:
    GeneParams &params;
};


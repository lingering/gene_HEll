// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "aux.h"

using namespace std;
using namespace seal;

float evalution_distance()
{

    EncryptionParameters parms(scheme_type::CKKS);


    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, { 60, 40, 40, 60 }));

    /*
    We choose the initial scale to be 2^40. At the last level, this leaves us
    60-40=20 bits of precision before the decimal point, and enough (roughly
    10-20 bits) of precision after the decimal point. Since our intermediate
    primes are 40 bits (in fact, they are very close to 2^40), we can achieve
    scale stabilization as described above.
    */
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    vector<double> index;
    cout<<"[+]opening gene info.\twaiting....."<<endl;
    string line;
    ifstream infile("index");
    
    uint32_t number;
    while(getline(infile,line)){
        infile >>number;
        index.push_back(number);
    }  
    infile.close();
    infile.clear(ios::goodbit);

    ifstream infile1("compare");
    vector<double> compare;
    double number1;
    while(getline(infile1,line)){
        infile1>>number1;
        compare.push_back(number1);
    }
    infile1.close();
    infile1.clear(ios::goodbit);

    ifstream infile2("use");
    vector<double> gene;
    double number2;
    while(getline(infile2,line)){
        infile2>>number2;
        gene.push_back(number2);
    }
    infile2.close();
    infile2.clear(ios::goodbit);
    cout<<gene.size()<<endl;
    //

/*
    pFile = fopen("index","rb");
    fseek(pFile,0,SEEK_END);
    lSize=ftell(pFile);
    rewind(pFile);
    buffer = (char*)malloc(sizeof(char)*lSize);
    if(buffer==NULL){
        cout<<"[-]malloc error!"<<endl;
            exit(0);
    }
    result = fread(buffer,1,lSize,pFile);
    for(int i=0;i<lSize;i++)
        gene.push_back(atoi(buffer[i]));
    if(result!=lSize){
        cout<<"[-]Reading your gene data failed, please contact the manager..exiting..."<<endl;
            exit(0);
    }
    fclose(pFile);*/
    cout<<"\tgene info:"<<endl;
    print_vector(gene,5,10);
    cout<<"\tdatabase info:"<<endl;
    print_vector(compare,5,10);
    cout<<"\tfilter info:"<<endl;
    print_vector(index,5,10);
    double scale = pow(2.0, 40);

    auto context = SEALContext::Create(parms);
    //print_parameters(context);
    cout << endl;


    KeyGenerator keygen(context);
    auto public_key = keygen.public_key();
    auto secret_key = keygen.secret_key();
    auto relin_keys = keygen.relin_keys();
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);
    size_t slot_count = encoder.slot_count();
    //cout << "Number of slots: " << slot_count << endl;
    //vector<double> aux1(slot_count,1);
    vector<double> input;
    vector<double> gene_1;   gene_1.assign(gene.begin(),gene.begin()+slot_count);
    vector<double> gene_2;   gene_2.assign(gene.begin()+slot_count,gene.begin()+slot_count*2);
    vector<double> gene_3;   gene_3.assign(gene.begin()+slot_count*2,gene.begin()+slot_count*3);
    vector<double> gene_4;   gene_4.assign(gene.begin()+slot_count*3,gene.end());

    vector<double> index_1;  index_1.assign(index.begin(),index.begin()+slot_count);
    vector<double> index_2;  index_2.assign(index.begin()+slot_count,index.begin()+slot_count*2);
    vector<double> index_3;  index_3.assign(index.begin()+slot_count*2,index.begin()+slot_count*3);
    vector<double> index_4;  index_4.assign(index.begin()+slot_count*3,index.end());

    vector<double> compare_1;compare_1.assign(compare.begin(),compare.begin()+slot_count);
    vector<double> compare_2;compare_2.assign(compare.begin()+slot_count,compare.begin()+slot_count*2);
    vector<double> compare_3;compare_3.assign(compare.begin()+slot_count*2,compare.begin()+slot_count*3);
    vector<double> compare_4;compare_4.assign(compare.begin()+slot_count*3,compare.end());
    
    Plaintext gene_1_plain,gene_2_plain,gene_3_plain,gene_4_plain;
    Plaintext index_1_plain,index_2_plain,index_3_plain,index_4_plain;
    Plaintext compare_1_plain,compare_2_plain,compare_3_plain,compare_4_plain;
    Plaintext aux_en;
    encoder.encode(gene_1,scale,gene_1_plain);
    encoder.encode(gene_2,scale,gene_2_plain);
    encoder.encode(gene_3,scale,gene_3_plain);
    encoder.encode(gene_4,scale,gene_4_plain);
    encoder.encode(index_1,scale,index_1_plain);
    encoder.encode(index_2,scale,index_2_plain);
    encoder.encode(index_3,scale,index_3_plain);
    encoder.encode(index_4,scale,index_4_plain);
    encoder.encode(compare_1,scale,compare_1_plain);
    encoder.encode(compare_2,scale,compare_2_plain);
    encoder.encode(compare_3,scale,compare_3_plain);
    encoder.encode(compare_4,scale,compare_4_plain);
    //encoder.encode(aux1,scale,aux_en);

    Ciphertext gene_1_enc,index_1_enc,compare_1_enc;
    encryptor.encrypt(gene_1_plain,gene_1_enc);
    encryptor.encrypt(index_1_plain,index_1_enc);
    encryptor.encrypt(compare_1_plain,compare_1_enc);
    
    //evaluate performance
    std::chrono::high_resolution_clock::time_point time_start, time_end;
	std::chrono::microseconds time_pram_gen_sum;
	std::chrono::microseconds time_calc_sum;
	time_start = std::chrono::high_resolution_clock::now();

    evaluator.multiply(index_1_enc,gene_1_enc,gene_1_enc);
    //evaluator.multiply_plain(compare_1_plain,aux_en,compare_1_plain);

    evaluator.relinearize_inplace(gene_1_enc,relin_keys);
    evaluator.rescale_to_next_inplace(gene_1_enc);
    parms_id_type parms_id = gene_1_enc.parms_id();
    evaluator.mod_switch_to_inplace(compare_1_enc, parms_id);
      
    

    Ciphertext sub_resu;
    gene_1_enc.scale()=pow(2.0,40);
    compare_1_enc.scale()=pow(2.0,40);
    evaluator.sub(compare_1_enc,gene_1_enc,sub_resu);
    
   /*  Plaintext test;
    vector<double> test_v;
    decryptor.decrypt(sub_resu,test);
    encoder.decode(test,test_v);
    print_vector(test_v);

    cout << "    + Modulus chain index for gene_encrypted: "
        << context->get_context_data(sub_resu.parms_id())->chain_index() << endl;
cout << "    + Modulus chain index for compare_encrypted: "
        << context->get_context_data(compare_1_enc.parms_id())->chain_index() << endl;*/
    evaluator.relinearize_inplace(sub_resu,relin_keys);
    //evaluator.rescale_to_next_inplace(gene_1_enc);

  
    Ciphertext final_resu;
    evaluator.square(sub_resu,final_resu);
    vector<double> reulst;
    Plaintext plain_result0;
    decryptor.decrypt(final_resu,plain_result0);
    encoder.decode(plain_result0,reulst);
    time_end=chrono::high_resolution_clock::now();
    time_calc_sum=chrono::duration_cast<
            chrono::microseconds>(time_end - time_start);
    float_t seconds=(float)time_calc_sum.count()/1000000;
    cout<<"[@] finished distance computing:(time consuming)->"<<seconds<<" seconds"<<endl;
    //print_vector(reulst);

    return  seconds;
}
float evalution_disease_detect(){
    EncryptionParameters parms(scheme_type::BFV);

    size_t poly_modulus_degree = 4096;
    parms.set_poly_modulus_degree(poly_modulus_degree);

    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
      
    parms.set_plain_modulus(256);

    auto context = SEALContext::Create(parms);

    KeyGenerator keygen(context);
    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();
   
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);

    Decryptor decryptor(context, secret_key);

    IntegerEncoder encoder(context);
    
    int x = 250;
    int y = 234;
    Plaintext x_plain=encoder.encode(x);
    Plaintext y_plain=encoder.encode(y);

    /*
    we then encrypt the plaintext, producing a ciphertext.
    */
    
    Ciphertext y_encrypted;
    Ciphertext x_encrypted;
    
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(y_plain, y_encrypted);
    

    std::chrono::high_resolution_clock::time_point time_start, time_end;
	std::chrono::microseconds time_pram_gen_sum;
	std::chrono::microseconds time_calc_sum;
	time_start = std::chrono::high_resolution_clock::now();
    evaluator.sub_inplace(x_encrypted,y_encrypted);
    
    time_end=chrono::high_resolution_clock::now();
    time_calc_sum=chrono::duration_cast<
            chrono::microseconds>(time_end - time_start);
    
    /*
    We decrypt the ciphertext and print the resulting plaintext in order to
    demonstrate correctness of the encryption.
    */
    Plaintext x_decrypted;
    float seconds=(float)time_calc_sum.count()/1000000;
    cout << "[@] disease detect time:"<<seconds<<"seconds"<<endl;
    decryptor.decrypt(x_encrypted, x_decrypted);
    if(encoder.decode_uint32(x_decrypted)<0)
        cout<<"[!!!] you are vulerable to this cancer.";
    return seconds;
}

#include <cstdint>
#include <vector>
#include "boost/asio.hpp"
#include "seal/seal.h"

using namespace std;
using namespace boost::asio;
using namespace seal;
//to show client and server has established a socket connection
const uint64_t NET_MAGIC_HELLO = 0xdeadbeeful; // 
const uint32_t NET_MAGIC_VECTOR_UINT64 = 0x76756938ul; // 'vui8'
const uint32_t NET_MAGIC_CIPHERTEXT = 0x63697074ul; // 'cipt'
const uint32_t NET_MAGIC_VECTOR_CIPHERTEXT = 0x76636970ul; // 'vcip'
const uint32_t NET_MAGIC_PUBLIC_KEY = 0x706b6579ul; // 'pkey'
const uint32_t NET_MAGIC_RELIN_KEYS = 0x72656c6eul; // 'reln'

class Networking
{
	public:
		Networking(ip::tcp::socket &socket);
		void set_seal_context(shared_ptr<SEALContext> new_context);
		uint32_t read_uint32();
		uint64_t read_uint64();
		void write_uint32(uint32_t value);
		void write_uint64(uint64_t value);
		
		void recv_command();
		void send_command();
		
		void read_uint64s(vector<uint64_t> &value);
		void write_uint64s(vector<uint64_t> &value);
		
		void read_enc_geneRNA(Ciphertext &ciphertext);
		void write_enc_geneRNA(Ciphertext &ciphertext);
		void read_enc_geneRNAs(vector<Ciphertext> &ciphertexts);
		void write_enc_geneRNAs(vector<Ciphertext> &ciphertexts);

		void read_public_key(PublicKey &pub_key);
		void write_public_key(PublicKey &pub_key);

		void read_relin_key(RelinKeys &relin_key);
		void write_relin_key(RelinKeys &relin_key);
	private:
		ip::tcp::socket &socket;
		boost::asio::streambuf read_buff;
		std::istream read_stream;
		boost::asio::streambuf write_buff;
		std::ostream write_stream;
		shared_ptr<SEALContext> seal_context;

};

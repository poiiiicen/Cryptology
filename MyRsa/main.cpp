#include <iostream>
#include <string>
#include <cstdio>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

#define N "FF807E694D915875B13F47ACDDA61CE11F62E034150F84660BF34026ABAF8C37"
#define E "010001"
#define D "45AEF3CB207EAD939BBDD87C8B0F0CFCC5A366A5AF2AC5FE1261D7547C625F51"

char plaintext[] =
	"01. A quick brown fox jumps over the lazy dog.\n"\
	"02. A quick brown fox jumps over the lazy dog.\n"\
	"03. A quick brown fox jumps over the lazy dog.\n";

unsigned char IV[] = "0123456789ABCDEFDEADBEEFBADBEAD!";

int main() {
	RSA *key = RSA_new();
	BIGNUM *pn = BN_new(), *pe = BN_new(), *pd = BN_new();
	BN_hex2bn(&pn, N);
	BN_hex2bn(&pe, E);
	BN_hex2bn(&pd, D);
	key->n = pn;
	key->e = pe;
	key->d = pd;
	//key->flags |= RSA_FLAG_NO_BLINDING;

	int blocksize = RSA_size(key);//the size of the key, also the size of the block
	int size = strlen(plaintext);//the size of plaintext
	int blocknum = (size - 1) / blocksize + 1;//the number of blocks
	unsigned char *xor = new unsigned char[blocksize + 3];
	unsigned char *buf = new unsigned char[blocksize + 3];
	unsigned char *plain = new unsigned char[blocknum * blocksize * 2 + 1];
	unsigned char *cipher = new unsigned char[blocknum * blocksize * 2 + 1];
	/*padding the plaintext with zero*/
	strncpy((char *)plain, plaintext, size);
	for (int i = size; i < blocknum * blocksize; ++i)
		plain[i] = 0;
	
	std::cout << "plaintext=\n" << plain << "Encrypting...\n";

	/*encrypt the plaintext*/
	strncpy((char *)xor, (char *)IV, blocksize);
	for (i = 0; i < blocknum; ++i){
		for (int j = 0; j < blocksize; ++j)
			xor[j] = xor[j] ^ plain[i * blocksize + j];
		RSA_public_encrypt(blocksize, xor, buf, key, RSA_NO_PADDING);
		for (j = 0; j < blocksize; j++)
			sprintf((char *)(cipher + i * blocksize * 2 + j * 2), "%02X", buf[j]);
		strncpy((char *)xor, (char *)buf, blocksize);
	}

	/*swap the last two blocks and truncate the ciphertext to the length of the original plaintext according to the ciphertext stealing*/
	unsigned char temp;
	for (i = 0; i < blocksize * 2; i++){
		temp = cipher[(blocknum - 2) * blocksize * 2 + i];
		cipher[(blocknum - 2) * blocksize * 2 + i] = cipher[(blocknum - 1) * blocksize * 2 + i];
		cipher[(blocknum - 1) * blocksize * 2 + i] = temp;
	}
	cipher[size * 2] = 0;
	
	std::cout << "ciphertext=\n" << cipher << "\nDecrypting...\n";
	
	/*decrypt the last block and use the plaintext to modify the ciphertext according to the ciphertext stealing*/
	for (i = 0; i < blocksize; ++i)
		sscanf((char *)(cipher + (blocknum - 2) * blocksize * 2 + i * 2), "%02X", &buf[i]);
	RSA_private_decrypt(blocksize, buf, xor, key, RSA_NO_PADDING);
	for (i = size - (blocknum - 1) * blocksize; i < blocksize; ++i){
		sprintf((char *)(cipher + (blocknum - 1) * blocksize * 2 + i * 2), "%02X", xor[i]);
	}
	
	/*swap the last two blocks according to the ciphertext stealing*/
	for (i = 0; i < blocksize * 2; i++){
		temp = cipher[(blocknum - 2) * blocksize * 2 + i];
		cipher[(blocknum - 2) * blocksize * 2 + i] = cipher[(blocknum - 1) * blocksize * 2 + i];
		cipher[(blocknum - 1) * blocksize * 2 + i] = temp;
	}
	
	/*decrypt the ciphertext*/
	for (i = blocknum - 1; i > 0; --i){
		RSA_private_decrypt(blocksize, buf, xor, key, RSA_NO_PADDING);
		for (int j = 0; j < blocksize; ++j){
			sscanf((char *)(cipher + (i - 1) * blocksize * 2 + j * 2), "%02X", &buf[j]);
		}
		for (j = 0; j < blocksize; ++j)
			plain[i * blocksize + j] = xor[j] ^ buf[j];
	}
	RSA_private_decrypt(blocksize, buf, xor, key, RSA_NO_PADDING);
	strncpy((char *)buf, (char *)IV, blocksize);
	for (i = 0; i < blocksize; ++i)
		plain[i] = xor[i] ^ buf[i];
	plain[size] = 0;

	std::cout << "plaintext=\n" << plain << "\n";

	unsigned char *t = new unsigned char[50];
	unsigned char *M = new unsigned char[80];
	unsigned char *MM = new unsigned char[80];
	unsigned char *MMM = new unsigned char[80];

	/*md5*/
	std::cout << "md5=\n";
	MD5(plain, size, t);
	for (i = 0; i < 16; ++i){
		printf("%02X", t[i]);
		M[i] = t[i];
	}
	std::cout << "\n";

	/*sha-1*/
	std::cout << "sha-1=\n";
	SHA1(plain, size, t);
	for (i = 0; i < 20; ++i){
		printf("%02X", t[i]);
		M[i + 16] = t[i];
	}
	std::cout << "\n";

	/*md5+sha-1*/
	std::cout << "md5+sha-1\n";
	for (i = 0; i < 36; ++i)
		printf("%02X", M[i]);
	std::cout << "\n";

	/*signature encrypt*/
	std::cout << "Encrypting...\nsignature=\n";
	RSA_private_encrypt(blocksize, M, MM + blocksize, key, RSA_NO_PADDING);
	strncpy((char *)(M + 36), (char *)(MM + 36), blocksize * 2 - 36);
	RSA_private_encrypt(blocksize, M + blocksize, MM, key, RSA_NO_PADDING);
	for (i = 0; i < 36; ++i)
		printf("%02X", MM[i]);
	std::cout << "\n";

	/*signature decrypt*/
	std::cout << "Decrypting...\nplaintext=\n";
	RSA_public_decrypt(blocksize, MM, MMM + blocksize, key, RSA_NO_PADDING);
	strncpy((char *)(MM + 36), (char *)(MMM + 36), blocksize * 2 - 36);
	RSA_public_decrypt(blocksize, MM + blocksize, MMM, key, RSA_NO_PADDING);
	bool flag = true;
	for (i = 0; i < 36; ++i){
		printf("%02X", MMM[i]);
		if (M[i] != MMM[i]) flag = false;
	}
	if (flag) std::cout << "\nSignature is correct.\n";
	else std::cout << "\nSignature is incorrect.\n";
	getchar();

	delete[] xor;
	delete[] buf;
	delete[] plain;
	delete[] cipher;
	delete[] t;
	delete[] M;
	delete[] MM;
	delete[] MMM;
	return 0;
}

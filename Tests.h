#ifndef TEST_H
#define TEST_H
#include"Definitions.h"
#include"random_util.h"
#include"functional_util.h"
#include"Util.h"
#include"KeySet.h"


using std::cout;
using std::string;


vector<uint8> encrypt(const vector<uint8>& rawData, const std::string& key);
vector<uint8> decrypt(const vector<uint8>& rawData, const std::string& key);
std::pair<vector<uint8>, string> genRandomDataAndKey(const uint dataLength, const uint keyLength);
vector<uint8> encodeLength(const vector<uint8>& bytes, const ShaKeySet& keys);
vector<uint8> decodeLength(const vector<uint8>& bytes);


void testEncryptionAtSize(const uint dataLength, const uint keyLength) {

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(dataLength, keyLength);

	vector<uint8> data = dataKeyPair.first;
	string key = dataKeyPair.second;

	vector<uint8> encrypted = encrypt(data, key);

	vector<uint8> decrypted = decrypt(encrypted, key);

	string truthVal = areEqual(data, decrypted) ? "true" : "false";

	cout << "encryption and decryption successful: " << truthVal << "\n";

}


void testEntropy() {

	string message = "This is my message.";

	string key = "myKey1";

	cout << message << "\n";

	vector<uint8> encrypted, decrypted;

	encrypted = encrypt(toBytes(message), key);
	encrypted[Random::randInt(encrypted.size())] = (uint8)Random::randInt(UCHAR_MAX);
	decrypted = decrypt(encrypted, key);

	printVec(encrypted);

	string enStr = toString(encrypted);

	cout << "size: " << enStr.size() << "\n";
	cout << "encrypted: " << enStr << "\n";



	string deStr = toString(decrypted);

	cout << "size: " << deStr.size() << "\n";
	cout << "decrypted: " << deStr << "\n";

}

bool testLengthEncoding() {

	vector<uint8> bytes = getPermutationVector<uint8>(23);

	ShaKeySet keys("asdf");

	vector<uint8> encoded = encodeLength(bytes, keys);

	for (int i = 0; i < encoded.size(); i++) {
		cout << (int)encoded[i] << "  ";
	}

	cout << "\n\n";
	printBits(encoded.back());

	cout << "\n\n";
	vector<uint8> decoded = decodeLength(encoded);

	cout << (vectorsEqual(bytes, decoded) ? "true" : "false") << "\n";

}

void testShuffleBits() {

	uint8 str[] = "hello world";
	cout << str << "\n";

	shuffleBits(str, 12, 32);

	cout << str << "\n";

	shuffleBits(str, 12, 54);

	cout << str << "\n";

	invShuffleBits(str, 12, 54);

	cout << str << "\n";

	invShuffleBits(str, 12, 32);

	cout << str << "\n";

}


#endif // !TEST_H
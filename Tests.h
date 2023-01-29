#ifndef TEST_H
#define TEST_H
#include"Definitions.h"
#include"random_util.h"
#include"functional_util.h"
#include"Util.h"


using std::cout;
using std::string;


vector<uint8> encrypt(const vector<uint8>& rawData, const std::string& key);
vector<uint8> decrypt(const vector<uint8>& rawData, const std::string& key);
std::pair<vector<uint8>, string> genDataAndKey();


void testEncryption() {

	std::pair<vector<uint8>, string> dataKeyPair = genDataAndKey();

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


#endif // !TEST_H
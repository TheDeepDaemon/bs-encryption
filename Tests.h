#ifndef TEST_H
#define TEST_H
#include"Definitions.h"
#include"random_util.h"
#include"functional_util.h"
#include"Util.h"
#include"KeySet.h"
#include"Encrypt.h"
#include<filesystem>


// verify that length encoding is working as intended
bool testLengthEncoding(const uint dataLength, const uint keyLength) {

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(dataLength, keyLength);

	vector<uint8> data = dataKeyPair.first;
	string key = dataKeyPair.second;

	ShaKeySet keySet(key);

	vector<uint8> encoded = encodeLength(data, keySet);

	if (encoded == data) {
		return false;
	}

	vector<uint8> decoded = decodeLength(encoded.data(), encoded.size());

	return (decoded == data);
}


// verify that bit shuffling works correctly
bool testShuffleBits(const uint size) {
	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(size, 0);

	const vector<uint8> originalBytes = dataKeyPair.first;
	vector<uint8> bytes = dataKeyPair.first;
	uint8* ptr = bytes.data();

	uint32 seed = (uint32)Random::randInt(UINT32_MAX);

	bool* tempArr = new bool[size * 8];

	shuffleBits(ptr, size, seed, tempArr);

	delete[] tempArr;

	if (bytes == originalBytes) {
		return false;
	}

	invShuffleBits(ptr, size, seed);

	return bytes == originalBytes;
}


// verify that the hidden keys encrypt correctly
bool testByteMapping() {

	ByteMappingGrid byteMapping = ByteMappingGrid::genByteMappingGrid();

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(BLOCK_SIZE, 0);

	vector<uint8> data = dataKeyPair.first;

	DataBlock block(data);

	byteMapping.mapBytes(block);

	vector<uint8> encrypted(block.data, block.data + BLOCK_SIZE);

	if (encrypted == data) {
		return false;
	}

	byteMapping.invMapBytes(block);

	vector<uint8> decrypted(block.data, block.data + BLOCK_SIZE);

	return (decrypted == data);
}


// test encryption for random data of a specific size and with a specific key length
bool testBytesEncryptionAtSize(const uint dataLength, const uint keyLength) {
	StoredData storedKeys;
	storedKeys.genRandomData();

	std::pair<vector<uint8>, string> dataKeyPair = genRandomDataAndKey(dataLength, keyLength);

	vector<uint8> data = dataKeyPair.first;
	string key = dataKeyPair.second;

	vector<uint8> encrypted = encrypt(data, storedKeys, key);

	vector<uint8> decrypted = decrypt(encrypted, storedKeys, key);

	return (data == decrypted);
}


// verify that vectors of bytes are encrypted and decrypted correctly
bool testBytesEncryption() {
	StoredData storedKeys;
	storedKeys.genRandomData();

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";
	vector<uint8> bytes(originalStr.begin(), originalStr.end());

	vector<uint8> encrypted = encrypt(bytes, storedKeys, "encryption key");

	if (encrypted == bytes) {
		return false;
	}

	vector<uint8> decrypted = decrypt(encrypted, storedKeys, "encryption key");

	return (bytes == decrypted);
}


// verify that strings are encrypted and decrypted correctly
bool testStringEncryption() {
	StoredData storedKeys;
	storedKeys.genRandomData();

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	string encrypted = encrypt(originalStr, storedKeys, "encryption key");

	if (encrypted == originalStr) {
		return false;
	}

	string decrypted = decrypt(encrypted, storedKeys, "encryption key");

	return (originalStr == decrypted);
}


// verify that vectors of bytes are encrypted and decrypted correctly when encoded as hex
bool testHexBytesEncryption() {
	StoredData storedKeys;
	storedKeys.genRandomData();

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";
	vector<uint8> bytes(originalStr.begin(), originalStr.end());

	vector<uint8> encrypted = encryptAsHex(bytes, storedKeys, "encryption key");

	if (encrypted == bytes) {
		return false;
	}

	vector<uint8> decrypted = decryptFromHex(encrypted, storedKeys, "encryption key");

	return (bytes == decrypted);
}


// verify that if encrypted data is corrupted, the decrypted result is corrupted
bool testCorruptedData() {
	StoredData storedKeys;
	storedKeys.genRandomData();

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	string encrypted = encrypt(originalStr, storedKeys, "encryption key");

	if (encrypted[4] != 'a') {
		encrypted[4] = 'a';
	}
	else {
		encrypted[4] = 'f';
	}

	string decrypted = decrypt(encrypted, storedKeys, "encryption key");

	return !(originalStr == decrypted);
}


// verify that the zero vector does not give a zero vector when encrypted
bool testZeroVector() {
	StoredData storedKeys;
	storedKeys.genRandomData();

	uint vectorSizeFloor = 10;
	uint vectorSizeCeil = 100;

	// create the null vector
	uint vectorSize = Random::randInt(vectorSizeFloor, vectorSizeCeil);
	vector<uint8> nullVector(vectorSize, 0);

	// encrypt and decrypt
	vector<uint8> encrypted = encrypt(nullVector, storedKeys, "zero vector test encryption key");
	vector<uint8> decrypted = decrypt(encrypted, storedKeys, "zero vector test encryption key");

	// verify
	bool encryptedIsNotZero = (encrypted != vector<uint8>(encrypted.size(), 0));

	bool correctlyDecrypted = (decrypted == nullVector);

	return encryptedIsNotZero && correctlyDecrypted;
}


// test that stored keys are saved properly
bool testStoredData() {
	string testDirectory = "test-files/";

	StoredData storedKeys1;
	storedKeys1.genRandomData();
	storedKeys1.save(testDirectory + "test-key-filename");

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	const string encrypted = encrypt(originalStr, storedKeys1, "encryption key");

	if (encrypted == originalStr) {
		return false;
	}

	StoredData storedKeys2;
	storedKeys2.loadData(testDirectory + "test-key-filename");

	const string decrypted = decrypt(encrypted, storedKeys2, "encryption key");

	return (decrypted == originalStr);
}


// test that the wrong stored keys will decrypt properly
bool testWrongStoredData() {
	string testDirectory = "test-files/";

	StoredData storedKeys1;
	storedKeys1.genRandomData();

	StoredData wrongStoredKeys;
	wrongStoredKeys.genRandomData();
	wrongStoredKeys.save(testDirectory + "wrong-test-key-filename");

	const string originalStr =
		"Lorem Ipsum, this is a message, this is more text, the red fox jumped over the lazy dog";

	const string encrypted = encrypt(originalStr, storedKeys1, "encryption key");

	if (encrypted == originalStr) {
		return false;
	}

	StoredData storedKeys2;
	storedKeys2.loadData(testDirectory + "wrong-test-key-filename");

	const string decrypted = decrypt(encrypted, storedKeys2, "encryption key");

	return (decrypted != originalStr);
}


// test that file encryption works
bool testFileEncryption() {
	string testDirectory = "test-files/";
	string fname = testDirectory + "test-file-encryption.txt";
	string encryptedFname = testDirectory + "test-file-encryption-encrypted.txt";
	string decryptedFname = testDirectory + "test-file-encryption-decrypted.txt";

	StoredData storedKeys;
	storedKeys.genRandomData();
	storedKeys.save(testDirectory + "test-key-filename");

	string str = "This is a test file. Test data, test data, test data, test data, test data, test data, test data.";
	dump(fname, str);

	string storedKeysFname = testDirectory + "test-key-filename";
	encryptFile(fname, encryptedFname, storedKeysFname, "encryption-key1");
	decryptFile(encryptedFname, decryptedFname, storedKeysFname, "encryption-key1");

	string originalFile = slurps(fname);
	string decryptedFile = slurps(decryptedFname);

	return originalFile == decryptedFile;
}


bool runTests() {

	std::filesystem::create_directories("./test-files");

	const uint lengthEncodingStartSize = 10;
	const uint lengthEncodingEndSize = 100;
	const uint lengthEncodingKeySize = 8;
	for (int i = lengthEncodingStartSize; i < lengthEncodingEndSize; i++) {
		if (!testLengthEncoding(i, lengthEncodingKeySize)) {
			cout << "Length encoding failed.\n";
			return false;
		}
	}

	const uint shuffleBitsStartSize = 8;
	const uint shuffleBitsEndSize = 20;
	for (uint i = shuffleBitsStartSize; i < shuffleBitsEndSize; i++) {
		if (!testShuffleBits(i)) {
			cout << "Bit shuffling test failed.\n";
			return false;
		}
	}

	if (!testBytesEncryption()) {
		cout << "Bytes encryption failed.\n";
		return false;
	}
	if (!testStringEncryption()) {
		cout << "String encryption failed.\n";
		return false;
	}
	if (!testHexBytesEncryption()) {
		cout << "Hex encryption failed.\n";
		return false;
	}

	const uint numDataSizes = 10;
	const uint numKeySizes = 10;
	const uint minDataMultiplier = 8;
	const uint dataMultiplierCeil = 100;
	const uint minKeySize = 6;
	for (int i = 0; i < numDataSizes; i++) {
		uint dataSize = i * Random::randInt(minDataMultiplier, dataMultiplierCeil);
		for (int j = 0; j < numKeySizes; j++) {
			int keySize = minKeySize + j;
			if (!testBytesEncryptionAtSize(dataSize, keySize)) {
				cout << "Encryption at data size=" << dataSize << ", and key size=" << keySize << "\n";
				return false;
			}
		}
	}

	if (!testCorruptedData()) {
		cout << "Data corruption test failed.\n";
		return false;
	}

	if (!testZeroVector()) {
		cout << "Zero vector test failed.\n";
		return false;
	}

	if (!testStoredData()) {
		cout << "Stored data test failed.\n";
		return false;
	}

	if (!testWrongStoredData()) {
		cout << "Wrong stored data test failed.\n";
		return false;
	}

	if (!testFileEncryption()) {
		cout << "File encryption test failed.\n";
		return false;
	}

	cout << "Finished tests, passed.\n";
	return true;
}


#endif // !TEST_H
#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H
#include"Definitions.h"
#include"file_util.h"


class FileBuffer {
private:
	uint it = 0;
	vector<uint8> fileData;

public:

	FileBuffer(const string& fname) {
		fileData = slurp<uint8>(fname);
	}

	vector<uint8> getNextBytes(const uint n) {
		const uint begin = it;
		const uint end = std::min(it + n, fileData.size());
		it = end;
		return vector<uint8>(fileData.begin() + begin, fileData.begin() + end);
	}

	uint8 getNextByte() {
		_ASSERT(it < fileData.size());
		return fileData[it++];
	}

	uint numBytesLeft() {
		return fileData.size() - it;
	}

	uint getFileSize() {
		return fileData.size();
	}

};


#endif // !FILE_BUFFER_H
#pragma once

#include <string>

typedef unsigned int UINT;

class Image {
private:
	UINT x, y;
	UINT n;

	unsigned char* imageData;

public:
	Image(unsigned char const* data, UINT x, UINT y, UINT n);
	~Image();

	unsigned char const* getImageData() const;
	UINT Width() const;
	UINT Height() const;
	UINT getComp() const;

	bool setNewImageData(unsigned char const* newImageData);
};

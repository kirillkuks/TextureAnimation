#pragma once

#include <string>

class Image {
private:
	size_t x, y;
	size_t n;

	unsigned char* imageData;

public:
	Image(unsigned char const* data, size_t x, size_t y, size_t n);
	~Image();

	unsigned char const* getImageData() const;
	size_t getWidth() const;
	size_t getHeight() const;
	size_t getComp() const;

	bool setNewImageData(unsigned char const* newImageData);
};

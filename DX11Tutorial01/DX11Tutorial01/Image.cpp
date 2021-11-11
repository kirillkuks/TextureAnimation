#include "Image.h"


#include <memory>

Image::Image(unsigned char const* data, UINT x, UINT y, UINT n) : x{ x }, y{ y }, n{ n } {
	UINT size = x * y * n;

	imageData = new unsigned char[size];
	std::memcpy(imageData, data, size * sizeof(unsigned char));
}

Image::~Image() {
	if (imageData) {
		delete[] imageData;
	}
}

unsigned char const* Image::getImageData() const {
	return imageData;
}

UINT Image::Width() const {
	return x;
}

UINT Image::Height() const {
	return y;
}

UINT Image::getComp() const {
	return n;
}

bool Image::setNewImageData(unsigned char const* newImageData) {
	UINT size = x * y * n;

	unsigned char* newData = new unsigned char[size];

	if (newData == nullptr) {
		return false;
	}

	delete imageData;

	std::memcpy(newData, newImageData, size);
	imageData = newData;

	return true;
}

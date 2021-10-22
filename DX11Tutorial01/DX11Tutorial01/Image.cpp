#include "Image.h"


#include <memory>

Image::Image(unsigned char const* data, size_t x, size_t y, size_t n) : x{ x }, y{ y }, n{ n } {
	size_t size = x * y * n;

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

size_t Image::getWidth() const {
	return x;
}

size_t Image::getHeight() const {
	return y;
}

size_t Image::getComp() const {
	return n;
}

bool Image::setNewImageData(unsigned char const* newImageData) {
	size_t size = x * y * n;

	unsigned char* newData = new unsigned char[size];

	if (newData == nullptr) {
		return false;
	}

	delete imageData;

	std::memcpy(newData, newImageData, size);
	imageData = newData;

	return true;
}

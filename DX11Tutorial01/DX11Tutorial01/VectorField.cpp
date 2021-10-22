#include "VectorField.h"

VectorField::VectorField(size_t x, size_t y) : field(x, std::vector<std::pair<int, int>>(y, { 5, 0 })) {}

VectorField::~VectorField() {}

unsigned char* VectorField::apply_field(unsigned char const* imageData, size_t x, size_t y, size_t n) const {
	size_t size = x * y * n;
	unsigned char* newImageData = new unsigned char[size * sizeof(unsigned char)];

	if (newImageData == nullptr) {
		return nullptr;
	}

	std::memcpy(newImageData, imageData, size);

	for (size_t i = 0; i < x; ++i) {
		for (size_t j = 0; j < y; ++j) {
			std::pair<int, int> const& vec = field[i][j];

			size_t n_i = i + vec.first;
			size_t n_j = j + vec.second;

			if (n_i < x && n_j < y) {
				size_t n_ind = n * (n_j + n_i * y);
				size_t ind = n * (j + i * y);

				newImageData[n_ind + 0] = imageData[ind + 0];
				newImageData[n_ind + 1] = imageData[ind + 1];
				newImageData[n_ind + 2] = imageData[ind + 2];
			}
		}
	}

	return newImageData;
}

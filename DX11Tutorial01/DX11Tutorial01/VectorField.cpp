#include "VectorField.h"

VectorField::VectorField(size_t x, size_t y) : field(x, std::vector<std::pair<int, int>>(y, { 2, 0 })) {
	setHalfSpeedTransformField(3);
	setCycleField();
}

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

void VectorField::invert() {
	size_t x = field.size();
	size_t y = field[0].size();

	std::vector<std::vector<std::pair<int, int>>> newField(x, std::vector<std::pair<int, int>>(y, { 0, 0 }));

	for (size_t i = 0; i < x; ++i) {
		for (size_t j = 0; j < y; ++j) {
			auto& vec = field[i][j];

			size_t new_i = i + vec.first;
			size_t new_j = j + vec.second;

			newField[new_i][new_j].first = -vec.first;
			newField[new_i][new_j].second = -vec.second;
		}
	}

	field = newField;
}

void VectorField::setCycleField() {
	size_t x = field.size();
	size_t y = field[0].size();


	for (size_t j = 0; j < y; ++j) {
		int speed_x = field[0][j].first;

		for (size_t i = 0; i < speed_x; ++i) {
			field[x - i - 1][j] = std::make_pair(-((int)x - speed_x), 0);
			field[x - i - 1][j] = std::make_pair(-((int)x - speed_x), 0);
		}
	}
}

void VectorField::setHalfSpeedTransformField(int speed) {
	size_t x = field.size();
	size_t y = field[0].size();

	size_t half_y = y / 2;

	for (size_t i = 0; i < x; ++i) {
		for (size_t j = 0; j <half_y; ++j) {
			field[i][j] = std::make_pair(speed, 0);
		}
	}
}

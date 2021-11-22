#include "VectorField.h"
#include <fstream>
#include <assert.h>

VectorField::VectorField(size_t x, size_t y) : field(x, std::vector<std::pair<int, int>>(y, { 1, 0 }))
{
	// setHalfSpeedTransformField(2);
	// setDiagField(4);
	// setCycleField();
	// int a = 2 + 4;
	// setCircleField();
	// setSinField();
	setField();
}

VectorField::~VectorField() {}

unsigned char* VectorField::apply_field(unsigned char const* imageData, size_t x, size_t y, size_t n) const
{
	size_t size = x * y * n;
	unsigned char* newImageData = new unsigned char[size * sizeof(unsigned char)];

	if (newImageData == nullptr)
	{
		return nullptr;
	}

	std::memcpy(newImageData, imageData, size);

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
		{
			std::pair<int, int> const& vec = field[i][j];

			size_t n_i = i + vec.first;
			size_t n_j = j + vec.second;

			if (n_i < x && n_j < y)
			{
				size_t n_ind = n * (n_j + n_i * x);
				size_t ind = n * (j + i * x);

				newImageData[n_ind + 0] = imageData[ind + 0];
				newImageData[n_ind + 1] = imageData[ind + 1];
				newImageData[n_ind + 2] = imageData[ind + 2];
			}
		}
	}

	return newImageData;
}

float* VectorField::raw_data() const
{
	size_t x = field.size();
	size_t y = field[0].size();

	float* srcData = new float[2 * x * y];
	assert(srcData);

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j) {
			float q = (float)field[i][j].first / (float)x;
			float p = (float)field[i][j].second / (float)y;

			srcData[2 * (j + i * x) + 0] = -(float)field[i][j].first / (float)x;
			srcData[2 * (j + i * x) + 1] = -(float)field[i][j].second / (float)y;
		}
	}

	return srcData;
}

void VectorField::invert()
{
	size_t x = field.size();
	size_t y = field[0].size();

	std::vector<std::vector<std::pair<int, int>>> newField(x, std::vector<std::pair<int, int>>(y, { 0, 0 }));

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
		{
			auto& vec = field[i][j];

			size_t new_i = i + vec.first;
			size_t new_j = j + vec.second;

			newField[new_i][new_j].first = -vec.first;
			newField[new_i][new_j].second = -vec.second;
		}
	}

	field = newField;
}

void VectorField::inv()
{
	size_t x = field.size();
	size_t y = field[0].size();

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
		{
			elem_t elem = field[i][j];

			field[i][j].first = -elem.first;
			field[i][j].second = -elem.second;
		}
	}

}

void VectorField::setCycleField()
{
	size_t x = field.size();
	size_t y = field[0].size();


	for (size_t j = 0; j < y; ++j)
	{
		int speed_x = field[0][j].first;

		for (size_t i = 0; i < speed_x; ++i)
		{
			field[x - i - 1][j] = std::make_pair(-((int)x - speed_x), field[x - i - 1][j].second);
		}
	}
}

void VectorField::setHalfSpeedTransformField(int speed)
{
	size_t x = field.size();
	size_t y = field[0].size();

	size_t half_y = y / 2;

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j <half_y; ++j)
		{
			field[i][j] = std::make_pair(speed, 0);
		}
	}
}

void VectorField::setDiagField(size_t turnsNum)
{
	size_t x = field.size();
	size_t y = field[0].size();

	size_t len = (size_t)(x / pow(2, turnsNum));

	int direction = 1;
	size_t p_counter = 0;

	for (size_t i = 0; i < x; ++i)
	{
		if (p_counter == len)
		{
			direction *= -1;
			p_counter = 0;
		}
		++p_counter;

		for (size_t j = 0; j < y; ++j)
		{
			// field[i][j] = std::make_pair(field[i][j].first, 1 * direction);
			field[i][j].second = 1 * direction;

			/*if (i > x / 2)
			{
				field[i][j] = std::make_pair(2, 0);
			}*/
		}
	}
}

void VectorField::setCircleField()
{
	size_t x = field.size();
	size_t y = field[0].size();

	int centerX = (int)x / 2;
	int centerY = (int)y / 2;

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
		{

			int newX = (int)i - centerX;
			int newY = (int)j - centerY;

			field[i][j].first = -(int)j;
			field[i][j].second = (int)i;
		}
	}

}

void VectorField::setSinField()
{
	size_t x = field.size();
	size_t y = field[0].size();

	int centerX = (int)x / 2;
	int centerY = (int)y / 2;

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
		{
			if (i > j && i >= y - j)
			{
				field[i][j].first = 0;
				field[i][j].second = 5;
			}

			if (i >= j && i < y - j)
			{
				field[i][j].first = 5;
				field[i][j].second = 0;
			}

			if (i < j && i <= y - j)
			{
				field[i][j].first = 0;
				field[i][j].second = -5;
			}

			if (i <= j && i > y - j)
			{
				field[i][j].first = -5;
				field[i][j].second = 0;
			}
		}
	}
}


void VectorField::setField()
{
	size_t x = field.size();
	size_t y = field[0].size();

	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j) 
		{
			field[i][j].first = (int)(100 * sin(3.141 * j / (float(y) / 2)));
			field[i][j].second = 0;
		}
	}
}

#pragma once
#include <vector>

class VectorField
{
private:
	std::vector<std::vector<std::pair<int, int>>> field;

public:
	VectorField(size_t x, size_t y);
	~VectorField();

	unsigned char* apply_field(unsigned char const* imageData, size_t x, size_t y, size_t n) const;

	void invert();

	void setCycleField();
	void setHalfSpeedTransformField(int speed);
};

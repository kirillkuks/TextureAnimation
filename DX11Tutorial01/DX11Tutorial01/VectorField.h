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
	float* raw_data() const;

	void invert();

	void setCycleField();
	void setHalfSpeedTransformField(int speed);
	void setDiagField(size_t turnsNum = 1);
	void setCircleField();
};

#pragma once
#include <vector>

class VectorField
{
	using elem_t = std::pair<int, int>;
private:
	std::vector<std::vector<elem_t>> field;

public:
	VectorField(size_t x, size_t y);
	~VectorField();

	unsigned char* apply_field(unsigned char const* imageData, size_t x, size_t y, size_t n) const;
	float* raw_data() const;

	void invert();
	void inv();

	void setCycleField();
	void setHalfSpeedTransformField(int speed);
	void setDiagField(size_t turnsNum = 1);
	void setCircleField();
	void setSinField();
	void setField();
};

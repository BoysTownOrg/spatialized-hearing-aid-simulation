#include <fir-filtering/FirFilter.h>

int main() {
	FirFilter filter{ std::vector<float>{ 1, 2, 3, 4, 5 } };
	std::vector<float> x(100000000);
	filter.process(&x[0], 100000000);
}
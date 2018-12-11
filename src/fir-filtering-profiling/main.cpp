#include <fir-filtering/FirFilter.h>

int main() {
	FirFilter filter{ std::vector<float>(10000) };
	std::vector<float> x(1024);
	for (int i = 0; i < 3520; ++i)
		filter.process(x);
}
#pragma once

struct testStruct
{
	testStruct()
		: x(0) {}
	testStruct(int x)
		: x(x) {}
	testStruct(int x, int y)
		: x(x) {}
	testStruct(int x, int y, int z)
		: x(x) {}
	~testStruct()
	{
#if DBG_testStruct
		std::cout << "Destroyed!\n";
#endif
	}

	testStruct(testStruct& other)
		: x(other.x)
	{
#if DBG_testStruct
		std::cout << "Copied()\n";
#endif
	}

	testStruct(testStruct&& other) noexcept
		: x(other.x)
	{
#if DBG_testStruct
		std::cout << "Moved()\n";
#endif
	}

	testStruct& operator=(const testStruct& other)
	{
#if DBG_testStruct
		std::cout << "Copied=\n";
#endif
		x = other.x;
		return *this;
	}

	testStruct& operator=(testStruct&& other) noexcept
	{
#if DBG_testStruct
		std::cout << "Moved2()\n";
#endif
		x = other.x;
		return *this;
	}

	int x = 0;
};

std::ostream& operator<<(std::ostream& os, const testStruct& other)
{
	os << other.x;
	return os;
}

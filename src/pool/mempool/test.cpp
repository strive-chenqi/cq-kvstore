#include "mempool.h"
#include <iostream>

class ActualClass {
	static int count;
	int No;

public:
	ActualClass() {
		No = count;
		count++;
	}

	void print() {
		std::cout << this << ": ";
		std::cout << "the " << No << "th object" << std::endl;
	}

	void* operator new(size_t size);
	void operator delete(void* p);
};

// 定义内存池对象
FixSizeMemoryPool<sizeof(ActualClass), 2> mp;

void* ActualClass::operator new(size_t size) {
	return mp.malloc();
}

void ActualClass::operator delete(void* p) {
	mp.free(p);
}

int ActualClass::count = 0;

int main() {
    void* p = mp.malloc();
    std::cout << p << std::endl;

    void* p1 = mp.malloc();
    std::cout << p1 << std::endl;

    void* p2 = mp.malloc();
    std::cout << p2 << std::endl;

    mp.free(p1);
    void* p3 = mp.malloc();
    std::cout << p3 << std::endl;

	// ActualClass* p1 = new ActualClass;
	// p1->print();

	// ActualClass* p2 = new ActualClass;
	// p2->print();
	// delete p1;

	// p1 = new ActualClass;
	// p1->print();

	// ActualClass* p3 = new ActualClass;
	// p3->print();



	// delete p1;
	// delete p2;
	// delete p3;
}

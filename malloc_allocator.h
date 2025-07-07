#ifndef _MALLOC_ALLOCATOR_H_
#define _MALLOC_ALLOCATOR_H_

// Allocator implementation detail based on default CRT malloc/free.
class MallocAllocator {
public:
	MallocAllocator() {}
	~MallocAllocator() {}

	void SetUp() {}
	void TearDown() {}

	void* Alloc(size_t size) {
		return ::malloc(size);
	}
	void Free(void* p) {
		::free(p);
	}
};

#endif // _MALLOC_ALLOCATOR_H_
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe

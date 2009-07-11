/**
 * Simple cyclic buffer implementation
 */

template<class T>
class CyclicBuffer {
	int maxSize;
	int start;
	int size;
	T *buffer;
public:
	CyclicBuffer(int size);
	~CyclicBuffer();
	bool push(const T &e);
	bool shift(T &e);
};

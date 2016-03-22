#include <vector>

using KEY_ID = std::pair<double,unsigned>;



class minPriorityQueue {

private:
	std::vector <KEY_ID> binaryHeap;
	std::vector <unsigned>  ID_HeapIdx;			
	void minHeapifyUp(unsigned heapIdx);
	void minHeapifyDown(unsigned heapIdx);
	void swapNode(unsigned heapIdx1, unsigned heapIdx2);

public:
	minPriorityQueue(unsigned IDsize);
	void print();
	double peek(unsigned ID); 
	bool IDisInQ(unsigned ID);
	KEY_ID extractMin();
	void decreaseKey(unsigned ID, double key);	
	unsigned getQSize() {return binaryHeap.size();}
};

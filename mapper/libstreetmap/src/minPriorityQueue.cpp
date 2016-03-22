#include "minPriorityQueue.h"
#include <limits>
#include <iostream>

const double kMaxDouble = std::numeric_limits<double>::max();
const unsigned kMaxUnsigned = std::numeric_limits<unsigned>::max();

minPriorityQueue::minPriorityQueue (unsigned IDsize) : binaryHeap(IDsize), ID_HeapIdx(IDsize) {
	//the IDs in the minPriorityQueue are unsigned integers from 0 to IDsize-1
	//initialize the KEYs to be max unsigned int
	for (unsigned i=0; i<binaryHeap.size(); i++) {
		binaryHeap[i].first = kMaxDouble;
		binaryHeap[i].second = i;
	}
	//map the IDs to indices in the heap
	for (unsigned i=0; i<ID_HeapIdx.size(); i++)  
		//Heap is indexed from 1 not 0
		ID_HeapIdx[i] = i+1;
}

void minPriorityQueue::minHeapifyUp(unsigned heapIdx) {
	if (heapIdx == 1) return;
	unsigned parent = heapIdx/2;
	if (binaryHeap[parent-1].first > binaryHeap[heapIdx-1].first){
		swapNode(parent, heapIdx);
		minHeapifyUp(parent);
	}

}

void minPriorityQueue::minHeapifyDown(unsigned heapIdx) {
	unsigned left = 2*heapIdx;
	unsigned right = 2*heapIdx + 1;
	unsigned minimum = heapIdx;
	
	//check left child
	if (left <= binaryHeap.size() && binaryHeap[left-1].first < binaryHeap[minimum-1].first)
		minimum = left;
	if (right <= binaryHeap.size() && binaryHeap[right-1].first < binaryHeap[minimum-1].first)
		minimum = right;
	if (minimum != heapIdx) {
		swapNode(minimum, heapIdx);
		minHeapifyDown(minimum);
	}
}

void minPriorityQueue::swapNode(unsigned heapIdx1, unsigned heapIdx2) {
	//update the ID_HeapIdx vector
	ID_HeapIdx[binaryHeap[heapIdx1-1].second] = heapIdx2;
	ID_HeapIdx[binaryHeap[heapIdx2-1].second] = heapIdx1;
	
	//make the swap
	KEY_ID tmpNode = binaryHeap[heapIdx1-1];
	binaryHeap[heapIdx1-1] = binaryHeap[heapIdx2-1];
	binaryHeap[heapIdx2-1] = tmpNode;
}


void minPriorityQueue::print() {
		unsigned l = 1;
		std::cout << "START:" << std::endl;
		for (unsigned i = 0;i < binaryHeap.size(); ++i){
			std::cout << "(k:" << binaryHeap[i].first << ", id:" << binaryHeap[i].second << ") ";
			if (i == l-1) {
				std::cout << std::endl;
				l *= 2;
			}
		}
		std::cout << "END:" << std::endl;
}

void minPriorityQueue::decreaseKey(unsigned ID, double key) {
	unsigned heapIdx = ID_HeapIdx[ID];
	binaryHeap[heapIdx-1].first = key;
	minHeapifyUp(heapIdx);
}

KEY_ID minPriorityQueue::extractMin() {
	KEY_ID retNode = binaryHeap[0];
	//need to erase the mapping in ID_HeapIdx	
	unsigned ID = retNode.second;
	ID_HeapIdx[ID] = kMaxUnsigned;

	//add the leaf node back to root and pop leaf
	binaryHeap[0] = binaryHeap[binaryHeap.size() - 1];
	ID_HeapIdx[binaryHeap[0].second] = 1;
	binaryHeap.pop_back();
	//maintain min heap structure
	minHeapifyDown(1);
	
	return retNode;
}

double minPriorityQueue::peek(unsigned ID) {
	unsigned heapIdx = ID_HeapIdx[ID];
	return binaryHeap[heapIdx-1].first;
}

bool minPriorityQueue::IDisInQ(unsigned ID) {
	if (ID_HeapIdx[ID] == kMaxUnsigned)
		return false;
	else
		return true;
}

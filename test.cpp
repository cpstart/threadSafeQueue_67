// Test para el threadsafe_queue
#include<iostream>
#include<string>

#include "thread_safe_queue.h" 

int main(){
	threadsafe_queue<int> mySafeQueue;
	mySafeQueue.push(1);
	mySafeQueue.push(2);
	while(!mySafeQueue.empty()){
		auto p = mySafeQueue.try_pop();
		std::cout << "poping: " << *p << '\n';
	}
		std::cout << "Its working.." << "\n";
	return 0;
}

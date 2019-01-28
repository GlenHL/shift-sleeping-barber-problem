// Solution.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> 

std::mutex m;
std::mutex mu;
std::condition_variable cv;
std::string data;
int seats=0;
bool ready = false;
bool processed = false;
bool waitingRoomFull = false;
bool haircutdone = true;
static const int num_threads = 40;
std::queue <int> q;
//int i = 0;

	void getSleep() {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lk(m);
		std::cout << "Barber is sleeping";
		std::this_thread::sleep_for(std::chrono::seconds(5));
		cv.wait(lk, [] {return ready; });
		
		// after the wait, we own the lock.
		//std::cout << "Barber thread is awake";
		data += " after processing";

		// Send data back to main()
		processed = true;
		//std::cout << "Worker thread signals data processing completed\n";

		// Manual unlocking is done before notifying, to avoid waking up
		// the waiting thread only to block again (see notify_one for details)
		lk.unlock();
		cv.notify_one();
	}
	void getHairCut(int i)
	{
		//std::cout << "Inside Thread :: ID  = " << std::this_thread::get_id() << std::endl;//random
		mu.lock();
		haircutdone = false;

		if (ready) {
			std::cout << "Getting HairCut" << i;
			
			std::this_thread::sleep_for(std::chrono::seconds(1 + (std::rand() % (5 - 1 + 1)))); //generate random value between 1 to 5
		}
		haircutdone = true;
		mu.unlock();
		/*{
			std::unique_lock<std::mutex> lk1(m);
			haircutdone = true;
			std::cout << "Haircut is done\n"<<i;
		}
		cv.notify_one();*/
	}
	void WaitingRoom(int i)
	{
		//std::unique_lock<std::mutex> lk1(m);
		//std::cout << "Inside Thread :: ID  = " << std::this_thread::get_id() << std::endl;//random
		//std::this_thread::sleep_for(std::chrono::seconds(5));
		
		//std::cout << "Seats " << seats;
		if (q.size() <=(size_t)seats) {
			//std::cout << "Pushing Elemnets " << i<<"size"<< q.size();
			std::cout << "Waiting Room :" << i;
			q.push(i);
			//cv.wait(lk1, [] {return haircutdone; });
			
			if (ready)
			{
				getHairCut(q.front());
				q.pop();
			}
			
		}
		else {
			std::terminate();
			std::cout << "Customer" << i << "leaving";
		}
	}
	/*void createThreads(int tid) {

		//std::cout << "createThreads " << tid << std::endl;
		if (waitingRoomFull) {
			std::terminate();
			std::cout << "Customer" << i << "leaving";
		}
		else {
			std::cout << "Waiting Room :";
			WaitingRoom( tid);
		}
		//std::thread customer(WaitingRoom,i);
	}*/

	int main()
	{
		std::cout << "Please enter seats in the waiting room: ";
		std::cin >> seats; //number of seats
		

		std::thread barber(getSleep);
		// send data to the barber thread
		{
			std::lock_guard<std::mutex> lk(m);
			ready = true;
		}
	
			cv.notify_one();

		// wait for the barber
		{
			std::unique_lock<std::mutex> lk(m);
			cv.wait(lk, [] {return processed; });
		}

	
		std::thread cust[num_threads];
		unsigned int n = std::thread::hardware_concurrency();
		
		//Launch a group of threads

		for (int i = 1; i <= num_threads; ++i) {
			cust[i]= std::thread(WaitingRoom,i);
			std::this_thread::sleep_for(std::chrono::seconds(3));
			
		}

		//std::thread t1(getHairCut,i);
		//std::thread t2(getHairCut);
		
		return 0;
	}

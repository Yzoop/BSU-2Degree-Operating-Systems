cmake_minimum_required(VERSION 3.16)
project(process_synchronization_clion_solution)

set(CMAKE_CXX_STANDARD 14)

add_executable(process_synchronization_clion_solution Receiver/Receiver.cpp Receiver/WinException.h Receiver/WinException.cpp Receiver/Event.cpp Receiver/Event.h Receiver/Semaphore.h Receiver/Process.h Receiver/Mutex.h Receiver/globals.h Receiver/Semaphore.cpp Receiver/Process.cpp Receiver/Mutex.cpp Sender/Sender.cpp)
#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>

using namespace std::chrono_literals;
 
int main()
{
    int i = 0;
    while (i < 20) {
        if(_kbhit())
        {
            break;
        }

        // print out commands
        std::cout << "Hello waiter\n" << std::flush;


        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_until(std::chrono::system_clock::now() + 0.5s);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        std::cout << "Waited " << elapsed.count() << " ms\n";
        i++;
    }

}

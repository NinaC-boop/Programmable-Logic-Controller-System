//GalilTest
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <chrono>
#include <thread>



#include <stdlib.h>
#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"



using namespace std;
using namespace std::chrono_literals;


GCon g = 0; //var used to refer to a unique connection



            //check return code from most gclib calls
void check(GReturn rc)
{
    if (rc != G_NO_ERROR)
    {
        cout << "ERROR: %d" << rc << endl;
        if (g)
            GClose(g);
    }
}



void Pulse();



int main()
{
    int PMHeartbeatCount = 0;
    double SetSteering = 0.0, SetSpeedF = 0.73, SetSpeedR = 0.73;
    char buf[1024]; //traffic buffer
    char Command[128] = "";
    string* Response;
    UINT64 Frequency = 5, Counter = 0;




    check(GOpen("192.168.1.120 -d", &g));



    //Start: Tests: Do these checks to make sure Galil communication is working
    // Checking Galil version 
    check(GVersion(buf, sizeof(buf)));
    Response = new string(buf);
    cout << "version: " << Response->c_str() << endl;



    // Checking RIO Information
    check(GInfo(g, buf, sizeof(buf)));
    Response = new string(buf);
    cout << "info: " << Response->c_str() << endl; //Print the connection info



    // Checking the time stamp
    check(GCommand(g, "MG TIME;", buf, sizeof(buf), 0)); //Send MG TIME. Because response is ASCII, don't care about bytes read.
    Response = new string(buf);
    cout << "response: " << Response->c_str() << endl; //Print the response


    int DigOut1 = 0b01110111;
    int DigOut2 = 0;


    float AnaOut1 = 0;
    sprintf_s(Command, "AO 7,%f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);

    AnaOut1 = 1;
    sprintf_s(Command, "AO 7,%f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);

    sprintf_s(Command, "WE 0;");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);

    //int count;
    int q = 0;
    *Response = "0";
    int new_response = 0;
    int old_response = 0;
    auto start = std::chrono::high_resolution_clock::now();
    int remainder;
    while (1) {
        if (_kbhit()) {
            break;
        }

        sprintf_s(Command, "QE %d;", q);
        // printf("%s\n", Command);
        GCommand(g, Command, buf, sizeof(buf), 0);
        Response = new string(buf);
        //count = stoi(*Response) / 4;


        new_response = stoi(*Response);
        if (old_response != new_response) {



            cout << "quadrature count: " << new_response << endl;
            //cout << "count: " << count << endl;
            remainder = new_response % 256;
            cout << remainder << endl;

            sprintf_s(Command, "OP %d;", remainder);
            printf("%s\n", Command);
            GCommand(g, Command, buf, sizeof(buf), 0);


            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << "Waited " << elapsed.count() << " ms\n";
            if (elapsed.count() >= 1000) {
                break;
            }
            auto start = std::chrono::high_resolution_clock::now();

            old_response = stoi(*Response);
        }
    }







    AnaOut1 = 0;
    sprintf_s(Command, "AO 7,%f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);


    if (g)
        GClose(g); //Don't forget to close!




    return G_NO_ERROR;
}



void Pulse()
{
    char PulseBuf[128];
    GCommand(g, "SB8", PulseBuf, sizeof(PulseBuf), 0);
    Sleep(500);
    GCommand(g, "CB8", PulseBuf, sizeof(PulseBuf), 0);
    Sleep(500);
}

/*
DigOut1 = 1;
int shiftR = 0;

int i = 0;
while (i < 15) {
    if (_kbhit())
    {
        break;
    }

    sprintf_s(Command, "OP %d;", DigOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);

    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_until(std::chrono::system_clock::now() + 0.5s);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Waited " << elapsed.count() << " ms\n";

    if (shiftR == 1) {
        DigOut1 >>= 1;
    }
    else {
        DigOut1 <<= 1;
    }


    if (DigOut1 == 0b10000000) {
        shiftR = 1;
    }
    else if (DigOut1 == 1) {
        shiftR = 0;
    }

    i++;
}

*/


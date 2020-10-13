#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <cstring>
#include <chrono>
#include <thread>


#include <stdlib.h>
#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"
#include "EmbeddedFunctions.h"
#include "Galil.h"



# define K_P 0
# define K_I 1
# define K_D 2



using namespace std;
using namespace std::chrono_literals;




int main(void) {

    /*


1. Write the value 45508 (0xB1C4) to the digital outputs

2. Write the value 113 (0x71) to the digital high byte

3. Write the value 1 to the 1st bit of digital output

4. Write the value 0 to the 13th bit of digital output

5. Read and print out the value of the 3rd bit of digital inputs

    */
    Galil asd;
    asd.DigitalOutput(0xB1C4);
    Sleep(1000);
    asd.DigitalByteOutput(1, 0x71);
    Sleep(1000);
    asd.DigitalBitOutput(1, 0);
    Sleep(1000);
    asd.DigitalBitOutput(0, 12);
    Sleep(1000);
    cout << int(asd.DigitalBitInput(2)) << endl;


    /*
    *
    *
    asd.CheckSuccessfulWrite();
    cout << asd;
    asd.CheckSuccessfulWrite();

    uint16_t value = 666;
    asd.DigitalOutput(value);
    asd.CheckSuccessfulWrite();

    asd.DigitalByteOutput(1, 22);
    Sleep(1000);
    asd.DigitalByteOutput(0, 103);
    Sleep(1000);
    asd.CheckSuccessfulWrite();

    // blink 7
    asd.DigitalBitOutput(0, 0);
    Sleep(1000);
    asd.DigitalBitOutput(1, 0);
    Sleep(1000);
    asd.DigitalBitOutput(0, 0);
    Sleep(1000);
    asd.DigitalBitOutput(1, 0);
    Sleep(1000);
    asd.CheckSuccessfulWrite();




    value = 666;
    asd.DigitalOutput(value); // 154, 2
    asd.CheckSuccessfulWrite();


    asd.DigitalInput();
    asd.DigitalByteInput(0);
    asd.DigitalByteInput(1);
    asd.DigitalBitInput(1);
    asd.CheckSuccessfulWrite();




    asd.WriteEncoder();

    asd.AnalogInputRange(7, 3);
    asd.AnalogOutput(7, 1);
    Sleep(1000);
    asd.AnalogOutput(7, 2);
    Sleep(1000);
    asd.AnalogOutput(7, 0);
    Sleep(2000);


    asd.setSetPoint(10);
    asd.setKd(0.7);
    asd.setKi(0.7);
    asd.setKp(1);



    // Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
    // The loop will run using the PID values specified in the data of this object, and has an
    // automatic timeout of 10s. You do not need to implement this function, it is defined in
    // GalilControl.lib. debug = 1 for debugging (it will print some values).
    cout << "Position Control" << endl;
    asd.PositionControl(1);
    Sleep(1000);

    // same as above. Setpoint interpreted as counts per second
    cout << "Speed Control" << endl;
    asd.SpeedControl(1);
    Sleep(1000);



    asd.WriteEncoder();
    asd.AnalogOutput(7, 3);

    */

    /*
        int new_response = 0;
        int old_response = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while (1) {
            if (_kbhit()) {
                break;
            }

            // reads encoder
            new_response = asd.ReadEncoder();

            if (old_response != new_response) {



                cout << "quadrature count: " << new_response << endl;


                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;
                std::cout << "Waited " << elapsed.count() << " ms\n";

                auto start = std::chrono::high_resolution_clock::now();

                old_response = new_response;
            }
        }

        asd.WriteEncoder();
        asd.AnalogOutput(7, 0);
        new_response = asd.ReadEncoder();
        cout << "quadrature count: " << new_response << endl;
    */










    /*

        // We know the, low bytes of the Galil is connected to a DAC, then a DVM
        // bit pattern --> DigitalOutput --> low byte LEDs --> DAC --> DVM
        //                                                         --> AnalogInput --> voltage
        // Use the equation of a line to find the relationship between bits & voltage: y = mx + b
        // Input is inverted with output, so we want 0 bits --> 5V and 255 bits --> -5
        // Digital range: 0 - 255 bits (1 byte)
        // Analog range: -5V - 5V
        // m = 5 - (-5) / (0 - 255) = - 10/255
        // voltage = -(bit_pattern / 255.0 * 10) + 5;

        // Send 0xAA to the DAC
        sprintf_s(Command, "OP %d;", 0xAA); // 0b10101010 or 170
        GCommand(g, Command, buf, sizeof(buf), nullptr);
        Console::WriteLine("Bit pattern: 0xAA");

        // Get the value from the DVM (on channel 0)
        sprintf_s(Command, "MG @AN[%d];", 0);
        GCommand(g, Command, buf, sizeof(buf), nullptr);
        std::string s = buf; // convert buf to a string
        float actualVoltage = atof(buf); // convert string to float

        // Convert the value to the expected voltage
        int value = 0xAA;
        float expectedVoltage = -(value / 255.0 * 10) + 5;

        // Print the results
        // Also look at the Camera to see the voltage measured by the DVM (lower digital display)
        Console::WriteLine("Actual Voltage: " + actualVoltage + "V");
        Console::WriteLine("Expected Voltage: " + expectedVoltage + "V");



    */

    asd.AnalogOutput(7, 0);


    return G_NO_ERROR;
}



/*


GCon g = 0; //var used to refer to a unique connection



            //check return code from most gclib calls
void storeError(GReturn rc)
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




    storeError(GOpen("192.168.1.120 -d", &g));



    //Start: Tests: Do these checks to make sure Galil communication is working
    // Checking Galil version
    storeError(GVersion(buf, sizeof(buf)));
    Response = new string(buf);
    cout << "version: " << Response->c_str() << endl;



    // Checking RIO Information
    storeError(GInfo(g, buf, sizeof(buf)));
    Response = new string(buf);
    cout << "info: " << Response->c_str() << endl; //Print the connection info



    // Checking the time stamp
    storeError(GCommand(g, "MG TIME;", buf, sizeof(buf), 0)); //Send MG TIME. Because response is ASCII, don't care about bytes read.
    Response = new string(buf);
    cout << "response: " << Response->c_str() << endl; //Print the response


    int DigOut1 = 0b01110111;
    int DigOut2 = 0;


    float AnaOut1 = 0;
    sprintf_s(Command, "AO 7,%f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), nullptr);

    AnaOut1 = 1;
    sprintf_s(Command, "AO 7,%f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), nullptr);

    sprintf_s(Command, "WE 0;");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), nullptr);

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
        GCommand(g, Command, buf, sizeof(buf), nullptr);
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
            GCommand(g, Command, buf, sizeof(buf), nullptr);


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
    GCommand(g, Command, buf, sizeof(buf), nullptr);


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
    GCommand(g, Command, buf, sizeof(buf), nullptr);

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
     j   shiftR = 1;
    }
    else if (DigOut1 == 1) {
        shiftR = 0;
    }

    i++;
}


*/
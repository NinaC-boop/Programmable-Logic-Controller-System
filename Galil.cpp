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




GCon g = 0;


/*
* #########################      HELPER FUNCTIONS      ##############################
*/

// Stores error according to GReturn code and ReadBuffer from given command
void storeError(GReturn rc, char(&buf)[1024])
{

    switch (rc) {
    case G_NO_ERROR: //!< Return value if function succeeded.
        strcpy_s(buf, G_NO_ERROR_S);
        break;
    case G_GCLIB_ERROR: //!< General library error. Indicates internal API caught an unexpected error. Contact Galil support if this error is returned, softwaresupport@galil.com.
        strcpy_s(buf, G_GCLIB_ERROR_S);
        break;
    case G_GCLIB_UTILITY_ERROR: //!< An invalid request value was specified to GUtility.
        strcpy_s(buf, G_GCLIB_UTILITY_ERROR_S);
        break;
    case G_GCLIB_UTILITY_IP_TAKEN: //!< The IP cannot be assigned because ping returned a reply.
        strcpy_s(buf, G_GCLIB_UTILITY_IP_TAKEN_S);
        break;
    case G_GCLIB_NON_BLOCKING_READ_EMPTY: //!< GMessage, GInterrupt, and GRecord can be called with a zero timeout. If there wasn't data waiting in memory, this error is returned.
        strcpy_s(buf, G_GCLIB_NON_BLOCKING_READ_EMPTY_S);
        break;
    case G_TIMEOUT: //!< Operation timed out. Timeout is set by the --timeout option in GOpen() and can be overriden by GSetting().
        strcpy_s(buf, G_TIMEOUT_S);
        break;
    case G_OPEN_ERROR: //!< Device could not be opened. E.G. Serial port or PCI device already open.
        strcpy_s(buf, G_OPEN_ERROR_S);
        break;
    case G_READ_ERROR: //!< Device read failed. E.G. Socket was closed by remote host. See @ref G_UTIL_GCAPS_KEEPALIVE.
        strcpy_s(buf, G_READ_ERROR_S);
        break;
    case G_WRITE_ERROR: //!< Device write failed. E.G. Socket was closed by remote host. See @ref G_UTIL_GCAPS_KEEPALIVE.
        strcpy_s(buf, G_WRITE_ERROR_S);
        break;
    case G_INVALID_PREPROCESSOR_OPTIONS: //!< GProgramDownload was called with a bad preprocessor directive.
        strcpy_s(buf, G_INVALID_PREPROCESSOR_OPTIONS_S);
        break;
    case G_COMMAND_CALLED_WITH_ILLEGAL_COMMAND: //!< GCommand() was called with an illegal command, e.g. ED, DL or QD.
        strcpy_s(buf, G_COMMAND_CALLED_WITH_ILLEGAL_COMMAND_S);
        break;
    case G_DATA_RECORD_ERROR: //!< Data record error, e.g. DR attempted on serial connection.
        strcpy_s(buf, G_DATA_RECORD_ERROR_S);
        break;
    case G_UNSUPPORTED_FUNCTION: //!< Function cannot be called on this bus. E.G. GInterrupt() on serial.
        strcpy_s(buf, G_UNSUPPORTED_FUNCTION_S);
        break;
    case G_FIRMWARE_LOAD_NOT_SUPPORTED: //!< Firmware is not supported on this bus, e.g. Ethernet for the DMC-21x3 series.
        strcpy_s(buf, G_FIRMWARE_LOAD_NOT_SUPPORTED_S);
        break;
    case G_ARRAY_NOT_DIMENSIONED: //!< Array operation was called on an array that was not in the controller's array table, see LA command.
        strcpy_s(buf, G_ARRAY_NOT_DIMENSIONED_S);
        break;
    case G_ILLEGAL_DATA_IN_PROGRAM: //!< Data to download not valid, e.g. \ in data.
        strcpy_s(buf, G_ILLEGAL_DATA_IN_PROGRAM_S);
        break;
    case G_UNABLE_TO_COMPRESS_PROGRAM_TO_FIT: //!< Program preprocessor could not compress the program within the user's constraints.
        strcpy_s(buf, G_UNABLE_TO_COMPRESS_PROGRAM_TO_FIT_S);
        break;
    case G_BAD_RESPONSE_QUESTION_MARK: //!< Operation received a ?, indicating controller has a TC error.
        strcpy_s(buf, G_BAD_RESPONSE_QUESTION_MARK_S);
        break;
    case G_BAD_VALUE_RANGE: //!< Bad value or range, e.g. GCon *g* variable passed to function was bad.
        strcpy_s(buf, G_BAD_VALUE_RANGE_S);
        break;
    case G_BAD_FULL_MEMORY: //!< Not enough memory for an operation, e.g. all connections allowed for a process already taken.
        strcpy_s(buf, G_BAD_FULL_MEMORY_S);
        break;
    case G_BAD_LOST_DATA: //!< Lost data, e.g. GCommand() response buffer was too small for the controller's response.
        strcpy_s(buf, G_BAD_LOST_DATA_S);
        break;
    case G_BAD_FILE: //!< Bad file path, bad file contents, or bad write.
        strcpy_s(buf, G_BAD_FILE_S);
        break;
    case G_BAD_ADDRESS: //!< Bad address
        strcpy_s(buf, G_BAD_ADDRESS_S);
        break;
    case G_BAD_FIRMWARE_LOAD: //!< Bad firmware upgrade
        strcpy_s(buf, G_BAD_FIRMWARE_LOAD_S);
        break;
    case G_GCAPS_OPEN_ERROR: //!< gcaps connection couldn't open. Server is not running or is not reachable.
        strcpy_s(buf, G_GCAPS_OPEN_ERROR_S);
        break;
    case G_GCAPS_SUBSCRIPTION_ERROR: //!< GMessage(), GRecord(), GInterrupt() called on a connection without --subscribe switch.
        strcpy_s(buf, G_GCAPS_SUBSCRIPTION_ERROR_S);
        break;
    }
    strcat_s(buf, "\n");
}


// Returns error value of an address connection, after initialising g with the open Galil
GReturn connectAddress(EmbeddedFunctions* Functions, GCon& g) {
    cout << "Enter the exact address of Galil (Press Enter for Default 192.168.1.120): ";
    char ip[100];
    cin.get(ip, 100);
    if (strcmp(ip, "") != 0) {
        strcat_s(ip, " -d");
        cout << "using " << ip << endl;
        return Functions->GOpen(ip, &g);
    }
    else {
        strcat_s(ip, "192.168.1.120 -d");
        cout << "using " << ip << endl;
        return Functions->GOpen("192.168.1.120 -d", &g);
    }
}




/*
* ########################              GALIL FUNCTIONS            ###########################
*/

// Default constructor. Initialize variables, open Galil connection and allocate memory. NOT AUTOMARKED
Galil::Galil() {
    char error[1024] = { '0' };
    Functions = new EmbeddedFunctions;      // Pointer to EmbeddedFunctions, through which all Galil Function calls will be made
    storeError(connectAddress(Functions, g), ReadBuffer);// Connection handle for the Galil, passed through most Galil function calls // Buffer to restore responses from the Galil
    for (int i = 0; i < 3; i++) {
        ControlParameters[i] = 0;           // Contains the controller gain values: K_p, K_i, K_d in that order 
    }
    setPoint = 0;					        // Control Setpoint
}





// Constructor with EmbeddedFunction initialization
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
    Functions = Funcs;                      // Pointer to EmbeddedFunctions, through which all Galil Function calls will be made

    char ip[100];
    strcpy_s(ip, address);
    strcat_s(ip, " -d");
    storeError(Functions->GOpen(ip, &g), ReadBuffer);// Connection handle for the Galil, passed through most Galil function calls // Buffer to restore responses from the Galil

    for (int i = 0; i < 3; i++) {
        ControlParameters[i] = 0;           // Contains the controller gain values: K_p, K_i, K_d in that order 
    }
    setPoint = 0;					        // Control Setpoint
}




// Default destructor. Deallocate memory and close Galil connection. NOT AUTOMARKED
Galil::~Galil() {
    delete Functions;
    Functions = NULL;
    if (g)
        storeError(GClose(g), ReadBuffer);
}





// DIGITAL OUTPUTS
// Write to all 16 bits of digital output, 1 command to the Galil
// low byte is lowest bits 
void Galil::DigitalOutput(uint16_t value) {
    char buf[1024];
    char Command[128] = "";

    int DigOut1 = 0b1111'1111;
    int DigOut2 = DigOut1 << 8;
    DigOut1 &= value;
    DigOut2 &= value;
    DigOut2 >>= 8;
    sprintf_s(Command, "OP %d, %d;", DigOut1, DigOut2);
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
}




// Write to one byte, either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
    char buf[1024];
    char Command[128] = "";

    int DigOut = int(value);

    if (bank) {
        sprintf_s(Command, "OP , %d;", DigOut); // bank 1
        printf("%s\n", Command);
        storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    }
    else {
        sprintf_s(Command, "OP %d;", DigOut); // bank 0
        printf("%s\n", Command);
        storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    }
}



// bit is oon or off and bit = 0-15 (the banks) "set bit and clear bit" SB CB
// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
    char buf[1024];
    char Command[128] = "";

    if (val == 1) {
        sprintf_s(Command, "SB %d;", bit); // bank 1
        printf("%s\n", Command);
        storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    }
    else {
        sprintf_s(Command, "CB %d;", bit); // bank 1
        printf("%s\n", Command);
        storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    }
}

// DIGITAL INPUTS

// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
// Stores two errors in two lines in ReadBuffer.
uint16_t Galil::DigitalInput() {
    uint16_t data = 0;

    data |= this->DigitalByteInput(0);
    data |= (this->DigitalByteInput(1) << 8);
    printf("Digital input: %d\n", data);


    return data;
}


// Read either high or low byte, as specified by user in 'bank' // 0 = low, 1 = high
uint8_t Galil::DigitalByteInput(bool bank) {
    uint8_t data = 0;
    uint8_t set = 0;
    char Command[128] = "";

    if (bank) {
        for (int i = 8; i < 16; i++) {
            set = this->DigitalBitInput(i);
            data |= (set << i - 8);
        }
    }
    else {
        for (int i = 0; i < 8; i++) {
            set = this->DigitalBitInput(i);
            data |= (set << i);
        }
    }

    printf("Bank: %d: %d\n", int(bank), data);

    return data;
}


bool Galil::DigitalBitInput(uint8_t bit) {	// Read single bit from current digital inputs. Above functions may use this function
    char buf[1024];
    char Command[128] = "";
    bool data;

//    if (bit < 8) {
    sprintf_s(Command, "MG @IN[%d];", bit);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    data = atoi(buf);
    data = 1 ^ data;
//    }
//    else {
//        sprintf_s(Command, "MG @OUT[%d];", bit);
//        storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
//        data = atoi(buf);
//    }

    //    printf("|%d|", data);

    return data;
}

bool Galil::CheckSuccessfulWrite() {	// Check the string response from the Galil to check that the last command executed correctly. 1 = succesful. NOT AUTOMARKED
    bool successful = 0;
    if (strcmp(ReadBuffer, G_NO_ERROR_S) == 0) {
        successful = 1;
    }
    printf("%s", ReadBuffer);

    return successful;
}






// ANALOG FUNCTIONS
// Read Analog channel and return voltage	
float Galil::AnalogInput(uint8_t channel) {
    char buf[1024];
    char Command[128] = "";

    sprintf_s(Command, "MG @AN[%d];", channel);
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
    return atoi(buf);
}

void Galil::AnalogOutput(uint8_t channel, double voltage) {		// Write to any channel of the Galil, send voltages as // 2 decimal place in the command string
    char buf[1024];
    char Command[128] = "";

    sprintf_s(Command, "AO %d, %0.2f;", channel, voltage);
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
}


void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {// Configure the range of the input channel with // the desired range code
    char buf[1024];
    char Command[128] = "";

    sprintf_s(Command, "DQ %d, %d;", channel, range);
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
}





// ENCODER / CONTROL FUNCTIONS
// Manually Set the encoder value to zero
void Galil::WriteEncoder() {
    char buf[1024];
    char Command[128] = "";

    sprintf_s(Command, "WE 0, 0;");
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);
}


int Galil::ReadEncoder() {
    // Read from Encoder

    char buf[1024];
    char Command[128] = "";

    sprintf_s(Command, "QE 0;");
    printf("%s\n", Command);
    storeError(GCommand(g, Command, buf, sizeof(buf), nullptr), ReadBuffer);

    return atoi(buf);
}
// Set the desired setpoint for control loops, counts or counts/sec
void Galil::setSetPoint(int s) {
    this->setPoint = s;
}	


// double ControlParameters[3] contains the controller gain values: K_p, K_i, K_d in that order 
// Set the proportional gain of the controller used in controlLoop()
void Galil::setKp(double gain) {
    this->ControlParameters[K_P] = gain;
}

// Set the integral gain of the controller used in controlLoop()
void Galil::setKi(double gain) {
    this->ControlParameters[K_I] = gain;
}	

// Set the derivative gain of the controller used in controlLoop()
void Galil::setKd(double gain) {
    this->ControlParameters[K_D] = gain;
}	

// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
// The loop will run using the PID values specified in the data of this object, and has an 
// automatic timeout of 10s. You do not need to implement this function, it is defined in
// GalilControl.lib. debug = 1 for debugging (it will print some values).
void Galil::PositionControl(bool debug) {}	

// same as above. Setpoint interpreted as counts per second
void Galil::SpeedControl(bool debug) {}	





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
















// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {
    string* Version;
    string* Info;
    char buf[1024];
    char error[1024];

    // Checking Galil version 
    storeError(GVersion(buf, sizeof(buf)), galil.ReadBuffer);
    Version = new string(buf);

    strcpy_s(error, galil.ReadBuffer);

    // Checking RIO Information
    storeError(GInfo(galil.g, buf, sizeof(buf)), galil.ReadBuffer);
    Info = new string(buf);

    strcat_s(error, "\n");
    strcat_s(error, galil.ReadBuffer);
    strcpy_s(galil.ReadBuffer, error);

    return output << "info: " << Info->c_str() << endl << endl << "version: " << Version->c_str() << endl << endl;
}





int main(void) {

    /*    EmbeddedFunctions* f = new EmbeddedFunctions;
    f->GOpen("192.168.1.120 -d", &g);
    */


    Galil asd;
    asd.CheckSuccessfulWrite();
    cout << asd;

    uint16_t value = 666;
    asd.DigitalOutput(value);

    asd.DigitalByteOutput(1, 22);
    asd.DigitalByteOutput(0, 103);

    // blink 7
    asd.DigitalBitOutput(0, 0);
    asd.DigitalBitOutput(1, 0);
    asd.DigitalBitOutput(0, 0);
    asd.DigitalBitOutput(1, 0);





    value = 666;
    asd.DigitalOutput(value); // 154, 2



    asd.DigitalInput();
    asd.DigitalByteInput(0);
    asd.DigitalByteInput(1);
    asd.DigitalBitInput(1);


    asd.WriteEncoder();




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
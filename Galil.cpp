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



using namespace std;
using namespace std::chrono_literals;




GCon g = 0;


// Stores error according to GReturn code and ReadBuffer from given command
void storeError(GReturn rc, char *buf)
{
    switch (rc) {
        case G_NO_ERROR: //!< Return value if function succeeded.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_NO_ERROR_S);
            break;
        case G_GCLIB_ERROR: //!< General library error. Indicates internal API caught an unexpected error. Contact Galil support if this error is returned, softwaresupport@galil.com.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCLIB_ERROR_S);
            break;
        case G_GCLIB_UTILITY_ERROR: //!< An invalid request value was specified to GUtility.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCLIB_UTILITY_ERROR_S);
            break;
        case G_GCLIB_UTILITY_IP_TAKEN: //!< The IP cannot be assigned because ping returned a reply.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCLIB_UTILITY_IP_TAKEN_S);
            break;
        case G_GCLIB_NON_BLOCKING_READ_EMPTY: //!< GMessage, GInterrupt, and GRecord can be called with a zero timeout. If there wasn't data waiting in memory, this error is returned.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCLIB_NON_BLOCKING_READ_EMPTY_S);
            break;
        case G_TIMEOUT: //!< Operation timed out. Timeout is set by the --timeout option in GOpen() and can be overriden by GSetting().
            strcpy_s(buf, strlen(buf) * sizeof(char), G_TIMEOUT_S);
            break;
        case G_OPEN_ERROR: //!< Device could not be opened. E.G. Serial port or PCI device already open.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_OPEN_ERROR_S);
            break;
        case G_READ_ERROR: //!< Device read failed. E.G. Socket was closed by remote host. See @ref G_UTIL_GCAPS_KEEPALIVE.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_READ_ERROR_S);
            break;
        case G_WRITE_ERROR: //!< Device write failed. E.G. Socket was closed by remote host. See @ref G_UTIL_GCAPS_KEEPALIVE.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_WRITE_ERROR_S);
            break;
        case G_INVALID_PREPROCESSOR_OPTIONS: //!< GProgramDownload was called with a bad preprocessor directive.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_INVALID_PREPROCESSOR_OPTIONS_S);
            break;
        case G_COMMAND_CALLED_WITH_ILLEGAL_COMMAND: //!< GCommand() was called with an illegal command, e.g. ED, DL or QD.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_COMMAND_CALLED_WITH_ILLEGAL_COMMAND_S);
            break;
        case G_DATA_RECORD_ERROR: //!< Data record error, e.g. DR attempted on serial connection.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_DATA_RECORD_ERROR_S);
            break;
        case G_UNSUPPORTED_FUNCTION: //!< Function cannot be called on this bus. E.G. GInterrupt() on serial.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_UNSUPPORTED_FUNCTION_S);
            break;
        case G_FIRMWARE_LOAD_NOT_SUPPORTED: //!< Firmware is not supported on this bus, e.g. Ethernet for the DMC-21x3 series.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_FIRMWARE_LOAD_NOT_SUPPORTED_S);
            break;
        case G_ARRAY_NOT_DIMENSIONED: //!< Array operation was called on an array that was not in the controller's array table, see LA command.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_ARRAY_NOT_DIMENSIONED_S);
            break;
        case G_ILLEGAL_DATA_IN_PROGRAM: //!< Data to download not valid, e.g. \ in data.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_ILLEGAL_DATA_IN_PROGRAM_S);
            break;
        case G_UNABLE_TO_COMPRESS_PROGRAM_TO_FIT: //!< Program preprocessor could not compress the program within the user's constraints.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_UNABLE_TO_COMPRESS_PROGRAM_TO_FIT_S);
            break;
        case G_BAD_RESPONSE_QUESTION_MARK: //!< Operation received a ?, indicating controller has a TC error.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_RESPONSE_QUESTION_MARK_S);
            break;
        case G_BAD_VALUE_RANGE: //!< Bad value or range, e.g. GCon *g* variable passed to function was bad.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_VALUE_RANGE_S);
            break;
        case G_BAD_FULL_MEMORY: //!< Not enough memory for an operation, e.g. all connections allowed for a process already taken.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_FULL_MEMORY_S);
            break;
        case G_BAD_LOST_DATA: //!< Lost data, e.g. GCommand() response buffer was too small for the controller's response.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_LOST_DATA_S);
            break;
        case G_BAD_FILE: //!< Bad file path, bad file contents, or bad write.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_FILE_S);
            break;
        case G_BAD_ADDRESS: //!< Bad address
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_ADDRESS_S);
            break;
        case G_BAD_FIRMWARE_LOAD: //!< Bad firmware upgrade
            strcpy_s(buf, strlen(buf) * sizeof(char), G_BAD_FIRMWARE_LOAD_S);
            break;
        case G_GCAPS_OPEN_ERROR: //!< gcaps connection couldn't open. Server is not running or is not reachable.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCAPS_OPEN_ERROR_S);
            break;
        case G_GCAPS_SUBSCRIPTION_ERROR: //!< GMessage(), GRecord(), GInterrupt() called on a connection without --subscribe switch.
            strcpy_s(buf, strlen(buf) * sizeof(char), G_GCAPS_SUBSCRIPTION_ERROR_S);
            break;
    }
}




// Returns error value of an address connection, after initialising g with the open Galil
GReturn connectAddress(EmbeddedFunctions *Functions, GCon &g) {
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





// Default constructor. Initialize variables, open Galil connection and allocate memory. NOT AUTOMARKED
Galil::Galil() {
    Functions = new EmbeddedFunctions;      // Pointer to EmbeddedFunctions, through which all Galil Function calls will be made
    storeError(connectAddress(Functions, g), ReadBuffer);          // Connection handle for the Galil, passed through most Galil function calls
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
    Functions->GOpen(ip, &g);
    // Connection handle for the Galil, passed through most Galil function calls

    strcpy_s(ReadBuffer, "");			    // Buffer to restore responses from the Galil
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
        GClose(g);
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
    GCommand(g, Command, buf, sizeof(buf), 0);
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
        GCommand(g, Command, buf, sizeof(buf), 0);
    }
    else {
        sprintf_s(Command, "OP %d;", DigOut); // bank 0
        printf("%s\n", Command);
        GCommand(g, Command, buf, sizeof(buf), 0);
    }
}



// bit is oon or off and bit = 0-15 (the banks) "set bit and clear bit" SB CB
// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
    char buf[1024];
    char Command[128] = "";

    int DigOut = 1;
    DigOut <<= bit;

    if (val) {
        sprintf_s(Command, "OP , %d;", DigOut); // bank 1
        printf("%s\n", Command);
        GCommand(g, Command, buf, sizeof(buf), 0);
    }
    else {
        sprintf_s(Command, "OP %d;", DigOut); // bank 0
        printf("%s\n", Command);
        GCommand(g, Command, buf, sizeof(buf), 0);
    }
}

// DIGITAL INPUTS
uint16_t Galil::DigitalInput() {
    // Return the 16 bits of input data
    // Query the digital inputs of the GALIL, See Galil command library @IN

    char buf[1024];
    char Command[128] = "";

    uint16_t data = 0;
    sprintf_s(Command, "MG @IN[1];");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
    data = atoi(Command);
    data <<= 8;
    sprintf_s(Command, "MG @IN[0];");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
    data &= atoi(Command);
    printf("%d\n", int(data));
    return data;
}
                                        
uint8_t Galil::DigitalByteInput(bool bank) {	// Read either high or low byte, as specified by user in 'bank'
    uint8_t data = 0;
    char buf[1024];
    char Command[128] = "";


    sprintf_s(Command, "MG @IN[%d];", bank);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
    data = atoi(Command);
    printf("Bank %d: %d\n", bank, int(data));
    return data;           // 0 = low, 1 = high
}


bool Galil::DigitalBitInput(uint8_t bit) {	// Read single bit from current digital inputs. Above functions
    char buf[1024];
    char Command[128] = "";

    uint16_t data = 0;
    sprintf_s(Command, "MG @IN[1];");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
    data = atoi(Command);
    data <<= 8;
    sprintf_s(Command, "MG @IN[0];");
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
    data &= atoi(Command);
    printf("%d\n", int(data));
    return data;
}

bool Galil::CheckSuccessfulWrite() {	// Check the string response from the Galil to check that the last // command executed correctly. 1 = succesful. NOT AUTOMARKED


    return 0;
}




// ENCODER / CONTROL FUNCTIONS
// Manually Set the encoder value to zero
void Galil::WriteEncoder() {
    char buf[1024];
    char Command[128] = "";

    float AnaOut1 = 0;
    sprintf_s(Command, "AO 7, %0.2f;", AnaOut1);
    printf("%s\n", Command);
    GCommand(g, Command, buf, sizeof(buf), 0);
}


// ANALOG FUNCITONS
float Galil::AnalogInput(uint8_t channel) {
    // Read Analog channel and return voltage			
    return 0;
}
void Galil::AnalogOutput(uint8_t channel, double voltage){		// Write to any channel of the Galil, send voltages as 
}

                                                        // 2 decimal place in the command string
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {// Configure the range of the input channel with
}
                                                        // the desired range code




















// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {
    string* Version;
    string* Info;
    char buf[1024];

    // Checking Galil version 
    storeError(GVersion(buf, sizeof(buf)), galil.ReadBuffer);
    Version = new string(buf);

    // Checking RIO Information
    storeError(GInfo(galil.g, buf, sizeof(buf)), galil.ReadBuffer);
    Info = new string(buf);

    return output << "info: " << Info->c_str() << endl << endl << "version: " << Version->c_str() << endl << endl;
}





int main(void) {

    /*    EmbeddedFunctions* f = new EmbeddedFunctions;
    f->GOpen("192.168.1.120 -d", &g);
    */


    Galil asd;
    uint16_t value = 666;
    asd.DigitalOutput(value);
    Sleep(500);
    asd.DigitalByteOutput(1, 44);
    Sleep(500);
    asd.DigitalByteOutput(0, 103);
    Sleep(500);
    asd.DigitalBitOutput(1, 7);
    Sleep(500);

    asd.DigitalInput();
    Sleep(500);
    asd.DigitalByteInput(1);
    Sleep(500);
    asd.DigitalBitInput(1);
    Sleep(500);


    asd.WriteEncoder();
    Sleep(500);


    cout << asd;

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
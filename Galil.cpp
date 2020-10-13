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




/*
* #########################      HELPER FUNCTIONS      ##############################
*/

// Stores error according to GReturn code and buffer from given command
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


// Changes response code to default ":" for errors from GOpen, GClose and other non-exhaustive error-prone GFunctions
void checkErrorSuccessful(char(&buf)[1024]) {
    string success_error = string(buf);
    success_error.pop_back();
    if (strcmp(success_error.c_str(), G_NO_ERROR_S) == 0) {
        strcpy_s(buf, ":");
    }
    else {
        strcpy_s(buf, "error");
    }
}






/*
* ########################              GALIL FUNCTIONS            ###########################
*/


// Default constructor. Initialize variables, open Galil connection and allocate memory. NOT AUTOMARKED
Galil::Galil() {
    Functions = new EmbeddedFunctions;                      // Pointer to EmbeddedFunctions, through which all Galil Function calls will be made

    storeError(connectAddress(Functions, g), ReadBuffer);   // Connection handle for the Galil, passed through most Galil function calls // Buffer to restore responses from the Galil
    checkErrorSuccessful(ReadBuffer);

    for (int i = 0; i < 3; i++) {
        ControlParameters[i] = 0;                           // Contains the controller gain values: K_p, K_i, K_d in that order 
    }
    setPoint = 0;					                        // Control Setpoint
}


// Constructor with EmbeddedFunction initialization
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
    Functions = Funcs;                                      // Pointer to EmbeddedFunctions, through which all Galil Function calls will be made

    char ip[100];
    strcpy_s(ip, address);
    strcat_s(ip, " -d");
    storeError(Functions->GOpen(ip, &g), ReadBuffer);        // Connection handle for the Galil, passed through most Galil function calls // Buffer to restore responses from the Galil
    checkErrorSuccessful(ReadBuffer);

    for (int i = 0; i < 3; i++) {
        ControlParameters[i] = 0;                            // Contains the controller gain values: K_p, K_i, K_d in that order 
    }
    setPoint = 0;					                         // Control Setpoint
}


// Default destructor. Deallocate memory and close Galil connection. NOT AUTOMARKED
Galil::~Galil() {
    delete Functions;
    Functions = NULL;
    if (g) {
        storeError(GClose(g), ReadBuffer);
        checkErrorSuccessful(ReadBuffer);
    }
}







// ####################################### DIGITAL OUTPUTS #######################################
// Write to all 16 bits of digital output, 1 command to the Galil
// low byte is lowest bits 
void Galil::DigitalOutput(uint16_t value) {
    char Command[128] = "";

    int DigOut1 = 0b1111'1111;
    int DigOut2 = DigOut1 << 8;
    DigOut1 &= value;
    DigOut2 &= value;
    DigOut2 >>= 8;

    sprintf_s(Command, "OP %d, %d;", DigOut1, DigOut2);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}


// Write to one byte, either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
    char Command[128] = "";
    int DigOut = int(value);

    if (bank) {
        sprintf_s(Command, "OP , %d;", DigOut); // bank 1
        GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    }
    else {
        sprintf_s(Command, "OP %d;", DigOut); // bank 0
        GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    }
}


// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
    char Command[128] = "";

    if (val == 1) {
        sprintf_s(Command, "SB %d;", bit); // sets bit on
        GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    }
    else {
        sprintf_s(Command, "CB %d;", bit); // sets bit off
        GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    }
}







// ####################################### DIGITAL INPUTS #######################################
// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
// Stores two errors in two lines in ReadBuffer.
uint16_t Galil::DigitalInput() {
    uint16_t data = 0;

    data |= this->DigitalByteInput(0);
    data |= (this->DigitalByteInput(1) << 8);

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

    return data;
}


// Read single bit from current digital inputs. Above functions may use this function
bool Galil::DigitalBitInput(uint8_t bit) {	
    char Command[128] = "";
    bool data;

    sprintf_s(Command, "MG @IN[%d];", bit);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    data = atoi(ReadBuffer);
    data = 1 ^ data;

    return data;
}


// Check the string response from the Galil to check that the last command executed correctly. 1 = succesful. NOT AUTOMARKED
bool Galil::CheckSuccessfulWrite() {	
    bool successful = 0;

    if (string(ReadBuffer).back() == ':') {
        successful = 1;
    }

    return successful;
}









// ####################################### ANALOG FUNCTIONS #######################################
// Read Analog channel and return voltage	
float Galil::AnalogInput(uint8_t channel) {
    char Command[128] = "";

    sprintf_s(Command, "MG @AN[%d];", channel);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
    return atof(ReadBuffer);
}


// Write to any channel of the Galil, send voltages as 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage) {
    char Command[128] = "";

    sprintf_s(Command, "AO %d, %0.2f;", channel, voltage);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}


/*
Configure the range of the input channel with the desired range code
1    0 to + 5  VDC
2    0 to + 10 VDC
3  - 5 to + 5  VDC
4 - 10 to + 10 VDC Default
*/
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
    char Command[128] = "";

    sprintf_s(Command, "DQ %d, %d;", channel, range);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}








// ####################################### ENCODER / CONTROL FUNCTIONS #######################################
// Manually Set the encoder value to zero
void Galil::WriteEncoder() {
    char Command[128] = "";

    sprintf_s(Command, "WE 0, 0;");
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}


// Read from Encoder
int Galil::ReadEncoder() {
    char Command[128] = "";

    sprintf_s(Command, "QE 0;");
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

    return atoi(ReadBuffer);
}


// Set the desired setpoint for control loops, counts or counts/sec
void Galil::setSetPoint(int s) {
    char Command[128] = "";

    sprintf_s(Command, "PS %d;", s);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

    this->setPoint = s;
}


// double ControlParameters[3] contains the controller gain values: K_p, K_i, K_d in that order 
// Set the proportional gain of the controller used in controlLoop()
void Galil::setKp(double gain) {
    char Command[128] = "";

    sprintf_s(Command, "KP %f;", gain);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

    this->ControlParameters[K_P] = gain;
}


// Set the integral gain of the controller used in controlLoop()
void Galil::setKi(double gain) {
    char Command[128] = "";

    sprintf_s(Command, "KI %f;", gain);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

    this->ControlParameters[K_I] = gain;
}


// Set the derivative gain of the controller used in controlLoop()
void Galil::setKd(double gain) {
    char Command[128] = "";

    sprintf_s(Command, "KD %f;", gain);
    GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

    this->ControlParameters[K_D] = gain;
}







// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& (operator<<(std::ostream& output, Galil& galil)) {
    string* Version;
    string* Info;
    char Error1[1024] = "";
    char Error2[1024] = "";

    // Checking Galil version 
    storeError(GVersion(galil.ReadBuffer, sizeof(galil.ReadBuffer)), Error1);
    Version = new string(galil.ReadBuffer);

    // Checking RIO Information
    storeError(GInfo(galil.g, galil.ReadBuffer, sizeof(galil.ReadBuffer)), Error2);
    Info = new string(galil.ReadBuffer);

    checkErrorSuccessful(Error1);
    checkErrorSuccessful(Error2);

    if ((strcmp(Error1, ":") == 0) && (strcmp(Error2, ":") == 0)) {
        strcpy_s(galil.ReadBuffer, ":");
    }
    return output << "info: " << Info->c_str() << endl << endl << "version: " << Version->c_str() << endl << endl;
}






//GalilTest
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"

#define MAX_BUFF_SIZE 1024
#define CMD_SIZE 128

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Threading::Tasks;

// Var used to refer to a unique connection
GCon g = 0;

// Check return code from most gclib calls
void check(GReturn rc)
{
	if (rc != G_NO_ERROR)
	{
		Console::WriteLine("ERROR: {0}", rc);
		if (g)
			GClose(g);
		Environment::Exit(rc);
	}
}

int main(array<System::String^>^ args)
{

	// Tutorial 3 Question 1
	char buf[MAX_BUFF_SIZE];
	char Command[CMD_SIZE] = "";

	// Connect to the Galil - double check this is the right IP in Galil Tools
	check(GOpen("192.168.1.120 -d", &g));

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
	GCommand(g, Command, buf, sizeof(buf), 0);
	Console::WriteLine("Bit pattern: 0xAA");

	// Get the value from the DVM (on channel 0)
	sprintf_s(Command, "MG @AN[%d];", 0);
	GCommand(g, Command, buf, sizeof(buf), 0);
	std::string s = buf; // convert buf to a string
	float actualVoltage = atof(buf); // convert string to float

	// Convert the value to the expected voltage
	int value = 0xAA;
	float expectedVoltage = -(value / 255.0 * 10) + 5;

	// Print the results
	// Also look at the Camera to see the voltage measured by the DVM (lower digital display)
	Console::WriteLine("Actual Voltage: " + actualVoltage + "V");
	Console::WriteLine("Expected Voltage: " + expectedVoltage + "V");

	// Close the Galil
	if (g)
		GClose(g);

	// Wait until the user has pressed a key to terminate
	Console::WriteLine("Terminating!");
	Console::ReadKey();

	return G_NO_ERROR;
}

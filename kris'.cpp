#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <chrono>
#include <Windows.h>
#include <iomanip>
#include <bitset>

//#include "gclib.h"
//#include "gclibo.h"
//#include "gclib_errors.h"
//#include "gclib_record.h"
#include "EmbeddedFunctions.h"
#include "Galil.h"



Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
	if (Funcs->GOpen(address, &g) != 0) {
		std::cout << "Permission failed" << std::endl;
		std::cout << Functions->GOpen(address, &g) << std::endl;
	}

	this->Functions = Funcs;
}

Galil::~Galil() {}


void Galil::DigitalOutput(uint16_t value) {
	uint8_t lowvalues = value & 0xFF;
	uint8_t highvalues = value & 0xFF00;

	char Command[128];
	sprintf_s(Command, "OP %hhu, %hhu;", lowvalues, highvalues);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::DigitalByteOutput(bool bank, uint8_t value) {

	char Command[128];
	if (bank)
		sprintf_s(Command, "OP, %d;", value);
	else if (!bank)
		sprintf_s(Command, "OP %d;", value);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}

void Galil::DigitalBitOutput(bool val, uint8_t bit) {

	char Command[128];
	if (val)
		sprintf_s(Command, "SB %d;", bit);
	else if (!val)
		sprintf_s(Command, "CB %d;", bit);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}

uint16_t Galil::DigitalInput() {

	char Command[1028];
	int i = 0, DigitalInput = 0, Sum = 0, Power = 0, j = 15;
	while (i < 16) {
		sprintf_s(Command, "MG @IN[%d];", i);
		Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
		DigitalInput = atoi(ReadBuffer);
		Power = (2 ^ j) * DigitalInput;
		Sum = Power + Sum;
		i++;
		j--;
	}


	std::cout << std::bitset< 16 >(Sum) << ' ';

	std::cout << "\nDigital Input = " << Sum << std::endl;

	return Sum;

}

uint8_t Galil::DigitalByteInput(bool bank) {
	//uint8_t DigitalByteInput;

	//if (bank)
		//sprintf_s(Command, )
	//else if (!bank)

		//DigitalByteInput = Functions->GCommand(g, MG @IN[1], ReadBuffer, sizeof(ReadBuffer), 0);

	return 0;
}

bool Galil::DigitalBitInput(uint8_t bit) { return 0; }
bool Galil::CheckSuccessfulWrite() { return 0; }



float Galil::AnalogInput(uint8_t channel) {  //Read Analog channel and return voltage		 TESTED

	char Command[1024];
	sprintf_s(Command, "MG @AN[%d];", channel);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
	float voltage = std::atof(ReadBuffer);
	// std::cout << std::setprecision(8);
	std::cout << "voltage = " << std::setprecision(8) << voltage << std::endl;

	return voltage;
}



void Galil::AnalogOutput(uint8_t channel, double voltage) { //TESTED

	//std::string Command =  "AO " + std::to_string(channel) + ", " + std::to_string(voltage) + ";";
	char Command[1024];
	sprintf_s(Command, "AO %d, %f;", channel, voltage);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

	// AO 7, 5 makes it spin A0 7, 5 makes it spin,
	// AO 7, 0 makes itAO 7, 1

};

void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {

	//Functions->GCommand(g, "AQ " + channel + ", " + range + ";", ReadBuffer, sizeof(ReadBuffer), nullptr);
	char Command[1024];
	sprintf_s(Command, "AQ %d, %d;", channel, range);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

};

void Galil::WriteEncoder() { //TESTED
	char Command[1024];
	sprintf_s(Command, "WE 6, 0");
	//set channel 0 and channel 1 to the specified values 
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

}
int Galil::ReadEncoder() { //TESTed

	char Command[1024];
	sprintf_s(Command, "QE 0");
	//char *Encoder_Reading; 
	// "QE" when typed into Galil returns the current position of the encoders inputs. Can specify QE 0 or QE 1 for specific channels
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
	//std::cout << Encoder_Reading << std::endl;
	//std::stoi(Encoder_Reading); 
	int reading = std::atoi(ReadBuffer);

	std::cout << reading << std::endl;
	return reading;

};

void Galil::setSetPoint(int s) {}
void Galil::setKp(double gain) {

	char Command[1028];
	sprintf_s(Command, "Kp, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}

void Galil::setKi(double gain) {
	char Command[1028];
	sprintf_s(Command, "Ki, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}

void Galil::setKd(double gain) {
	char Command[1028];
	sprintf_s(Command, "Kd, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);

}

void PositionControl(bool debug);	// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
											// The loop will run using the PID values specified in the data of this object, and has an 
											// automatic timeout of 10s. You do not need to implement this function, it is defined in
											// GalilControl.lib. debug = 1 for debugging (it will print some values).
void SpeedControl(bool debug);		// same as above. Setpoint interpreted as counts per second



int main(void) {


	EmbeddedFunctions* embf = new EmbeddedFunctions;
	GCStringIn addr = "192.168.1.120 -d";
	//Galil(embf, addr);

	Galil* galil = new Galil(embf, addr);
	//galil->DigitalOutput(255)
	GCon g = 0;
	GOpen("192.168.1.120 -d", &g);

	//demo test code here??:
	//galil->AnalogInputRange(7, 2); 
	galil->DigitalOutput(0b1111111111111111);
	//galil->AnalogOutput(7, 5);


	//galil->AnalogInput(7);
	galil->DigitalInput();

	if (g)
		GClose(g);

	return 0;
}
//#include "gclib.h"
//#include "gclibo.h"
//#include "gclib_errors.h"
//#include "gclib_record.h"
#include "EmbeddedFunctions.h"
#include "Galil.h"


Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) {
	if (Funcs->GOpen(address, &g) != 0) {
		std::cout << "Permission failed" << std::endl;
		std::cout << Functions->GOpen(address, &g) << std::endl;
	}
	this->Functions = Funcs;
}
Galil::~Galil() {}

void Galil::DigitalOutput(uint16_t value) {
	uint8_t lowvalues = value & 0xFF;
	uint8_t highvalues = value & 0xFF00;
	char Command[128];
	sprintf_s(Command, "OP %hhu, %hhu;", lowvalues, highvalues);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	char Command[128];
	if (bank)
		sprintf_s(Command, "OP, %d;", value);
	else if (!bank)
		sprintf_s(Command, "OP %d;", value);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	char Command[128];
	if (val)
		sprintf_s(Command, "SB %d;", bit);
	else if (!val)
		sprintf_s(Command, "CB %d;", bit);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
uint16_t Galil::DigitalInput() {
	char Command[1028];
	int i = 0, DigitalInput = 0, Sum = 0, Power = 0, j = 15;
	while (i < 16) {
		sprintf_s(Command, "MG @IN[%d];", i);
		Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
		DigitalInput = atoi(ReadBuffer);
		Power = (2 ^ j) * DigitalInput;
		Sum = Power + Sum;
		i++;
		j--;
	}

	std::cout << std::bitset< 16 >(Sum) << ' ';
	std::cout << "\nDigital Input = " << Sum << std::endl;
	return Sum;
}
uint8_t Galil::DigitalByteInput(bool bank) {
	//uint8_t DigitalByteInput;
	//if (bank)
		//sprintf_s(Command, )
	//else if (!bank)
		//DigitalByteInput = Functions->GCommand(g, MG @IN[1], ReadBuffer, sizeof(ReadBuffer), 0);
	return 0;
}
bool Galil::DigitalBitInput(uint8_t bit) { return 0; }
bool Galil::CheckSuccessfulWrite() { return 0; }


float Galil::AnalogInput(uint8_t channel) {  //Read Analog channel and return voltage		 TESTED
	char Command[1024];
	sprintf_s(Command, "MG @AN[%d];", channel);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
	float voltage = std::atof(ReadBuffer);
	// std::cout << std::setprecision(8);
	std::cout << "voltage = " << std::setprecision(8) << voltage << std::endl;
	return voltage;
}


void Galil::AnalogOutput(uint8_t channel, double voltage) { //TESTED
	//std::string Command =  "AO " + std::to_string(channel) + ", " + std::to_string(voltage) + ";";
	char Command[1024];
	sprintf_s(Command, "AO %d, %f;", channel, voltage);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
	// AO 7, 5 makes it spin A0 7, 5 makes it spin,
	// AO 7, 0 makes itAO 7, 1
};
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	//Functions->GCommand(g, "AQ " + channel + ", " + range + ";", ReadBuffer, sizeof(ReadBuffer), nullptr);
	char Command[1024];
	sprintf_s(Command, "AQ %d, %d;", channel, range);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
};
void Galil::WriteEncoder() { //TESTED
	char Command[1024];
	sprintf_s(Command, "WE 6, 0");
	//set channel 0 and channel 1 to the specified values 
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}
int Galil::ReadEncoder() { //TESTed
	char Command[1024];
	sprintf_s(Command, "QE 0");
	//char *Encoder_Reading; 
	// "QE" when typed into Galil returns the current position of the encoders inputs. Can specify QE 0 or QE 1 for specific channels
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
	//std::cout << Encoder_Reading << std::endl;
	//std::stoi(Encoder_Reading); 
	int reading = std::atoi(ReadBuffer);
	std::cout << reading << std::endl;
	return reading;
};
void Galil::setSetPoint(int s) {}
void Galil::setKp(double gain) {
	char Command[1028];
	sprintf_s(Command, "Kp, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}
void Galil::setKi(double gain) {
	char Command[1028];
	sprintf_s(Command, "Ki, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}
void Galil::setKd(double gain) {
	char Command[1028];
	sprintf_s(Command, "Kd, %f, %f", gain, gain);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), nullptr);
}
void PositionControl(bool debug);	// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
											// The loop will run using the PID values specified in the data of this object, and has an 
											// automatic timeout of 10s. You do not need to implement this function, it is defined in
											// GalilControl.lib. debug = 1 for debugging (it will print some values).
void SpeedControl(bool debug);		// same as above. Setpoint interpreted as counts per second


int main(void) {

	EmbeddedFunctions* embf = new EmbeddedFunctions;
	GCStringIn addr = "192.168.1.120 -d";
	//Galil(embf, addr);
	Galil* galil = new Galil(embf, addr);
	//galil->DigitalOutput(255)
	GCon g = 0;
	GOpen("192.168.1.120 -d", &g);
	//demo test code here??:
	//galil->AnalogInputRange(7, 2); 
	galil->DigitalOutput(0b1111111111111111);
	//galil->AnalogOutput(7, 5);

	//galil->AnalogInput(7);
	galil->DigitalInput();
	if (g)
		GClose(g);
	return 0;
}
```
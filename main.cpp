#define _USE_MATH_DEFINES

#include <iostream>
#include <tuple>
#include <cmath>
#include <math.h>

using namespace std;

// Creates a complex number member using radians and units
class ComplexNumber {
public:
	ComplexNumber() {
		re = 0;
		im = 0;
	}

	ComplexNumber(float r, float i) {
		re = r;
		im = i;
	}

	tuple<float, float> getComplex() {
		return make_tuple(re, im);
	}

	void setComplex(float r, float i) {
		re = r;
		im = i;
	}

	float modulus() {
		return sqrt(pow(re, 2) + pow(re, 2));
	}
	
	float phase() {
		if (re > 0) {
			return atan(im / re);
		}
		if (re < 0) {
			if (im >= 0) {
				return atan(im / re) + float(M_PI);
			}
			else {
				return atan(im / re) - float(M_PI);
			}
		}
		if (re == 0) {
			if (im > 0) {
				return (float(M_PI) / 2);
			}
			if (im < 0) {
				return (-float(M_PI) / 2);
			}
			return NAN;
		}
	}

	void rotate(float angle) {
		re *= cos(angle);
		im *= sin(angle);
	}

	void printComplex() {
		cout << re << " + j" << im << endl;
	}

protected:

private:
	float re;
	float im;
};

int main(void) {
	ComplexNumber c = ComplexNumber();
	c.printComplex();

	c.setComplex(4, 6);
	c.printComplex();

	c.rotate(2.3);
	c.printComplex();

	//EmbeddedFunctions *blah;
	//blah->GOpen("GALILPCI2 -d");

	// EthernetSend('192.168....', "OP 0,0;");
	return 0;
}
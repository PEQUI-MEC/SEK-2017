#ifndef ULTRASSOM_NXT_H_
#define ULTRASSOM_NXT_H_


#include <iostream>
#include <string>

using namespace std;

class Ultrassom_nxt {
public:
	Ultrassom_nxt(string porta, int dist_min = 0, int dist_max = 35);
	int le_centimetro();

	static string INPUT_1;// Sensor port 1
	static string INPUT_2;// Sensor port 2
	static string INPUT_3;// Sensor port 3
	static string INPUT_4;// Sensor port 4


private:
	string local_sensores = "/sys/class/lego-sensor/";
	string porta, arq_value0;
	bool conectado = false;
	int dist_min, dist_max;
};

#endif /* ULTRASSOM_NXT_H_ */

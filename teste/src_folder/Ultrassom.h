#ifndef ULTRASSOM_H_
#define ULTRASSOM_H_

#include <chrono>
#include <unistd.h>
#include <math.h>
#include <M_arquivos.h>
#include "ev3dev.h"

class Ultrassom {
public:
	Ultrassom(string sensor_port, double dist_max = 35, double dist_min = 0);
	double le_centimetro();

private:
	ev3dev::ultrasonic_sensor US ;
	double dist_max;
	double dist_min;
};
#endif /* ULTRASSOM_H_ */

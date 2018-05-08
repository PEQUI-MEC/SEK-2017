#include "Ultrassom.h"

Ultrassom::Ultrassom(string sensor_port, double dist_max, double dist_min)
: dist_max(dist_max),
  dist_min(dist_min),
  US(ev3dev::ultrasonic_sensor(sensor_port))
{
}

double Ultrassom::le_centimetro(){
		int leitura = 0;

		leitura = US.distance_centimeters();

		if(leitura > dist_max) leitura = dist_max;
		if(leitura < dist_min) leitura = dist_min;
		return leitura;

}

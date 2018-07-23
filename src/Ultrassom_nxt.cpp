#include <fstream>
#include "Ultrassom_nxt.h"


string Ultrassom_nxt::INPUT_1  = "in1:i2c1";  //!< Sensor port 1
string Ultrassom_nxt::INPUT_2  = "in2:i2c1";  //!< Sensor port 2
string Ultrassom_nxt::INPUT_3  = "in3:i2c1";  //!< Sensor port 3
string Ultrassom_nxt::INPUT_4  = "in4:i2c1";  //!< Sensor port 4


Ultrassom_nxt::Ultrassom_nxt(string porta, int dist_min, int dist_max)
:porta(porta),
 dist_min(dist_min),
 dist_max(dist_max)
{
	string tmp;
	for(int i = 0; i < 13; i ++) { // verifica em qual porta esta o sensor
		tmp = local_sensores + "sensor" + std::to_string(i) + "/address";
		ifstream is(tmp, ifstream::in);
		if(is.is_open()) {
			string t;
			is >> t;

			if(t == porta) {
				cout<<"CONN: ";
				conectado = true;
				cout<<i<<" - "<<t<<endl;
				arq_value0 = local_sensores + "sensor" + std::to_string(i) + "/value0";
			}
		}
		is.close();
	}
}


int Ultrassom_nxt::le_centimetro() {
	ifstream is(arq_value0, ifstream::in);
	if(conectado && is.is_open()) {
		int leitura = 0;
		is >> leitura;
		is.close();
		if(leitura > dist_max) leitura = dist_max;
		if(leitura < dist_min) leitura = dist_min;
		return leitura;
	} else return -1;
}

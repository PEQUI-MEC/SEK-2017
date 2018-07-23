#ifndef GARRA_H_
#define GARRA_H_

#include "Const.h"

using namespace std;

// A garra abre no sentido negativo, o limite é -42
class Garra {
public:
	Garra(string motor_port, int graus_abertura_do_motor, string tipo);
	void abrir();
	void fechar();

private:
	bool garra_aberta = false;
	ev3dev::large_motor garra;
	int graus_abertura_do_motor = 0;
	string tipo;
};



#endif /* GARRA_H_ */

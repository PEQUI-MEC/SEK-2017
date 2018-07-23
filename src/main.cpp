#include <iostream>
#include <unistd.h>
#include <chrono>
#include <string>
#include "M_arquivos.h"
#include "Sensor_cor_hsv.h"
#include "Controlador_robo.h"
#include "Mapeamento.h"
#include "Resgate.h"
#include "Ultrassom.h"
#include "Garra.h"

using namespace std;

int main() {
	system("setfont Greek-TerminusBold20x10.psf.gz");

	Controlador_robo robot(false, "debug posicao direto no pwm.m");
	Sensor_cor_hsv cor(ev3dev::INPUT_1, ev3dev::INPUT_2,false,"leitura_sensor_cor_hsv");
	Ultrassom ultraE(ev3dev::INPUT_3);
	Ultrassom ultraD(ev3dev::INPUT_4);
	Mapeamento mapa(&robot, &cor);
	Resgate resgate(&robot, &cor, &ultraE, &ultraD, ev3dev::OUTPUT_C, ev3dev::OUTPUT_D);


	robot.inicializar_thread_aceleracao();
	
	cout << endl << endl << endl << "comecar?" << endl;
	while(!ev3dev::button::enter.process());
	usleep(1000000*0.1);
	while(!ev3dev::button::enter.process());
	cout << "\n\n\n\n\n\n";

	mapa.mapear();
	resgate.resgatar();

	while(!ev3dev::button::enter.process());
	robot.parar();
	usleep(1000*500);
	robot.finalizar_thread_aceleracao();

	ev3dev::button::back.pressed();

	cout << "Bye!" << endl;
	usleep (1000000);
	return 0;
}

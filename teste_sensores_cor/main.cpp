/* Projeto teste_sensores_cor, main.cpp:
 * salva varias leituras do sensor de cor utilizando a
 * conversao de rgb para hsva
 */
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <string>
#include "M_arquivos.h"
#include "ev3dev.h"
#include "src_folder/Sensor_cor_hsv.h"
#include "src_folder/Controlador_robo.h"
int main(){
	system("setfont Greek-TerminusBold20x10.psf.gz");
	Controlador_robo robot(false, "debug posicao direto no pwm.m"); // fator_croda = 1.005
	Sensor_cor_hsv cor(ev3dev::INPUT_1, ev3dev::INPUT_2,true,"leitura_sensor_cor_hsv");
	robot.inicializar_thread_aceleracao();

	robot.calibra_sensor_cor(&cor);

	while(!ev3dev::button::up.pressed()){
		while(!ev3dev::button::enter.pressed() && !ev3dev::button::up.pressed());
		while(ev3dev::button::enter.pressed());
		robot.andar(30);
		while(!ev3dev::button::enter.pressed() && !ev3dev::button::up.pressed())
			cout<<cor.ler_cor_E()<<";"<<cor.ler_cor_D()<<endl;
		robot.parar();
		usleep(1000000*0.01);
		while(ev3dev::button::enter.pressed());
	}
	cor.fecha_arquivo();
	robot.finalizar_thread_aceleracao();
}

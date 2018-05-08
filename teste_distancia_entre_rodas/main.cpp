/* Projeto teste_distancia_entre_rodas, main.cpp:
 * far o robo girar 360 graus no proprio eixo para verificar
 * a distancia entre os centros de cada roda;
 *
 * ATENCAO: o raio da roda ja deve ter sido preveamente ajustado
 */

#include <iostream>
#include <unistd.h>
#include <chrono>
#include <string>
#include "M_arquivos.h"
#include "ev3dev.h"
#include "src_folder/Controlador_robo.h"

using namespace std;


int main(){

	Controlador_robo robo(true, "debug posicao direto no pwm.m");
	robo.inicializar_thread_aceleracao();
	robo.girar(360);
	robo.finalizar_thread_aceleracao();
}

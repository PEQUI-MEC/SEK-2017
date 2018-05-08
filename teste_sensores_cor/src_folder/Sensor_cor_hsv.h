/**
 * H = hue define a cor, 0a 360, cores predominantes de 60 em 60 sendo:
 * 		0 vermelho;
 * 		60 amarelo;
 * 		120 verde;
 * 		180 azul claro;
 * 		240 azul escuro;
 * 		300 rosa;
 * S = Saturacao 0 a 1 o quao desbotado a cor esta
 * V = value 0 a 1 o quao claro esta a cor (0 preto)
 */

#ifndef SENSOR_COR_HSV_H_
#define SENSOR_COR_HSV_H_

#include <tuple>
#include "ev3dev.h"
#include <unistd.h>
#include <math.h>
#include "M_arquivos.h"
#include "Const.h"

using namespace std;

class Sensor_cor_hsv {
public:
	Sensor_cor_hsv(string sensor_port_E, string sensor_port_D,
			bool debug = false, string nome_arquivo = "");
	Cor ler_cor_E();
	Cor ler_cor_D();
	ev3dev::color_sensor *get_sensor_E();
	ev3dev::color_sensor *get_sensor_D();
	void set_fatores_rgb(double *fatores_E, double *fatores_D);
	void set_maximos_minimos(double *maximos_E, double *maximos_D);
	void fecha_arquivo();
	HSV RGBtoHSV(RGB rgb);

private:
	bool debug = false;
	M_arquivos *arquivo_E;
	M_arquivos *arquivo_D;


	ev3dev::color_sensor sensor_E;
	ev3dev::color_sensor sensor_D;


	/*
	 * pensar na possibilidade de criar um struct com estes limites e colocar na const.h
	 */
	double limites_H_Vermelho_E[2] = {10, 340}; // verificar se esta fora
	double limites_H_Verde_E[2] = {120, 170}; // verificar se esta dentro
	double limites_H_Amarelo_E[2] = {10, 50}; // verificar se esta dentro
	double maximo_S_Branco_E = 0;
	double minimo_V_Branco_E = 0;
	double maximo_V_Preto_E = 0;
	double minimo_V_Preto_E = 0;

	double limites_H_Vermelho_D[2] = {12, 340}; // verificar se esta fora
	double limites_H_Verde_D[2] = {120, 170}; // verificar se esta dentro
	double limites_H_Amarelo_D[2]= {12, 50}; // verificar se esta dentro
	double maximo_S_Branco_D = 0;
	double minimo_V_Branco_D = 0;
	double maximo_V_Preto_D = 0;
	double minimo_V_Preto_D = 0;

	double range_H_preto_fora = 55; // usado so na nossa arena

	double fator_escalimetro_rgb_E[3] = {1,1,1}; // mudar o rgb para a escala 0 a 255
	double fator_escalimetro_rgb_D[3] = {1,1,1}; // mudar o rgb para a escala 0 a 255
};

#endif /* SENSOR_COR_HSV_H_ */

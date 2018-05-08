#ifndef CONST_H_
#define CONST_H_

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include "ev3dev.h"
#include <unistd.h>


using namespace std;

enum estados_arena{faixa, leu_fora, leu_nda, intersec, rampa, terminado, atencao, captura, salva}; //usado na classe Mapeamento
enum direcao{ndDirecao, frente, direita, esquerda, traz};// usado na classe Mapeamento
enum Cor{ndCor, preto, branco, vermelho, verde, amarelo, fora}; // cores possiveis
enum flag_aceleracao{ndAcel, linha_reta, parar, girar}; // usado dentro do Controlador_robo

struct RGB{ // utilizado na classe Sensor_cor_hsv
	int r = 0, g = 0, b = 0;
};

struct HSV{ // utilizado na classe Sensor_cor_hsv
	double h = 0, s = 0, v = 0;
};




// usado na classe MApeamento
struct direcao_checkpoint {
	direcao checkpoint_vermelho;
	direcao checkpoint_verde;
	direcao checkpoint_amarelo;
};

const bool automapear_3_checkpoint = true;

/* == 1 se estiver indo do ponto de start para a rampa
 * == -1 se estiver indo da rampa para o ponto de start
 */
extern int sentido_navegacao;


/* Variaveis de mapeamento de intersecção e de bonecos
 * inicializadas no inicio do arquivo Mapeamento.cpp
 * */
extern direcao_checkpoint cp; // intersecao
extern int qnt_cruzamentos;
const int total_cruzamentos_teste = 8;
const bool arena_pequi = false; // testar o robo na nossa arena e na arena oficial
const bool utilizar_arq = true; //utlizar arquivo de mapeamento ou nao
#endif

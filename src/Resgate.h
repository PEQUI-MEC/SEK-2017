#ifndef RESGATE_H_
#define RESGATE_H_

#include "Controlador_robo.h"
#include "Sensor_cor_hsv.h"
#include  "Ultrassom.h"
#include "Const.h"
#include "Garra.h"

#define distancia_boneco 23 //MODIFICAR DEPOIS

class Resgate {
public:

	Resgate(Controlador_robo *, Sensor_cor_hsv *, Ultrassom *, Ultrassom *, string porta_cancela, string porta_garra);
	void resgatar();

	/*
	 * metodo usado para ir do inicio para o final, o robo deve estar
	 * no inicio da arena, antes da primeira intersecao e virado para frente (1),
	 * este metodo termina com o robo na ultima intersec, virado para traz (-1) e
	 * depois de alinhar para traz na intersec um pouco para frente
	 */
	void ir_para_final();
	void go_to_plaza();
	void go_to_plaza2();

private:
	Controlador_robo *robo;
	Sensor_cor_hsv *sensor;
	Ultrassom *ultraE;
	Ultrassom *ultraD;
	Garra garra;
	Garra cancela;
	//ev3dev::large_motor *motorCancela;

	/*********************************************************************************************/
	/****************************VARIAVEIS DO CONTROLE DE DIRECAO*********************************/
	/*********************************************************************************************/
	// pwm principal, quando o robo esta andando e procurando
	int pwm_busca = 40;

	int carga_bonecos = 0;
	int capacidade_bonecos = 1;

	direcao direcao_boneco = direcao::ndDirecao;
	// cor que o referente a intersecao que o robo acabou de sair
	//Cor cor_atual = Cor::ndCor;

	// direcao referente a intersecao que o robo acabou de sair
	//direcao direcao_atual = direcao::ndDirecao;

	//flags para salvar ultima leitura da cor quando necessario
	Cor cor_E = Cor::ndCor;
	Cor	cor_D = Cor::ndCor;

	// flag que controla o comportamento do robo de acordo com onde ele esta
	estados_arena estd = estados_arena::faixa;
	/*-------------------------------------------------------------------------------------------*/
	/*-------------------------FIM VARIAVEIS DO CONTROLE DEDIRECAO--------------------------------*/
	/*-------------------------------------------------------------------------------------------*/


	/*********************************************************************************************/
	/***************************METODOS DO CONTROLE DE DIRECAO************************************/
	/*********************************************************************************************/

	// quando o robo estiver saindo de um lado ele volta e realinha
	void realinha(direcao lado_saindo);
	// quando o robo chegar em uma intersec (vermelho, verde, azul ou preto)
	void intersec();

	/* Método utilizado em: mapeamento_intersec
	 * Se o robô passar por uma interssecção onde a mesma já foi previamente mapeada
	 * ele segue pelo "caminho certo"
	 */
	void caminho_certo();


	/*-------------------------------------------------------------------------------------------*/
	/*-------------------------FIM METODOS DO CONTROLE DE DIRECAO--------------------------------*/
	/*-------------------------------------------------------------------------------------------*/


	/*********************************************************************************************/
	/******************************VARIAVEIS DO RESGATE*******************************************/
	/*********************************************************************************************/
	bool viu_boneco_E = false;
	bool viu_boneco_D = false;
	int dist_boneco_E = 999;
	int dist_boneco_D = 999;
	bool confirma_rampa = false;
	/*-------------------------------------------------------------------------------------------*/
	/*------------------------------FIM VARIAVEIS DO RESGATE-------------------------------------*/
	/*-------------------------------------------------------------------------------------------*/

	/*********************************************************************************************/
	/*********************************METODOS DO RESGATE******************************************/
	/*********************************************************************************************/
	void captura_rogerio();
	void captura_luana();

	/*-------------------------------------------------------------------------------------------*/
	/*--------------------------------FIM METODOS DO RESGATE-------------------------------------*/
	/*-------------------------------------------------------------------------------------------*/

};
#endif

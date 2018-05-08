//TODO fazer fim_cidade (aguardando e mail do chair sobre local da marcacao na rampa)
//TODO (testado uma vez)tirar verificacao do 3 checkpoint de forma que fique facil voltar
/*TODO (TESTAR) verificar todos os lugares onde ele chama o metodo realinhar,
 * em algum lugar o robo viu cores diferentes com os sensores e entrou em um loop onde
 * ele ficava somente realinhando
 */

#ifndef MAPEAMENTO_H_
#define MAPEAMENTO_H_

#include "Controlador_robo.h"
#include "Sensor_cor_hsv.h"
#include "Const.h"

#include <vector>
#include <fstream>
#include <string.h>

using namespace std;


class Mapeamento {
public:
	void saidinha_ultima_intersec();
	Mapeamento(Controlador_robo *, Sensor_cor_hsv *);

	// inicia rotina de mapeamento
	void mapear();

private:
	// objetos utilizadospara controlar o hardware
	Controlador_robo *robo;
	Sensor_cor_hsv *sensor;


	/*********************************************************************************************/
	/**************************VARIAVEIS DO MAPEAMENTO DE DIRECAO*********************************/
	/*********************************************************************************************/
	// cor que o referente a intersecao que o robo acabou de sair
	Cor cor_atual = Cor::ndCor;

	// direcao referente a intersecao que o robo acabou de sair
	direcao direcao_atual = direcao::ndDirecao;

	//	intersecao atual ja foi mapeada? se nao,
	//  quando encontrar uma nova intersecao mapear a atual
	bool confirmacao_status = false;

	// robo esta voltando de um dead-end?
	bool dead_end = false;

	//flags para salvar ultima leitura da cor quando necessario
	Cor cor_E = Cor::ndCor;
	Cor	cor_D = Cor::ndCor;

	// flag que controla o comportamento do robo de acordo com onde ele esta
	estados_arena estd = estados_arena::faixa;

	// contador de quantas cores ja mapeou
	int qnt_cores_mapeadas = 0;

	/*-------------------------------------------------------------------------------------------*/
	/*-------------------------FIMVARIAVEIS DO MAPEAMENTO DEDIRECAO------------------------------*/
	/*-------------------------------------------------------------------------------------------*/


	/*********************************************************************************************/
	/***************************METODOS DO MAPEAMENTO DE DIRECAO**********************************/
	/*********************************************************************************************/

	// quando o robo estiver saindo de um lado ele volta e realinha
	void realinha(direcao lado_saindo);
	// quando o robo chegar em uma intersec (vermelho, verde, azul ou preto)
	void intersec();

	/* Método de mapemento das direções das intersecções*/
	void mapeamento_intersec();


	/* Método utilizado em: mapeamento_intersec
	 * Se o robô passar por uma interssecção onde a mesma já foi previamente mapeada
	 * ele segue pelo "caminho certo"
	 */
	void caminho_certo();

	/*verifica se a cor que o robo esta sobre quando chamar o metodo ja foi mapeada*/
	bool cor_ja_mapeada();

	/*
	 * metodo para fazer a saidinha da ultima intersec, ele eh chamado
	 * no final do metodo mapear, quando o robo esta detro da ultima intersec
	 * e de frente para a direcao correta, que seria a direcao da rampa
	 */
	//void saidinha_ultima_intersec();


	/*
	 * metodo para voltar pro inicio e ganhas os 20 pontinhos
	 */
	void volta_200pto();
	/*-------------------------------------------------------------------------------------------*/
	/*-------------------------FIM METODOS DO MAPEAMENTO DE DIRECAO------------------------------*/
	/*-------------------------------------------------------------------------------------------*/

	ifstream file;
	ofstream outfile;
	string c;
	bool arq_existente = false;
	bool todas_cores_mapeadas = false;
	bool pegar_informacoes_arq();
};


#endif

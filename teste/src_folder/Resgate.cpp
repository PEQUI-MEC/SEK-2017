#include "Resgate.h"


Resgate::Resgate(Controlador_robo *robo, Sensor_cor_hsv *sensor, Ultrassom_nxt *ultraE, Ultrassom_nxt *ultraD)
:robo(robo), sensor(sensor), ultraE(ultraE), ultraD(ultraD){}

void Resgate::resgatar(){
	cout << endl << endl << endl << "RESGATAR!!" << endl;
	robo->girar(180);
	while(robo->get_estado() == flag_aceleracao::girar);
	robo->alinhar(sensor, direcao::traz);
	robo->andar(60);
	usleep(1000000*0.5); // so pra sair da cor que ele alinhou


	int count_intersec = 0;
	int count_branco_apos_intersec = 0;
	Cor corE_corD_iguais = Cor::ndCor;
	int mudanca_cor_fim_cidade = 0;

	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();
		//TODO ler os ultras aqui

		switch (estd){
		case estados_arena::leu_fora:
			cout << "fora:";
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			mudanca_cor_fim_cidade = 0;
			if(cor_E == Cor::fora)
				realinha(direcao::esquerda);
			else if (cor_D == Cor::fora)
				realinha(direcao::direita);
			else {
				estd = estados_arena::atencao;
				cout << "ATENCAO!!" << endl;
			}

			break;


		case estados_arena::intersec:
			cout << "intersec?";
			while(true){
				if(sensor->ler_cor_E() != cor_E){
					realinha(direcao::esquerda);
				}
				else if(sensor->ler_cor_D() != cor_D){
					realinha(direcao::direita);
				}
				else
					break;
				cout << "nao, loop infinito?" << endl;//FIXME tratar
				robo->alinhar(sensor, direcao::frente);
				robo->andar(30, 0.02);
			}
			cout << "SIM" << endl;
			intersec();
			while( !(sensor->ler_cor_E() == Cor::branco) || !(sensor->ler_cor_D() == Cor::branco))
			{
				count_branco_apos_intersec ++;
				if(count_branco_apos_intersec >=5){
					robo->parar();
					cout << "o robo terminou a intersecao e nao esta no branco" << endl;
					cout << "esperando 10 seg" << endl;
					usleep(1000000*10);
					estd = estados_arena::faixa;
				}
			}
			estd = estados_arena::faixa;
			cout << "FAIXA!!" << endl;
			break;


		case estados_arena::faixa:
			//cout << "faixa" << endl;
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			mudanca_cor_fim_cidade = 0;
			robo->andar(70);
			if (cor_E != Cor::branco || cor_D != Cor::branco){
				estd = estados_arena::atencao;
				cout << "ATENCAO!!" << endl;
			}

			break;


		case estados_arena::atencao:
			/* TODO fazer tratamento do caso que sensor fique muito tempo em uma interface
			 */
			//cout << "atencao" << endl;
			if(count_intersec == 0)
				robo->andar(30);

			if(cor_E == Cor::branco && cor_D == Cor::branco)
			{
				estd = estados_arena::faixa;
				cout << "FAIXA!!" << endl;
				break;
			}
			if (cor_E == Cor::fora || cor_D == Cor::fora)
			{
				estd = estados_arena::leu_fora;
				cout << "FORA!!" << endl;
				break;
			}
			if((cor_E == Cor::vermelho && cor_D == Cor::vermelho) ||
					(cor_E == Cor::verde && cor_D == Cor::verde) ||
					(cor_E == Cor::azul && cor_D == Cor::azul) ||
					(cor_E == Cor::preto && cor_D == Cor::preto))
			{
				robo->andar(20);
				if(corE_corD_iguais != cor_E){
					corE_corD_iguais = cor_E;
					mudanca_cor_fim_cidade ++;
					cout << "INTERFACE" << endl;
					count_intersec = 0;
				}else
					count_intersec ++;

				switch(mudanca_cor_fim_cidade){
				case 1:
					if(count_intersec >= 8) // robo desacelerando, passa a cor rapido
						estd = estados_arena::intersec;

					break;
				case 2:
					//					if(count_intersec >= 25) // robo ja devagar, demora mais pra passar a cor
					//						estd = estados_arena::intersec;
					//					break;
					//				case 3:
					estd = estados_arena::terminado;
					cout << "MAP TERMINADO!!" << endl;
					break;
				}
				usleep(1000000*0.08);
			}

			break; // 	FIM DO case estados_arena::atencao:


		} //FIM DO switch (estd){
	} // FIM DO while(estd != estados_arena::terminado){

	cout << "fim da arena" << endl;
	robo->parar();
	usleep(1000000*8);
}

void Resgate::intersec() {
	//fazer esse alinhamento somente se nao for preto
	if(cor_E != Cor::preto){
		robo->alinhar(sensor, direcao::traz);
		robo->andar(50, 0.15 + robo->get_pintao()); // vai pro meio do quadrado
	}
	cor_E = sensor->ler_cor_E();
	cor_D = sensor->ler_cor_D();
	/*
	if(true) mapeamento_intersec();// verificar se eh o fim da cidade
	else {
		arq_map->arquivo_map(cp);
		estd = estados_arena::terminado;
	}
	*/
}



void Resgate::realinha(direcao lado_saindo) {
	double pwm_sp = robo->get_pwm_sp();
	int grau = 12;
	if(lado_saindo == direcao::esquerda)
	{
		cout<<"saiu E"<<endl;
		robo->parar();
		robo->andar(-80,0.08);
		robo->girar(-grau);
		while(robo->get_estado() == flag_aceleracao::girar);
		//robo->andar(80, 0.07); // anda pra frente necessario?
		robo->andar(pwm_sp);
	}

	else if(lado_saindo == direcao::direita)
	{
		cout<<"saiu D"<<endl;
		robo->parar();
		robo->andar(-80,0.08);
		robo->girar(grau);
		while(robo->get_estado() == flag_aceleracao::girar);
		//robo->andar(80, 0.08); // anda pra frente necessario?
		robo->andar(pwm_sp);
	}
	else{
		cout << "realinha argumento errado" << endl;
		robo->parar();
		usleep(1000000*5);
	}
	usleep(1000000*0.3);
}




void Resgate::inverter_ponteiros_pos(){


}


void Resgate::caminho_certo (){
	if (sensor->ler_cor_E() == Cor::vermelho && sensor->ler_cor_D() == Cor::vermelho){
		if (cp.checkpoint_vermelho == direcao::direita){
			robo->girar(-90);;
		}

		else if (cp.checkpoint_vermelho == direcao::esquerda){
			robo->girar(90);
		}
	}

	else if (sensor->ler_cor_E() == Cor::verde && sensor->ler_cor_D() == Cor::verde){
		if (cp.checkpoint_verde == direcao::direita){
			robo->girar(-90);
		}

		else if (cp.checkpoint_verde == direcao::esquerda){
			robo->girar(90);
		}
	}

	else if (sensor->ler_cor_E() == Cor::azul && sensor->ler_cor_D() == Cor::azul){
		if (cp.checkpoint_azul == direcao::direita){
			robo->girar(-90);
		}

		else if (cp.checkpoint_azul == direcao::esquerda){
			robo->girar(90);
		}
	}

	while(robo->get_estado() == flag_aceleracao::girar);
}

#include "Resgate.h"


Resgate::Resgate(Controlador_robo *robo, Sensor_cor_hsv *sensor, Ultrassom *ultraE, Ultrassom *ultraD, string porta_cancela, string porta_garra)
:robo(robo), sensor(sensor), ultraE(ultraE), ultraD(ultraD),
 garra(Garra(porta_garra, 20, "quelicera")), cancela(Garra(porta_cancela, 48, "cancela"))
{}

void Resgate::resgatar(){
	//carga_bonecos = capacidade_bonecos;
	sentido_navegacao = 1;
	robo->andar(pwm_busca);
	usleep(1000000*0.5); // esperar sair da cor que ele alinhou


	int dist_boneco_E = 999; // dist do ultra pro boneco_E
	int dist_boneco_D = 999; // dist do ultra pro boneco_D
	bool vi_boneco = false; // viu boneco na leitura do ultra
	bool vi_boneco0 = false; // viu boneco na leitura anterior do ultra
	int cont = 0; // contador para confirmar a leitura do ultra
	int count_intersec = 0; // contador para confirmar quando entrar na intersecao
	int count_branco_apos_intersec = 0; // contador para confirmar a saida da intersecao
	int cnt_rampa = 0;
	Cor corE_corD_iguais = Cor::ndCor; // quando os sensores lerem a mesma cor, para comparacao

	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();
		dist_boneco_E = ultraE->le_centimetro();
		dist_boneco_D = ultraD->le_centimetro();

		if(carga_bonecos >= capacidade_bonecos)
			pwm_busca = 70;
		else pwm_busca = 40;
		if(estd != estados_arena::atencao)cout << dist_boneco_E<<";" << dist_boneco_D << endl;

		if((dist_boneco_E <= distancia_boneco || dist_boneco_D <= distancia_boneco) &&
				carga_bonecos <capacidade_bonecos &&
				estd != estados_arena::atencao)
		{
			cout << dist_boneco_E<<";" << dist_boneco_D<< "BONECO?" << endl;
			pwm_busca = 15;
			robo->andar(pwm_busca);
			cont ++;
			if(cont >= 1){
				cout << "SIM" << endl;
				estd = estados_arena::captura;
			}
		} else	cont = 0;


		switch (estd){
		case estados_arena::leu_fora:
			cout << "fora:";
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			if(cor_E == Cor::fora)
				realinha(direcao::esquerda);
			else if (cor_D == Cor::fora)
				realinha(direcao::direita);
			else if(qnt_cruzamentos >=total_cruzamentos_teste &&
					carga_bonecos >= capacidade_bonecos){ //saindo da ultima intersec, subir rampa
				estd = estados_arena::rampa;
				cout << "RAMPA!" << endl;
			}
			else{
				estd = estados_arena::atencao;
				cout << "ATENCAO!!" << endl;
			}

			break; // FIM CASE leu_fora:


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
				robo->andar(20);
				usleep(1000000*0.1);
				count_branco_apos_intersec ++;
				if(count_branco_apos_intersec >=20){
					robo->parar();
					cout << "o robo terminou a intersecao e nao esta no branco" << endl;
					cout << "esperando 3 seg" << endl;
					usleep(1000000*1);
					estd = estados_arena::faixa;
					break;
				}
			}
			robo->alinhar(sensor, direcao::traz);
			if(qnt_cruzamentos<= 0){
				estd = estados_arena::terminado;
				cout << endl << endl << "TERMINOU RESGATE"<< endl;
			}
			else{
				estd = estados_arena::faixa;
				cout << "FAIXA!!" << endl;
				if(carga_bonecos <= capacidade_bonecos)
					robo->andar(60, 0.2); // anda p colocar o us na faixa
			}

			break; // FIM CASE intersec:


		case estados_arena::faixa:
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			if(carga_bonecos >= capacidade_bonecos)
				robo->andar(70);
			else
				robo->andar(pwm_busca);
			if (cor_E != Cor::branco || cor_D != Cor::branco){
				if(qnt_cruzamentos >=total_cruzamentos_teste &&
						sentido_navegacao == 1){//saindo da ultima intersec, subir rampa
					// *** ALINHAR PELA TANGENTE PARA VOLTAR CERTO OU IR PARA RAMPA CERTO
					robo->andar(-50,0.1);
					robo->girar(45+180);
					robo->andar(40);
					while(sensor->ler_cor_E() != Cor::fora);
					robo->parar();
					robo->andar(-30,0.05);
					robo->girar(-45);
					if(carga_bonecos >= 1){
						estd = estados_arena::rampa;
						cout << "RAMPA" << endl;
						break;
					}else{
						robo->andar(-50,0.2);
						robo->girar(180);
						sentido_navegacao = -1;
					}
				}
				else{//saindo da ultima intersec,nao subir rampa, voltar
					estd = estados_arena::atencao;
					cout << "ATENCAO!!" << endl;
				}
			}

			break; // FIM CASE faixa:


		case estados_arena::atencao:
			/* TODO fazer tratamento do caso que sensor fique muito tempo em uma interface
			 */
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
					(cor_E == Cor::amarelo && cor_D == Cor::amarelo) ||
					(cor_E == Cor::preto && cor_D == Cor::preto))
			{
				robo->andar(20);
				if(corE_corD_iguais != cor_E){
					corE_corD_iguais = cor_E;
					cout << "INTERFACE" << endl;
					count_intersec = 0;
				}else
					count_intersec ++;

				if(count_intersec >= 8) // confirmar intersec
					estd = estados_arena::intersec;
			}
			usleep(1000000*0.08);

			break; // 	FIM DO case estados_arena::atencao:


		case estados_arena::rampa:
			// aqui eu estou no inicio da rampa
			robo->andar(-60, 0.28); // FIXME olhar isso na pista oficial
			robo->andar(-40);
			cnt_rampa = 0;
			cout << "kd rampa" << endl;
			while(true){
				usleep(1000000*0.1);
				if(sensor->ler_cor_E() == Cor::vermelho ||
						sensor->ler_cor_D() == Cor::vermelho)
				{
					cnt_rampa ++;
				}
				else cnt_rampa = 0;

				if(cnt_rampa >=4)
					break;
			}
			cout << "viu vermelho" << endl;
			robo->andar(40,0.04);
			cout << "saiu vermelho" << endl;
			robo->alinhar(sensor, direcao::traz);
			robo->andar(-50, 0.3); // FIXME olhar isso na pista oficial
			robo->girar(180);
			estd = estados_arena::salva;
			cout << "salvaaa"<<endl;
			usleep(1000000*2);



			break;


		case estados_arena::captura: // caso ver algum boneco
			captura_rogerio();
			pwm_busca = 40;
			// o estado eh atribuido dentro do metodo de captura
			//captura_luana();

			break; // FIM CASE captura:


		case estados_arena::salva:
			confirma_rampa = false;
			robo->andar(70);
			usleep(1000000*2);
			while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
			go_to_plaza();
			//TODO fazer retorno para continuar resgatando
			break;// FIM CASE salva:


		} //FIM DO switch (estd){
	} // FIM DO while(){
}


void Resgate::intersec() {
	if(sentido_navegacao == -1)
		qnt_cruzamentos --;
	else
		qnt_cruzamentos ++;

	robo->alinhar(sensor, direcao::traz);
	robo->andar(50, 0.15 + robo->get_pintao()); // vai pro meio do quadrado
	caminho_certo();
	cor_E = sensor->ler_cor_E();
	cor_D = sensor->ler_cor_D();
	robo->andar(70);// sai da intersec
	while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
	usleep(1000000*0.3);

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






void Resgate::caminho_certo (){
	if (sensor->ler_cor_E() == Cor::vermelho && sensor->ler_cor_D() == Cor::vermelho){
		if (cp.checkpoint_vermelho == direcao::direita){
			if(sentido_navegacao == -1)
				robo->girar(90);
			else
				robo->girar(-90);
		}

		else if (cp.checkpoint_vermelho == direcao::esquerda){
			if(sentido_navegacao == -1)
				robo->girar(-90);
			else
				robo->girar(90);
		}
	}

	else if (sensor->ler_cor_E() == Cor::verde && sensor->ler_cor_D() == Cor::verde){
		if (cp.checkpoint_verde == direcao::direita){
			if(sentido_navegacao == -1)
				robo->girar(90);
			else
				robo->girar(-90);
		}

		else if (cp.checkpoint_verde == direcao::esquerda){
			if(sentido_navegacao == -1)
				robo->girar(-90);
			else
				robo->girar(90);
		}
	}

	else if (sensor->ler_cor_E() == Cor::amarelo && sensor->ler_cor_D() == Cor::amarelo){
		if (cp.checkpoint_amarelo == direcao::direita){
			if(sentido_navegacao == -1)
				robo->girar(90);
			else
				robo->girar(-90);
		}

		else if (cp.checkpoint_amarelo == direcao::esquerda){
			if(sentido_navegacao == -1)
				robo->girar(-90);
			else
				robo->girar(90);
		}
	}

	//while(robo->get_estado() == flag_aceleracao::girar);
}

/*
 * quando terminar a captura o robo esta no sentido 1, no meio da pista
 * e entra no estado de salvar
 */
void Resgate::captura_rogerio() {
	dist_boneco_E = ultraE->le_centimetro();
	dist_boneco_D = ultraD->le_centimetro();
	if(dist_boneco_E <= distancia_boneco){ // >>>>>>>>>>>>>>>>>>>>> viu boneco a esquerda
		cout << "ESQ!" << endl;
		robo->parar();
		robo->andar(-30, 0.11);
		cancela.abrir();
		robo->girar(90);
		robo->andar(60);
		int tem_boneco = 0;
		int cnt = 0;
		while(tem_boneco == 0){
			if(sensor->ler_cor_E() == Cor::fora && sensor->ler_cor_D() == Cor::fora){
				robo->andar(-60, 0.15-robo->get_pintao());
				robo->girar(-90);
				tem_boneco = -1;
			}
			cnt ++;
			usleep(1000000*0.1);
			if(cnt >= 16) // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> delay pro robo capturar
				tem_boneco = 1;
		}
		robo->parar();
		cancela.fechar();
		if(tem_boneco == 1){
			cancela.fechar();
			robo->andar(-60, 0.08);

			cancela.abrir();
			robo->andar(60, 0.08);
			cancela.fechar();
			robo->andar(-60, 0.08);

			cancela.abrir();
			robo->andar(60, 0.08);
			cancela.fechar();

			robo->andar(-60, 0.20);
			carga_bonecos ++;
			if(carga_bonecos >=capacidade_bonecos){ // cheio, salva
				if(sentido_navegacao -1)
					robo->girar(90);
				else
					robo->girar(-90);

				sentido_navegacao = 1;
				//estd = estados_arena::salva;
			}
			else{ // ainda nao cheio, continuar a procurar
				robo->girar(-90);
				while(robo->get_estado() == flag_aceleracao::girar);
				estd = estados_arena::faixa;
			}
		}else estd = estados_arena::faixa;
	}
	else if(dist_boneco_D <= distancia_boneco){ // >>>>>>>>>>>>>>>>>>>>> viu boneco a direita
		cout << "DIR!" << endl;
		robo->parar();
		robo->andar(-30, 0.11);
		cancela.abrir();
		robo->girar(-90);
		robo->andar(60);
		int tem_boneco = 0;
		int cnt = 0;
		while(tem_boneco == 0){
			if(sensor->ler_cor_E() == Cor::fora && sensor->ler_cor_D() == Cor::fora){
				robo->andar(-60, 0.15-robo->get_pintao());
				robo->girar(-90);
				tem_boneco = -1;
			}

			cnt ++;
			usleep(1000000*0.1);
			if(cnt >= 16) // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> delay pro robo capturar
				tem_boneco = 1;
		}
		robo->parar();
		cancela.fechar();
		if(tem_boneco == 1){
			cancela.fechar();
			robo->andar(-60, 0.08);

			cancela.abrir();
			robo->andar(60, 0.08);
			cancela.fechar();
			robo->andar(-60, 0.08);

			cancela.abrir();
			robo->andar(60, 0.08);
			cancela.fechar();

			robo->andar(-60, 0.17);
			carga_bonecos ++;
			if(carga_bonecos >=capacidade_bonecos){ // cheio, saiva
				if(sentido_navegacao -1)
					robo->girar(-90);
				else
					robo->girar(90);
				sentido_navegacao = 1;
				//estd = estados_arena::salva;
			}
			else{ // ainda nao cheio, continuar a procurar
				robo->girar(90);
				while(robo->get_estado() == flag_aceleracao::girar);
				estd = estados_arena::faixa;
			}
		}else estd = estados_arena::faixa;
	}
	else{
		cout << "NAO!" << endl;
	}
	estd = estados_arena::faixa;
}

/*
 * metodo de captura da luana, o robo deve ter acabado de detectar o boneco
 * para chamar este metodo
 */
void Resgate::captura_luana() {
	Controlador_robo robo(true, "debug posicao direto no pwm.m");
	Garra garra(ev3dev::OUTPUT_D, 135, "garra");
	Garra cancela(ev3dev::OUTPUT_C, 45, "cancela");
	Ultrassom ultraE(Ultrassom(ev3dev::INPUT_3));

	robo.inicializar_thread_aceleracao();

	cout << "Fazer teste pega_boneco" << endl;
	while(!ev3dev::button::enter.process());

	robo.andar(25);
	while(!ev3dev::button::up.process());


	usleep(1000000*0.3);
	robo.parar();


	cancela.abrir();
	garra.abrir();

	robo.girar(90);
	while(robo.get_estado() == flag_aceleracao::girar);

	robo.andar(30, robo.get_pintao()+0.02);

	garra.fechar();

	robo.andar(20);
	cout << "Vou entrar no for" << endl;
	for(unsigned count_resg = 0; count_resg < 1; count_resg++){
		garra.abrir();
		usleep(1000000);
		garra.fechar();
	}
	cout << "saindo do for" << endl;

	robo.parar();

	garra.fechar();
	cancela.fechar();

	robo.andar(-30, 0.135);
	robo.girar(-90);
	while(robo.get_estado() == flag_aceleracao::girar);

}

/*
 * metodo usado para ir do inicio para o final, o robo deve estar
 * no inicio da arena, antes da primeira intersecao e virado para frente (1),
 * este metodo termina com o robo na ultima intersec, virado para traz (-1) e
 * depois de alinhar para traz na intersec um pouco para frente
 */
void Resgate::ir_para_final() {
	int count_intersec = 0; // contador para confirmar quando entrar na intersecao
	int count_branco_apos_intersec = 0; // contador para confirmar a saida da intersecao
	Cor corE_corD_iguais = Cor::ndCor; // quando os sensores lerem a mesma cor, para comparacao
	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();

		switch (estd){
		case estados_arena::leu_fora:
			cout << "fora:";
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			if(cor_E == Cor::fora)
				realinha(direcao::esquerda);
			else if (cor_D == Cor::fora)
				realinha(direcao::direita);
			else {
				estd = estados_arena::atencao;
				cout << "ATENCAO!!" << endl;
			}

			break; // FIM CASE leu_fora:


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

			if(qnt_cruzamentos>=3){ // se caso for a ultima intersec,
				sentido_navegacao = -1;
				robo->girar(180);
				while(robo->get_estado() == flag_aceleracao::girar);
				robo->alinhar(sensor,direcao::traz);
				robo->andar(30,0.05);
				estd = estados_arena::terminado;
				break;
			}

			intersec();
			while( !(sensor->ler_cor_E() == Cor::branco) || !(sensor->ler_cor_D() == Cor::branco))
			{
				robo->andar(20);
				usleep(1000000*0.1);
				count_branco_apos_intersec ++;
				if(count_branco_apos_intersec >=20){
					robo->parar();
					cout << "o robo terminou a intersecao e nao esta no branco" << endl;
					cout << "esperando 3 seg" << endl;
					usleep(1000000*1);
					estd = estados_arena::faixa;
					break;
				}
			}
			robo->alinhar(sensor, direcao::traz);
			estd = estados_arena::faixa;
			cout << "FAIXA!!" << endl;

			break; // FIM CASE intersec:


		case estados_arena::faixa:
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			robo->andar(60);
			if (cor_E != Cor::branco || cor_D != Cor::branco){
				estd = estados_arena::atencao;
				cout << "ATENCAO!!" << endl;
			}

			break; // FIM CASE faixa:


		case estados_arena::atencao:
			/* TODO fazer tratamento do caso que sensor fique muito tempo em uma interface
			 */
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
					(cor_E == Cor::amarelo && cor_D == Cor::amarelo) ||
					(cor_E == Cor::preto && cor_D == Cor::preto))
			{
				robo->andar(20);
				if(corE_corD_iguais != cor_E){
					corE_corD_iguais = cor_E;
					cout << "INTERFACE" << endl;
					count_intersec = 0;
				}else
					count_intersec ++;

				if(count_intersec >= 8) // confirmar intersec
					estd = estados_arena::intersec;
			}
			usleep(1000000*0.08);


			break; // 	FIM DO case estados_arena::atencao:

		} //FIM DO switch (estd){
	} // FIM DO while(){






	sentido_navegacao = -1;
	cout << endl << endl << endl << "RESGATAR!!" << endl;
	robo->girar(180);
	while(robo->get_estado() == flag_aceleracao::girar);
	robo->alinhar(sensor, direcao::traz);
}

/*
 * metodo que leva o boneco para o plaza, o robo deve estar na rampa, subindo
 * alinhado para traz com o verde
 */
void Resgate::go_to_plaza() {
	Cor cor_E = Cor::ndCor;
		Cor	cor_D = Cor::ndCor;
		int count_nwhite = 0;

		robo->andar(70);
		while(true){
			cor_E = sensor->ler_cor_E();
			cor_D = sensor->ler_cor_D();

			cout << cor_E << "   " << cor_D << endl;

			if(count_nwhite >= 10) break;

			if(cor_E != Cor::branco && cor_D != Cor::branco)
				count_nwhite++;
			else count_nwhite = 0;
			usleep(1000000*0.1);
		}
		robo->parar();

		robo->andar(70, 0.25);

		cancela.abrir();

		robo->andar(-70);
		while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
		usleep(100000);

		robo->parar();
		//cancela->fechar();

		//robo.alinhar(&sensor, direcao::frente);

		cancela.fechar();

		robo->girar(-90);
		while(robo->get_estado() == flag_aceleracao::girar);

		robo->andar(70, 0.40);

		robo->girar(-90);
		while(robo->get_estado() == flag_aceleracao::girar);

		robo->andar(70, 0.5);
		robo->andar(-30, 0.05);

		robo->girar(-90);
		while(robo->get_estado() == flag_aceleracao::girar);

		robo->andar(50);
		while(ultraE->le_centimetro() < 30);
		robo->parar();
		usleep(1000000*0.5);
		robo->girar(-90);
		while(robo->get_estado() == flag_aceleracao::girar);

		robo->andar(-50, 0.05);
		while(true){
			robo->andar(-50);
			cor_E = sensor->ler_cor_E();
			cor_D = sensor->ler_cor_D();

			if(cor_E == Cor::fora)
				realinha(direcao::esquerda);
			else if (cor_D == Cor::fora)
				realinha(direcao::direita);

			if(cor_E == Cor::verde && cor_D == Cor::verde){
				robo->alinhar(sensor, direcao::traz);
				robo->andar(-50, 0.05);
				robo->andar(-50);
				while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
				//usleep(1000000*0.5);
				robo->girar(180);
				robo->parar();
				cout << "terminei!!!" << endl;
				usleep(1000000*3);
				break;
			}
		}
}

void Resgate::go_to_plaza2(){
	Cor cor_E = Cor::ndCor;
	Cor	cor_D = Cor::ndCor;
	int count_nwhite = 0;

	robo->andar(60);
	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();

		cout << cor_E << "   " << cor_D << endl;

		if(count_nwhite >= 10) break;

		if(cor_E != Cor::branco && cor_D != Cor::branco)
			count_nwhite++;
		else count_nwhite = 0;
		usleep(1000000*0.1);
	}
	robo->parar();

	robo->andar(40, 0.22);


	robo->andar(-30);
	while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
	usleep(100000);

	robo->parar();


	robo->andar(-60);

	while(true){
		if((cor_E == Cor::vermelho && cor_D == Cor::vermelho) ||
				(cor_E == Cor::verde && cor_D == Cor::verde))
		{
			//robo->alinhar(sensor, direcao::traz);
			break;
		}
	}

	robo->andar(-30);
	while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
	usleep(1000000*0.5);
	robo->alinhar(sensor, direcao::traz);
	robo->andar(-30);
	usleep(1000000*4);

	while(true){
		if(sensor->ler_cor_E() == Cor::branco && sensor->ler_cor_D() == Cor::branco){
			robo->parar();
			robo->girar(180);
			break;
		}
	}

}

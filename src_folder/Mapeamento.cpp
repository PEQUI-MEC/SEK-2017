#include "Mapeamento.h"

//********************VARIAVEIS GLOBAIS DECLARADAS DENTRO DO CONST**********//
int sentido_navegacao = 1;
direcao_checkpoint cp = {direcao::ndDirecao, direcao::ndDirecao, direcao::ndDirecao};
int qnt_cruzamentos = 0;
//********************VARIAVEIS GLOBAIS DECLARADAS DENTRO DO CONST**********//


Mapeamento::Mapeamento(Controlador_robo *robo, Sensor_cor_hsv *sensor)
:robo(robo), sensor(sensor)
{
	file.open("map.txt");
	if (!( getline(file, c)))
		outfile.open("map.txt", ios::app);

	else
		arq_existente = true;

}

/*
 * Primeiro número representa a cor, sendo:
 * 0 - Vermelho
 * 1 - Verde
 * 2 - Amarelo
 *
 * O segundo valor representa a direcao, sendo:
 * 1 - em frente
 * 2 - direita
 * 3 - esquerda
 *
 */

bool Mapeamento::pegar_informacoes_arq(){
	int count_intersec_map = 1;
	string x = "0123";

	do{
		if(c[0] == x[0]){
			if(c[1] == x[1])
				cp.checkpoint_vermelho = direcao::frente;
			else if(c[1] == x[2])
				cp.checkpoint_vermelho = direcao::direita;
			else if(c[1] == x[3])
				cp.checkpoint_vermelho = direcao::esquerda;
		}

		else if(c[0] == x[1]){
			if(c[1] == x[1])
				cp.checkpoint_verde = direcao::frente;
			else if(c[1] == x[2])
				cp.checkpoint_verde = direcao::direita;
			else if(c[1] == x[3])
				cp.checkpoint_verde = direcao::esquerda;
		}

		else if(c[0] == x[2]){
			if(c[1] == x[1])
				cp.checkpoint_amarelo = direcao::frente;
			else if(c[1] == x[2])
				cp.checkpoint_amarelo = direcao::direita;
			else if(c[1] == x[3])
				cp.checkpoint_amarelo = direcao::esquerda;
		}

		if (!( getline(file, c))) break;
		count_intersec_map++;

	} while(count_intersec_map <= 3);

	if(count_intersec_map == 3) return true;
	else return false;
}


/* rotina de mapeamento, controla o robo inteiro enquanto mapeia as cores
 * e faz o controle das threads de mapear os bonecos
 */
void Mapeamento::mapear(){
	if(arq_existente && utilizar_arq){
		todas_cores_mapeadas = pegar_informacoes_arq();
		cout << "informacoes pegadas" << endl;

		cout << cp.checkpoint_vermelho << endl;
		cout << cp.checkpoint_verde << endl;
		cout << cp.checkpoint_amarelo << endl;
		usleep(1000000*2);
	}

	cout << endl << endl << endl << "bora mapear!!" << endl;
	direcao_atual = direcao::ndDirecao;

	int count_intersec = 0; // entrou na intersec
	int count_branco_apos_intersec = 0; // saiu da intersec e nao ta no branco, conta
	Cor corE_corD_iguais = Cor::ndCor;

	while(estd != estados_arena::terminado){
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
			if(qnt_cruzamentos>= total_cruzamentos_teste){
				cout << "VOLTAR 200PTO" << endl; // robo na ultima intersec
				robo->andar(60,0.2);
				sentido_navegacao = -1;
				robo->girar(180);
				while(robo->get_estado() == flag_aceleracao::girar);
				estd = estados_arena::faixa;
			}
			else if(qnt_cruzamentos == 0 && sentido_navegacao == -1){
				robo->andar(60,0.18);
				sentido_navegacao = 1;
				robo->girar(180);
				while(robo->get_estado() == flag_aceleracao::girar);
				estd = estados_arena::terminado;
				cout << endl << endl << "MAP TERMINADO!"<< endl;
			}
			else{
				estd = estados_arena::faixa;
				cout << "FAIXA!!" << endl;
			}

			break; // FIM CASE intersec:


		case estados_arena::faixa:
			count_intersec = 0;
			count_branco_apos_intersec = 0;
			corE_corD_iguais = Cor::ndCor;
			robo->andar(80);
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

				if(count_intersec >= 4) // confirmar intersec
					estd = estados_arena::intersec;
			}
			usleep(1000000*0.08);

			break; // 	FIM DO case estados_arena::atencao:


		} //FIM DO switch (estd){
	} // FIM DO while(estd != estados_arena::terminado){
}

/* quando o robo entra em uma intersecao dentro do metodo mapear chama-se esse metodo.
 * considera-se dentro da intersecao quando os 2 sensores detectarem a mudanca de cor e fizer
 * 3 leituras para confirmar essa intersecao, equivale aos sensores de cor entrar dentro da incersecao
 * mais ou menos 2 cm na velocidade de atencao (30) e as rodas ainda continuam pra fora, o robo pode estar
 * desalinhado
 */
void Mapeamento::intersec() {
	//fazer esse alinhamento somente se nao for preto, se for preto ja entrar no mapeamento_intersec
	if(cor_E != Cor::preto){
		robo->alinhar(sensor, direcao::traz);
		robo->andar(50, 0.15 + robo->get_pintao()); // vai pro meio do quadrado
	}

	cor_E = sensor->ler_cor_E();
	cor_D = sensor->ler_cor_D();

	mapeamento_intersec();
	//usleep(1000000*0.3); // delay saindo da intersec
}


/* mapeia aquela cor e faz a proxima movimentacao do robo, se for a primeira intersecao inicializa a thread
 * de boneco, se nao, verifica se eh um dead-end ou outra intersecao, se for outra intersecao mapeia a atual
 * se necessario e verifica se ja eh uma intersecao conhecida ou nao, se for conhecida segue pro caminho certo,
 * se nao for conhecida a nova intersecao passa a ser a atual e o robo comeca a mapea-la, se mapear duas intersecoes
 * a terceira ja eh mapeada automaticamente
 */
void Mapeamento::mapeamento_intersec() {
	if(cor_E != cor_D) // verifica se esta como previsto, se nao da um aviso e nao continua
	{
		//robo->parar();
		cout << "mapeamento_intersec sensores com leituras diferentes, press enter" << endl;
		//		while(!ev3dev::button::enter.process());
		//		usleep(1000000*0.1);
		//		while(!ev3dev::button::enter.process());
	}

	/*Primeira intersecção*/
	if(cor_atual == Cor::ndCor){
		qnt_cruzamentos += sentido_navegacao;
		cout << "primeira intersec" << endl;
		cor_atual = sensor->ler_cor_D();
		if(arq_existente){
			cout << "já sei o caminho" << endl;
			confirmacao_status = true;

			if(cor_atual == Cor::vermelho)
				direcao_atual = cp.checkpoint_vermelho;
			else if(cor_atual == Cor::verde)
				direcao_atual = cp.checkpoint_verde;
			else if(cor_atual == Cor::amarelo)
				direcao_atual = cp.checkpoint_amarelo;

			caminho_certo();
			//qnt_cores_mapeadas++; //TODO
		}
		else{
			direcao_atual = direcao::direita;
			robo->girar(-90);
			while(robo->get_estado() == flag_aceleracao::girar);
		}


		robo->andar(70);
		while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
	}

	/*Depois da primeira intersecção*/
	else{

		/*Enconro de Dead-end*/
		if(cor_E == Cor::preto || cor_D == Cor::preto){
			dead_end = true;
			cout << "cheguei Dead-end" << endl;
			robo->andar(50,0.05);
			robo->girar(180);
			while(robo->get_estado() == flag_aceleracao::girar);
			while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco){
				robo->andar(30);
			}
			robo->alinhar(sensor, direcao::traz);
			robo->andar(30);
			while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
		}

		/*Encontro de outra intersecção que nao eh preto*/
		else{

			/*
			 * Se não for a primeira intersecção nem dead-end, nem a volta de um dead-end tempos três opções:
			 * 1- Se for a volta de um dead-end tratar o "giro" para a proxima opção de caminho
			 * 2- se for uma cor que ainda não foi mapeada, pode ser a atual ou uma nova, setar a direcao da cor atual
			 *    e mapear cor nova, ou seguir caminho certo se for a mesma cor atual
			 * 3- se for uma cor conhecida (mapeada), setar a direcao da cor atual e seguir para cor certa
			 *
			 * Setar corretamente a confirmacao_status
			 */

			if(!dead_end) qnt_cruzamentos += sentido_navegacao;
			/*voltando de um dead-end*/
			if(dead_end){
				cout << "voltei Dead-end" << endl;
				robo->girar(-90);
				while(robo->get_estado() == flag_aceleracao::girar);

				if (direcao_atual == direcao::direita)
					direcao_atual = direcao::frente;
				else if(direcao_atual == direcao::frente)
					direcao_atual = direcao::esquerda;

				robo->andar(70);
				while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
				dead_end = false;
			}

			/*Cor nao mapeada, pode ser igual a atual ou outra cor*/
			else if(!cor_ja_mapeada())
			{
				cout << "cor nao mapeada" << endl;
				if(!confirmacao_status){ // se cor atual nao foi mapeada preveamente mapeia
					if(cor_atual == Cor::vermelho){
						cp.checkpoint_vermelho = direcao_atual;


						if(utilizar_arq)
							outfile << "0"  << direcao_atual << endl;
					}
					else if(cor_atual == Cor::verde){
						cp.checkpoint_verde = direcao_atual;

						if(utilizar_arq)
							outfile << "1"  << direcao_atual << endl;
					}
					else if (cor_atual == Cor::amarelo){
						cp.checkpoint_amarelo = direcao_atual;
						if(utilizar_arq)
							outfile << "2"  << direcao_atual << endl;
					}
					qnt_cores_mapeadas ++;
				}

				// se sim, seta a ultima direcao quando terminar de mapear a segunda
				// funcionamento: cada direcao eh um numero de 1 a 3,
				// se eu somar as duas primeiras direcoes ja conhecidas e diminuir de 6
				// eu saberei qual a direcao que falta, pra eu saber qual cor que nao foi
				// mapeada eh so eu pegar o endereco da cor que nao foi mapeada
				// entao se a cor for mapeada eu somo, se nao foi mapeada eu pego o endereco dela
				// no final eu verifico se realmente falta somente uma direcao/cor e atribuo a direcao
				if(automapear_3_checkpoint && qnt_cores_mapeadas == 2){
					int soma_direcoes = 0; // define qual direcao nao foi mapeada
					direcao * ultimo_checkpoint = nullptr;
					if( cp.checkpoint_vermelho != direcao::ndDirecao){
						soma_direcoes += cp.checkpoint_vermelho;
						//if(utilizar_arq) outfile << "0";
					}
					else  ultimo_checkpoint = &cp.checkpoint_vermelho;

					if( cp.checkpoint_verde != direcao::ndDirecao){
						soma_direcoes += cp.checkpoint_verde;
						//if(utilizar_arq) outfile << "1";
					}
					else  ultimo_checkpoint = &cp.checkpoint_verde;

					if( cp.checkpoint_amarelo != direcao::ndDirecao){
						soma_direcoes += cp.checkpoint_amarelo;
						//if(utilizar_arq) outfile << "2";
					}
					else  ultimo_checkpoint = &cp.checkpoint_amarelo;

					*ultimo_checkpoint = (direcao)(6-soma_direcoes);
					//if(utilizar_arq) outfile << (*ultimo_checkpoint) << endl;
					cout <<endl << endl << endl << "ultima direcao:"<< *ultimo_checkpoint << endl;
					cout << "ultima cor, seguir" << endl;
					usleep(1000000*0.1);
					caminho_certo();
					confirmacao_status = true;


					qnt_cores_mapeadas ++;
				}
				else if(cor_E != cor_atual){
					cout << "cor desconhecida" << endl;
					cor_atual = cor_E;
					direcao_atual = direcao::direita;
					robo->girar(-90);
					while(robo->get_estado() == flag_aceleracao::girar);
					confirmacao_status = false;
				}
				else{
					cout << "mesma cor, seguir" << endl;
					caminho_certo();
					confirmacao_status = true;
				}

				robo->andar(70);
				while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
				dead_end = false;
			}

			/*Cor ja mapeada, pode ser igual a atual ou outra cor*/
			else {
				cout << "Cor já mapeada" << endl;
				if (!confirmacao_status){
					if(cor_atual == Cor::vermelho){
						cp.checkpoint_vermelho = direcao_atual;
						if(utilizar_arq)
							outfile << "0"  << direcao_atual << endl;
					}
					else if(cor_atual == Cor::verde){
						cp.checkpoint_verde = direcao_atual;
						if(utilizar_arq)
							outfile << "1"  << direcao_atual << endl;
					}
					else if (cor_atual == Cor::amarelo){
						cp.checkpoint_amarelo = direcao_atual;
						if(utilizar_arq)
							outfile << "2"  << direcao_atual << endl;
					}
					qnt_cores_mapeadas ++;
				}

				if(cor_E != cor_atual){
					cout << "cor conhecida" << endl;
					cor_atual = cor_E;
				}
				else{
					cout << "mesma cor, seguir" << endl;
				}

				caminho_certo();
				confirmacao_status = true;

				robo->andar(70);
				while(sensor->ler_cor_E() == cor_E || sensor->ler_cor_D() == cor_D);
				dead_end = false;
			}
		}
	}
}

/* O robo gira pra direcao certa da nova cor, ou seja, intersecao que
 * ele esta sobre quando o metodo eh chamado, lembrando que a variavel cor_atual guarda a cor
 * da intersecao que o robo esta vindo e esta mapeando no momento
 */
void Mapeamento::caminho_certo (){
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

	while(robo->get_estado() == flag_aceleracao::girar);
}


/*
 * Retorna true se a nova cor ja estiver sido mapeada anteriormente, se for a mesma da cor atual
 * o metodo vai retornar false, pois a atribuicao de direcao eh feito depois do uso desse metodo
 * logo duas cores iguais seguidas mas nao mapeada anteriormente retorna false
 */
bool Mapeamento::cor_ja_mapeada(){
	switch(cor_E){
	case Cor::vermelho:
		if( cp.checkpoint_vermelho != direcao::ndDirecao) return true;
		break;

	case Cor::verde:
		if( cp.checkpoint_verde != direcao::ndDirecao) return true;
		break;

	case Cor::amarelo:
		if( cp.checkpoint_amarelo != direcao::ndDirecao) return true;
		break;

	}
	return false;
}

/*
 * quando eh necessario voltar e girar pra casos como o robo saindo da pista por exemplo
 * isso nao implica que o robo vai ficar reto, ele so "para de sair" da pista
 */
void Mapeamento::realinha(direcao lado_saindo) {
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


/*
 * metodo para fazer a saidinha da ultima intersec, ele eh chamado
 * no final do metodo mapear, quando o robo esta detro da ultima intersec
 * e de frente para a direcao correta, que seria a direcao da rampa
 */
void Mapeamento::saidinha_ultima_intersec() {
	/* funcionamento do metodo:
	 * o robo alinha de frente pra intersec, depois anda ate ver branco nos
	 * 2 sensores pra ter certeza q saiu da intersec;
	 * anda ate uma distancia ou o sensor nao ver mais branco;
	 * sensor nao viu mais branco, é a rampa logo depois da intersec;
	 * robo andou uma distancia vendo branco, é porque tem uma rua antes
	 * da rampa;
	 * sempre confirmando leitura para evitar leitura errada
	 */
	//TODO secundario tratar a rampa
	robo->alinhar(sensor, direcao::frente);
	robo->andar(20);
	while(sensor->ler_cor_E() != Cor::branco && sensor->ler_cor_D() != Cor::branco){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();
	} // testado, ve branco no inicio da rampa mesmo

	int cnt_nao_branco = 0; //
	while(sensor->ler_cor_E() == Cor::branco && sensor->ler_cor_D() == Cor::branco){

	}

	robo->parar();
	usleep(1000000*10);
	robo->andar(40);
	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();

		if (cor_E != Cor::branco || cor_D != Cor::branco){

		}

	}


}

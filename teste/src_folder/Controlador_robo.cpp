#include "Controlador_robo.h"


Controlador_robo::Controlador_robo(bool debug, string nome_arquivo)
:nome_arquivo(nome_arquivo),
 debug(debug)
{
	motorE = new ev3dev::large_motor(ev3dev::OUTPUT_A);
	motorD = new ev3dev::large_motor(ev3dev::OUTPUT_B);

	motorE->reset();
	motorD->reset();
	motorE->set_stop_action("hold");
	motorD->set_stop_action("hold");


	if(debug){
		arquivo = new M_arquivos(nome_arquivo, 5); // tempo, ang_motorE, ang_motorD, erro, pwm Direito
		arquivo_aberto = true;
	}
}


void Controlador_robo::andar (int pwm_sp){
	estado = flag_aceleracao::linha_reta;
	usleep(1000*10);
	this->pwm_sp = -pwm_sp;
}


void Controlador_robo::andar (int pwm_sp, double distancia){
	motorE->reset();
	motorD->reset();
	motorE->set_stop_action("hold");
	motorD->set_stop_action("hold");
	andar(pwm_sp);

	if(pwm_sp > 0) while(get_distancia() < distancia){}
	if(pwm_sp < 0) while(get_distancia() > -distancia){}

	parar();
}


void Controlador_robo::parar () {
	estado = flag_aceleracao::parar;
	usleep(1000*200);
}


void Controlador_robo::girar(int angulo_robo_graus){
	this->angulo_robo_graus = angulo_robo_graus;
	estado = flag_aceleracao::girar;
	usleep(1000*100);
}


void Controlador_robo::alinhar_para_traz(Sensor_cor *cor){
	int cor_E;
	int cor_D;
	int v = -15;
	do{
		andar(v, 0.01);
		cor_E = cor->ler_cor_E();
		cor_D = cor->ler_cor_D();
		v = v*(-1);
	}while(cor_E == Cor::nda || cor_D == Cor::nda);
	cout<<"alinhando:"<<cor_E<<";"<<cor_D<<endl;
	motorD->set_duty_cycle_sp(25);
	motorE->set_duty_cycle_sp(25); // sentido inverso devido a reducao de engrenagem
	motorE->run_direct();
	motorD->run_direct();
	bool motorE_parado = false,
			motorD_parado = false;
	while(!motorE_parado || !motorD_parado){
		if(cor->ler_cor_D() != cor_D && !motorD_parado){
			usleep(1000*80);
			if(cor->ler_cor_D() != cor_D){
				motorD->run_forever(); // pra n dar pau no stop
				motorD->stop();
				motorD_parado = true;
			}
		}
		if(cor->ler_cor_E() != cor_E && !motorE_parado){
			usleep(1000*80);
			if(cor->ler_cor_E() != cor_E){
				motorE->run_forever(); // pra n dar pau no stop
				motorE->stop();
				motorE_parado = true;
			}
		}
	}
	usleep(1000*300);
}


bool Controlador_robo::inicializar_thread_aceleracao(){
	thread_controle_velocidade = thread(&Controlador_robo::loop_controle_aceleracao, this);
	thread_controle_velocidade.detach();
	usleep(1000*500);
	return thread_rodando;
}


bool Controlador_robo::finalizar_thread_aceleracao(){
	if(thread_rodando){
		thread_rodando = false;
		while(arquivo_aberto){}
	}
	usleep(1000*100);
	return true;
}

flag_aceleracao Controlador_robo::get_estado(){
	return estado;
}

/*
 *  retorna a distancia em linha reta que o robo andou
 * ATENCAO: toda vez que o robo girar os tacometros sao resetados
 */
double Controlador_robo::get_distancia(){
	return distancia_linha_reta;
}

/*
 * retorna a velocidade do robo em m/s
 */
double Controlador_robo::get_velocidade(){
	return -(motorE->speed() + motorD->speed())/2*3.141592/180*relacao_engrenagem*raio_roda;
}

void Controlador_robo::loop_controle_aceleracao(){
	thread_rodando = true;
	t_inicial = Time::now();
	while (thread_rodando) {
		t_final = Time::now();
		delta_t = t_final - t_inicial;
		t_inicial = t_final;
		tempo += delta_t.count();
		arquivo->elementos_arq(tempo, (double)motorE->position(), (double)motorD->position(), erro, pwm);


		switch(estado){
		case flag_aceleracao::nd :
			usleep(1000*50);
			break;

		case flag_aceleracao::linha_reta :
			distancia_linha_reta =
					-(motorE->position()+motorD->position())/2 *3.141592/180*relacao_engrenagem*raio_roda;
			/*
			 * devido a relacao inverter o sentido de giro, um aumento na posicao da
			 * roda significa uma diminuicao na posicao do motor, sendo assim
			 * se a posicao do motorE > motorD entao a rodaE esta mais para traz
			 * em relacao a rodaD, entao o erro eh positivo
			 * se o robo estiver torto no sentido ANTI-HORARIO
			 */
			erro = (double)motorE->position()*fator_croda - (double)motorD->position();
			/*
			 * se o erro estiver positivo o proporcional (kp) sera positivo
			 * se o erro atual for maior que o erro anterior, ou seja, o erro aumentou
			 * o diferencial (kd) vai ser positivo, aumentando mais ainda o pid
			 */
			pid = kp*erro + kd*(erro-erro_anterior);
			erro_anterior = erro;


			/*
			 * erro positivo >> pid positivo >> motorE > motorD >> diminuir o pwm
			 * do motorE e aumentar o pwm do motorD
			 */
			pwm = pwm_retardada - pid;
			if(pwm > 100) pwm = 100;
			if(pwm < -100) pwm = -100;
			motorE->set_duty_cycle_sp((int)pwm);

			pwm = pwm_retardada + pid;
			if(pwm > 100) pwm = 100;
			if(pwm < -100) pwm = -100;
			motorD->set_duty_cycle_sp((int)pwm);
			usleep(1000*5);

			motorE->run_direct();
			motorD->run_direct();

			if(pwm_retardada < pwm_sp-aceleracao*delay/1000)
				pwm_retardada += aceleracao*delay/1000;
			if(pwm_retardada > pwm_sp+aceleracao*delay/1000)
				pwm_retardada -= aceleracao*delay/1000;
			break;

		case flag_aceleracao::girar :
			motorE->stop();
			motorD->stop();
			pwm_sp = 0;
			pwm_retardada = 0.0;
			pwm = 0;
			usleep(1000*300);
			motorE->set_position_sp((angulo_robo_graus*raio_robo/raio_roda)/relacao_engrenagem);
			motorD->set_position_sp(-(angulo_robo_graus*raio_robo/raio_roda)/relacao_engrenagem);
			motorE->set_speed_sp(300);
			motorD->set_speed_sp(300);
			motorE->run_to_rel_pos();
			motorD->run_to_rel_pos();
			usleep(1000*100);
			while((motorE->speed() > 2 || motorE->speed() < -2) &&
					estado == flag_aceleracao::girar){ }

			motorE->run_forever(); // so funciona se chamar o run_forever
			motorD->run_forever(); // antes de parar, caso contrario o robo fica louco
			motorE->stop();
			motorD->stop();
			usleep(1000*300);
			if(estado == flag_aceleracao::girar) // se o giro terminar a thread fica ociosa
				estado = flag_aceleracao::nd; // se for interrompida, vai para a proxima acao
			motorE->reset();
			motorD->reset();
			motorE->set_stop_action("hold");
			motorD->set_stop_action("hold");
			break;

		case flag_aceleracao::parar :
			motorE->stop();
			motorD->stop();
			pwm_sp = 0;
			pwm_retardada = 0.0;
			pwm = 0;
			erro = 0;
			estado = flag_aceleracao::nd;
			break;
		}
	}
	motorE->stop();
	motorD->stop();
	if(arquivo_aberto){
		arquivo->fecha_arq();
		arquivo->string_arq("plot(t,x4);");
		arquivo_aberto = false;
	}
}

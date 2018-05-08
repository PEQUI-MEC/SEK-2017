#include "Controlador_robo.h"


Controlador_robo::Controlador_robo(bool debug, string nome_arquivo)
:nome_arquivo(nome_arquivo),
 debug(debug)
{
	motorE = new ev3dev::large_motor(ev3dev::OUTPUT_A);
	motorD = new ev3dev::large_motor(ev3dev::OUTPUT_B);

	reset_motores();


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
	reset_motores();
	andar(pwm_sp);
	double posicao_atual = get_distancia_linha_reta();
	if(pwm_sp > 0)
		while(get_distancia_linha_reta() < distancia+posicao_atual);
	if(pwm_sp < 0)
		while(get_distancia_linha_reta() > -distancia+posicao_atual);

	parar();
}


void Controlador_robo::parar () {
	estado = flag_aceleracao::parar;
	while(estado != flag_aceleracao::ndAcel);
}


void Controlador_robo::girar(int angulo_robo_graus){
	this->angulo_robo_graus = angulo_robo_graus;
	reset_motores();
	estado = flag_aceleracao::girar;
	usleep(1000000*0.3);
	while(get_estado() == flag_aceleracao::girar);
}


void Controlador_robo::alinhar(Sensor_cor_hsv *cor, direcao dir){
	parar();
	int cor_E;
	int cor_D;
	int v_ajuste = -15;

	cor_E = cor->ler_cor_E();
	cor_D = cor->ler_cor_D();


	cout<<"alinhando:"<<cor_E<<";"<<cor_D<<endl;
	if(qnt_cruzamentos >= total_cruzamentos_teste){
		motorD->set_duty_cycle_sp(dir == direcao::traz?30:-30);
		motorE->set_duty_cycle_sp(dir == direcao::traz?32:-32); // sentido inverso devido a reducao de engrenagem
	}else {
		motorD->set_duty_cycle_sp(dir == direcao::traz?20:-20);
		motorE->set_duty_cycle_sp(dir == direcao::traz?22:-22); // sentido inverso devido a reducao de engrenagem
	}
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
	parar();
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


void Controlador_robo::alinha_portal(Sensor_cor_hsv *sensor_cor){
	alinhar(sensor_cor, direcao::traz);
	andar(30,0.2);
	girar(45);
	while(get_estado() == flag_aceleracao::girar);

	andar(30);
	while(sensor_cor->ler_cor_E() != Cor::fora);
	parar();
	andar(-30,0.06);
	girar(-44);
	while(get_estado() == flag_aceleracao::girar);
	andar(50);
}

/*
 * retorna o atual estado de movimento do robo,
 * flag_aceleracao{ndAcel, linha_reta, parar, girar};
 */
flag_aceleracao Controlador_robo::get_estado(){
	return estado;
}


/*
 *  retorna a distancia em linha reta que o robo andou
 * ATENCAO: toda vez que o robo girar os tacometros sao resetados
 */
double Controlador_robo::get_distancia_linha_reta(){
	return distancia_linha_reta;
}


/*
 * retorna a distancia absoluta que o robo andou total
 * desde o inicio do codigo ou desde o ultimo reset_distancia_absoluta
 */
double Controlador_robo::get_distancia_absoluta(){
	return distancia_absoluta + distancia_linha_reta;
}


/*
 * reseta a distancia_absoluta
 */
void Controlador_robo::reset_distancia_absoluta(){
	distancia_absoluta = 0;
}


/*
 * retorna a velocidade do robo em m/s
 */
double Controlador_robo::get_velocidade(){
	return -(motorE->speed() + motorD->speed())/2*3.141592/180*relacao_engrenagem*raio_roda;
}


/*
 * retorna o tamanho do meu pintao
 * distancia entre o eixo e os sensores de cor
 */
double Controlador_robo::get_pintao(){
	return tamanho_do_pintao;
}


/*
 * retorna o pwm_sp considerando positivo para frente
 */
double Controlador_robo::get_pwm_sp(){
	return -pwm_sp;
}

void Controlador_robo::reset_motores(){
	motorE->reset();
	motorD->reset();
	motorE->set_stop_action("hold");
	motorD->set_stop_action("hold");
	distancia_absoluta += distancia_linha_reta;
	distancia_linha_reta = 0;
}


void Controlador_robo::loop_controle_aceleracao(){
	thread_rodando = true;
	t_inicial = Time::now();
	while (thread_rodando) {
		t_final = Time::now();
		delta_t = t_final - t_inicial;
		t_inicial = t_final;
		tempo += delta_t.count();
		if(debug)
			arquivo->elementos_arq(tempo, (double)motorE->position(), (double)motorD->position(), erro, pwm);


		switch(estado){
		case flag_aceleracao::ndAcel :
			//usleep(1000*50);
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
			//usleep(1000*300);
			motorE->set_position_sp((angulo_robo_graus*raio_robo/raio_roda)/relacao_engrenagem);
			motorD->set_position_sp(-(angulo_robo_graus*raio_robo/raio_roda)/relacao_engrenagem);
			motorE->set_speed_sp(300);
			motorD->set_speed_sp(300);
			motorE->run_to_rel_pos();
			motorD->run_to_rel_pos();
			usleep(1000000*0.1);
			while((motorE->speed() > 2 || motorE->speed() < -2) &&
					estado == flag_aceleracao::girar){ }

			motorE->run_forever(); // so funciona se chamar o run_forever
			motorD->run_forever(); // antes de parar, caso contrario o robo fica louco
			motorE->stop();
			motorD->stop();
			//usleep(1000*300);
			if(estado == flag_aceleracao::girar) // se o giro terminar a thread fica ociosa
				estado = flag_aceleracao::ndAcel; // se for interrompida, vai para a proxima acao
			reset_motores();
			break;

		case flag_aceleracao::parar :
			motorE->stop();
			motorD->stop();
			//usleep(1000000*0.1);
			reset_motores();
			pwm_sp = 0;
			pwm_retardada = 0.0;
			pwm = 0;
			erro = 0;
			erro_anterior = 0;
			pid = 0;
			estado = flag_aceleracao::ndAcel;
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

void Controlador_robo::calibra_sensor_cor(Sensor_cor_hsv *sensor_cor) {
	bool usar_minimo = false; // usar o minimo pra calcular o fator?
	// referencia dos sensores de cor
	ev3dev::color_sensor *sensor_E = sensor_cor->get_sensor_E();
	ev3dev::color_sensor *sensor_D = sensor_cor->get_sensor_D();
	tuple<int,int,int> sample_E, sample_D; // amostra dos sensores

	/*valores de maximo para o branco e minimo para o preto
	 */
	double max_rgb_E[3] = {0,0,0},
			max_rgb_D[3] = {0,0,0},
			min_rgb_E[3] = {0,0,0},
			min_rgb_D[3] = {0,0,0};
	vector<RGB> rgb_branco_E, rgb_branco_D, rgb_preto_E, rgb_preto_D;



	// calcula os valores maximos no branco enquanto o robo anda
	cout << "calibra branco andando" << endl;
	motorE->set_speed_sp(-200);
	motorD->set_speed_sp(-200);
	while(!ev3dev::button::enter.process());
	usleep(0.3*1000000);
	ev3dev::button::enter.process();
	motorE->run_forever();
	motorD->run_forever();
	while(!ev3dev::button::enter.process()){
		sample_E = sensor_E->raw();
		sample_D = sensor_D->raw();

		RGB rgbe, rgbd;
		rgbe.r = get<0>(sample_E);
		rgbe.g = get<1>(sample_E);
		rgbe.b = get<2>(sample_E);
		rgb_branco_E.push_back(rgbe);
		rgbd.r = get<0>(sample_D);
		rgbd.g = get<1>(sample_D);
		rgbd.b = get<2>(sample_D);
		rgb_branco_D.push_back(rgbd);

		if(max_rgb_E[0] < get<0>(sample_E)) max_rgb_E[0] = get<0>(sample_E);// essa buceta nao funciona dentro de um for
		if(max_rgb_D[0] < get<0>(sample_D)) max_rgb_D[0] = get<0>(sample_D);
		if(max_rgb_E[1] < get<1>(sample_E)) max_rgb_E[1] = get<1>(sample_E);
		if(max_rgb_D[1] < get<1>(sample_D)) max_rgb_D[1] = get<1>(sample_D);
		if(max_rgb_E[2] < get<2>(sample_E)) max_rgb_E[2] = get<2>(sample_E);
		if(max_rgb_D[2] < get<2>(sample_D)) max_rgb_D[2] = get<2>(sample_D);
	}
	motorE->stop();
	motorD->stop();
	cout << endl << endl
			<< "valores max:"<< endl
			<< max_rgb_E[0] << ";" << max_rgb_E[1] << ";" << max_rgb_E[2] << ";" << endl
			<< max_rgb_D[0] << ";" << max_rgb_D[1] << ";" << max_rgb_D[2] << ";" << endl;
	usleep(0.3*1000000);
	ev3dev::button::enter.process();



	// calibra os valores minimos no preto enquanto o robo anda
	cout << "calibra preto andando" << endl;
	if(usar_minimo){
		for(int j = 0 ; j < 3 ; j++){
			min_rgb_E[j] = 500;
			min_rgb_D[j] = 500;
		}
	}
	while(!ev3dev::button::enter.process());
	usleep(0.3*1000000);
	ev3dev::button::enter.process();
	motorE->run_forever();
	motorD->run_forever();
	while(!ev3dev::button::enter.process()){
		sample_E = sensor_E->raw();
		sample_D = sensor_D->raw();

		RGB rgbe, rgbd;
		rgbe.r = get<0>(sample_E);
		rgbe.g = get<1>(sample_E);
		rgbe.b = get<2>(sample_E);
		rgb_preto_E.push_back(rgbe);
		rgbd.r = get<0>(sample_D);
		rgbd.g = get<1>(sample_D);
		rgbd.b = get<2>(sample_D);
		rgb_preto_D.push_back(rgbd);

		if(usar_minimo){
			if(min_rgb_E[0] > get<0>(sample_E)) min_rgb_E[0] = get<0>(sample_E);// essa buceta nao funciona dentro de um for
			if(min_rgb_D[0] > get<0>(sample_D)) min_rgb_D[0] = get<0>(sample_D);
			if(min_rgb_E[1] > get<1>(sample_E)) min_rgb_E[1] = get<1>(sample_E);
			if(min_rgb_D[1] > get<1>(sample_D)) min_rgb_D[1] = get<1>(sample_D);
			if(min_rgb_E[2] > get<2>(sample_E)) min_rgb_E[2] = get<2>(sample_E);
			if(min_rgb_D[2] > get<2>(sample_D)) min_rgb_D[2] = get<2>(sample_D);
		}
	}
	motorE->stop();
	motorD->stop();
	cout << endl << endl
			<< "valores min:"<< endl
			<< min_rgb_E[0] << ";" << min_rgb_E[1] << ";" << min_rgb_E[2] << ";" << endl
			<< min_rgb_D[0] << ";" << min_rgb_D[1] << ";" << min_rgb_D[2] << ";" << endl;
	usleep(0.3*1000000);
	ev3dev::button::enter.process();

	reset_motores();



	/*
	 * calcula o fator de escala do rgb dos 2 sensores pra colocar a leitura de 0 a 255 para
	 * uma correta conversao de rgb pra hsv
	 */

	for(int i = 0; i < 3 ; i++){
		max_rgb_E[i] = 255/(max_rgb_E[i] - min_rgb_E[i]);
		max_rgb_D[i] = 255/(max_rgb_D[i] - min_rgb_D[i]);
	}
	sensor_cor->set_fatores_rgb(max_rgb_E, max_rgb_D);



	/*
	 * calcula os limites nos valores hsv para ser
	 * as condicoes de avaliacao da cor quando realizar leitura
	 */
	HSV hsv;
	double valores_E[4], valores_D[4];
	valores_E[0] = 0; // maximo_S_Branco_E
	valores_E[1] = 2; // minimo_V_Branco_E
	valores_E[2] = 0; // maximo_V_Preto_E
	valores_E[3] = 2; // minimo_V_Preto_E

	valores_D[0] = 0; // maximo_S_Branco_D
	valores_D[1] = 2; // minimo_V_Branco_D
	valores_D[2] = 0; // maximo_V_Preto_D
	valores_D[3] = 2; // minimo_V_Preto_D

	for(int i = 0 ; i < rgb_branco_E.size() ; i++){
		// muda a escala do rgb antes de converter pra hsv
		rgb_branco_E[i].r *= max_rgb_E[0];
		rgb_branco_E[i].g *= max_rgb_E[1];
		rgb_branco_E[i].b *= max_rgb_E[2];
		rgb_branco_D[i].r *= max_rgb_D[0];
		rgb_branco_D[i].g *= max_rgb_D[1];
		rgb_branco_D[i].b *= max_rgb_D[2];

		hsv = sensor_cor->RGBtoHSV(rgb_branco_E[i]);
		if(valores_E[0] < hsv.s) valores_E[0] = hsv.s;
		if(valores_E[1] > hsv.v) valores_E[1] = hsv.v;
		hsv = sensor_cor->RGBtoHSV(rgb_branco_D[i]);
		if(valores_D[0] < hsv.s) valores_D[0] = hsv.s;
		if(valores_D[1] > hsv.v) valores_D[1] = hsv.v;
	}

	for(int i = 0 ; i < rgb_preto_E.size() ; i++){
		// muda a escala do rgb antes de converter pra hsv
		rgb_preto_E[i].r *= max_rgb_E[0];
		rgb_preto_E[i].g *= max_rgb_E[1];
		rgb_preto_E[i].b *= max_rgb_E[2];
		rgb_preto_D[i].r *= max_rgb_D[0];
		rgb_preto_D[i].g *= max_rgb_D[1];
		rgb_preto_D[i].b *= max_rgb_D[2];

		hsv = sensor_cor->RGBtoHSV(rgb_preto_E[i]);
		if(valores_E[2] < hsv.v) valores_E[2] = hsv.v;
		if(valores_E[3] > hsv.v) valores_E[3] = hsv.v;
		hsv = sensor_cor->RGBtoHSV(rgb_preto_D[i]);
		if(valores_D[2] < hsv.v) valores_D[2] = hsv.v;
		if(valores_D[3] > hsv.v) valores_D[3] = hsv.v;
	}
	if(arena_pequi){
		valores_E[0] += 0.02; // maximo_s_Branco_E
		valores_E[1] -= 0.066; // minimo_V_Branco_E
		valores_E[2] += 0.05; // maximo_V_Preto_E
		valores_E[3] -= 0.005; // minimo_V_Preto_E

		valores_D[0] += 0.02; // maximo_s_Branco_D
		valores_D[1] -= 0.066; // minimo_V_Branco_D
		valores_D[2] += 0.05; // maximo_V_Preto_D
		valores_D[3] -= 0.005; // minimo_V_Preto_D
	}else{
		valores_E[0] += 0.01; // maximo_s_Branco_E
		valores_E[1] -= 0.05; // minimo_V_Branco_E
		valores_E[2] += 0.05; // maximo_V_Preto_E
		valores_E[3] -= 0.005; // minimo_V_Preto_E

		valores_D[0] += 0.01; // maximo_s_Branco_D
		valores_D[1] -= 0.05; // minimo_V_Branco_D
		valores_D[2] += 0.05; // maximo_V_Preto_D
		valores_D[3] -= 0.005; // minimo_V_Preto_D
	}
	sensor_cor->set_maximos_minimos(valores_E, valores_D);
	cout <<"min branco:" << endl << valores_E[0] << ";" << valores_D[0] << endl;
}

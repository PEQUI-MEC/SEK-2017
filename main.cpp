#include <iostream>
#include <unistd.h>
#include <chrono>
#include <string>
#include "M_arquivos.h"
#include "src_folder/Sensor_cor_hsv.h"
#include "src_folder/Controlador_robo.h"
#include "src_folder/Mapeamento.h"
#include "src_folder/Resgate.h"
#include "src_folder/Ultrassom.h"

#include "src_folder/Garra.h"

using namespace std;


typedef chrono::high_resolution_clock Time;


void teste_rogerio(){
	Controlador_robo robot(false, "debug posicao direto no pwm.m");
	Sensor_cor_hsv cor(ev3dev::INPUT_1, ev3dev::INPUT_2,false,"leitura_sensor_cor_hsv");
	Ultrassom ultraE(ev3dev::INPUT_3);
	Ultrassom ultraD(ev3dev::INPUT_4);
	Mapeamento mapa(&robot, &cor);
	Resgate resgate(&robot, &cor, &ultraE, &ultraD, ev3dev::OUTPUT_C, ev3dev::OUTPUT_D);


	robot.inicializar_thread_aceleracao();
	/*
	 * teste da classe controlador_robo
	 */
	//	while(!ev3dev::button::enter.process());
	//	usleep(1000000*0.1);
	//	while(!ev3dev::button::enter.process());
	//	robot.andar(50, 0.3);
	//	cout << robot.get_distancia_absoluta() << ";" << robot.get_distancia_linha_reta() << endl;
	//	usleep(1000000*5);
	//	robot.andar(50, 0.3);
	//	cout << robot.get_distancia_absoluta() << ";" << robot.get_distancia_linha_reta() << endl;
	//	while(!ev3dev::button::enter.process())
	//		cout<<robot.get_velocidade()<<endl;
	//	usleep(1000*2000);
	//	robot.girar(360*2);
	//	usleep(1000*2000);
	//	robot.andar(50);
	//	usleep(1000*2000);
	//	robot.andar(-50);
	//	usleep(1000*2000);
	//	robot.girar(-360);
	//while(!ev3dev::button::enter.process());

	/*
	 * teste classe controlador_robo medir o tamanho do pintao
	 */
	//	while(!ev3dev::button::enter.process());
	//	usleep(1000000*0.1);
	//	while(!ev3dev::button::enter.process());
	//	robot.andar(50);
	//	while(cor.ler_cor_E() == Cor::branco);
	//	robot.parar();
	//	robot.andar(20, 0.058); // o tamanho do pintao eh 0.058
	//	robot.andar(20, 0.15); // testando se ele vai parar no meio mesmo
	//	robot.girar(-90);

	/*
	 * teste classe mapeamento com classe sensor cor hsv
	 */
	//robot.calibra_sensor_cor(&cor);
	//cor.fecha_arquivo();
	//robot.andar(-40);
	cout << endl << endl << endl << "comecar?" << endl;
		while(!ev3dev::button::enter.process());
		usleep(1000000*0.1);
		while(!ev3dev::button::enter.process());
		cout << endl << endl << endl << endl << endl << endl;
	//mapa.saidinha_ultima_intersec();
	//cp.checkpoint_verde = direcao::direita;
	//cp.checkpoint_amarelo = direcao::esquerda;

	mapa.mapear();
	//	cout <<"SAIU DO MAPEAMENTO JA"<<endl;
	//	Garra cancela(ev3dev::OUTPUT_C, 48, "cancela");
	//	cancela.abrir();
	//	usleep(1000000);
	//	cancela.fechar();
	//	usleep(1000000);
	//	robot.andar(-70);
//		while(!ev3dev::button::enter.process());
//		usleep(1000000*0.1);
//		while(!ev3dev::button::enter.process());
	//usleep(1000000*2);
	//resgate.ir_para_final();
	//cout << endl << endl << "PODE INICIAR CAPTURA"<< endl;
	//robot.girar(360);
	//qnt_cruzamentos = total_cruzamentos_teste;
	resgate.resgatar();


	/*
	 * teste do alinhamento do portal
	 */
	//	robot.calibra_sensor_cor(&cor);
	//	while(!ev3dev::button::enter.process());
	//	usleep(1000000*0.1);
	//	while(!ev3dev::button::enter.process());
	//	robot.alinha_portal(&cor);


	/*
	 * teste classe Garra
	 */
	//	Garra garra(ev3dev::OUTPUT_D, 45);
	//	while(!ev3dev::button::enter.process());
	//	usleep(1000000*0.1);
	//	while(!ev3dev::button::enter.process());
	//	garra.abrir();
	//	usleep(1000000*1);
	//	garra.fechar();
	//	usleep(1000000*1);
	//	robot.andar(60);
	//	while(!ev3dev::button::enter.process());
	//	usleep(1000000*0.1);
	//	while(!ev3dev::button::enter.process());
	//	robot.parar();


	/*
	 * teste da classe sensor_cor_hsv
	 * pega alguns valores e guarda no arquivo
	 */
	//	//robot.calibra_sensor_cor(&cor);
	//			while(!ev3dev::button::up.pressed()){
	//				while(!ev3dev::button::enter.pressed() && !ev3dev::button::up.pressed());
	//				while(ev3dev::button::enter.pressed());
	//				//robot.andar(30);
	//				while(!ev3dev::button::enter.pressed() && !ev3dev::button::up.pressed()){
	//					cout<<cor.ler_cor_E()<<";"<<cor.ler_cor_D()<<endl;
	//					usleep(1000000*0.1);
	//				}
	//				robot.parar();
	//				usleep(1000000*0.01);
	//				while(ev3dev::button::enter.pressed());
	//			}
	//			cor.fecha_arquivo();

	/*
	 * teste da classe de cor
	 * define os ranges de quanto a leitura da cor varia
	 */
	//	while(!ev3dev::button::back.process()){
	//		while(!ev3dev::button::enter.process()){}
	//		usleep(1000*800);
	//		ev3dev::button::enter.process();
	//		robot.andar(30);
	//		cor.define_ranges();
	//		robot.parar();
	//		usleep(1000*800);
	//		ev3dev::button::enter.process();
	//	}


	/*
	 * teste da classe de cor
	 * imprime a leitura rgb do sensor esquerdo
	 */
	//	while(!ev3dev::button::back.process()){
	//		cor.printa_rgb_E();
	//	}


	/*
	 * VERIFICA SE A VELOCIDADE LIDA EH A VELOCIDADE REAL
	 * RESULTADOS BEM SATISFATORIOS, PRECISAO DE 0.001 m/s
	 */
	//	double 	velo_robo = 0,
	//			dist = 0,
	//			ang_robo = 0,
	//			posicao_inicial = 0;
	//	chrono::system_clock::time_point t_inicial;
	//	chrono::system_clock::time_point t_final;
	//	chrono::duration<double> delta_t; //Usar dt = t1-t2 e dt.count() para pegar o tempo em seg
	//	t_inicial = Time::now();
	//	robot.andar(70);
	//	while(robot.get_distancia() < 3){}
	//	velo_robo = robot.get_velocidade();
	//	robot.parar();
	//	t_final = Time::now();
	//	delta_t = t_final - t_inicial;
	//	cout<<3/delta_t.count()<<endl;
	//	cout<<velo_robo<<endl;


	/*
	 * TESTE ANDAR RE
	 */
	//robot.andar(-50, 0.5);


	/*
	 * TESTE COM SENSOR TOUCH PARA VERIFICAR TEMPO
	 */
	//	ev3dev::touch_sensor toque(ev3dev::INPUT_1);
	//	while(!ev3dev::button::enter.process()){
	//		//while(!toque.is_pressed()){}
	//		t_inicial = Time::now();
	//		//toque.is_pressed();
	//		//toque.is_pressed();
	//		//toque.is_pressed();
	//		usleep(1000*1000);
	//		//while(toque.is_pressed()){}
	//		t_final = Time::now();
	//		delta_t = t_final - t_inicial;
	//		cout<<delta_t.count()<<endl;
	//		usleep(1000*3000);
	//	}

	while(!ev3dev::button::enter.process());
	robot.parar();
	usleep(1000*500);
	robot.finalizar_thread_aceleracao();
}


void competicao(){
	Controlador_robo robot(false, "debug posicao direto no pwm.m");
	Sensor_cor_hsv cor(ev3dev::INPUT_1, ev3dev::INPUT_2,false,"leitura_sensor_cor_hsv");
	Ultrassom ultraE(ev3dev::INPUT_3);
	Ultrassom ultraD(ev3dev::INPUT_4);
	Mapeamento mapa(&robot, &cor);
	Resgate resgate(&robot, &cor, &ultraE, &ultraD, ev3dev::OUTPUT_C, ev3dev::OUTPUT_D);


	robot.inicializar_thread_aceleracao();

	mapa.mapear();
	cout <<"SAIU DO MAPEAMENTO JA"<<endl;

	//usleep(1000000*2);
	//resgate.ir_para_final();
	//cout << endl << endl << "PODE INICIAR CAPTURA"<< endl;
	//qnt_cruzamentos = total_cruzamentos_teste;
	resgate.resgatar();
}

void realinha(Controlador_robo *robo, direcao lado_saindo) {
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

void go_to_plaza(Controlador_robo *robo, Sensor_cor_hsv *sensor, Ultrassom *ultraE, Garra *cancela) {
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

	cancela->abrir();

	robo->andar(-70);
	while(sensor->ler_cor_E() != Cor::branco || sensor->ler_cor_D() != Cor::branco);
	usleep(100000);

	robo->parar();
	//cancela->fechar();

	//robo.alinhar(&sensor, direcao::frente);

	cancela->fechar();

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
			realinha(robo, direcao::esquerda);
		else if (cor_D == Cor::fora)
			realinha(robo, direcao::direita);

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

void go_to_plaza2(Controlador_robo *robo, Sensor_cor_hsv *sensor, Ultrassom *ultraE){
	Cor cor_E = Cor::ndCor;
	Cor	cor_D = Cor::ndCor;
	int count_nwhite = 0;

	robo->andar(60);
	while(true){
		cor_E = sensor->ler_cor_E();
		cor_D = sensor->ler_cor_D();

		cout << cor_E << "   " << cor_D << endl;

		if(cor_E == Cor::verde && cor_D == Cor::verde){
			usleep(1000000*0.1);
			robo->alinhar(sensor, direcao::traz);
		}

		if(count_nwhite >= 10) break;

		if(cor_E != Cor::branco && cor_D != Cor::branco)
			count_nwhite++;
		else count_nwhite = 0;
		usleep(1000000*0.1);
	}
	robo->parar();

	robo->andar(40, 0.25);


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


void teste_rampa(){
	Controlador_robo robo(false, "debug posicao direto no pwm.m");
	Sensor_cor_hsv cor(ev3dev::INPUT_1, ev3dev::INPUT_2,false,"leitura_sensor_cor_hsv");
	Ultrassom ultraE(ev3dev::INPUT_3);
	Ultrassom ultraD(ev3dev::INPUT_4);
	Mapeamento mapa(&robo, &cor);
	//Resgate resgate(&robo, &cor, &ultraE, &ultraD);
	Garra g(ev3dev::OUTPUT_C, 40, "cancela");



	robo.inicializar_thread_aceleracao();

	cout << "Teste rampa!!!" << endl;
	while(!ev3dev::button::enter.process());

	robo.andar(70);
	usleep(1000000*2);
	while(cor.ler_cor_E() != Cor::branco || cor.ler_cor_D() != Cor::branco);
	//go_to_plaza(&robo, &cor, &ultraE );
}



int main(){
	system("setfont Greek-TerminusBold20x10.psf.gz");

	teste_rogerio();
	//teste_rampa();

	ev3dev::button::back.pressed();

	cout << "Teste finalizado. Bye!" << endl;
	usleep (1000000);
	return 0;
}

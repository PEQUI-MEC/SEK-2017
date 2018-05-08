#include "Sensor_cor_hsv.h"

static double Min(double a, double b) {
	return a <= b ? a : b;
}

static double Max(double a, double b) {
	return a >= b ? a : b;
}

Sensor_cor_hsv::Sensor_cor_hsv(string sensor_port_E, string sensor_port_D,
		bool debug, string nome_arquivo)
:sensor_E(ev3dev::color_sensor(sensor_port_E)),
 sensor_D(ev3dev::color_sensor(sensor_port_D)),
 debug(debug)
{
	if(debug){
		string s = "_E.m";
		s = nome_arquivo + s;
		arquivo_E = new M_arquivos(s, 7); // tempo, 3rgb, 3hsv
		s = "_D.m";
		s = nome_arquivo + s;
		arquivo_D = new M_arquivos(s, 7); // tempo, 3rgb, 3hsv
	}

	if(arena_pequi){
		limites_H_Vermelho_E[0] = 22;// verificar se esta fora
		limites_H_Vermelho_E[1] = 340;
		limites_H_Verde_E[0] = 110; // verificar se esta dentro
		limites_H_Verde_E[1] = 150;
		limites_H_Amarelo_E[0] = 22; // verificar se esta dentro
		limites_H_Amarelo_E[1] = 50;

		limites_H_Vermelho_D[0] = 24;// verificar se esta fora
		limites_H_Vermelho_D[1] = 340;
		limites_H_Verde_D[0] = 100; // verificar se esta dentro
		limites_H_Verde_D[1] = 150;
		limites_H_Amarelo_D[0] = 24; // verificar se esta dentro
		limites_H_Amarelo_D[1] = 50;
	}
}


Cor Sensor_cor_hsv::ler_cor_E() {
	tuple<int, int, int> sample;
	sample = sensor_E.raw();
	RGB rgb;
	rgb.r = get<0>(sample);
	rgb.g = get<1>(sample);
	rgb.b = get<2>(sample);
	rgb.r *= fator_escalimetro_rgb_E[0];
	rgb.g *= fator_escalimetro_rgb_E[1];
	rgb.b *= fator_escalimetro_rgb_E[2];

	HSV hsv = RGBtoHSV(rgb);

	if(debug) arquivo_E->elementos_arq((double)0,
			(double)rgb.r, (double)rgb.g, (double)rgb.b,
			(double)hsv.h, (double)hsv.s, (double)hsv.v);

	if(hsv.v > minimo_V_Branco_E)
		if( hsv.s < maximo_S_Branco_E) return Cor::branco;

	if(arena_pequi){
		if(hsv.v < maximo_V_Preto_E)
			if((125 - range_H_preto_fora < hsv.h) &&
					(hsv.h < 125 + range_H_preto_fora)) return Cor::preto;
			else
				return Cor::fora;
	}
	else{
		if(hsv.v > minimo_V_Preto_E && hsv.v < maximo_V_Preto_E) return Cor::preto;
		if(hsv.v < minimo_V_Preto_E) return Cor::fora;
	}

	if(hsv.h < limites_H_Vermelho_E[0] || hsv.h > limites_H_Vermelho_E[1]) return Cor::vermelho;
	if(hsv.h > limites_H_Verde_E[0] && hsv.h < limites_H_Verde_E[1]) return Cor::verde;
	if(hsv.h > limites_H_Amarelo_E[0] && hsv.h < limites_H_Amarelo_E[1])return Cor::amarelo;
	return Cor::ndCor;
}


Cor Sensor_cor_hsv::ler_cor_D() {
	tuple<int, int, int> sample;
	sample = sensor_D.raw();
	RGB rgb;
	rgb.r = get<0>(sample);
	rgb.g = get<1>(sample);
	rgb.b = get<2>(sample);
	rgb.r *= fator_escalimetro_rgb_D[0];
	rgb.g *= fator_escalimetro_rgb_D[1];
	rgb.b *= fator_escalimetro_rgb_D[2];

	HSV hsv = RGBtoHSV(rgb);

	if(debug) arquivo_D->elementos_arq((double)0,
			(double)rgb.r, (double)rgb.g, (double)rgb.b,
			(double)hsv.h, (double)hsv.s, (double)hsv.v);

	if(hsv.v > minimo_V_Branco_D)
		if( hsv.s < maximo_S_Branco_D) return Cor::branco;

	if(arena_pequi){
		if(hsv.v < maximo_V_Preto_D)
			if((104 - range_H_preto_fora < hsv.h) &&
					(hsv.h < 104 + range_H_preto_fora)) return Cor::preto;
			else
				return Cor::fora;
	}
	else{
		if(hsv.v > minimo_V_Preto_D && hsv.v < maximo_V_Preto_D) return Cor::preto;
		if(hsv.v < minimo_V_Preto_D) return Cor::fora;
	}

	if(hsv.h < limites_H_Vermelho_D[0] || hsv.h > limites_H_Vermelho_D[1]) return Cor::vermelho;
	if(hsv.h > limites_H_Verde_D[0] && hsv.h < limites_H_Verde_D[1]) return Cor::verde;
	if(hsv.h > limites_H_Amarelo_D[0] && hsv.h < limites_H_Amarelo_D[1])return Cor::amarelo;

	return Cor::ndCor;
}

void Sensor_cor_hsv::set_fatores_rgb(double *valores_E, double *valores_D) {
	fator_escalimetro_rgb_E[0] = valores_E[0];
	fator_escalimetro_rgb_E[1] = valores_E[1];
	fator_escalimetro_rgb_E[2] = valores_E[2];


	fator_escalimetro_rgb_D[0] = valores_D[0];
	fator_escalimetro_rgb_D[1] = valores_D[1];
	fator_escalimetro_rgb_D[2] = valores_D[2];


	if(debug)
		cout << "fatores_r:" << endl
		<< fator_escalimetro_rgb_E[0] << ";" << fator_escalimetro_rgb_D[0] << endl;
}

void Sensor_cor_hsv::set_maximos_minimos(double *maximos_E, double *maximos_D){
	maximo_S_Branco_E = maximos_E[0];
	minimo_V_Branco_E  = maximos_E[1];
	maximo_V_Preto_E  = maximos_E[2];
	minimo_V_Preto_E = maximos_E[3];

	maximo_S_Branco_D = maximos_D[0];
	minimo_V_Branco_D  = maximos_D[1];
	maximo_V_Preto_D  = maximos_D[2];
	minimo_V_Preto_D = maximos_D[3];
}


HSV Sensor_cor_hsv::RGBtoHSV(RGB rgb){
	HSV hsv;
	double delta, min;

	min = Min(Min(rgb.r, rgb.g), rgb.b);
	hsv.v = Max(Max(rgb.r, rgb.g), rgb.b);
	delta = hsv.v - min;

	if (hsv.v == 0.0)
		hsv.s = 0;
	else
		hsv.s = delta / hsv.v;

	if (hsv.s == 0)
		hsv.h = 0.0;

	else
	{
		if (rgb.r == hsv.v)
			hsv.h = (rgb.g - rgb.b) / delta;
		else if (rgb.g == hsv.v)
			hsv.h = 2 + (rgb.b - rgb.r) / delta;
		else if (rgb.b == hsv.v)
			hsv.h = 4 + (rgb.r - rgb.g) / delta;

		hsv.h *= 60;

		if (hsv.h < 0.0)
			hsv.h = hsv.h + 360;
	}
	hsv.v = hsv.v/255;
	return hsv;
}

void Sensor_cor_hsv::fecha_arquivo(){
	if(debug){
		arquivo_E->fecha_arq();
		arquivo_D->fecha_arq();
	}
	string strr = "%maximos e minimos: \n% " +
			to_string(fator_escalimetro_rgb_E[0])+ "\n% " +
			to_string(fator_escalimetro_rgb_E[1])+ "\n% " +
			to_string(fator_escalimetro_rgb_E[2])+ "\n% " +
			to_string(maximo_S_Branco_E) + "\n% " +
			to_string(minimo_V_Branco_E) + "\n% " +
			to_string(maximo_V_Preto_E) + "\n% " +
			to_string(minimo_V_Preto_E) + "\n" +
			"plot(x4);";
	arquivo_E->string_arq(strr);
	strr = "%maximos e minimos: \n% " +
			to_string(fator_escalimetro_rgb_D[0])+ "\n% " +
			to_string(fator_escalimetro_rgb_D[1])+ "\n% " +
			to_string(fator_escalimetro_rgb_D[2])+ "\n% " +
			to_string(maximo_S_Branco_D) + "\n% " +
			to_string(minimo_V_Branco_D) + "\n% " +
			to_string(maximo_V_Preto_D) + "\n% " +
			to_string(minimo_V_Preto_D) + "\n" +
			"plot(x4);";
	arquivo_D->string_arq(strr);
	debug = false;
}

ev3dev::color_sensor *Sensor_cor_hsv::get_sensor_E() {
	return &sensor_E;
}

ev3dev::color_sensor *Sensor_cor_hsv::get_sensor_D() {
	return &sensor_D;
}

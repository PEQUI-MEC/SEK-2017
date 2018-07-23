#ifndef M_ARQUIVOS_H_
#define M_ARQUIVOS_H_
#include <iostream>
#include <stdarg.h>
#include <fstream>
#include <list>
#include <string>

using namespace std;

class M_arquivos {
public:
	M_arquivos ()
:nome_arquivo (""), tamanho_coluna (0){}

	M_arquivos (string nome, int tamanho)
	: nome_arquivo (nome), tamanho_coluna (tamanho) {}


	/*
	 * Método elementos_arq extrai todas as variáveis passadas por parâmetro
	 * e coloca numa lista de elementos.
	 * A lista estará no modelo:
	 * t1,x1.1, x2.1, ..., xn.1, ..., tm, xm.1, xm.2, ..., xm.n
	 */
	void elementos_arq (double xn, ...);


	/*
	 *  Metódo fecha_arq pega a lista gerada a cada chamada da função elementos_arq
	 *  e escreve em um arquivo.
	 *  O arquivo estará no modelo:
	 *
	 *  t = [t1 t2 t3 ... tn];
	 *  x1 = [x1.1 x1.2 x1.3 ... x1.n];
	 *  ...
	 *  xm = [xm.1 xm.2 xm.3 ... xmn];
	 */
	void fecha_arq ();


	/*
	 * Metódo que ao final dos vetores do arquivo escreve uma string
	 * a cada vez que o mesmo for chamado e pula uma linha em sequência
	 */
	void string_arq (string str);

private:
	list<double> elementos;
	list<double>::iterator it1, it2; //Iterador de controle da lista de elementos
	ofstream outfile; //Variável de controle do arquivo
	string nome_arquivo;
	int tamanho_coluna; //Quantidade de vetores

};

#endif /*M_ARQUIVOS_H_ */

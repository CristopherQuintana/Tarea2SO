#include <omp.h>
#include <iostream>
#include <vector>
#include <mutex>
using namespace std;
#include <stdlib.h> //atoi
#include <fstream>
#include <unistd.h> //sleep

//este programa sólo funciona para hebras menores a 10

int main(int argc, char *argv[]){
	mutex myMutex;
	ifstream ips;
	string linea, arg = argv[2];	//convertir argv2 a string para luego poder sumarlo		
	vector <string> l_ips;		//estructura dinámica para ips, no sabemos cuantas serán
	string l_result[6];		//estructura estática para resultado, no serán más de 6 así que podemos usar array
	ips.open(argv[1]);		//abrir archivo
	int i, j = 0, perd;
	string salida, estado;
	while(!ips.eof()){		//obtener ips desde archivo
		getline(ips, linea);
		if(linea.length()){
			l_ips.push_back(linea);	//guardar cada ip en vector
		}
	}
	ips.close();
	cout << "IP            Trans. Rec. Perd. Estado" <<endl;
	cout << "--------------------------------------" <<endl;
	#pragma omp parallel num_threads((int)l_ips.size())		//cantidad de hebras es igual al tamaño del vector de ips
	{
		#pragma omp for private(i)				//ciclo interno OMP
		for(i = 0; i < (int)l_ips.size(); i++){
			sleep(i);			
			//sleep es necesario para evitar yuxtaposición sin afectar el aspecto paralelo de las hebras
			l_ips[i] = "ping -q -c" + arg + " " + l_ips[i] + " > info.txt"; //texto a ejecutar en consola
			system(l_ips[i].c_str());	// ejecución en consola
			myMutex.lock();			// necesitamos usar mutex debido a conflictos con el array
			ips.open("info.txt");	// abrimos el texto
			while(!ips.eof()){
				getline(ips, linea);
				if(linea.length()){
					l_result[j] = linea;	//guardamos en array
					j++;			//pasamos al siguiente elemento
				}
			}
			j = 0;					//volvemos a 0
			ips.close();				//cerramos
			perd = stoi(l_result[2].substr(0,1)) - stoi(l_result[2].substr(23,1));
			//obtenemos las pérdidas restando las transacciones con las recibidas
			estado = stoi(l_result[2].substr(23,1)) ? "UP" : "DOWN"; 
			//si las perdidas son 0 el estado es DOWN, caso contrario es UP
			salida = l_result[1].substr(4,13) + "   " + l_result[2].substr(0,1) + "    " + l_result[2].substr(23,1) + "     " + to_string(perd) + "     " + estado;
			//info que necesitamos de cada IP
			cout << salida << endl;
			myMutex.unlock();	// salimos del mutex
		}
	}
	return 0;
}

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

float euclidiana(vector<float>& v1, vector<float>& v2)
{
    float sum = 0;
    for (int i = 0; i < v1.size(); i++) 
    {
        sum += pow(v1[i] - v2[i], 2);
    }

    return sqrt(sum);
}

vector<float> leerExcel(string nombre)
{
    ifstream archivo(nombre);
    vector<float> datos;
    string linea;

    while (getline(archivo, linea))
    {
        datos.push_back(stof(linea));
    }

    return datos;
}

int main()
{
    vector<float> archivo1 = leerExcel("C:\\Users\\alumno-b305\\Downloads\\1.csv");
    vector<float> archivo2 = leerExcel("C:\\Users\\alumno-b305\\Downloads\\2.csv");
    vector<float> archivo3 = leerExcel("C:\\Users\\alumno-b305\\Downloads\\3.csv");
    vector<float> archivo4 = leerExcel("C:\\Users\\alumno-b305\\Downloads\\4.csv");

    float d1 = euclidiana(archivo1, archivo2);
    float d2 = euclidiana(archivo1, archivo3);
    float d3 = euclidiana(archivo1, archivo4);

    cout << "Distancia de 1 a 2: " << d1 << endl;
    cout << "Distancia de 1 a 3: " << d2 << endl;
    cout << "Distancia de 1 a 4: " << d3 << endl;

    float menor = d1;
    string masSimilar = "2";

    if (d2 < menor) 
    {
        menor = d2;
        masSimilar = "3";
    }

    if (d3 < menor) 
    {
        menor = d3;
        masSimilar = "4";
    }

    cout << endl;

    cout << "El archivo mas similar a 1 es el " << masSimilar << ", con distancia: " << menor << endl;

    return 0;
}

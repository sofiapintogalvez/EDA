#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <random>

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

void crearPuntos(int n, int d, vector<vector<float>>& general)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for(int i = 0; i < n; i++)
    {
        vector<float> puntos;
        for(int j = 0; j < d; j++)
        {
            puntos.push_back(dis(gen));
        }
        general.push_back(puntos);
    }
}

int main()
{
    int n = 100;
    int d;
    vector<vector<float>> general;

    cout << "Ingrese la dimension: ";
    cin >> d;

    crearPuntos(n, d, general);

    ofstream rpta("rptas5000.csv");

    for(int i = 0; i < general.size(); i++)
    {
        for(int j = i + 1; j < general.size(); j++)
        {
            float distancia = euclidiana(general[i], general[j]);
            rpta << distancia << endl;
        }
    }

    rpta.close();

    cout << "Se creo el csv con las distancias" << endl;

    return 0;
}
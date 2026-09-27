#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>

using namespace std;

struct Point
{
    float x;    // izquierda o derecha
    float y;    // arriba o abajo
    float z;    // adelante o atras

    Point(float a, float b, float c) : x(a), y(b), z(c) {}
    Point() : x(0), y(0), z(0) {}
};

vector<Point> cargarPuntos(const string &nombreArchivo)
{
    vector<Point> puntos;
    ifstream archivo(nombreArchivo);

    if(!archivo.is_open())
    {
        cout << "No se pudo abrir el archivo" << endl;
        return puntos;
    }

    float x, y, z;
    char coma;

    while(archivo >> x >> y >> z)
    //while(archivo >> x >> coma >> y >> coma >> z)
        puntos.push_back(Point(x, y, z));
    archivo.close();

    cout << "Se subieron los puntos <3: " << puntos.size() << endl;
    return puntos;
}

void calcularRaiz(const vector<Point> &puntos, Point &bottomLeft, double &h)
{
    float minX = puntos[0].x;
    float minY = puntos[0].y;
    float minZ = puntos[0].z;

    float maxX = puntos[0].x;
    float maxY = puntos[0].y;
    float maxZ = puntos[0].z;

    for(int i = 1; i < puntos.size(); i++)
    {
        if(puntos[i].x < minX)
            minX = puntos[i].x;

        if(puntos[i].y < minY)
            minY = puntos[i].y;

        if(puntos[i].z < minZ)
            minZ = puntos[i].z;

        if(puntos[i].x > maxX)
            maxX = puntos[i].x;

        if(puntos[i].y > maxY)
            maxY = puntos[i].y;

        if(puntos[i].z > maxZ)
            maxZ = puntos[i].z;
    }

    float rangoX = maxX - minX;
    float rangoY = maxY - minY;
    float rangoZ = maxZ - minZ;

    h = max(rangoX, max(rangoY, rangoZ));

    bottomLeft = Point(minX, minY, minZ);
}

float euclidiana(const Point &p1, const Point &p2)
{
    float x1 = float(p1.x - p2.x);
    float y1 = float(p1.y - p2.y);
    float z1 = float(p1.z - p2.z);

    return sqrt(x1 * x1 + y1 * y1 + z1 * z1);
}

float euclidianaCubo(Point punto, Point esquina, double h)
{
    float dx = 0;
    float dy = 0;
    float dz = 0;

    // eje x
    if(punto.x < esquina.x)
        dx = esquina.x - punto.x;

    else if(punto.x > esquina.x + h)
        dx = punto.x - (esquina.x + h);

    // eje y
    if(punto.y < esquina.y)
        dy = esquina.y - punto.y;

    else if(punto.y > esquina.y + h)
        dy = punto.y - (esquina.y + h);

    // eje z
    if(punto.z < esquina.z)
        dz = esquina.z - punto.z;

    else if(punto.z > esquina.z + h)
        dz = punto.z - (esquina.z + h);

    return sqrt(dx * dx + dy * dy + dz * dz);
}

class Octree
{
    private:
        Octree* children[8];    // cada children[i] es otro nodo octree
        vector<Point> points;   // Point *points, guarda los puntos que pertenecen a ese nodo
        
        Point bottomLeft;
        double h;
        int nPoints = 0;
        int capacidad;

        //extras
        Octree* buscarNode(const Point &p);
        void dividirNode(Octree *node);
        void buscarCercano(const Point &p, float &radio, Point &pCercano, float &menorDist);

    public:
        Octree(double _h, int _c, Point _bl) : h(_h), capacidad(_c), bottomLeft(_bl)
        {
            for(int i = 0; i < 8; i++)
                children[i] = nullptr;
        }

        bool exist(const Point &p);
        void insert(const Point &p);
        Point find_closest(const Point &p, int radio);
        void getDatosNodo(const Point &p, Point &bl, double &altura);
        void print(int nivel);

        void generarOBJ(ofstream &archivo, int &indice);
};

Octree* Octree::buscarNode(const Point &p)
{
    Octree* node = this;

    while(node->children[0] != nullptr)
    {
        Point centro(node->bottomLeft.x + node->h / 2, node->bottomLeft.y + node->h / 2, node->bottomLeft.z + node->h / 2);

        if(p.x < centro.x && p.y < centro.y && p.z < centro.z)
            node = node->children[0];
        
        else if(p.x >= centro.x && p.y < centro.y && p.z < centro.z)
            node = node->children[1];
        
        else if(p.x < centro.x && p.y >= centro.y && p.z < centro.z)
            node = node->children[2];
        
        else if(p.x >= centro.x && p.y >= centro.y && p.z < centro.z)
            node = node->children[3];
        
        else if(p.x < centro.x && p.y < centro.y && p.z >= centro.z)
            node = node->children[4];
        
        else if(p.x >= centro.x && p.y < centro.y && p.z >= centro.z)
            node = node->children[5];
        
        else if(p.x < centro.x && p.y >= centro.y && p.z >= centro.z)
            node = node->children[6];
        
        else if(p.x >= centro.x && p.y >= centro.y && p.z >= centro.z)
            node = node->children[7];
    }
    return node;
}

void Octree::dividirNode(Octree *node)
{
    double mitad = node->h / 2;

    for(int i = 0; i < 8; i++)
    {
        Point nuevoBottomLeft;

        if(i == 0)
            nuevoBottomLeft = node->bottomLeft;
        
        else if(i == 1)
            nuevoBottomLeft = Point(node->bottomLeft.x + mitad, node->bottomLeft.y, node->bottomLeft.z);
        
        else if(i == 2)
            nuevoBottomLeft = Point(node->bottomLeft.x, node->bottomLeft.y + mitad, node->bottomLeft.z);
                
        else if(i == 3)
            nuevoBottomLeft = Point(node->bottomLeft.x + mitad, node->bottomLeft.y + mitad, node->bottomLeft.z);
                
        else if(i == 4)
            nuevoBottomLeft = Point(node->bottomLeft.x, node->bottomLeft.y, node->bottomLeft.z + mitad);
        
        else if(i == 5)
            nuevoBottomLeft = Point(node->bottomLeft.x + mitad, node->bottomLeft.y, node->bottomLeft.z + mitad);

        else if(i == 6)
            nuevoBottomLeft = Point(node->bottomLeft.x, node->bottomLeft.y + mitad, node->bottomLeft.z + mitad);
    
        else if(i == 7)
            nuevoBottomLeft = Point(node->bottomLeft.x + mitad, node->bottomLeft.y + mitad, node->bottomLeft.z + mitad);

        node->children[i] = new Octree(mitad, node->capacidad, nuevoBottomLeft);
    }

    vector<Point> tmp = node->points;
    node->points.clear();
    node->nPoints = 0;

    for(auto &punto : tmp)
        insert(punto);
}

void Octree::buscarCercano(const Point &p, float &radio, Point &pCercano, float &menorDist)
{
    if(children[0] == nullptr)
    {
        for(int i = 0; i < points.size(); i++)
        {
            float distancia = euclidiana(p, points[i]);
            
            if(distancia <= radio && distancia < menorDist)
            {
                menorDist = distancia;
                pCercano = points[i];
            }
        }
        return;
    }

    for(int i = 0; i < 8; i++)
    {
        if(children[i] == nullptr) continue;

        float distCubo = euclidianaCubo(p, children[i]->bottomLeft, children[i]->h);

        if(distCubo < menorDist)
            children[i]->buscarCercano(p, radio, pCercano, menorDist);
    }
}

bool Octree::exist(const Point &p)
{
    Octree* node = buscarNode(p);

    for(auto &punto : node->points)
    {
        if(p.x == punto.x && p.y == punto.y && p.z == punto.z)
            return true;
    }
    return false;
}

void Octree::insert(const Point &p)
{
    Octree* node = buscarNode(p);

    if(node->nPoints < node->capacidad)
    {
        node->points.push_back(p);
        node->nPoints++;
    }
    else
    {
        dividirNode(node);
        insert(p);
    }
}

Point Octree::find_closest(const Point &p, int radio)
{
    Point pCercano;
    float r2 = (float)radio;
    float menorDist = r2;
    buscarCercano(p, r2, pCercano, menorDist);
    return pCercano;
}

void Octree::getDatosNodo(const Point &p, Point &bl, double &altura)
{
    Octree* node = buscarNode(p);

    bl = node->bottomLeft;
    altura = node->h;
}

void Octree::print(int nivel)
{
    for(int i = 0; i < nivel; i++)
        cout << "  ";

    if(children[0] == nullptr)
    {
        cout << "Hoja: ";
        cout << "bottomLeft = (" << bottomLeft.x << "," << bottomLeft.y << "," << bottomLeft.z << ") ";
        cout << "| h = " << h;
        cout << " | nPuntos = " << nPoints << endl;

        for(int i = 0; i < points.size(); i++)
            cout << " (" << points[i].x << "," << points[i].y << "," << points[i].z << ")" << endl;
    }
    else
    {
        cout << "Nodo: " << endl;
        cout << "bottomLeft = (" << bottomLeft.x << "," << bottomLeft.y << "," << bottomLeft.z << ") ";
        cout << "| h = " << h;

        for(int i = 0; i < 8; i++)
        {
            if(children[i] != nullptr)
                children[i]->print(nivel + 1);
        }
    }
}

void Octree::generarOBJ(ofstream &archivo, int &indice)
{
    // Si es una hoja
    if(children[0] == nullptr)
    {
        if(nPoints == 0)
            return;

        float x = bottomLeft.x;
        float y = bottomLeft.y;
        float z = bottomLeft.z;

        float x2 = x + h;
        float y2 = y + h;
        float z2 = z + h;

        // 8 vertices del cubo
        archivo << "v " << x  << " " << y  << " " << z  << endl;
        archivo << "v " << x2 << " " << y  << " " << z  << endl;
        archivo << "v " << x2 << " " << y2 << " " << z  << endl;
        archivo << "v " << x  << " " << y2 << " " << z  << endl;

        archivo << "v " << x  << " " << y  << " " << z2 << endl;
        archivo << "v " << x2 << " " << y  << " " << z2 << endl;
        archivo << "v " << x2 << " " << y2 << " " << z2 << endl;
        archivo << "v " << x  << " " << y2 << " " << z2 << endl;

        // 6 caras del cubo
        archivo << "f " << indice << " " << indice + 1 << " " << indice + 2 << " " << indice + 3 << endl;
        archivo << "f " << indice + 4 << " " << indice + 5 << " " << indice + 6 << " " << indice + 7 << endl;
        archivo << "f " << indice << " " << indice + 1 << " " << indice + 5 << " " << indice + 4 << endl;
        archivo << "f " << indice + 2 << " " << indice + 3 << " " << indice + 7 << " " << indice + 6 << endl;
        archivo << "f " << indice + 1 << " " << indice + 2 << " " << indice + 6 << " " << indice + 5 << endl;
        archivo << "f " << indice + 3 << " " << indice << " " << indice + 4 << " " << indice + 7 << endl;

        indice += 8;

        return;
    }

    // Si no es hoja, seguimos recorriendo sus hijos
    for(int i = 0; i < 8; i++)
    {
        if(children[i] != nullptr)
            children[i]->generarOBJ(archivo, indice);
    }
}

int main()
{
    vector<Point> puntos = cargarPuntos("aguila.xyz");
    Point bottomLeft;
    double h;

    calcularRaiz(puntos, bottomLeft, h);

    cout << "\tRAIZ" << endl;
    cout << "bottomLeft = (" << bottomLeft.x << "," << bottomLeft.y << "," << bottomLeft.z << ")" << endl;
    cout << "h = " << h << endl;

    int N;
    cout << "Ingrese N (capacidad cubo): ";
    cin >> N;
    Octree arbol(h, N, bottomLeft);

    for(int i = 0; i < puntos.size(); i++)
        arbol.insert(puntos[i]);
    //arbol.print(0);

    ofstream archivoOBJ("octree.obj");
    int indice = 1;
    arbol.generarOBJ(archivoOBJ, indice);
    archivoOBJ.close();
    cout << "Se genero el .obj" << endl;

    Point p(21.0f, 5.0f, 143.0f);
    Point rpta = arbol.find_closest(p, 50);
    cout << "\nMas cercano a (" << p.x << "," << p.y << "," << p.z << ") con radio = 50 y N = 50:" << endl;
    cout << "(" << rpta.x << "," << rpta.y << "," << rpta.z << ")" << endl;

    Point bl;
    double altura;
    arbol.getDatosNodo(rpta, bl, altura);

    cout << "\nbottomLeft X = (" << bl.x << "," << bl.y << "," << bl.z << ")" << endl;
    cout << "h X = " << altura << endl;
    cout << endl;

    return 0;
}

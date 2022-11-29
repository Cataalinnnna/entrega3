#include <cstdlib>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <math.h>
using namespace std;

 
float infinite(){
    return 9999999999;
}
int demandLHtotal = 0;
class Nodo{
    public:
        int tipo;
        int ID;
        float x;
        float y;
        bool visitado;
        int demand;
        Nodo(int tipo, int ID, float x, float y) {
            this->tipo = tipo;
            this->ID = ID;
            this->x = x;
            this-> y = y;
            this-> visitado = false;
            this->demand = 0;
        };
};

ostream& operator<<(ostream& os, Nodo &o) {
    os << "Tipo : " << o.tipo << " | ID : " << o.ID << " | x : " << o.x << " | y: " << o.y << " | Vistado?:  " << o.visitado<< "| Demanda : "<< o.demand<< endl;
   
    return os;
}


class Vehicle{
    public:
        int Maxcap;
        int Actualcap;
        list <int> route;
        float distance;
        int linehaul_demand;
        int backhaul_demand;
        Vehicle(int Maxcap){
            this->Maxcap = Maxcap;
            this->Actualcap = 0;
            this->route = list<int>();
            this->distance = 0;
            this -> linehaul_demand = 0;
            this -> backhaul_demand = 0;
        };

        void add_to_route(int node_id) {
            this->route.push_back(node_id);
        };

        void delete_from_route(int node_id) {
            this->route.remove(node_id);
        };

};
ostream& operator<<(ostream& os, Vehicle &o) {
    os << "Distance : " << o.distance << " | linehaul_demand:  " << o.linehaul_demand<< "| backhaul_demand : "<< o.backhaul_demand<< endl;
   
    return os;
}


vector <Nodo> ListaNodos;
vector <Vehicle> ListaVehicles;
map <int, map<int,float>> adymatrix ;
//funcion que nos permita encontrar un nodo en funcion de su ID
int FindIndexByID(int id){
    for (int i = 0; i < ListaNodos.size(); i++ ){
        if(ListaNodos[i].ID == id){
            return i;  
        }
    }
    
    // nunca debería entrar a este return, es solo para evitar warnings en la compilación
    return -1;
}

void ReadFile(string namefile){
    
    ifstream file;
    file.open(namefile);
    string linea, info;
    int N;
    int i = 0;
// Obtener línea de archivo, y almacenar contenido en "linea"

    while (getline(file, linea)) {

        if(i == 0){
        // Lo vamos imprimiendo
            //cout << "Una línea: ";
            //cout << linea << endl;
            N = stoi(linea,nullptr,0);
        }
        else if(i <= N){
            
            stringstream ss(linea);
            string word;
            int j = 0;
            int tipo,ID;
            float x,y;
            while (ss >> word){
                if (j == 0){
                    tipo = stoi(word,nullptr,0);
                }else if(j == 1){
                    ID = stoi(word,nullptr,0);
                }else if(j == 2){
                    x = stof(word);
                }else if(j == 3){
                    y = stof(word);
                
                }
                j++;
            }
            ListaNodos.push_back(Nodo(tipo,ID,x,y));
        }
        else if(i ==N+1){
            stringstream ss2(linea);
            string word;
            int capVehicle, cantVehiculos;
            int w = 0;
            while (ss2 >> word){
                
                if ( w == 0) {
                    cantVehiculos = stoi(word, nullptr,0);
                }
                if (w == 1){
                    capVehicle = stoi(word,nullptr,0);
                }
                w++;
            }

            while (cantVehiculos > 0) {
                ListaVehicles.push_back(Vehicle(capVehicle));
                cantVehiculos -= 1;
            }


        }else{
            stringstream ss3(linea);
            string word;
            int z = 0;
            int ID, demand;
            while (ss3>> word){
                if (z==0){
                    ID = stoi(word,nullptr,0);
                    //cout<<"id "<< ID <<endl;
                }else if(z==1){
                    demand = stoi(word,nullptr,0);
                } z++;
                }
            
                //Nodo& node = ListaNodos[ID];
                for (int i = 0; i< ListaNodos.size(); i++){
                    if(ListaNodos[i].ID == ID){
                        ListaNodos[i].demand = demand;
                        if(ListaNodos[i].tipo == 1){
                            
                            demandLHtotal += demand;
                            
                        }

                        break;
                    }
                }
              }
        i++;
        }
     
}

float Eudistance(float x1, float x2, float y1, float y2){
    float x = pow((x2-x1),2);
    float y = pow((y2-y1),2);
    return sqrt(x+y);

}


void generateADYmatrix(){
    for(int i = 0; i<ListaNodos.size();i++){

        for (int j = 0 ; j<ListaNodos.size();j++){
            //no ese esta referenciando a si mismo
            if (ListaNodos[i].ID == ListaNodos[j].ID){
                adymatrix[ListaNodos[i].ID].insert({ListaNodos[j].ID, infinite()});
                //continue;
            // depot no puede ir directo a un backhaul
            }else if(ListaNodos[i].tipo == 0 && ListaNodos[j].tipo == 2 ){
                adymatrix[ListaNodos[i].ID].insert({ListaNodos[j].ID, infinite()});
                //continue;
            }//back haull no puede ir directo al linehaull 
            else if (ListaNodos[i].tipo == 2 && ListaNodos[j].tipo == 1 ){
                adymatrix[ListaNodos[i].ID].insert({ListaNodos[j].ID, infinite()});
                //continue;
            }else{
                float distancia =  Eudistance(ListaNodos[i].x, ListaNodos[j].x, ListaNodos[i].y, ListaNodos[j].y);
                adymatrix[ListaNodos[i].ID].insert({ListaNodos[j].ID,distancia});
            }
        }    
    }
}




pair<int, float> get_min_node(Vehicle truck, Nodo node){
    float min_cost = infinite();
    Nodo tempnodo = Nodo(-1,-1,-1,-1);
    Nodo min_cost_node = tempnodo;
  
    float last_min_cost = min_cost;
    Nodo last_min_cost_node = min_cost_node;
    vector <Nodo> opciones;
    int clientes = 0;
    
    //n tenemos el par de valor, nodo y distancia
    for ( auto n : adymatrix[node.ID]){
        int id = n.first ;
        float distancia = n.second;

        int index = FindIndexByID(id);
        cout<<"index"<<endl;
        cout<<index<<endl;
        
        Nodo n_values = ListaNodos[index];
        cout<<ListaNodos[index]<<endl;
        if (n_values.visitado == true){
            continue;
        }

        // no puedo entregar más inventario del que poseo
        if(n_values.tipo == 1 && n_values.demand >  truck.Actualcap) {
            continue;
        }

        // no puedo recoger más inventario del máximo que tengo disponible
        if(n_values.tipo == 2 && n_values.demand > (truck.Maxcap - truck.Actualcap)){
            continue;
        }

        if (n_values.tipo != 0){
            clientes ++;
        }
        opciones.push_back(n_values);

    }
    if (clientes == 0){
        opciones.push_back(ListaNodos[0]);
    }
    for (auto m : opciones){
    
        }
    if(opciones.size()==1 && opciones[0].tipo == 0 ){
        return std::make_pair(opciones[0].ID, 0);
    }
    for ( auto n : opciones){
        
        
        if (adymatrix[node.ID][n.ID]< min_cost){
         
            last_min_cost = min_cost;
            last_min_cost_node = min_cost_node;

            min_cost = adymatrix[node.ID][n.ID];
            min_cost_node = n;
        }

        if (min_cost_node.ID != -1 ){
            if (min_cost_node.tipo == 0){
                if (clientes > 0) {
                    min_cost = last_min_cost;
                    min_cost_node = last_min_cost_node;
                }           
            }
        }
        
    }
    
    return std::make_pair(FindIndexByID(min_cost_node.ID), min_cost);
}

void printroute(list<int> ruta){
    for (auto const &i: ruta) {
        std::cout << i <<" ";
    }
     cout<<endl;
}

int Greedy(){
    list <int> greedy_solution;
    int quality = 0;
    int clients = 0;
    int vehicules = 0;
    int time = 0;

    for (int truck = 0 ; truck < ListaVehicles.size(); truck++){
         

        ListaVehicles[truck].Actualcap = min(demandLHtotal, ListaVehicles[truck].Maxcap);
        if (demandLHtotal > ListaVehicles[truck].Maxcap) {
            ListaVehicles[truck].Actualcap = ListaVehicles[truck].Maxcap;
        }
        else {
            ListaVehicles[truck].Actualcap = demandLHtotal;
        }

        Nodo start = ListaNodos[0];

        ListaVehicles[truck].add_to_route(start.ID);
       
        Nodo current_node = start;
        bool flag = true; 

        while (flag){

            pair<int, float> value = get_min_node(ListaVehicles[truck], current_node);
            
            int next_node_index = value.first ;
            cout<<next_node_index<<endl;
            Nodo next_node = ListaNodos[next_node_index]; 
            


            ListaNodos[next_node_index].visitado = true;
           
            float cost = value.second;
            ListaVehicles[truck].distance+=cost;
             
            
            if (ListaNodos[next_node_index].tipo == 1){

                
                ListaVehicles[truck].linehaul_demand+= ListaNodos[next_node_index].demand;
                
                ListaVehicles[truck].Actualcap-= ListaNodos[next_node_index].demand;

            }else if(ListaNodos[next_node_index].tipo == 2){
                ListaVehicles[truck].backhaul_demand+= ListaNodos[next_node_index].demand;
                ListaVehicles[truck].Actualcap+= ListaNodos[next_node_index].demand;
            }
            ListaVehicles[truck].add_to_route(next_node.ID);
       
          
            current_node = next_node;
          
         
            if (current_node.ID == start.ID){
                
                flag = false;
            }
            vehicules ++;
            
            
            
            
        }
        demandLHtotal -= ListaVehicles[truck].linehaul_demand;
        if (demandLHtotal == 0){
                break;
            }

    }
    return 0;




}




//3. comenzar hc -> swap entre nodos de rutas.

void EscribirArchivo(string file){
    ofstream archivo;
    list <int>ruta;

    archivo.open(file+".out");
    for (auto v : ListaVehicles){
        for (auto const &i: v.route) {
            
            archivo<< i<<" ";
        }
        
       
        archivo<< v<<endl;
            
    }
   
    cout <<endl;
}

/*void HillClimbing(){
    
    Nodo start = ListaNodos[0];
    int depot = 0;
    int IDNodoA;
    int indexNodoB;
    int ubi =0;
    for (auto v : ListaVehicles){

         for (auto const &i: v.route) {
            //aqui voy a tener todos los id que son parte de las rutas
            //while (depot <1){} ver que no sea el depot
            IDNodoA = i;
           while (ubi < ListaVehicles.size()){
                 for (auto const &i: ListaVehicles[ubi+1].route){
                    //obtendr-e los id de la siguiente solucion
                    
                 }
           }
             
        
        
    
        }
      ubi++;\
      }
}*/

int main(int argc, char *argv[]){
    printf("Ingrese nombre del archivo ");
    string file;
    std::cin >> file;
    std::cout<< "nombre del archivo ingresado: "<<file<<std::endl;
    ReadFile(file+".txt");
    generateADYmatrix();
    //Imprime matriz de adyacencia
     /*for (auto v : adymatrix){
        for(auto w : adymatrix.at(v.first)){
        cout << adymatrix[v.first][w.first] << " ";
        }
        cout<< endl;
    
    };*/
    Greedy();
    
    cout << "RUTA: " << endl;
    for (auto v : ListaVehicles){
        printroute(v.route);
        cout<<v<<endl;  
    }
    cout <<endl;
    EscribirArchivo(file);
    return 0;
}

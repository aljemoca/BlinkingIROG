
//Para el algoritmo de clasificacin
#define NCENTROS  3  
#define LONGITUD 63






/*****************************************************
*               CLASE KMEANS                         *
******************************************************/




/*
  Este es un clasificador no supervisado que trabaja
  con datos en una dimension. Busca clasificar tres
  eventos: derivada positiva, derivada negativa y ninguna
  de las anterior. Para ello utiliza un array de centroides
  con los promedios de los valores de cada clase. Los
  centroides se van actualizando siempre, pero a partir
  de 256 datos en cada, dicha actualizacion se realiza 
  de forma mas rapida.
  
  Asociados a cada uno de los centroides, existe un contador
  que evalua el numero de veces que se ha actualizado el centroide
  Permite calcular el promedio de forma mas eficiente sin la necesidad
  de almacenar una elevada cantidad de datos.
  
  Esta clase tiene una funcion miembre y dos constructores. 
  Los constructores permiten inicializar los centroides y poner
  a cero los contadores.
  La funcion evaluar(), cuando se invoca, permite actualizar
  los centroides, incrementar los contadores cuando proceda y
  devolver el resultado de la clasificacion.
  
  Esta funcion miembro recibe un dato, calcula su distancia
  a los tres centroides. Determina cual es el centroide mas
  cercano, devuelve 0, 1 y 2, dependiendo del centroide 
  mas proximo y actualiza el centroide cercano. 
  
  
*/
class Kmeans{
    float centroide[NCENTROS];

public:
    unsigned char hab;
    Kmeans();
    Kmeans(int *);
    signed char evaluar(int );
    void vercentroides(int *);

};

Kmeans::Kmeans(){
  centroide[0]=512-60;  
  centroide[1]=512;   
  centroide[2]=512+60;  
/*for (unsigned char k=0;k<NCENTROS;k++)
      contador[k] = 0; 
*/

};
  
Kmeans::Kmeans(int *p){
   for (unsigned char k=0;k<NCENTROS;k++)
   {
     centroide[k] = (float)p[k]; 
//     contador[k] = 0; 
   }
  
};

signed char Kmeans::evaluar(int data){
    float distancia[NCENTROS];
    float minimo;
    unsigned char sol=-1;
 
 
   //Distancias a los centroides
   for ( unsigned char p=0;p<NCENTROS;p++)
      distancia[p]= abs(centroide[p] - static_cast<float>(data) ); 
    //Funcion que indica cual es el mas cercano
    minimo = findmin(&distancia[0],NCENTROS);
    
    for ( unsigned char p=0;p<NCENTROS;p++)
	      if (minimo == distancia[p] )
        {
	        sol = p;  //Devuelve el centroide mas cercano
	        if ( minimo > 2)
          {    
            
            centroide[p] = LONGITUD*centroide[p] + static_cast<float>(data);
           // centroide[p] /= (LONGITUD+1);
           //centroide[p] = roundb(centroide[p], LONGITUD+1);
            centroide[p] = centroide[p] / (LONGITUD+1);
           if(p==0)
           {               
           /* centroide[NCENTROS-1] = LONGITUD*centroide[NCENTROS-1] + max( (float)data,centroide[0]);
            //centroide[NCENTROS-1] = roundb(centroide[NCENTROS-1],LONGITUD+1);
            //centroide[NCENTROS-1] /= (LONGITUD+1);
            centroide[NCENTROS-1] =  centroide[NCENTROS-1] / (LONGITUD+1);
            */
           }
            
          }
	  //centroide[p]=static_cast<int>(  ( static_cast<long int> (contador[p]) * static_cast<long int>(centroide[p]) + static_cast<long int>(data) )/ (contador[p]+1) );
        }//Actualizacion del centroide
    return sol-1;
};

void Kmeans::vercentroides(int *c)
{
   for(uchar p =0;p<NCENTROS;p++)
    c[p]=static_cast<int>(round(centroide[p])); 
}



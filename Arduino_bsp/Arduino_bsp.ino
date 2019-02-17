


/****************************************************************************************************
 
  Before starting using this file, please copy the bsp folder, containing the classes and methods,
  to the library directory in Arduino IDE.

******************************************************************************************************/


/*
  Software Architecture for Basic Signal Processing Applications in Open Hardware Platforms
  Alberto J. Molina-Cantero et al.
  
  The software is made of different tasks wherein each task is concurrently executed.
  To ease this kind of execution, tasks are all connected by a set of software
  queues.

  A task manager is in charge of executing  tasks sequentally.
  When a specific task contains a value in its associated input queue, the
  task manager executes it, pops the data out of the queue, processes the
  data, and generates a result which is also pushed into the output queue.

  Below is a schematic of the task execution.
  
  Task                 Task                   Task        
       -> HLQueues ->      -> DSP_Queue1 ->      
  HL                   DSP1                  ser App    
  
  This program allows you to smooth data coming from two analog channels at 
  a rate of 250Hz.
  
 
 */
  

//Include the library
#include "LBSP.h"
//#include "adicionales.h"
#include "adicionales.c"
#include "Kmeans.h" 


/************************************************************************
 * 
 * FUNCTIONS PROTOTYPES BEGIN
 * 
 ***********************************************************************/
// Prototypes
void Task_Manager(void);
void DSP_layer(void);
void Classification_layer(void);
void Pulsos_layer(void);
void GestorEventos(void);
void Gestor_VolBlink(void);

/************************************************************************
 * 
 * FUNCTIONS PROTOTYPES END
 * 
 ***********************************************************************/



/************************************************************************
 * 
 * HARDWARE LAYER CONFIGURATION BEGIN
 * 
 ***********************************************************************/
const int ledInfrarrojo = 9;

//Select the number of analog channels
ushort Number_ADC_Channels=1;

//Inital Analog Channel.
ushort IAC=0;

//Sampling rate per channel
#define Fs 250

//Timer0 Resolution configuration
byte TIMER0_RES = 10; // Sets the resolution time in ms. By default 10ms. 
                      //Change it if other resolutions are required.


// Now we create the lowest layer in the processing pipeline, containing 
// the interrupt routines, and queues to give data at a fixed rate.

// Example, a Hardware Layer with 2 channels, at Fs=250, starting at channel 0
// with 2 depth queues and a timer0 resolution of 10ms.
// ushort Number_ADC_Channels=2;
// ushort IAC=0;
// byte TIMER0_RES=10;
// #define Fs 250
// bsp Hard_Layer(IAC, Number_ADC_Channels, TIMER0_RES,2,Fs);

//The hardLayer object creates two queues of length 2 where data
//associated to channels 0 and 1 are stored at a rate of 250Hz/channel

//If different queues length are required, give a matrix of number
// Example: bsp Hard_Layer(IAC, Number_ADC_Channels, TIMER0_RES, {2,3}, Fs);
// In this case, the channel 0 has a 2-ushort length queue while the channel 1 has
// a 3-ushort length queue.


//Declare your HardwareLayer Object here

bsp Hard_Layer(IAC,Number_ADC_Channels, TIMER0_RES,1,Fs);

/************************************************************************
 * 
 * HARDWARE LAYER CONFIGURATION END
 * 
 ***********************************************************************/





/************************************************************************
 * 
 * QUEUES BEGIN
 * Add the queues between layers
 * 
 ***********************************************************************/

                     

//Below, add the number of queues you need.
//For example, to create a three-integer-length queue you should write down
// Queue<int> Cola(3); 
//For a 2-float-length queue you should write down
// Queue<float> Col1(2);
// 

Queue<int> ColaDSP;
Queue<int> ColaPulsos;
Queue<int> ColaEventos;
Queue<int> ColaVol;

/************************************************************************
 * 
 * QUEUES END
 * 
 ***********************************************************************/




/************************************************************************
 * 
 * TIMERS BEGIN
 * Add the timers you need for your application
 * By default the TIMER0 resultion is 10ms.
 ***********************************************************************/
   //Example of a 1 sec delay at 10ms resolution: timer t1(100)

timer t1(100);

 
 /************************************************************************
 * 
 * TIMERS END
 *
 ***********************************************************************/






/************************************************************************
 * 
 * FILTERS AND BLOCKS. BEGIN
 * Declare the filters or blocks you will use later
 ***********************************************************************/

//Example: A polyphasic filter of length 10, and decimation =2
//ppfilter  filter1(9,2); 
//Example: A n-tap FIR o IIR filter
//filter filter2;  
//Example: A sliding window with L=10 and D=2;
//block Window(10,2);

#define D 1
ppfilter filter1(7,D);
//filter filter1;

/************************************************************************
 * 
 * FILTERS AND BLOCKS.  END
 *
 ***********************************************************************/



/************************************************************************
 * 
 * DEFINE THE COEFFICIENTS OF THE FILTERS. BEGIN
 * 
 * Use Octave/Matlab to obtain the coeeficients. Follow the instruccions
 * in the Appendix 2 to help you find them. Remember to use the Q15 format
 * 
 ***********************************************************************/

//Example1 (for the declared filter1 object): 
//int  h[10]= { -29,3,53,122,175,122,....};
//int a = 32767;

//Example2 (for the filter2 object):
//int bb[3]= {422, 843, 422};
//int ab[3] = {20991, -32768, 13463}; 

int h[7]= {691 ,  3358,   7509,   9647,   7509,   3358,    691};
//int h[5] = { 2503,    8100 ,  11559,    8100,    2503};
//int b[5] = {4096, 13255, 18915, 13255,4096 };
int a = 32767;

/************************************************************************
 * 
 * DEFINE THE COEFFICIENTS OF THE FILTERS. END
 *
 ***********************************************************************/



void setup()
{

  //Configuring the serial port and leds for visualization
  
  Hard_Layer.start();
  Serial.begin(115200);


  pinMode(ledInfrarrojo,OUTPUT); //Activamos el led de infrarrojo
  digitalWrite(ledInfrarrojo,HIGH);

/************************************************************************
 * 
 * ADDING THE LIST OF TIMERS TO THE TIMER LIST. BEGIN
 * 
 * add timers to a list so that the interrupt routine can automatically
 * update their content. If a declared timer is not added to the list, then 
 * it can be used as a counter.
 *
 **********************************************************************/

//Example of adding a declared timer to the timer_list: 
//  HardLayer.addTimerList(&t1); 


/************************************************************************
 * 
 * ADDING THE LIST OF TIMERS TO THE TIMER LIST. END
 *
 **********************************************************************/

  Hard_Layer.addTimerList(&t1); 

/************************************************************************
 * 
 * ASSIGNING THE COEFFICIENTS TO THE FILTER. BEGIN
 * 
 **********************************************************************/
  
 //Example1 for a ppfilter object: filter1.assign(h);  
 //Example2 for a filter object: filter2.assign(2,2,bb,ab);

// filter1.assign(4,0,h,&a);

 filter1.assign(h);

/************************************************************************
 * 
 * ASSIGNING THE COEFFICIENTS TO THE FILTER. END
 * 
 **********************************************************************/


}

void loop()
{
  //if(!t1.expired())
  Task_Manager();
  }

void Task_Manager(void)
{
  DSP_layer();
  Classification_layer();
  Pulsos_layer();
  GestorEventos();
  Gestor_VolBlink();
  }

void DSP_layer(void)
{
  ushort input;
  uchar ava;
  int datafil;
    
  if( !Hard_Layer.isEmpty(0))
  {
    input = Hard_Layer.dequeue(0);
  //  Serial.print(input);
 // datafil=filter1.assess(static_cast<int>(input));
    datafil  = filter1.assess(static_cast<int>(input),&ava);
    if (ava)
   // ColaDSP.enqueue(input);
    // Serial.print(input);
   //  Serial.print(" ");
    { 
     Serial.print(input);
    // Serial.print(" ");
     //Serial.print(datafil);
     // Serial.print(" ");
      ColaDSP.enqueue(datafil);
  }
//    else
    //  Serial.println(input);
    //  Serial.println(datafil);
  }
}


void Classification_layer(void)
{
  int data;
  static Kmeans ClasUsu;
  signed char res;
  int centroides[3];

  /*
    The classification layer labels each sample as -1, 0, 1. The 
    label -1 is associated to negative waves, while the 1 is assigned to
    posstive ones. The label 0 is assigned to samples belonging to the baseline.

    The following layer receives these labels and the signal, using the following
    relationship:

      Data = classificationResult * |Sample - 512|
      

   Data contains the positive and negative waves and 0 otherwise. The baseline in
   the sample (512) is also removed. 
  */
  if(!ColaDSP.isempty())
  {
      data = ColaDSP.dequeue();
      res= ClasUsu.evaluar(data);
      ColaPulsos.enqueue(res*abs(data-512));
      /*ClasUsu.vercentroides(centroides);
      for(uchar n=0;n<3;n++)
      {
        Serial.print(" ");
        Serial.print(centroides[n]);
        }
        Serial.println("");
       */
/*      Serial.print(" "); 
      Serial.print(res);
  */  //  Serial.print(" ");
      //Serial.print(res*abs(data-512));
  }
}
void Pulsos_layer(void)
{
  int data;
  static uchar estado;
  static  int contador;
  signed char res;
  //static uchar space;
  static uchar WavesLength;
  uint temporal;
  static uint maximo,posmaximo;

/*
  This layer receives waves and classification. It generates 
  an output which is positive or negative for positive and negative
  waves respectively. Each non-zero output is modulated by the 
  distance of wave peak (in absolute value) from the end of the wave.

  Only waves with a length longer than MinWaveLength are taken into
  account
*/
  
  #define N -1
  #define P 1
 /*Define the minimum wave length */
  #define MinWaveLength 0
  //WavesLength = 0; //New in this version
  //maximo=0;
  
    if(!ColaPulsos.isempty())
    {
       data = ColaPulsos.dequeue();
       res=0;
       switch(estado)
        {
          case 0:
            if( data != 0)
             {
                contador = contador +data-1;
                estado = 1;
                WavesLength+=1;
                temporal = abs(data);
                if( temporal > maximo )
                {
                    maximo = temporal;
                    posmaximo=1; //Lo pongo a 1 para que res sea N o P. Estaba igual a 0
                  }
             }; 
             break;
         case 1:
            if (data != 0)
            {
              contador = contador + data -1;
              temporal = abs(data);
              WavesLength++;
              posmaximo++;
              if( temporal > maximo )
              {
                    maximo = temporal;
                    posmaximo=1;
               }
            }
            else
            {
              if(WavesLength> MinWaveLength)
              {
                   if (contador <= 0)
                        res=N*posmaximo;  //res es ahora N o P ya que posmaximo es como minimo 1
                   else
                        res=P*posmaximo;
              }
              contador=0;
              estado=0;
              WavesLength=0;
              maximo =0;
            };
            break;
      }
      ColaEventos.enqueue(res);
     // Serial.print(" ");
     // Serial.print(res*100);
      }
   
  }


void GestorEventos(void)
{
  
 // #define Blink -100
 // #define Wink 0
  #define ShortBlink 1
  #define LongBlink 2
  #define Tsblink 98 
  // Tparpadeo = 0.4*Fs/D
  #define Tlbkink  2*Fs
  // Twink 3*Fs/D
  static bool estado;
  static int tiempo;
  int  in;
  int out;
  /*
    This layer checks the sequence N-P and its duration to
    decide the concurrence of a short or long blink.
  */
  if(!ColaEventos.isempty())
  {
    
    in = ColaEventos.dequeue();
    //out=-200;
    out=0;
   
    switch(estado)
    {
      case 0:
          if (in < 0)
          //if(in == N)  
          {
            estado=1;
            tiempo = abs(in);
          }
        break;
      case 1:
          tiempo++;
          //if(in==N)
          if(in<0)
            tiempo=abs(in);
          //if(in == P)
          if(in > 0)
          {
              tiempo -= abs(in); //Esto estaba quitado y creo que era un error  
              if(tiempo <=Tsblink)
              {
                out = ShortBlink;
              };
              if( tiempo>Tsblink && tiempo<Tlbkink )
              {
                out = LongBlink;
              };
              estado=0;
             // tiempo=0;
          }
        break;
        
    }
    ColaVol.enqueue(out);
    //Serial.print(",");
    //Serial.println(out*100);
  //  Serial.print(" ");
  //  Serial.println(tiempo);
  }
  }


void Gestor_VolBlink(void)
{
  #define ShortBlink 1
  #define LongBlink 2
  // Tparpadeo = 0.4*Fs/D
  #define Timeout 300 //1.2*Fs
  // Twink 3*Fs/D
  static bool estado;
  static int tiempo;
  int  in;
  int out;
  /*
    This layer checks the sequence N-P and its duration to
    decide the concurrence of a short or long blink.
  */
  if(!ColaVol.isempty())
  {
    
    in = ColaVol.dequeue();
    out=0;
    if( in == LongBlink)
    {
      estado = 0;
      out |= 2;
      }
      else
      {
        switch(estado)
        {
          case 0:if(in == ShortBlink)
            {
              out|=1;
              estado=1;
              tiempo=0;
              } 
            break;
          case 1: tiempo++;
            if(tiempo>Timeout)
              estado=0;
            if(in==ShortBlink)
            {
              estado=0;
              out |=5;
              }
             if(in == LongBlink)
             {
              estado=0;
              out|=3;
              }
            break;
          }
        }
       Serial.print(",");
       Serial.println(out*100);

  }
  
}

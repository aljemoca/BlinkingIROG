#include "adicionales.h"


float findmin (float *array, unsigned char n)
{
	float minimo=array[1];

	for (unsigned char p = 0;p<n;p+=2)
	{
		if(minimo > array[p])
			minimo = array[p];
	}
	return minimo;
}


int roundb( int num, int den)
{
  int res;
  if ( num%den == den)
  {
    res=num;
    if (res%2 == 1)
       res++;
  }
  else
    res=round(num/den);
   
  return res;
}



//---------------------------------------------------------------------------

#pragma hdrstop

#include "wngen.h"
#include <math.h>
//#include <cstdlib>
#include <cstdlib>
#include <ctime>
#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifndef max(a, b)
#define max(a, b) ( ( (a) > (b) ) ? (a) : (b) )
#endif

#ifndef min(a, b)
#define min(a, b) ( ( (a) < (b) ) ? (a) : (b) )
#endif

//===========================================================================
// Белый шум

CWhiteNoiseGenerator::CWhiteNoiseGenerator( double p_dW0 )
{
    setW0( p_dW0 );
}

//---------------------------------------------------------------------------

void CWhiteNoiseGenerator::setW0( double p_dW0 )
{
    m_dW0 = max(1e-10,p_dW0);
    m_dDT = 2 * M_PI / m_dW0;
    m_dSigma = 1/sqrt(m_dDT);
    Reset();
}

//---------------------------------------------------------------------------

void CWhiteNoiseGenerator::Reset()
{
    m_iIntervalCount = 0;
    srand ( time(NULL) );     // инициализация функции rand значением функции time
//    Randomize();
}

//---------------------------------------------------------------------------

double CWhiteNoiseGenerator::getValue( double p_dT )
{
    static double ls_dRandomValue = 0;
    if ( (int)(p_dT / m_dDT) != m_iIntervalCount )
    {
     //   ls_dRandomValue = RandG(0, m_dSigma);
        ls_dRandomValue = gaussrand(10.5, m_dSigma);
        m_iIntervalCount = (int)(p_dT / m_dDT);
    }
    return ls_dRandomValue;
}

//---------------------------------------------------------------------------


double CWhiteNoiseGenerator::gaussrand(double MO, double sko)
{
    float sum=0, x;

    for (int i=0;i<25;i++)
        sum+=1.0*rand()/RAND_MAX;
    x = (sqrt(2.0)*(sko)*(sum-12.5))/1.99661 + MO;

    return x;
}

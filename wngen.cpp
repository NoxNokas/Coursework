//---------------------------------------------------------------------------

#pragma hdrstop

#include "wngen.h"
#include <math.h>
#include <chrono>
#include <random>
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
    m_dDT =2 * M_PI / m_dW0;
    m_dSigma = 1/sqrt(m_dDT);
    Reset();
}

//---------------------------------------------------------------------------

void CWhiteNoiseGenerator::Reset()
{  
    m_iIntervalCount = 0;
}

//---------------------------------------------------------------------------

double CWhiteNoiseGenerator::getValue( double p_dT )
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution (0,m_dSigma);
    static double ls_dRandomValue = 0;
    if ( (int)(p_dT / m_dDT) != m_iIntervalCount )
    {
        ls_dRandomValue = distribution(generator);
        m_iIntervalCount = (int)(p_dT / m_dDT);
    }
    return ls_dRandomValue;
}

//---------------------------------------------------------------------------

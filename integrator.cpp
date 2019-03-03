#include <stdio.h>
#include <math.h>
#include "integrator.h"
#include "model.h"
#include <exception>

#define max(a, b) ( ( (a) > (b) ) ? (a) : (b) )
#define min(a, b) ( ( (a) < (b) ) ? (a) : (b) )

TIntegrator::TIntegrator()
    : m_ldEps( 1e-4 )
    , m_ldEpsGlobal( 0 )
    , m_pMathModel( NULL )
{
}

//===========================================================================
// class CDormandPrinceMethod

const long double CDormandPrinceMethod::c[7] = { 0, 1./5, 3./10, 4./5, 8./9, 1., 1. };
const long double CDormandPrinceMethod::a[7][6] = {
    { 0. },
    { 1./5 },
    { 3./40, 9./40 },
    { 44./45, -56./15, 32./9 },
    { 19372./6561, -25360./2187, 64448./6561, -212./729 },
    { 9017./3168, -355./33, 46732./5247, 49./176, -5103./18656 },
    { 35./384, 0., 500./1113, 125./192, -2187./6784, 11./84 }
};
const long double CDormandPrinceMethod::  b[7] = { 35./384, 0., 500./1113, 125./192, -2187./6784, 11./84, 0 };
const long double CDormandPrinceMethod::__b[7] = { 5179./57600, 0., 7571./16695, 393./640, -92097./339200, 187./2100, 1./40 };

//---------------------------------------------------------------------------

CDormandPrinceMethod::CDormandPrinceMethod()
    : TIntegrator()
{
}

template <class T>
struct STypeZero
{
    STypeZero()
    {
        T l_tTmp = 1;
        while ( ( l_tTmp /= 2 ) + 1 > 1 )
            value = l_tTmp;
    }
    T value;
};
 STypeZero< long double > g_ldZero;


//---------------------------------------------------------------------------

long double CDormandPrinceMethod::Run()
{
 //   if ( ! dynamic_cast<const TModel*>( m_pMathModel ) )
     //   throw Exception("Error: Bad model!"); //�� ���� ������� ��������� Exception �� ������ exception!?
  //      throw std::exception();                 //����� ���� ����� ���

    long double // ��� ����� ��� �������������� (������������� �� �������� ���� ��������������)
                t = m_pMathModel->getT0(),
                // ��� ����� ��� ������ (������������� ��������� �� �������� ���������)
                l_ldTOut = t,
                // ��� �������� �����
                t1 = m_pMathModel->getT1(),
                // ��� ���
                h,
                // ��� ��� ����� ��������� (���������������� ���������� ������)
                h_new = m_pMathModel->getSamplingFrequency(),
                // ��� ������ ���������
              __e = 0;

    QVector<long double> // ��� ������ �� ������ ����
            X = m_pMathModel->getInitialConditions(),
            // ��� ������ �� ����� ����
            X1( X.size() ),
            // ��� ������ �� ����� ���� ��� ���������
          __X1( X.size() ),
            // ��� ������ ��� ������
            Xout ( X.size() ),
            // ��� ����� ��� ���������� ������������� �
            Y( X.size() );

    // ���������� ����������� (����� �������� ��������������)
    m_ldEpsGlobal = 0;

    // �������������� ���������� ����������� � ������ (����������� � � ���������� � ��������� ��������)
    int l_iResultCapacity =  (int)( (m_pMathModel->getT1() - m_pMathModel->getT0()) / m_pMathModel->getSamplingFrequency() ) + 1;
    // �������������� ��������� ����������� � ������, ����� ��������� ������� �� ��������� ������
    m_pMathModel->setResultCapacity( l_iResultCapacity );
    // �������������� ����� ������������ (��� ������� ��� ���������� � ������)
    m_pMathModel->addResultToLagBuffer( X, t );

    // �������������� ����������� ������������� K[j]
    for ( int j = 7; j > 0; --j, K[j].resize( X.size() ) );

    // ������� ����
    while ( t < t1 )
    {
        // ������������� ��� �� ��������
        h = min( h_new, m_pMathModel->getMaxStep() );

        // ��������� ������������ �
        for ( int j = 0; j < 7; j++ )
        {
            for ( int k = X.size()-1; k >= 0; k-- )
            {
                Y[k] = X[k];
                for ( int s = 0; s < j; s++ )
                {
                    Y[k] += K[s][k] * a[j][s] * h;
                }
            }
            m_pMathModel->getRight( Y, t + c[j] * h, K[j] );
        }

        // ��������� ����� �������� ����������� � ��������� ������ (4 � 5 �������)
        __e = 0;
        for ( int k = X.size()-1; k >= 0; k-- )
        {
            X1[k] = __X1[k] = X[k];
            for ( int j = 0; j < 7; j++ )
            {
                X1[k] += K[j][k] * b[j] * h;
                __X1[k] += K[j][k] * __b[j] * h;
            }
            __e += powl( h * (X1[k] - __X1[k]) / max( max( fabsl(X[k]), fabsl(X1[k]) ), max((long double)1e-5, 2* g_ldZero.value/m_ldEps) ) , 2 );
        }
        __e = sqrtl( __e / X.size() );

        // ��������� ����
        h_new = h / max( 0.1, min( 5., powl(__e / m_ldEps, 0.2)/0.9 ) );

        // ���� ��������� ������ ��������� ������������� ��������, �������� ������� ��� ������
        if ( __e > m_ldEps )
            continue;

        // ���������� ������ ������������ (��� ������� ��� ���������� � ������)
        m_pMathModel->addResultToLagBuffer( X, t );

        // ���������� �����������
        while ( (l_ldTOut < t + h) && (l_ldTOut <= t1) )
        {
            long double l_ldTheta = (l_ldTOut - t)/h,
                        l_aB[6];
            // ������������ ������������ ������� ������
            l_aB[0] = l_ldTheta * ( 1 + l_ldTheta*(-1337./480. + l_ldTheta*(1039./360. + l_ldTheta*(-1163./1152.))));
            l_aB[1] = 0;
            l_aB[2] = 100. * powl(l_ldTheta, 2) * (1054./9275. + l_ldTheta*(-4682./27825. + l_ldTheta*(379./5565.)))/3.;
            l_aB[3] = -5. * powl(l_ldTheta, 2) * (27./40. + l_ldTheta*(-9./5. + l_ldTheta*(83./96.)))/2.;
            l_aB[4] = 18225. * powl(l_ldTheta, 2) * (-3./250. + l_ldTheta*(22./375. + l_ldTheta*(-37./600.)))/848.;
            l_aB[5] = -22. * powl(l_ldTheta, 2) * (-3./10. + l_ldTheta*(29./30. + l_ldTheta*(-17./24.)))/7.;

            // �������� ��������� ��� ������
            for ( int k = X.size()-1; k >= 0; k-- )
            {
                long double l_ldSum  = 0;
                for ( int j = 5; j >= 0; j-- )
                    l_ldSum += l_aB[j] * K[j][k];
                Xout[k] = X[k] + h * l_ldSum;
            }
            // ������
            m_pMathModel->addResult( Xout, l_ldTOut );
            // ���������� ����� ������
            l_ldTOut += m_pMathModel->getSamplingFrequency();
        }

        // ���� ��������� �� �������������, �� ����������� ����� � ��������� X
        X = X1;
        t += h;

        // ������� ����� ���������� �������� ��� ���������� ���������� �����������
        m_ldEpsGlobal++;
    }
    m_ldEpsGlobal = m_ldEps / pow( m_ldEpsGlobal, 1.5 );
    return m_ldEpsGlobal;
}

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
     //   throw Exception("Error: Bad model!"); //мб надо создать наследник Exception от класса exception!?
  //      throw std::exception();                 //пусть пока будет так

    long double // Это время для интегрирования (увеличивается на величину шага интегрирования)
                t = m_pMathModel->getT0(),
                // Это время для выдачи (увеличивается дискретно на величину плотности)
                l_ldTOut = t,
                // Это конечное время
                t1 = m_pMathModel->getT1(),
                // Это шаг
                h,
                // Это шаг после коррекции (инициализируется плотностью выдачи)
                h_new = m_pMathModel->getSamplingFrequency(),
                // Это ошибка локальная
              __e = 0;

    QVector<long double> // Это вектор на начало шага
            X = m_pMathModel->getInitialConditions(),
            // Это вектор на конец шага
            X1( X.size() ),
            // Это вектор на конец шага для коррекции
          __X1( X.size() ),
            // Это вектор для выдачи
            Xout ( X.size() ),
            // Это буфер для вычисления коэффициентов К
            Y( X.size() );

    // Глобальная погрешность (всего процесса интегрирования)
    m_ldEpsGlobal = 0;

    // Предполагаемое количество результатов в выдаче (согласовано с её плотностью и суммарным временем)
    int l_iResultCapacity =  (int)( (m_pMathModel->getT1() - m_pMathModel->getT0()) / m_pMathModel->getSamplingFrequency() ) + 1;
    // Подготавливаем хранилище результатов в модели, чтобы уменьшить расходы на выделение памяти
    m_pMathModel->setResultCapacity( l_iResultCapacity );
    // Инициализируем буфер запаздывания (при условии его реализации в модели)
    m_pMathModel->addResultToLagBuffer( X, t );

    // Инициализируем размерности коэффициентов K[j]
    for ( int j = 7; j > 0; --j, K[j].resize( X.size() ) );

    // Главный цикл
    while ( t < t1 )
    {
        // Устанавливаем шаг на итерацию
        h = min( h_new, m_pMathModel->getMaxStep() );

        // Вычисляем коэффициенты К
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

        // Вычисляем новые значения результатов и локальной ошибки (4 и 5 порядок)
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

        // Коррекция шага
        h_new = h / max( 0.1, min( 5., powl(__e / m_ldEps, 0.2)/0.9 ) );

        // Если локальная ошибка превышает установленную величину, пытаемся сделать шаг заново
        if ( __e > m_ldEps )
            continue;

        // Наполнение буфера запаздывания (при условии его реализации в модели)
        m_pMathModel->addResultToLagBuffer( X, t );

        // Наполнение результатов
        while ( (l_ldTOut < t + h) && (l_ldTOut <= t1) )
        {
            long double l_ldTheta = (l_ldTOut - t)/h,
                        l_aB[6];
            // Рассчитываем коэффициенты плотной выдачи
            l_aB[0] = l_ldTheta * ( 1 + l_ldTheta*(-1337./480. + l_ldTheta*(1039./360. + l_ldTheta*(-1163./1152.))));
            l_aB[1] = 0;
            l_aB[2] = 100. * powl(l_ldTheta, 2) * (1054./9275. + l_ldTheta*(-4682./27825. + l_ldTheta*(379./5565.)))/3.;
            l_aB[3] = -5. * powl(l_ldTheta, 2) * (27./40. + l_ldTheta*(-9./5. + l_ldTheta*(83./96.)))/2.;
            l_aB[4] = 18225. * powl(l_ldTheta, 2) * (-3./250. + l_ldTheta*(22./375. + l_ldTheta*(-37./600.)))/848.;
            l_aB[5] = -22. * powl(l_ldTheta, 2) * (-3./10. + l_ldTheta*(29./30. + l_ldTheta*(-17./24.)))/7.;

            // Получаем результат для выдачи
            for ( int k = X.size()-1; k >= 0; k-- )
            {
                long double l_ldSum  = 0;
                for ( int j = 5; j >= 0; j-- )
                    l_ldSum += l_aB[j] * K[j][k];
                Xout[k] = X[k] + h * l_ldSum;
            }
            // Выдача
            m_pMathModel->addResult( Xout, l_ldTOut );
            // Наращиваем время выдачи
            l_ldTOut += m_pMathModel->getSamplingFrequency();
        }

        // Если коррекция не потребовалась, то увеличиваем время и обновляем X
        X = X1;
        t += h;

        // Считаем общее количество итераций для вычисления глобальной погрешности
        m_ldEpsGlobal++;
    }
    m_ldEpsGlobal = m_ldEps / pow( m_ldEpsGlobal, 1.5 );
    return m_ldEpsGlobal;
}

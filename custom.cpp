//---------------------------------------------------------------------------

#include "custom.h"
#include <math.h>
//---------------------------------------------------------------------------
// Задача Аренсторфа (начальные условия 1)

const long double TArenstorfModel::m  = 0.012277471;

TArenstorfModel::TArenstorfModel() : TLagModel()
{

    X0.resize(4);
    X0[0] = 0.994;
    X0[1] = 0;
    X0[2] = 0;
    X0[3] = -2.0015851063790825224053786222;
}

//---------------------------------------------------------------------------

void TArenstorfModel::getRight( const QVector<long double>& X, long double t, QVector<long double>& Y )
{
    Y.resize(4);
    D1 = pow( pow( X[0] + m, 2 ) + pow( X[1], 2 ), 1.5 );
    D2 = pow( pow( X[0] + m - 1, 2 ) + pow( X[1], 2 ), 1.5 );
    Y[0] = X[2];
    Y[1] = X[3];
    Y[2] = X[0] + 2 * X[3] - (1 - m)*(X[0] + m) / D1 - m * (X[0] + m - 1) / D2;
    Y[3] = X[1] - 2 * X[2] - (1 - m)*X[1] / D1 - m * X[1] / D2;
}

//---------------------------------------------------------------------------
// Кинетика ферментативной реакции
TEnzymaticModel::TEnzymaticModel()
{
    tau = 4.;
    lambda = 0.2;
    T = 5;
    M = 10.5;
    D = 2.25;
    K = 4.7434164903;
    m_oWNGenerator.setW0(100);
    m_ldMaxStep = m_oWNGenerator.getDeltaT();
    m_oLagBuffer.setTimeDepth(tau + 0.1);
    X0.resize(5);
    X0[0] = 62.;
    X0[1] = 0.;
    X0[2] = 0.;
    X0[3] = 20.;
    X0[4] = 0;

}

//---------------------------------------------------------------------------

void TEnzymaticModel::getRight( const QVector<long double>& X, long double t, QVector<long double>& Y )
{
    Y.resize(5);
    z = 1 / pow(1 + 0.0005 * m_oLagBuffer.getDataByTime(t-tau)[3], 3);
    Y[0] = M + X[4] - z * X[0];
    Y[1] = z * X[0] - X[1] ;
    Y[2] = X[1] - X[2];
    Y[3] = X[2] - 0.5 * X[3];
    Y[4] = (1/T) * (K * m_oWNGenerator.getValue(t) - X[4]);
}

//---------------------------------------------------------------------------

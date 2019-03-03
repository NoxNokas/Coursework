#pragma once//---------------------------------------------------------------------------



#include "model.h"
#include <QVector>
#include "wngen.h"
//---------------------------------------------------------------------------
// Задача Аренсторфа (начальные условия 1)

class TArenstorfModel  : public TLagModel
{
    protected:
    static const long double m;
    long double D1, D2;
    public:
        TArenstorfModel(  );
        ~TArenstorfModel(  ) { };
        void getRight( const  QVector<long double>& X, long double t,  QVector<long double>& Y );
};


//---------------------------------------------------------------------------
// Кинетика ферментативной реакции

class TEnzymaticModel : public TLagModel
{
    protected:
        long double z, tau, K, T, D, lambda;

        CWhiteNoiseGenerator m_oWNGenerator;
    public:
        TEnzymaticModel();
        void getRight( const QVector<long double>& X, long double t, QVector<long double>& Y );
};
//---------------------------------------------------------------------------

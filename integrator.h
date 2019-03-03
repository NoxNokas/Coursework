
#pragma once
// Подключение заголовочного файла библиотеки векторных и матричных операций
#include <QVector>
//---------------------------------------------------------------------------




// Опережающие декларации

class TModel;

class TIntegrator
{
    protected:
        // Указатель на обрабатываемую модель
        TModel *m_pMathModel;
        // Погрешности: заданная максимальная погрешность на шаге, результирующая глобальная погрешность
        long double m_ldEps, m_ldEpsGlobal;

    public:
        TIntegrator();

        inline void setPrecision( long double p_ldEps ) { this->m_ldEps = p_ldEps; }
        inline long double getPrecision() const { return m_ldEps; }
        inline long double getGlobalPrecision() const { return m_ldEpsGlobal; }
        inline void setMathModel( TModel* p_pMathModel ) { this->m_pMathModel = p_pMathModel; }
        inline const TModel* getMathModel() const { return m_pMathModel; }
        // Абстрактный метод, реализующий процедуру численного интегрирования и возвращающий глобальную погрешность вычислений
        virtual long double Run() = 0;
};

//---------------------------------------------------------------------------

class CDormandPrinceMethod : public TIntegrator
{
    private:
        // Коэффициенты a,b,c
        static const long double c[7], a[7][6], b[7], __b[7];
        // Коэффициенты K[i]
        QVector<long double> K[7];


    public:
        CDormandPrinceMethod();
        long double Run();

};

//---------------------------------------------------------------------------



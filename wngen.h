#ifndef WNGEN_H
#define WNGEN_H

//===========================================================================
// Класс генератора белого шума

class CWhiteNoiseGenerator
{
    protected:
        double m_dW0, m_dDT, m_dSigma;  // Параметры белого шума (частота среза системы, интервал корреляции, СКО случайного процесса)
        int m_iIntervalCount;           // количество пройденных интервалов корреляции от момента времени t = 0 (обеспечивает эволюцию СП)

    public:
        CWhiteNoiseGenerator( double p_dW0 = 1e5 );
        void Reset();
        void setW0( double p_dW0 );
        inline double getW0() const { return m_dW0; }
        inline double getDeltaT() const { return m_dDT; }
        double getValue( double p_dT );
};

//---------------------------------------------------------------------------
#endif // WNGEN_H

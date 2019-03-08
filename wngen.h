#ifndef WNGEN_H
#define WNGEN_H

//===========================================================================
// ����� ���������� ������ ����

class CWhiteNoiseGenerator
{
    protected:
        double m_dW0, m_dDT, m_dSigma;  // ��������� ������ ���� (������� ����� �������, �������� ����������, ��� ���������� ��������)
        int m_iIntervalCount;           // ���������� ���������� ���������� ���������� �� ������� ������� t = 0 (������������ �������� ��)

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

//---------------------------------------------------------------------------

#pragma once

// ����������� ������������� ����� ���������� ��������� � ��������� ��������
//#include "array.h"
#include "integrator.h"

#include <QDebug>
#include <QVector>

class TDataStorage : protected QVector<QVector<long double>>
{
    protected:

        // ������ �������� �������
        QVector <long double> m_vTime;

        // ������� ���������� ����������
        int m_iUsedCapacity;

        // ���������� ����� � ������������ (������������ � getResultByTime)
        int m_iInterpolationPointCount;

    public:

        // �����������
        TDataStorage();
        // ���������� ���������� �������� �������� (��� ��������� ��������� ������)
        TDataStorage& setLength( int p_iLength );

        inline int getLength() const { return QVector<QVector<long double>>::size(); }
        inline int getHigh() const { return QVector<QVector<long double>>::size() - 1; }

        // ���������� ����������� ��������
        inline int getUsedCapacity() const { return m_iUsedCapacity; }
        // ������ � �������� ������
        inline const QVector<long double>& operator[] ( int i ) const { return QVector<QVector<long double>>::operator[](i); }
        void addData( const QVector<long double>& Y, long double t );
        // ��������� ������� � ������ �������
        const QVector<long double>& getDataByTime( long double p_ldT, bool p_bUseInterpolation = false ) const;
        // ��������� ��������� ������� �� �������
        inline long double getTime( int i ) const { return m_vTime.at(i); }

        // ���������� ������ �� ��������� �������� (��������� ��� ������������� ����� ���������� � getResultByTime() )
        TDataStorage& SortByTime();
};

class TBoundedDataStorage : public TDataStorage
{
    protected:

        // �����, ��� �������� �������� �������� � ������
        long double m_ldTimeDepth;

        // ����� ��������� ������������ ����� ��������...
        inline TDataStorage& setLength( int p_iLength ) { return TDataStorage::setLength( p_iLength ); }

    public:
        // TBoundedDataStorage
        TBoundedDataStorage();

        // ���������� �������� ��������, ��� �������� ����� ��������� ��������
        TBoundedDataStorage& setTimeDepth( long double p_ldTimeDepth );
        inline long double getTimeDepth() const { return m_ldTimeDepth; }

        // ���������� ����� ������
        void addData( const QVector<long double>& Y, long double t );

        // ��������� ������� ����������, ����������� � ������� ������� ( ����������� ������ ������������ )
        inline const QVector<long double>& getDataByTime( long double p_ldT ) const { return TDataStorage::getDataByTime( p_ldT, true ); }

};


//---------------------------------------------------------------------------
// ������� ����� ������ ��� �����������
class TModel
{
    protected:
        // ��������� �������
        QVector<long double> X0;

        // ������� ������������� (1/�) - �������� ����� ������������
        long double m_ldSamplingFrequency;
        // ����������� ���������� ��� ������������� (��� �������������) read-only
        long double m_ldMaxStep;
        // ������ � ��������� ������� ��������������
        long double m_ldT0, m_ldT1;

        // ��������� �����������
        TDataStorage m_oResultStorage;

    public:
        // ����� ����������� ������� - ������������� ���������� ��-���������
        TModel()
            : m_ldSamplingFrequency( 1e-2 )
            , m_ldMaxStep( 0.062 )
            , m_ldT0( 0 )
            , m_ldT1( 160 )
        {}

        // ������ �������� ���������
        inline void Run( TIntegrator *p_pMethod ) { p_pMethod->setMathModel( this ); p_pMethod->Run(); }

        // ������������� ������� ������ ������ (X - ����, t - ����������� ��������, Y - �����)
        virtual void getRight( const QVector<long double>& X, long double t, QVector<long double>& Y ) = 0;
        // ��������������� ������� ������ ������
        inline QVector<long double> getRight( const QVector<long double>& X, long double t )
        {
            QVector<long double> Y;
            this->getRight( X, t, Y );
            return Y;
        }
        // ������� ��������� �������
        inline void setInitialConditions( const QVector<long double>& X0 ) { this->X0 = X0; }
        // ��������� ��������� �������
        inline QVector<long double> getInitialConditions() const { return X0; }
        // ������� ������� - �� ����������� ������� ��������� �������
        inline int getOrder() const { return X0.size(); }

        // ���������� �������� ������
        inline void setSamplingFrequency( long double p_ldSamplingFrequency ) { this->m_ldSamplingFrequency = p_ldSamplingFrequency; }
        inline long double getSamplingFrequency() const { return m_ldSamplingFrequency; }

        // ���������� ��������� ���������� �������� ������
        inline long double getT0() const { return m_ldT0; }
        inline long double getT1() const { return m_ldT1; }
        inline void setT0( long double p_ldT0 ) { this->m_ldT0 = p_ldT0; }
        inline void setT1( long double p_ldT1 ) { this->m_ldT1 = p_ldT1; }

        // ����������� ���������� ��� �������������� (���������� ��� �����������)
        inline long double getMaxStep() const { return m_ldMaxStep; }


        inline const TDataStorage& getResultStorage() const { return m_oResultStorage; }

        // �������������� ������������� ��������� ��� �����������
        // ������ �����������
        inline virtual void addResult( const QVector<long double>& Y, long double t ) { m_oResultStorage.addData( Y, t ); }
        inline void setResultCapacity( int p_iResultCapacity ) { m_oResultStorage.setLength( p_iResultCapacity ); }
        // ������ � ����� ������������ (����������� �� ������ ���� ��������������)
        inline virtual void addResultToLagBuffer( const QVector<long double>& Y, long double t ) {}; // empty
};

class TLagModel : public TModel
{
    protected:
        TBoundedDataStorage m_oLagBuffer;
    public:
        TLagModel() : TModel() {}
        inline void addResultToLagBuffer( const QVector<long double>& Y, long double t ) { m_oLagBuffer.addData(Y, t); /*qDebug() << Y*/ }
};



//---------------------------------------------------------------------------

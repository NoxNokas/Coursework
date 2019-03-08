//---------------------------------------------------------------------------

#pragma once

// Подключение заголовочного файла библиотеки векторных и матричных операций
//#include "array.h"
#include "integrator.h"

#include <QDebug>
#include <QVector>

class TDataStorage : protected QVector<QVector<long double>>
{
    protected:

        // Вектор значений времени
        QVector <long double> m_vTime;

        // Счетчик количества добавлений
        int m_iUsedCapacity;

        // Количество точек в интерполяции (используется в getResultByTime)
        int m_iInterpolationPointCount;

    public:

        // Конструктор
        TDataStorage();
        // Установить количество хранимых векторов (для ускорения выделения памяти)
        TDataStorage& setLength( int p_iLength );

        inline int getLength() const { return QVector<QVector<long double>>::size(); }
        inline int getHigh() const { return QVector<QVector<long double>>::size() - 1; }

        // Количество заполненных векторов
        inline int getUsedCapacity() const { return m_iUsedCapacity; }
        // Доступ к элементу данных
        inline const QVector<long double>& operator[] ( int i ) const { return QVector<QVector<long double>>::operator[](i); }
        void addData( const QVector<long double>& Y, long double t );
        // Получение вектора в момент времени
        const QVector<long double>& getDataByTime( long double p_ldT, bool p_bUseInterpolation = false ) const;
        // Получение временной отметки по индексу
        inline long double getTime( int i ) const { return m_vTime.at(i); }

        // Сортировка данных по временным отметкам (выполнять при необходимости перед обращением к getResultByTime() )
        TDataStorage& SortByTime();
};

class TBoundedDataStorage : public TDataStorage
{
    protected:

        // Время, для которого хранятся значения в буфере
        long double m_ldTimeDepth;

        // Явную процедуру установления длины скрываем...
        inline TDataStorage& setLength( int p_iLength ) { return TDataStorage::setLength( p_iLength ); }

    public:
        // TBoundedDataStorage
        TBoundedDataStorage();

        // Установить времнной интервал, для которого будут храниться значения
        TBoundedDataStorage& setTimeDepth( long double p_ldTimeDepth );
        inline long double getTimeDepth() const { return m_ldTimeDepth; }

        // Добавление блока данных
        void addData( const QVector<long double>& Y, long double t );

        // Получение вектора результата, отнесенного к моменту времени ( интерпояция всегда используется )
        inline const QVector<long double>& getDataByTime( long double p_ldT ) const { return TDataStorage::getDataByTime( p_ldT, true ); }

};


//---------------------------------------------------------------------------
// Базовый класс модели для интегратора
class TModel
{
    protected:
        // Начальные условия
        QVector<long double> X0;

        // Частота дискретизации (1/с) - интервал между результатами
        long double m_ldSamplingFrequency;
        // Максимально допустимый шаг инегрирования (при необходимости) read-only
        long double m_ldMaxStep;
        // Начало и окончание времени интегрирования
        long double m_ldT0, m_ldT1;

        // Хранилище результатов
        TDataStorage m_oResultStorage;

    public:
        // Общий конструктор моделей - инициализация параметров по-умолчанию
        TModel()
            : m_ldSamplingFrequency( 1e-2 )
            , m_ldMaxStep( 0.062 )
            , m_ldT0( 0 )
            , m_ldT1( 160 )
        {}

        // Запуск процесса обработки
        inline void Run( TIntegrator *p_pMethod ) { p_pMethod->setMathModel( this ); p_pMethod->Run(); }

        // Перегружаемая функция правых частей (X - вход, t - независимый аргумент, Y - выход)
        virtual void getRight( const QVector<long double>& X, long double t, QVector<long double>& Y ) = 0;
        // Неперегружаемая функция правых частей
        inline QVector<long double> getRight( const QVector<long double>& X, long double t )
        {
            QVector<long double> Y;
            this->getRight( X, t, Y );
            return Y;
        }
        // Задание начальных условий
        inline void setInitialConditions( const QVector<long double>& X0 ) { this->X0 = X0; }
        // Получение начальных условий
        inline QVector<long double> getInitialConditions() const { return X0; }
        // Порядок системы - по размерности вектора начальных условий
        inline int getOrder() const { return X0.size(); }

        // Управление частотой выдачи
        inline void setSamplingFrequency( long double p_ldSamplingFrequency ) { this->m_ldSamplingFrequency = p_ldSamplingFrequency; }
        inline long double getSamplingFrequency() const { return m_ldSamplingFrequency; }

        // Управление временным интервалом эволюции модели
        inline long double getT0() const { return m_ldT0; }
        inline long double getT1() const { return m_ldT1; }
        inline void setT0( long double p_ldT0 ) { this->m_ldT0 = p_ldT0; }
        inline void setT1( long double p_ldT1 ) { this->m_ldT1 = p_ldT1; }

        // Максимально допустимый шаг интегрирования (информация для интегратора)
        inline long double getMaxStep() const { return m_ldMaxStep; }


        inline const TDataStorage& getResultStorage() const { return m_oResultStorage; }

        // Дополнительный универсальный интерфейс для интегратора
        // Запись результатов
        inline virtual void addResult( const QVector<long double>& Y, long double t ) { m_oResultStorage.addData( Y, t ); }
        inline void setResultCapacity( int p_iResultCapacity ) { m_oResultStorage.setLength( p_iResultCapacity ); }
        // Запись в буфер запаздывания (выполняется на каждом шаге интегрирования)
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

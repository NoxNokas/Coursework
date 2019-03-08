//---------------------------------------------------------------------------

#include "model.h"
#include "math.h"
#include <math.h>
#include "iostream"

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

STypeZero< long double > g_ldZero1;


//===========================================================================
// Хранилище результатов

// Конструктор
TDataStorage::TDataStorage()
    : m_iUsedCapacity ( 0 )
    , m_iInterpolationPointCount( 6 )
{
}

//---------------------------------------------------------------------------

TDataStorage& TDataStorage::setLength( int p_iLength )
{
    p_iLength = std::max(0, p_iLength);
    //->setLength( p_iLength );
    for(int i = 0; i <= p_iLength; i++){
       QVector<QVector<long double>>::append(QVector<long double>(5, 0));
       m_vTime.append(0.);
    }
    if ( p_iLength < m_iUsedCapacity )
        m_iUsedCapacity = p_iLength;
    return *this;
}

//---------------------------------------------------------------------------
// Управление сохранением результатов

void TDataStorage::addData( const QVector<long double>& Y, long double t )
{
    //qDebug() << "came " << (double)t << " " << (double)Y[1];
    if ( m_iUsedCapacity < this->getLength() )
    {
        QVector<QVector<long double>>::operator[]( m_iUsedCapacity ) = Y;
        m_vTime[ m_iUsedCapacity ] = t;
    }
    else
    {
        QVector<QVector<long double>>::append( Y );
        m_vTime.append( t );
    }

    m_iUsedCapacity++;
   // qDebug() << "added " << (double)m_vTime[ m_iUsedCapacity - 1 ] << " " << (double)(*this)[t][2];
   // qDebug()<<"add " << (double)t;
}

//---------------------------------------------------------------------------

const QVector<long double>& TDataStorage::getDataByTime( long double p_ldT, bool p_bUseInterpolation ) const
{
    // Выполняется бинарный поиск в отсортированном по временным отметкам хранилище
    int i = 0,
        j = m_iUsedCapacity - 1,
        m;
    while ( (j-i) > 1 )
    {
        m = static_cast<int>(i + (j-i)/2);
        if ( p_ldT > m_vTime[m] )
            i = m;
        else
            j = m;
    }

    if ( p_bUseInterpolation )
    {
        // Выполняется интерполяция по Лагранжу
        int l_iLeft = std::max( 0., j - m_iInterpolationPointCount / 2. ),
            l_iRight = std::min( m_iUsedCapacity - 1, l_iLeft + m_iInterpolationPointCount );
        // Не позволяем экстраполировать
        p_ldT = std::min( m_vTime[l_iRight], std::max( m_vTime[l_iLeft], p_ldT ) );

        static QVector<long double> ls_vRes( (*this)[i].size() );
        //static QVector<long double> ls_vRes( this->at(i).size() );
        for(int i = 0; i < ls_vRes.size()-1; i++) {
            ls_vRes[i] = 0.;
        }

        long double l_ldK;
        for (int i = l_iLeft; i <= l_iRight; i++)
        {
            l_ldK = 1.;
            for (int j = l_iLeft; j <= l_iRight; j++)
            {
                if ( i != j )
                    l_ldK *= ( p_ldT - m_vTime[j] ) / ( m_vTime[i] - m_vTime[j] );
            }
            for (int k = ls_vRes.size()-1; k >= 0; k--)
                ls_vRes[k] += l_ldK * (*this)[i][k];
        }
        return ls_vRes;
    }
    else
        //qDebug()<<"getByData";
        // Возвращает результат, ближайший к указанной временной отметке
        return ( fabsl(p_ldT - m_vTime[i]) < fabsl(p_ldT - m_vTime[j]) ) ?
            (*this)[i]:
            (*this)[j];

}

//---------------------------------------------------------------------------

TDataStorage& TDataStorage::SortByTime()
{
    this->setLength( m_iUsedCapacity );
    bool l_bFinish;
    int j = 0;
    do
    {
        l_bFinish = true;
        for ( int i = j+1; i < this->getLength(); i++ )
            if ( m_vTime[i] < m_vTime[j] )
            {
                QVector<long double> temp =  QVector<QVector<long double>>::at(j);
                QVector<QVector<long double>>::replace(j, QVector<QVector<long double>>::at(i));
                QVector<QVector<long double>>::replace(i, temp);

                long double tmp = m_vTime.at(j);
                m_vTime.replace(j, m_vTime.at(i));
                m_vTime.replace(i, tmp);


                l_bFinish = false;
            }
        j++;
    }
    while ( ! l_bFinish );
  //  qDebug() << "SortByTime";
    return *this;

}

//===========================================================================

//const int CBoundedDataStorage::mc_iInterpolationPointCount = 20;

TBoundedDataStorage::TBoundedDataStorage()
    : TDataStorage()
    , m_ldTimeDepth(0)
{
    // Для более точной интерполяции
    m_iInterpolationPointCount = 12;
}

//---------------------------------------------------------------------------

TBoundedDataStorage& TBoundedDataStorage::setTimeDepth( long double p_ldTimeDepth )
{
    if ( (this->getHigh() > 0) && (p_ldTimeDepth < (m_vTime[ this->getHigh() ] - m_vTime[0])) )
    {
        // При необходимости обрезаем массив снизу в соответствии с указанным значением глубины
        int i = 0,
        j = this->getHigh(),
        m;
        while ( (j-i) > 1 )
        {
            m = i + (j-i)/2;
            if ( (m_vTime[ this->getHigh() ] - p_ldTimeDepth) > m_vTime[m] )
                i = m;
            else
                j = m;
        }
        if ( i > 0 )
        {
            for(int k = i; k <= this->getLength() - i; k++){
                //this->replace(0, QVector<long double>(1, 0));
                this->replace(0, this->at(k));
                m_vTime.replace(0, k);
            }
            this->setLength( this->getLength() - i );
//            this->replace( i, this->size() - i );
//            m_vTime.replace( i, this->length() - i );
//            this->setLength( this->getLength() - i );
        }
    }
    m_ldTimeDepth = p_ldTimeDepth;
    return *this;
}

//---------------------------------------------------------------------------
// Добавлять можно только последовательно возрастающие моменты времени

void TBoundedDataStorage::addData( const QVector<long double>& Y, long double t )
{
    if ( this->getLength() > 0 )
    {
        if ((t - m_vTime[0]) >= m_ldTimeDepth )
        {
            // Сдвигаем все вниз, устанавливаем последний блок
            QVector<QVector<long double>>::append( Y );
            m_vTime.append( t );

            QVector<QVector<long double>>::removeFirst();
            m_vTime.removeFirst();
           // qDebug() << "addedToBounded " << (double)m_vTime[t] << " " << (double)(*this)[t][0];
            return;
        }
        else if ( t < (m_vTime[ this->getHigh() ] + g_ldZero1.value ) )
            return;
    }

    // Если все остальное не выполнилось, просто добавляем блок
    QVector<QVector<long double>>::append( Y );
    m_vTime.append( t );
    m_iUsedCapacity++;

   //qDebug()<<"addDataBounded";
}



#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void showResult(TEnzymaticModel* model);
void statCharacteristics();

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    //TEnzymaticModel* Model1 = new TEnzymaticModel();
    //showResult(Model1);
    statCharacteristics();
    //delete Model1;

}

void MainWindow::statCharacteristics(){
    const int N = 5;
    TIntegrator* Integrator = new CDormandPrinceMethod();
        Integrator->setPrecision(1e-32);

        //Массив векторов реализаций
        QVector<QVector<QVector<long double>>> Result(N);
        for (int i = 0; i < N; i++){
            Result[i].resize(5);
            for (int j = 0; j < 5; j++){
                Result[i][j].resize(1600);
            }
        }
        //Мат.ожидание
        QVector<QVector<long double>> math_o(5);
        for (int i = 0; i < 5; i++){
            math_o[i].resize(1600);
        }
        for (int i = 0; i < 5; i++){
            for (int j = 0; j < 1600; j++){
                math_o[i][j] = 0;
            }
        }
        //Dispersion
        QVector<QVector<long double>> dispersion(5);
        for (int i = 0; i < 5; i++){
            dispersion[i].resize(1600);
        }
        for (int i = 0; i < 5; i++){
            for (int j = 0; j < 1600; j++){
                dispersion[i][j] = 0;
            }
        }
        //Correlation
        QVector<QVector<QVector<QVector<double>>>> correlation(1600);
        for (int i = 0; i < 1600; i++){
            correlation[i].resize(1600);
            for (int j = 0; j < 1600; j++){
                correlation[i][j].resize(5);
                for (int k = 0; k < 5; k++){
                    correlation[i][j][k].resize(5);
                }
            }
        }


        for (int i = 0; i < 1600; i++){
            for (int j = 0; j < 1600; j++){
                for (int k = 0; k < 5; k++){
                    for (int l = 0; l < 5; l++){
                        correlation[i][j][k][l] = 0;
                    }
                }
            }
        }

        //Оценивание статистических характеристик динамических систем методом Монте-Карло
        for (int i = 0; i < N; i++){
            TEnzymaticModel* model = new TEnzymaticModel();
            model->setT1( 160 );
            model->setSamplingFrequency( 0.1 );

            printf("Run = [%1i]",i);
            printf("\n");
            //Запуск модели
            model->Run(Integrator);
            //Присваиваем результат
            TDataStorage l_result = model->getResultStorage();
            for (int j = 0; j < 5; j++){
                for (int k = 0; k < 1600; k++){
                    Result[i][j][k] = l_result[k][j];
                }
            }
            delete model;
        }

            //Мат.ожидание
            for (int k = 0; k < 1600; k++){
                for (int j = 0; j < 5; j++){
                    for (int i = 0; i < N; i++){
                        math_o[j][k] += (long double)Result[i][j][k] / N;
                    }
                    printf("%6.3f ; ", (double)math_o[j][k]);
                }
                printf("\n");
            }

            //Дисперсия
            for (int k = 0; k < 1600; k++){
                for (int j = 0; j < 5; j++){
                    for (int i = 0; i < N; i++){
                         dispersion[j][k] += (((long double)Result[i][j][k] - math_o[j][k]) * ((long double)Result[i][j][k] - math_o[j][k]))/(N-1);
                    }
                  //  printf("%10.7f ; ", (double)dispersion[j][k]);
                }
              //  printf("\n");
            }


            //КОРРЕЛЯЦИОННЫЕ МОМЕНТЫ
            for (int i = 0; i < 1600; i++){              //t1
                for (int j = 0; j < 1600; j++){          //t2
                    for (int k = 0; k < 5; k++){          //row_vector
                        for (int l = 0; l < 5; l++){      //column
                            for (int r = 0; r < N; r++){
                                correlation[i][j][k][l] += ((((long double)Result[r][k][i] - math_o[k][i]) * ((long double)Result[r][l][j] - math_o[l][j])) / (N - 1));
                            }
                        }
                    }
                }
            }

            QFile data00("D:\corr00.txt");
            if (data00.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data00);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][0][0] << '\n';

                    }
                }
                data00.close();
            }
            QFile data11("D:\corr11.txt");
            if (data11.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data11);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][1][1] << '\n';

                    }
                }
                data11.close();
            }
            QFile data22("D:\corr22.txt");
            if (data22.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data22);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][2][2] << '\n';

                    }
                }
                data22.close();
            }
            QFile data33("D:\corr33.txt");
            if (data33.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data33);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][3][3] << '\n';

                    }
                }
                data33.close();
            }
            QFile data01("D:\corr01.txt");
            if (data01.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data01);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][0][1] << '\n';

                    }
                }
                data01.close();
            }
            QFile data02("D:\corr02.txt");
            if (data02.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data02);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][0][2] << '\n';

                    }
                }
                data02.close();
            }
            QFile data03("D:\corr03.txt");
            if (data03.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data03);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][0][3] << '\n';

                    }
                }
                data03.close();
            }
            QFile data12("D:\corr12.txt");
            if (data12.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data12);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][1][2] << '\n';

                    }
                }
                data12.close();
            }
            QFile data13("D:\corr13.txt");
            if (data13.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data13);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][1][3] << '\n';

                    }
                }
                data13.close();
            }
            QFile data23("D:\corr23.txt");
            if (data23.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&data23);
                for (int i = 0; i < 1600; i++){              //t1
                    for (int j = 0; j < 1600; j++){          //t2
                        out << i*0.1 << " " << j*0.1 << " "<< (double)correlation[i][j][2][3] << '\n';

                    }
                }
                data23.close();
            }

}

void MainWindow::showResult(TEnzymaticModel* model){
    TIntegrator* Integrator = new CDormandPrinceMethod();
    model->setT1( 160 );
    model->setSamplingFrequency( 0.01 );
    Integrator->setPrecision(1e-32);
    //Запуск модели
    model->Run(Integrator);
    //Присваиваем начальные условия
    QVector<long double> Res = model->getInitialConditions();
    //Присваиваем результат
    TDataStorage Result = model->getResultStorage();


    printf ("\nVector Res: \n");
    for (int z=0; z<model->getOrder(); z++)
    {
        printf("%f   ", (double)Res[z]);
    }
    printf("\n---------------------------------------------------------------------------");

    printf("\nCount string = %i", Result.getLength()-1);
    printf("\nCount column = %i", 4);

    printf("\n---------------------------------------------------------------------------");

//        printf("\nResult: \n");
//        for (int i=0; i<Result.getLength()-1; i++)
//        {
//            printf("%10.7f ; ", (double)Result.getTime(i));
//            for (int j=0; j<4; j++)
//            {

//                printf("%28.26f ; ", (double)Result[i][j]);
//            }
//            printf("\n");
//        }

//---------------------------------------------------------------------------

    QVector<double> t(Result.getLength()-1);

    for (int i=0; i<Result.getLength()-1; i++){ t[i]=Result.getTime(i); }

    QVector<double> Y0(Result.getLength()-1);
    QVector<double> Y1(Result.getLength()-1);
    QVector<double> Y2(Result.getLength()-1);
    QVector<double> Y3(Result.getLength()-1);
    QVector<double> Y4(Result.getLength()-1);

    for(int i=0; i<Result.getLength()-1; i++){
        Y0[i]=Result[i][0];
        Y1[i]=Result[i][1];
        Y2[i]=Result[i][2];
        Y3[i]=Result[i][3];
        Y4[i]=Result[i][4];
    }
    //Search max and min-------------------------------------------------
    long double mini = 100000, maxi = -100000;
        for(int i=0; i<Result.getLength()-1; i++){
            if (ui->checkBox->isChecked()){
                if (maxi<(long double)Result[i][0]){maxi = (long double)Result[i][0];}
                if (mini>(long double)Result[i][0]){mini = (long double)Result[i][0];}
            }
            if (ui->checkBox_2->isChecked()){
                if (maxi<(long double)Result[i][1]){maxi = (long double)Result[i][1];}
                if (mini>(long double)Result[i][1]){mini = (long double)Result[i][1];}
            }
            if (ui->checkBox_3->isChecked()){
                if (maxi<(long double)Result[i][2]){maxi = (long double)Result[i][2];}
                if (mini>(long double)Result[i][2]){mini = (long double)Result[i][2];}
            }
            if (ui->checkBox_4->isChecked()){
                if (maxi<(long double)Result[i][3]){maxi = (long double)Result[i][3];}
                if (mini>(long double)Result[i][3]){mini = (long double)Result[i][3];}
            }
            if (ui->checkBox_5->isChecked()){
                if (maxi<(long double)Result[i][4]){maxi = (long double)Result[i][4];}
                if (mini>(long double)Result[i][4]){mini = (long double)Result[i][4];}
            }
        }

//---------------------------------------------------------------------------

    int iter_check = 0;
    ui->Graph->clearGraphs();

    if(ui->checkBox->isChecked())
    {
        ui->Graph->addGraph();
        ui->Graph->graph(iter_check)->setData(t, Y0);
        ui->Graph->graph(iter_check)->setPen(QColor(0, 0, 255, 255));
        iter_check++;
    }
    if(ui->checkBox_2->isChecked())
    {
        ui->Graph->addGraph();
        ui->Graph->graph(iter_check)->setData(t, Y1);
        ui->Graph->graph(iter_check)->setPen(QColor(0, 255, 0, 255));
        iter_check++;
    }
    if(ui->checkBox_3->isChecked())
    {
        ui->Graph->addGraph();
        ui->Graph->graph(iter_check)->setData(t, Y2);
        ui->Graph->graph(iter_check)->setPen(QColor(255, 0, 0, 255));
        iter_check++;
    }
    if(ui->checkBox_4->isChecked())
    {
        ui->Graph->addGraph();
        ui->Graph->graph(iter_check)->setData(t, Y3);
        ui->Graph->graph(iter_check)->setPen(QColor(0, 0, 0, 255));
        iter_check++;
    }
    if(ui->checkBox_5->isChecked())
    {
        ui->Graph->addGraph();
        ui->Graph->graph(iter_check)->setData(t, Y4);
        ui->Graph->graph(iter_check)->setPen(QColor(255, 0, 255, 255));
        iter_check++;
    }
    iter_check = 0;

    ui->Graph->xAxis->setLabel("t");
    ui->Graph->yAxis->setLabel("Y");

    ui->Graph->xAxis->setRange(0., 160);
    ui->Graph->yAxis->setRange(mini, maxi);

    ui->Graph->replot();


    delete Integrator;

}



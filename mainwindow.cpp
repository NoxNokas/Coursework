#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void showResult(TEnzymaticModel* model);

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    TEnzymaticModel* Model1 = new TEnzymaticModel();
    showResult(Model1);
    delete Model1;

}

void MainWindow::showResult(TEnzymaticModel* model){
    TIntegrator* Integrator = new CDormandPrinceMethod();
        model->setT1( 160 );
        model->setSamplingFrequency( 0.001 );
        Integrator->setPrecision(1e-16);
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
//        for (int i=0; i<Result.getLength(); i++)
//        {
//            printf("%10.7f ; ", (double)Result.getTime(i));
//            for (int j=0; j<4; j++)
//            {

//                printf("%28.26f ; ", (double)Result[i][j]);
//            }
//            printf("\n");
//        }


        //Math FF
        long double FF_M = 0;
        for (int i=0; i<Result.getLength()-1; i++){
            FF_M += (long double)Result[i][4];
        }
        FF_M /= (Result.getLength()-1);
        printf("\n");
        printf("FF_M = %15.10f ; ", (double)FF_M);
        printf("\n");

        //Dispersion FF
        long double FF_D = 0;
        for (int i=0; i<Result.getLength()-1; i++){
            FF_D += ((long double)Result[i][4] - FF_M) * ((long double)Result[i][4] - FF_M);
        }
        FF_D /= (Result.getLength()-2);
        printf("FF_D = %15.10f ; ", (double)FF_D);

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



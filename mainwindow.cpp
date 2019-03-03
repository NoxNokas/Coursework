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
        model->setSamplingFrequency( 0.01 );
        Integrator->setPrecision(1e-16);
        //Запуск модели
        model->Run(Integrator);
        //Присваиваем начальные условия
        QVector<long double> Res = model->getInitialConditions();
        //Присваиваем результат
        TDataStorage Result = model->getResultStorage();
 //   qDebug() << (double)Res[z];

        printf ("\nVector Res: \n");
        for (int z=0; z<model->getOrder(); z++)
        {
            printf("%f   ", (double)Res[z]);
        }
        printf("\n---------------------------------------------------------------------------");

        printf("\nCount string Matrix_x = %i", Result.getLength());
        printf("\nCount column Matrix_x = %i", 4);

        printf("\n---------------------------------------------------------------------------");

        printf("\nResult: \n");
        for (int i=0; i<Result.getUsedCapacity(); i++)
        {
            printf("%10.7f ; ", (double)Result.getTime(i));
            for (int j=0; j<4; j++)
            {

                printf("%28.26f ; ", (double)Result[i][j]);
            }
            printf("\n");
        }

//---------------------------------------------------------------------------

        QVector<double> t(Result.getUsedCapacity());

        for (int i=0; i<Result.getUsedCapacity(); i++){ t[i]=Result.getTime(i); }

        QVector<double> Y0(Result.getUsedCapacity());
        QVector<double> Y1(Result.getUsedCapacity());
        QVector<double> Y2(Result.getUsedCapacity());
        QVector<double> Y3(Result.getUsedCapacity());

        for(int i=0; i<Result.getUsedCapacity(); i++){
            Y0[i]=Result[i][0];
            Y1[i]=Result[i][1];
            Y2[i]=Result[i][2];
            Y3[i]=Result[i][3];
        }
        //Search max and min
        double mini = 100000, maxi = -100000;
        for (int j=0; j<4; j++){
            for(int i=0; i<Result.getUsedCapacity(); i++){
                if (maxi<(double)Result[i][j]){maxi = (double)Result[i][j];}
                if (mini>(double)Result[i][j]){mini = (double)Result[i][j];}
            }
        }



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
        iter_check = 0;

        ui->Graph->xAxis->setLabel("t");
        ui->Graph->yAxis->setLabel("Y");

        ui->Graph->xAxis->setRange(0., 160);
        ui->Graph->yAxis->setRange(mini, maxi);

        ui->Graph->replot();


        delete Integrator;
}



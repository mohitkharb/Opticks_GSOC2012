#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui>
#include <QProcess>
#include <QDir>
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <string>
#include<sstream>
#include <QDebug>
#include<QList>
MainWindow::MainWindow(int rows, int cols, std::vector<double> dataR, std::vector<double> dataG, std::vector<double> dataB, int bandnum, QWidget *parent) :
	Rows(rows), Cols(cols), dataR(dataR), dataG(dataG), dataB(dataB),num_band(bandnum),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Show_process(std::string mod)
{

    FILE *procOut = _popen(mod.c_str(), "r");

    if(procOut == NULL)
        exit(1);

    char output[200];
    while(fgets(output, sizeof(output), procOut) != NULL) {
        ui->textBrowser->append(output);
    }
    fclose(procOut);

}


void MainWindow::on_msdir_clicked()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    ms_dir = QFileDialog::getExistingDirectory(this,
                                    tr("QFileDialog::getExistingDirectory()"),
                                    ui->dirLabel->text(),
                                    options);
        if (!ms_dir.isEmpty())
            ui->dirLabel->setText(ms_dir);
}






void MainWindow::on_pushButton_ms_clicked()
{
    ui->textBrowser->setText("calculating mean shift.");
        if(!ms_dir.isEmpty())
        {


            std::cout << Rows << " -- " << Cols << std::endl;
            FILE *fp21 = fopen("C:\\Windows\\Temp\\testing.ppm", "wb"); /* b - binary mode */
            fprintf(fp21, "P6\n%d %d\n255\n", Cols, Rows);
            
            
            for(int i = 0;i < Rows; i++){
                for(int k = 0;k < Cols; k++){

                    static unsigned char color[3];
                    color[0]	= dataB[i*Cols + k ];
                    color[1]    = dataG[i*Cols + k ];
                    color[2]	= dataR[i*Cols + k ];
                  //  std::cout << int (color[0]) << " " << int (color[1]) << " " << int (color[2]) << std::endl;
                    fwrite(color, 1, 3, fp21);

                }
            }


            fclose(fp21);
            




        //QString img = ui->lineEdit->text();
        QString sbw = ui->lineEdit_s1->text();
        QString rbw = ui->lineEdit_r1->text();
        QString mra = ui->lineEdit_m1->text();
       

        //std::cout << img_fullname.to

        img_name = "OpticksImage";


        



        QString file_exp = ms_dir;
        file_exp.append("\\exp");
        
		QFile expFile(file_exp.toUtf8().data());
        expFile.open(QIODevice::ReadWrite);

        QString file_try=ms_dir;
        file_try.append("\\try");

        QFile tryFile(file_try.toUtf8().data());
        tryFile.open(QIODevice::WriteOnly);

        QTextStream in(&expFile);
        QTextStream out(&tryFile);
        int i=1;
        QString line = in.readLine();
        while(!in.atEnd()){
            if(i==6){
                out<<"SpatialBandwidth = "<< sbw <<";\n";
            }
            else if(i==7){
                out<<"RangeBandwidth = "<< rbw <<";\n";
            }
            else if(i==8){
                out<<"MinimumRegionArea ="<< mra <<";\n";
            }
            else if(i==15){
        
                out<<"Load('C:\\Windows\\Temp\\testing.ppm',IMAGE);\n";
            }
            else if(i==23){
                out<<"Save('C:\\Windows\\Temp\\seg_"<< img_name<<".ppm', PPM, SEGM_IMAGE);\n";

            }
            else{
                out<<line<<"\n";
            }
            line =in.readLine();
            i++;
        }
        tryFile.close();
        expFile.close();

        QString mod1 = ms_dir;
        
        mod1.append("\\meanshift.exe ");

        mod1.append(ms_dir);
        mod1.append("\\try");
        model=mod1.toUtf8().data();

        Show_process(model);

        }
        else ui->textBrowser->setText("Select directory and file names.");
}

void MainWindow::on_pushButton_cd_clicked()
{


       if(!ms_dir.isEmpty())
       {
            data1 = readPPMImage(imgsize,"C:/Windows/Temp/testing.ppm",num_band);
    std::cout<<"here1"<<std::endl;
               i1 = Image(Rows, Cols, 3, data1);
        
			   labels1 = prepareNewLabels("C:/Windows/Temp/map_label_pruning_spurious_regions.txt",Rows,Cols);
   std::cout<<"here2"<<std::endl;

               merge_map = (int **)malloc(sizeof(int *)*Rows);
               for(int i=0; i < Rows; i++)
                       merge_map[i] = (int *)malloc(sizeof(int)*Cols);

               for(int i=0; i<Rows; i++)
                       for(int j=0; j<Cols; j++)
                               merge_map[i][j] = -1;


               
               object_info1 = prepareObjectInfo(Rows,Cols,labels1,labels1,merge_map,NO1);//using labels two times
               std::cout<<"here3"<<std::endl;
               i1.createSegmentedImageInfo(merge_map);
               std::cout<<"here4"<<std::endl;
               i1.createObjects(NO1,object_info1); // this function in turn calls the createAttribute function also
               std::cout<<"here5"<<std::endl;
               ui->textBrowser->setText("Feature Extraction Done.");




         }
       else ui->textBrowser->setText("Select correct Mean Shift directory or Image name missing.");
}
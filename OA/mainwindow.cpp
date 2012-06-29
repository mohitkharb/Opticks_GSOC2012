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
    QDialog(parent),
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


void MainWindow::on_pushButton_2_clicked()
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
        QString sbw = ui->lineEdit->text();
        QString rbw = ui->lineEdit_2->text();
        QString mra = ui->lineEdit_3->text();
       

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

void MainWindow::on_pushButton_3_clicked()
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

void MainWindow::on_pushButton_clicked()
{
	 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    ms_dir = QFileDialog::getExistingDirectory(this,
                                    tr("QFileDialog::getExistingDirectory()"),
                                    ui->label_2->text(),
                                    options);
        if (!ms_dir.isEmpty())
            ui->label_2->setText(ms_dir);
}
void MainWindow::on_pushButton_4_clicked()
{
	int i= atoi(ui->lineEdit_4->text().toUtf8().data());
	std::string fname ="C:/Windows/Temp/testing.ppm";
	pbox = new paint_box(fname,Rows,Cols,i,labels1,this);
	pbox->show();
}
void MainWindow::on_pushButton_5_clicked()
{
	classificationImage();
	AttributeBased *a= new AttributeBased(Rows,Cols,labels1,data1,i1,num_band,1);
	a->show();
}
void MainWindow:: insertvalue(int n, int x, int y,int **labels)
{

    int val = labels[y][x];
 


    std::vector<int> temp;
    temp = class_m[n];
    
    if(temp.size()!=0)
    {
        temp.push_back(val);
        class_m[n] = temp;
        
 
    }
    else
    {
        std::vector<int> newtemp;
        newtemp.push_back(val);
        class_m[n]= newtemp;
        

    }



}
void MainWindow::GetMinor(float **src, float **&dest, int row, int col, int order)
{
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;

    for(int i = 0; i < order; i++ )
    {
        if( i != row )
        {
            colCount = 0;
            for(int j = 0; j < order; j++ )
            {
                // when j is not the element
                if( j != col )
                {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }

}



float MainWindow::CalcDeterminant( float **mat, int order)
{
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if( order == 1 )
        return mat[0][0];

    // the determinant value
    float det = 0;

    // allocate the cofactor matrix
    float **minor = (float **)malloc(sizeof(float *)*(order-1));
    for(int i=0;i<order-1;i++)
        minor[i] = (float *)malloc(sizeof(float)*(order-1));

    for(int i = 0; i < order; i++ )
    {
        // get minor of element (0,i)
        GetMinor( mat, minor, 0, i , order);
        // the recusion is here!

        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
    }

    return det;
}


void MainWindow::MatrixInversion(float **A, int order, float **&Y)
{
    // get the determinant of a
    float det = 1.0/CalcDeterminant(A,order);

    // memory allocation
    float *temp = (float *)malloc(sizeof(float)*(order-1)*(order-1));
    float **minor = (float **)malloc(sizeof(float *)*(order-1));
    for(int i=0;i<order-1;i++)
        minor[i] = (float *)malloc(sizeof(float)*(order-1));

    for(int j=0;j<order;j++)
    {
        for(int i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,order);
            Y[i][j] = det*CalcDeterminant(minor,order-1);
            if( (i+j)%2 == 1)
                Y[i][j] = -Y[i][j];
        }
    }

}

float **MainWindow::getCovarianceMartix(int numberOfSamples, float **dataArray, float **meanVector, int classNumber, int ndims)
{
    int i, j, k;

    //malloc space for cov-matrix here
    float **cov = (float **)malloc(sizeof(float *)*ndims);
    for(i=0; i<ndims; i++)
        cov[i] = (float *)malloc(sizeof(float)*ndims);

    for(i=0; i<ndims; i++)
        for(j=0; j<ndims; j++)
            cov[i][j] = 0.0;

    for(int i=0; i<numberOfSamples; i++){
        for(int j=0; j<ndims; j++)
            std::cout << dataArray[i][j] << " ";
        std::cout << "\n";
    }

    for(i=0; i<numberOfSamples; i++)
        for(j=0; j<ndims; j++)
            dataArray[i][j] -= meanVector[classNumber][j];

    std::cout << "mean vector" << std::endl;
    for(int i=0; i<ndims; i++)
        std::cout << meanVector[classNumber][i] << " ";
    std::cout << "\n";

    std::cout << "data matrix"<< std::endl;
    for(int i=0; i<numberOfSamples; i++){
        for(int j=0; j<ndims; j++)
            std::cout << dataArray[i][j] << " ";
        std::cout << "\n";
    }


    for (i=0; i< ndims; i++) {
        for (j = i; j < ndims; j++) {
            for (k=0; k < numberOfSamples; k++) {
                float val = dataArray[k][i]*dataArray[k][j];
                cov[i][j] += val;
            }
            cov[i][j] = cov[i][j]/(1.0*numberOfSamples);
            cov[j][i] = cov[i][j];
        }
    }
    std::cout << "cov mat in function" << std::endl;
    for(int i=0; i<ndims; i++){
        for(int j=0; j<ndims; j++)
            std::cout << cov[i][j] << " ";
        std::cout << "\n";
    }

    return(cov);
}

float MainWindow::calculateMahaDist(float *sample, float **class_mean, float **full_cov_mat, int class_number, int ndims)
{
    float term = 0.0;

    float *mat1 = (float *)malloc(sizeof(float)*ndims);
    for(int i=0; i<ndims; i++)
        mat1[i] = 0.0;

    for(int i=0; i<ndims; i++)
        for(int j=0; j<ndims; j++)
            mat1[i] += (sample[j] - class_mean[class_number][j]) * (full_cov_mat[class_number*ndims + j][i]);

    for(int i=0; i<ndims; i++)
        term += mat1[i] * (sample[i] - class_mean[class_number][i]);


    return(term);

}

void MainWindow::classificationImage(){
    int current_class=0, i = 0, k, ndims = 7; //ndims is the number of dimensions of the data samples
    int class_num = int (class_m.size());

    float value_r, value_g, value_b, texture_coarseness, texture_direction, texture_contrast, texture_roughness;
    float **class_mean;

    class_mean = (float **)malloc(sizeof(float *)*class_num);
    for(int i=0; i<class_num; i++)
        class_mean[i] = (float *)malloc(sizeof(float)*ndims);
    for(int i=0; i<class_num; i++)
        for(int j=0; j<ndims; j++)
            class_mean[i][j] = 0.0;

    float **full_cov_mat = (float **)malloc(sizeof(float *)*(class_num*ndims));
    for(int i=0; i<class_num*ndims; i++)
        full_cov_mat[i] = (float *)malloc(sizeof(float)*ndims);

    float **class_cov_mat;
    float **inverted_class_cov_mat = (float **)malloc(sizeof(float *)*ndims);
    for(int i=0; i<ndims; i++)
        inverted_class_cov_mat[i] = (float *)malloc(sizeof(float)*ndims);


    std::cout << "before the loop: calculating class means" << std::endl;
    while(current_class < class_num)
    {
        
        i = 0 ;
        std::vector <int> tempvect = class_m[current_class];

        //create data matrix here
        float **data_samples = (float **)malloc(sizeof(float *)*(tempvect.size()));
        for(int p=0; p< (int) tempvect.size(); p++)
            data_samples[p] = (float *)malloc(sizeof(float)*ndims);

        for(int m=0; m<ndims; m++)
            for(int n=0; n<ndims; n++ )
                inverted_class_cov_mat[m][n] = 0.0;
        
        while(i < (int) tempvect.size() )
        {
            k = tempvect[i];

            value_r = i1.objectArray[k].fVector.meanRed;
            value_g = i1.objectArray[k].fVector.meanGreen;
            value_b = i1.objectArray[k].fVector.meanBlue;
            texture_coarseness = i1.objectArray[k].fVector.coarseness;
            texture_direction = i1.objectArray[k].fVector.direction;
            texture_contrast = i1.objectArray[k].fVector.contrast;
            texture_roughness = i1.objectArray[k].fVector.roughness;

            data_samples[i][0] = value_r;
            data_samples[i][1] = value_g;
            data_samples[i][2] = value_b;
            data_samples[i][3] = texture_coarseness;
            data_samples[i][4] = texture_direction;
            data_samples[i][5] = texture_contrast;
            data_samples[i][6] = texture_roughness;

            class_mean[current_class][0] += value_r;
            class_mean[current_class][1] += value_g;
            class_mean[current_class][2] += value_b;
            class_mean[current_class][3] += texture_coarseness;
            class_mean[current_class][4] += texture_direction;
            class_mean[current_class][5] += texture_contrast;
            class_mean[current_class][6] += texture_roughness;

            i++;
        }
        class_mean[current_class][0] /= tempvect.size();
        class_mean[current_class][1] /= tempvect.size();
        class_mean[current_class][2] /= tempvect.size();
        class_mean[current_class][3] /= tempvect.size();
        class_mean[current_class][4] /= tempvect.size();
        class_mean[current_class][5] /= tempvect.size();
        class_mean[current_class][6] /= tempvect.size();

        /*std::cout << class_mean[current_class][0] << " " <<
                     class_mean[current_class][1] << " " <<
                     class_mean[current_class][2] << " " <<
                     class_mean[current_class][3] << " " <<
                     class_mean[current_class][4] << " " <<
                     class_mean[current_class][5] << " " <<
                     class_mean[current_class][6] << std::endl;
					 */
        
        // call covariance matrix function here
        class_cov_mat = getCovarianceMartix(tempvect.size(), data_samples, class_mean, current_class, ndims);
        

        // call Matrix inversion function here
        
        MatrixInversion(class_cov_mat, ndims, inverted_class_cov_mat);

        for(int m=0; m<ndims; m++)
            for(int n=0; n<ndims; n++)
                full_cov_mat[current_class*ndims + m][n] = inverted_class_cov_mat[m][n];


        current_class++;
    }
    
    for(int i=0; i<class_num*ndims; i++){
        for(int j=0; j<ndims; j++)
            std::cout << full_cov_mat[i][j] << " ";
    
    }


    float min_dev=10000000;
    int j = 0;
    float *sample= (float *)malloc(sizeof(float)*ndims);
    float mDist=0.0;

    //  FILE *new_class;
    // new_class = fopen("C:\\Windows\\Temp\\new_class_1.txt","w");

    while( j < NO1)
    {
        value_r = i1.objectArray[j].fVector.meanRed;
        value_g = i1.objectArray[j].fVector.meanGreen;
        value_b = i1.objectArray[j].fVector.meanBlue;
        texture_coarseness = i1.objectArray[j].fVector.coarseness;
        texture_direction = i1.objectArray[j].fVector.direction;
        texture_contrast = i1.objectArray[j].fVector.contrast;
        texture_roughness = i1.objectArray[j].fVector.roughness;

        sample[0] = value_r;
        sample[1] = value_g;
        sample[2] = value_b;
        sample[3] = texture_coarseness;
        sample[4] = texture_direction;
        sample[5] = texture_contrast;
        sample[6] = texture_roughness;

        min_dev = 10000000.0;
        mDist = 0.0;
        int class_flag=0;
        for(i=0; i<class_num; i++)
        {
            //call Mahalanobis Distance function for each object here
            mDist = calculateMahaDist(sample,class_mean,full_cov_mat,i,ndims);

            if(mDist < min_dev)
            {
                //my_new_class = i;
                i1.objectArray[j].fVector.classNumber = i+1; // i+1 : so that the class number starts from 1 instead of 0
                class_flag = 1;
                min_dev = mDist;
            }
        }
        if(class_flag == 0){
            //std::cout << sample[0] << " " <<  sample[1] << " " <<  sample[2] << " " <<  sample[3] << " " <<  sample[4] << " " <<  sample[5] << " " <<  sample[6] << " " << std::endl;
            for(int cs=0; cs<class_num; cs++){
                float dis = calculateMahaDist(sample,class_mean,full_cov_mat,cs,ndims);
    
            }
    
        }

        //    fprintf(new_class,"%d\n",i1.objectArray[j].fVector.classNumber);
        j++;

    }
    class_m.clear();
    //  fclose(new_class);

}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QWidget>
#include "codes.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    QString fileName1;
    std::string model;
    std::string tmp;
    QString fileName;
    QString img_name;
    QString segName;
    QString ms_dir;

    int imgsize[2];// 0: rows, 1: cols
    int NO_I;
    float LOW,HIGH;//
    int **merge_map,*classes,**final_codes;
    //individual variables
    int ***data1;//data1 :img1.ppm,
    int NO1;
    int **labels1;
	std::vector<double> dataR;
	std::vector<double> dataG;
	std::vector<double> dataB;
	int Rows;
	int Cols;
    std::vector< std::vector< int > >object_info1;

    Image i1;

    int num_band;
    explicit MainWindow(int rows, int cols, std::vector<double> dataR, std::vector<double> dataG, std::vector<double> dataB,int bandnum, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    
    void on_msdir_clicked();

    void on_pushButton_ms_clicked();

    void on_pushButton_cd_clicked();


  
private:
    Ui::MainWindow *ui;
    void Show_process(std::string model);
};

#endif // MAINWINDOW_H

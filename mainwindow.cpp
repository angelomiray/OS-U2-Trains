#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Cria o trem com seu (ID, posição X, posição Y)
    trem1 = new Trem(1, 60, 94);
    trem2 = new Trem(2, 465, 30);
    trem3 = new Trem(3, 870, 94);
    trem4 = new Trem(4, 195, 226);
    trem5 = new Trem(5, 600, 287);

    /*
     * Conecta o sinal UPDATEGUI à função UPDATEINTERFACE.
     * Ou seja, sempre que o sinal UPDATEGUI foi chamado, será executada a função UPDATEINTERFACE.
     * Os 3 parâmetros INT do sinal serão utilizados na função.
     * Trem1 e Trem2 são os objetos que podem chamar o sinal. Se um outro objeto chamar o
     * sinal UPDATEGUI, não haverá execução da função UPDATEINTERFACE
     */
    connect(trem1,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem2,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem3,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem4,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));
    connect(trem5,SIGNAL(updateGUI(int,int,int)),SLOT(updateInterface(int,int,int)));



}

//Função que será executada quando o sinal UPDATEGUI for emitido
void MainWindow::updateInterface(int id, int x, int y){
    switch(id){
        case 1: //Atualiza a posição do objeto da tela (quadrado) que representa o trem1
            ui->label_trem1->setGeometry(x,y,21,17);
            trem1->setVelocidade(ui->horizontalSlider_T1->value());
            break;
        case 2: //Atualiza a posição do objeto da tela (quadrado) que representa o trem2
            ui->label_trem2->setGeometry(x,y,21,17);
            trem2->setVelocidade(ui->horizontalSlider_T2->value());
            break;
        case 3: //Atualiza a posição do objeto da tela (quadrado) que representa o trem3
            ui->label_trem3->setGeometry(x,y,21,17);
            trem3->setVelocidade(ui->horizontalSlider_T3->value());
            break;
        case 4: //Atualiza a posição do objeto da tela (quadrado) que representa o trem4
            ui->label_trem4->setGeometry(x,y,21,17);
            trem4->setVelocidade(ui->horizontalSlider_T4->value());
            break;
        case 5: //Atualiza a posição do objeto da tela (quadrado) que representa o trem5
            ui->label_trem5->setGeometry(x,y,21,17);
            trem5->setVelocidade(ui->horizontalSlider_T5->value());
            break;
        default:
            break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * Ao clicar, trens começam execução
 */
void MainWindow::on_pushButton_clicked()
{
    trem1->start();
    trem2->start();
    trem3->start();
    trem4->start();
    trem5->start();
}

/*
 * Ao clicar, trens param execução
 */
void MainWindow::on_pushButton_2_clicked()
{
    trem1->terminate();
    trem2->terminate();
    trem3->terminate();
    trem4->terminate();
    trem5->terminate();
}

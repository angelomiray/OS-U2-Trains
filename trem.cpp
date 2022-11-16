#include "trem.h"
#include <QtCore>

//Construtor
Trem::Trem(int ID, int x, int y){
    this->ID = ID;
    this->x = x;
    this->y = y;
    velocidade = 100;
}

void Trem::setVelocidade(int value) { //o programa utiliza o sleep para simular a velocidade. Logo, quanto maior o valor do slider
    velocidade = 200 - value;                // mais lento ficará o trem. logo, modificamos o cálculo para que faça mais sentido aumentar
}                                              //o slider e aumentar a velocidade

//Função a ser executada após executar trem->START
void Trem::run(){
    while(true){
        switch(ID){
            case 1:     //Trem 1
                if (x == 60 && y > 30) {
                    y == 34 ? y -=4 : y -= 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos
                }
                else if (y == 30 && x < 330) {
                    x += 10;
                }
                else if (x == 330 && y < 150)
                    y += 10;
                else
                    x -= 10;

                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 2: //Trem 2
                if (y == 30 && x < 600)
                    x == 595 ? x += 5 : x += 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos
                else if (x == 600 && y < 150)
                    y += 10;
                else if (y == 150 && x > 330)
                    x -= 10;
                else
                    y -= 10;
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 3: //Trem 3
                if (x == 870 && y < 150)
                    y == 144 ? y += 6 : y += 10;
                else if (y == 150 && x > 600)
                    x -= 10;
                else if (x == 600 && y > 30)
                    y -= 10;
                else
                    x += 10;
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 4: //Trem 4
                if (x == 195 && y > 150) {
                    if (y == 156) {
                        y -= 6;
                    } else if (y == 157) {
                        y -= 7;
                    } else {
                        y -= 10;
                    }
                }
                else if (y == 150 && x < 465)
                    x += 10;
                else if (x == 465 && y < 287)
                    y == 280 ? y += 7 : y += 10;
                else
                    x -= 10;
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 5: //Trem 5
                if (y == 287 && x > 465) {
                    if (x == 470) x -= 5;
                    else x -= 10;
                }
                else if (x == 465 && y > 150)
                    if (y == 157) y -= 7;
                    else y -= 10;
                else if (y == 150 && x < 735)
                    x += 10;
                else
                    if (y == 280) y += 7;
                    else y+= 10;
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            default:
                break;
        }
        msleep(velocidade);
    }
}





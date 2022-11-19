#include "trem.h"
#include <QtCore>

std::condition_variable criticalSection;

std::mutex mutexCS00;
bool readyT1CS00 = true;
bool readyT2CS00 = true;

std::mutex mutexCS02;
bool readyT1CS02 = true;
bool readyT4CS02 = true;

std::mutex mutexCS03;
bool readyT2CS03 = true;
bool readyT4CS03 = true;

//Construtor
Trem::Trem(int ID, int x, int y){
    this->ID = ID;
    this->x = x;
    this->y = y;
    velocidade = 100;
}

//o programa utiliza o sleep para simular a velocidade
// quanto maior o valor do slider mais lento ficará o trem
// modificamos o cálculo para que faça mais sentido aumentar o slider e aumentar a velocidade

void Trem::setVelocidade(int value) {
   velocidade = 200 - value;
}

//Função a ser executada após executar trem->START
void Trem::run(){
    while (true) {
        switch (ID) {
            case 1:     //Trem 1
                if (x == 60 && y > 30) {
                    y == 34 ? y -=4 : y -= 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos
                } else if (y == 30 && x < 330) {

                    if (x == 300) { // ESPERANDO T2 SAIR DA CS 00

                        std::unique_lock<std::mutex> ul(mutexCS00);

                        criticalSection.wait(ul, [] { return readyT2CS00; });

                        ul.unlock();
                    }

                    x += 10;

                } else if (x == 330 && y < 150) {                    

                    if (y < 130) { //CRITICAL SECTION 00 (T1 VS T2)
                        mutexCS00.lock();
                            readyT1CS00 = false;
                            y += 10;
                            criticalSection.notify_all();
                        mutexCS00.unlock();
                    } else {
                        y += 10;
                    }

                    if (y == 130) { //waiting if T4 are in the CS02
                        std::unique_lock<std::mutex> ul(mutexCS02);

                        criticalSection.wait(ul, [] { return readyT4CS02; }); // until readyT4CS02 is true

                        ul.unlock();
                    }

                } else {
                    if (x == 300) {
                        readyT1CS00 = true; //terminou de utilizar a seção crítica 00
                        criticalSection.notify_all();
                    }

                    if (x >= 165) { // CRITICAL SECTION 02 (T1 vs T4)
                        mutexCS02.lock();
                            readyT1CS02 = false;
                            x -= 10;
                            criticalSection.notify_all();
                        mutexCS02.unlock();
                    } else { // out of critical section
                        x -= 10;
                        readyT1CS02 = true;
                        criticalSection.notify_all();
                    }
                }

                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 2: //Trem 2
                if (y == 30 && x < 600) {

                    x == 595 ? x += 5 : x += 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos

                    if (x == 370 || x == 375) { //T2 FORA DA CS 00
                        readyT2CS00 = true;
                        criticalSection.notify_all();
                    }

                }
                else if (x == 600 && y < 150) {
                    y += 10;
                } else if (y == 150 && x > 330) {

                    if (x == 490) { // ESPERA T4 SAIR DE CS03
                        std::unique_lock<std::mutex> ul(mutexCS03);

                        criticalSection.wait(ul, [] { return readyT4CS03; });

                        ul.unlock();
                    }

                    if (x > 360 && x <= 470) { //CRITICAL SECTION 03 ( T2 VS T4 )
                        mutexCS03.lock();
                            readyT2CS03 = false;
                            x -= 10;
                            criticalSection.notify_all();
                        mutexCS00.unlock();
                    }

                    if (x == 360) { //ESPERA T1 SAIR DA CS00
                        std::unique_lock<std::mutex> ul(mutexCS00);

                        criticalSection.wait(ul, [] { return readyT1CS00; });

                        ul.unlock();
                    } else {
                        x -= 10;
                    }

                } else { // CRITICAL SECTION 00 ( T1 VS T2 )
                    mutexCS00.lock();
                        readyT2CS00 = false;
                        y -= 10;
                        criticalSection.notify_all();
                    mutexCS00.unlock();
                }
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

                    if (y == 176 || y == 177 || y == 170) {
                        std::unique_lock<std::mutex> ul(mutexCS02);
                        criticalSection.wait(ul, [] { return readyT1CS02; });
                        ul.unlock();
                    }

                    if (y == 156) {
                        y -= 6;
                    } else if (y == 157) {
                        y -= 7;
                    } else {
                        y -= 10;
                    }
                }
                else if (y == 150 && x < 465) {                    

                    if (x <= 351) { //CRITICAL SECTION 02 (T1 vs T4)
                        mutexCS02.lock();
                            readyT4CS02 = false;
                            criticalSection.notify_all();
                        mutexCS02.unlock();
                    } else {
                        readyT4CS02 = true; //terminou de utilizar
                        criticalSection.notify_all();
                    }

                    if (x == 300) {
                        std::unique_lock<std::mutex> ul(mutexCS03);

                        criticalSection.wait(ul, [] { return readyT2CS03; });

                        ul.unlock();
                    }

                    if (x > 360 && x <= 470) { //CRITICAL SECTION 03 (T2 VS T4)
                       mutexCS03.lock();
                           readyT4CS03 = false;
                           criticalSection.notify_all();
                       mutexCS03.unlock();
                   } else {
                        readyT4CS03 = true; //terminou de utilizar
                        criticalSection.notify_all();
                    }

                   x += 10;


                } else if (x == 465 && y < 287) {
                    y == 280 ? y += 7 : y += 10;                
                } else {
                    x -= 10;
                }
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





#include "trem.h"
#include <QtCore>
#include <iostream>

std::condition_variable criticalSection;

std::mutex mutexCS00;
bool freeCS00 = true;
bool readyT1CS00 = true;
bool readyT2CS00 = true;

std::mutex mutexCS01;
bool freeCS01 = true;
bool readyT2CS01 = true;
bool readyT3CS01 = true;

std::mutex mutexCS02;
bool freeCS02 = true;
bool readyT1CS02 = true;
bool readyT4CS02 = true;

std::mutex mutexCS03;
bool freeCS03 = true;
bool readyT2CS03 = true;
bool readyT4CS03 = true;

std::mutex mutexCS04;
bool freeCS04 = true;
bool readyT2CS04 = true;
bool readyT5CS04 = true;

std::mutex mutexCS05;
bool freeCS05 = true;
bool readyT3CS05 = true;
bool readyT5CS05 = true;

std::mutex mutexCS06;
bool freeCS06 = true;
bool readyT4CS06 = true;
bool readyT5CS06 = true;

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
   if (value == 0) velocidade = -1; // 200 - 0 == 200, entao ele nao ficaria parado
   else velocidade = 200 - value;
}

//Função a ser executada após executar trem->START
void Trem::run(){
    while (true) {

        switch (ID) {

            case 1:     //Trem 1
                if (velocidade != -1) {
                    if (x == 60 && y > 30) {
                        y == 34 ? y -= 4 : y -= 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos
                    } else if (y == 30 && x < 330) {                        

                        if (x == 300) { // ESPERANDO T2 SAIR DA CS 00

                            std::unique_lock<std::mutex> ul(mutexCS00);

                            criticalSection.wait(ul, [] { return (freeCS00) &&
                                                        (freeCS02 || freeCS03) &&
                                                        (freeCS02 || readyT2CS04) &&
                                                        (freeCS02 || freeCS04 || freeCS06) &&
                                                        (freeCS02 || freeCS06 || freeCS01 || freeCS05); }); //** readyT2CS04

                            ul.unlock();
                        }

                        if (x >= 300 && x <= 330) { //INÍCIO CS00
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT1CS00 = false;                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        }

                        x += 10;

                    } else if (x == 330 && y < 150) {

                        if (y <= 150) { //CRITICAL SECTION 00 (T1 VS T2)
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT1CS00 = false;                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 130) { //T4 na CS02?
                            std::unique_lock<std::mutex> ul(mutexCS02);

                            criticalSection.wait(ul, [] { return (freeCS02); }); // until readyT4CS02 is true

                            ul.unlock();
                        }

                        if (y >= 130 && y <= 150) { // INÍCIO CS02
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT1CS02 = false;                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();
                        }

                        y += 10;

                    } else {                                               
                        if (x == 300) {
                            mutexCS00.lock();
                                freeCS00 = true;
                                readyT1CS00 = true; //terminou de utilizar a seção crítica 00                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        } else if (x > 300) {
                            mutexCS00.lock();
                                readyT1CS00 = false;
                                freeCS00 = false;                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        }                        

                        if (x == 170) { // FIM CRITICAL SECTION 02 (T1 vs T4)

                            mutexCS02.lock();
                                freeCS02 = true;
                                readyT1CS02 = true;                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();


                        } else if (x > 170) {
                            mutexCS02.lock();
                                readyT1CS02 = false;
                                freeCS02 = false;                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();
                        }
                        x -= 10;
                    }
                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 2: //Trem 2
                if (velocidade != -1) {
                    if (y == 30 && x < 600) {
                        if (x == 350 || x == 355) { //T2 FORA DA CS 00
                            mutexCS00.lock();
                                freeCS00 = true;
                                readyT2CS00 = true;                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        } else if (x >= 330 && x < 355) {
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT2CS00 = false;
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 560 || x == 565) { //ESPERA T3 SAIR DO CS01
                            std::unique_lock<std::mutex> ul(mutexCS01);

                            criticalSection.wait(ul, [] { return (freeCS01) &&
                                                         (freeCS04 || freeCS05) &&
                                                         (freeCS00 || freeCS02 || freeCS06) &&
                                                         (freeCS03 || freeCS06 || freeCS05) &&
                                                         (freeCS00 || freeCS02 || freeCS06 || freeCS05); });

                            ul.unlock();
                        }

                        if (x >= 560 && x < 600) { //ENTROU CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT2CS01 = false;
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }

                        x == 595 ? x += 5 : x += 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos



                    } else if (x == 600 && y < 150) {

                        if (y <= 150) { //ESTÁ NA CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT2CS01 = false;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 130) {
                            std::unique_lock<std::mutex> ul(mutexCS04);

                            criticalSection.wait(ul, [] { return (freeCS04) &&
                                                        (freeCS03 || freeCS06) &&
                                                        (freeCS00 || freeCS02 || freeCS06); });

                            ul.unlock();
                        }
                        if (y >= 130 && y <= 150) { //ao entrar na CS04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT2CS04 = false;                                
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        }

                        y += 10;


                    } else if (y == 150 && x > 330) {                       

                        if (x == 570) { // FIM DA CS01
                            mutexCS01.lock();
                                freeCS01 = true;
                                readyT2CS01 = true;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }                        

                        if (x == 440) {
                            mutexCS04.lock();
                                freeCS04 = true;
                                readyT2CS04 = true;                                
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        } else if (x > 440) { // CS04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT2CS04 = false;                                
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 490) { // ESPERA T4 SAIR DE CS03
                            std::unique_lock<std::mutex> ul(mutexCS03);

                            criticalSection.wait(ul, [] { return (freeCS03) && (freeCS00 || freeCS02); });

                            ul.unlock();
                        }

                        if (x > 330 && x <= 490) { //CRITICAL SECTION 03 ( T2 VS T4 )
                            mutexCS03.lock();
                                freeCS03 = false;
                                readyT2CS03 = false;                                
                            mutexCS03.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 360) { //ESPERA T1 SAIR DA CS00
                            std::unique_lock<std::mutex> ul(mutexCS00);

                            criticalSection.wait(ul, [] { return (freeCS00); });

                            ul.unlock();
                        }

                        if (x <= 360 && x >= 330) { //INÍCIO CS00
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT2CS00 = false;                                
                            mutexCS00.unlock();
                            criticalSection.notify_all();
                        }

                        x -= 10;

                    } else { // CRITICAL SECTION 00 ( T1 VS T2 )

                        if (y == 130) { // SAIU DA CS03
                            mutexCS03.lock();
                                freeCS03 = true;
                                readyT2CS03 = true;                                
                            mutexCS03.unlock();
                            criticalSection.notify_all();
                        }

                        mutexCS00.lock();
                            freeCS00 = false;
                            readyT2CS00 = false;                                                        
                        mutexCS00.unlock();
                        criticalSection.notify_all();

                        y -= 10;
                    }
                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 3: //Trem 3
                if (velocidade != -1) {
                    if (x == 870 && y < 150) {
                        y == 144 ? y += 6 : y += 10;
                    } else if (y == 150 && x > 600) {

                        if (x == 760) {
                            std::unique_lock<std::mutex> ul(mutexCS05);
                            criticalSection.wait(ul, [] { return (freeCS05) &&
                                                    (freeCS01 || freeCS04) &&
                                                    (freeCS03 || freeCS06 || freeCS01) &&
                                                    (freeCS02 || freeCS06 || freeCS01) &&
                                                    (freeCS00 || freeCS02 || freeCS06 || freeCS01); });

                            ul.unlock();
                        }

                        if (x <= 760) { //ENTROU NA CS05
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT3CS05 = false;
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 630) { //ESPERA T2 SAIR DO CS01
                            std::unique_lock<std::mutex> ul(mutexCS01);

                            criticalSection.wait(ul, [] { return (freeCS01); });

                            ul.unlock();
                        }
                        if (x <= 630 && x >= 600) {
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }

                        x -= 10;

                    } else if (x == 600 && y > 30) {                       

                        if (y == 130) {
                            mutexCS05.lock();
                                freeCS05 = true;
                                readyT3CS05 = true;                                
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        } else if (y <= 150 && y > 130) {
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT3CS05 = false;                                
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        }                        

                        if (y >= 30) { //T3 CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }

                        y -= 10;

                    } else {

                        if (x == 620) {
                            mutexCS01.lock();
                                freeCS01 = true;
                                readyT3CS01 = true;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        } else if (x >= 600 && x < 620) {
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;                                
                            mutexCS01.unlock();
                            criticalSection.notify_all();
                        }

                        x += 10;

                    }

                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 4: //Trem 4
                if (velocidade != -1) {
                    if (x == 195 && y > 150) {

                        if (y == 166 || y == 167 || y == 160) { // ESPERA T1 SAIR DA CS02 e (T1 SAIR DO CS00 && T2 SAIR DO CS03 --> EVITAR DEADLOCK)

                            std::unique_lock<std::mutex> ul(mutexCS02);

                            criticalSection.wait(ul, [] { return (freeCS02) &&
                                                (freeCS00 || freeCS03) &&
                                                (freeCS04 || freeCS06) &&
                                                (freeCS06 || freeCS01 || freeCS05) &&
                                                (freeCS00 || freeCS04 || freeCS06) &&
                                                 (freeCS00 || freeCS01 || freeCS06 || freeCS05); });
                            ul.unlock();

                        }

                        if (y <= 167 && y >= 150) { //INÍCIO CS02
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT4CS02  = false;                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 156) {
                            y -= 6;
                        } else if (y == 157) {
                            y -= 7;
                        } else {
                            y -= 10;
                        }

                    } else if (y == 150 && x < 465)     {

                        if (x == 355) {
                            mutexCS02.lock();
                                freeCS02 = true;
                                readyT4CS02 = true; //terminou de utilizar                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();
                        } else if (x < 355) {
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT4CS02 = false;                                
                            mutexCS02.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 305) { //ESPERA T2 SAIR DO CS03
                            std::unique_lock<std::mutex> ul(mutexCS03);

                            criticalSection.wait(ul, [] { return (freeCS03) &&
                                                            (freeCS04 || freeCS06) &&
                                                            (freeCS01 || freeCS06) &&
                                                            (freeCS06 || freeCS01 || freeCS05); });

                            ul.unlock();
                        }

                        if (x >= 305 && x <= 465) { //CRITICAL SECTION 03 (T2 VS T4)
                           mutexCS03.lock();
                               freeCS03 = false;
                               readyT4CS03 = false;                               
                           mutexCS03.unlock();
                           criticalSection.notify_all();
                       }

                        if (x == 435) {
                            std::unique_lock<std::mutex> ul(mutexCS06);

                            criticalSection.wait(ul, [] { return (freeCS06); });

                            ul.unlock();
                        }

                        if (x >= 435 && x <= 465) {
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT4CS06 = false;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        x += 10;

                    } else if (x == 465 && y < 287) {
                        if (y >= 150 && y < 167) {
                            mutexCS03.lock();
                                freeCS03 = false;
                                readyT4CS03 = false;                                
                            mutexCS03.unlock();
                            criticalSection.notify_all();
                        }

                        if (y <= 287) {
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT4CS06 = false;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 160) {
                            y += 7;
                        } else {
                            y += 10;
                        }

                        if (y == 167) {
                            mutexCS03.lock();
                                freeCS03 = true;
                                readyT4CS03 = true; // terminou de utilizar a CS03                                
                            mutexCS03.unlock();
                            criticalSection.notify_all();
                        }

                    } else {

                        if (x == 435) { //PAROU DE USAR CS06
                            mutexCS06.lock();
                                freeCS06 = true;
                                readyT4CS06 = true;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        x -= 10;
                    }

                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 5: //Trem 5
                if (velocidade != -1) {
                    if (y == 287 && x > 465) {

                        if (x == 490 || x == 495) {

                            std::unique_lock<std::mutex> ul(mutexCS06);

                            criticalSection.wait(ul, [] { return (freeCS06) &&
                                                        (freeCS01 || freeCS05 || freeCS03) &&
                                                        (freeCS02 || freeCS00 || freeCS04) &&
                                                        (freeCS00 || freeCS02 || freeCS01 || freeCS05) &&
                                                        (freeCS03 || freeCS04); });

                            ul.unlock();
                        }

                        if (x <= 495 && x > 465) { //INICIO CS06
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT5CS06 = false;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 470) x -= 5;
                        else x -= 10;

                    } else if (x == 465 && y > 150) {

                        if (y > 150) { //CS06
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT5CS06 = false;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 177) {
                            std::unique_lock<std::mutex> ul(mutexCS04);

                            criticalSection.wait(ul, [] { return (freeCS04) && (freeCS01 || freeCS05); });

                            ul.unlock();
                        }

                        if (y <= 177 && y >= 150) { // INICIO CS04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT5CS04 = false;
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        }

                        if (y == 157) y -= 7;
                        else y -= 10;

                    } else if (y == 150 && x < 735) {

                        if (x == 495) {
                            mutexCS06.lock();
                                freeCS06 = true;
                                readyT5CS06 = true;                                
                            mutexCS06.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 625) {
                            mutexCS04.lock();
                                freeCS04 = true;
                                readyT5CS04 = true;                                
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        } else if (x < 625) {
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT5CS04 = false;                                
                            mutexCS04.unlock();
                            criticalSection.notify_all();
                        }

                        if (x == 575) {
                            std::unique_lock<std::mutex> ul(mutexCS05);

                            criticalSection.wait(ul, [] { return (freeCS05); });

                            ul.unlock();
                        }

                        if (x >= 575) {
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT5CS05 = false;                                
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        }

                        x += 10;

                    } else {

                        if (y == 170) {
                            mutexCS05.lock();
                                freeCS05 = true;
                                readyT5CS05 = true;                                
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        } else if (y < 170) {
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT5CS05 = false;                                
                            mutexCS05.unlock();
                            criticalSection.notify_all();
                        }


                    if (y == 280) y += 7;
                    else y += 10;
                }
              }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            default:
                break;
        }

        if (velocidade != -1) msleep(velocidade);
    }
}





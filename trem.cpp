#include "trem.h"
#include <QtCore>

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

                        x += 10;

                        if (x == 300) { // ESPERANDO T2 SAIR DA CS 00

                            std::unique_lock<std::mutex> ul(mutexCS00);

                            criticalSection.wait(ul, [] { return (freeCS00) &&
                                                        (readyT4CS02 || readyT2CS03) &&
                                                        (readyT4CS02 || readyT2CS04 || readyT5CS06); });

                            ul.unlock();
                        } else if (x > 300 && x <= 330) { //INÍCIO CS00
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT1CS00 = false;
                                criticalSection.notify_all();
                            mutexCS00.unlock();
                        }



                    } else if (x == 330 && y < 150) {

                        y += 10;


                        if (y <= 150) { //CRITICAL SECTION 00 (T1 VS T2)
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT1CS00 = false;
                                criticalSection.notify_all();
                            mutexCS00.unlock();
                        }

                        if (y == 130) { //T4 na CS02?
                            std::unique_lock<std::mutex> ul(mutexCS02);

                            criticalSection.wait(ul, [] { return (freeCS02); }); // until readyT4CS02 is true

                            ul.unlock();
                        }

                        if (y > 130 && y <= 150) { // INÍCIO CS02
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT1CS02 = false;
                                criticalSection.notify_all();
                            mutexCS02.unlock();
                        }

                    } else {

                        x -= 10;

                        if (x >= 300) {
                            mutexCS00.lock();
                                readyT1CS00 = false;
                                freeCS00 = false;
                                criticalSection.notify_all();
                            mutexCS00.unlock();
                        } else {
                            freeCS00 = true;
                            readyT1CS00 = true; //terminou de utilizar a seção crítica 00
                            criticalSection.notify_all();
                        }

                        if (x >= 165) { // CRITICAL SECTION 02 (T1 vs T4)
                            mutexCS02.lock();
                                readyT1CS02 = false;
                                freeCS02 = false;
                                criticalSection.notify_all();
                            mutexCS02.unlock();
                        } else { // terminou de utilizar a seção crítica 02
                            freeCS02 = true;
                            readyT1CS02 = true;
                            criticalSection.notify_all();
                        }
                    }
                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 2: //Trem 2
                if (velocidade != -1) {
                    if (y == 30 && x < 600) {

                        if (x == 350) { //T2 FORA DA CS 00
                            freeCS00 = true;
                            readyT2CS00 = true;
                            criticalSection.notify_all();
                        }

                        if (x == 570 || x == 575) { //ESPERA T3 SAIR DO CS01
                            std::unique_lock<std::mutex> ul(mutexCS01);

                            criticalSection.wait(ul, [] { return (freeCS01) &&
                                                         (readyT5CS04 || readyT3CS05) &&
                                                         (readyT1CS00 || readyT4CS02 || readyT5CS06) &&
                                                         (readyT4CS03 || readyT5CS06 || readyT3CS05); });

                            ul.unlock();
                        } else if (x > 575 && x <= 595) { //ENTROU CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT2CS01 = false;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                        x == 595 ? x += 5 : x += 10; //limite com base no tamanho do trem (quadrado) e o posicionamento inicial nos trilhos



                    } else if (x == 600 && y < 150) {

                        if (y <= 150) { //ESTÁ NA CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT2CS01 = false;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                        if (y == 130) {
                            std::unique_lock<std::mutex> ul(mutexCS04);

                            criticalSection.wait(ul, [] { return (freeCS04) &&
                                                        (readyT4CS03 || readyT5CS06) &&
                                                        (readyT1CS00 || readyT4CS02 || readyT5CS06); });

                            ul.unlock();
                        } else if (y > 130 && y <= 140) { //ao entrar na CS04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT2CS04 = false;
                                criticalSection.notify_all();
                            mutexCS04.unlock();
                        }

                        y += 10;


                    } else if (y == 150 && x > 330) {

                        if (x == 570) { // FIM DA CS01
                            mutexCS01.lock();
                                freeCS01 = true;
                                readyT2CS01 = true;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                        x -= 10;

                        if (x >= 440) { // CS04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT2CS04 = false;
                                criticalSection.notify_all();
                            mutexCS04.unlock();
                        } else {
                            mutexCS03.lock();
                                freeCS04 = true;
                                readyT2CS04 = true;
                                criticalSection.notify_all();
                            mutexCS03.unlock();
                        }

                        if (x == 490) { // ESPERA T4 SAIR DE CS03
                            std::unique_lock<std::mutex> ul(mutexCS03);

                            criticalSection.wait(ul, [] { return (freeCS03) && (readyT1CS00 || readyT4CS02); });

                            ul.unlock();
                        }

                        if (x > 330 && x < 465) { //CRITICAL SECTION 03 ( T2 VS T4 )
                            mutexCS03.lock();
                                freeCS03 = false;
                                readyT2CS03 = false;
                                criticalSection.notify_all();
                            mutexCS03.unlock();
                        }

                        if (x == 360) { //ESPERA T1 SAIR DA CS00
                            std::unique_lock<std::mutex> ul(mutexCS00);

                            criticalSection.wait(ul, [] { return (freeCS00); });

                            ul.unlock();
                        } else if (x < 360 && x >= 330) { //INÍCIO CS00
                            mutexCS00.lock();
                                freeCS00 = false;
                                readyT2CS00 = false;
                                criticalSection.notify_all();
                            mutexCS00.unlock();
                        }

                    } else { // CRITICAL SECTION 00 ( T1 VS T2 )
                        if (y == 130) { // SAIU DA CS03
                            freeCS03 = true;
                            readyT2CS03 = true;
                            criticalSection.notify_all();
                        }

                        mutexCS00.lock();
                            freeCS00 = false;
                            readyT2CS00 = false;
                            y -= 10;
                            criticalSection.notify_all();
                        mutexCS00.unlock();
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
                                                    (readyT2CS01 || readyT5CS04) &&
                                                    (readyT4CS03 || readyT5CS06 || readyT2CS01) &&
                                                    (readyT4CS02 || readyT5CS06 || readyT2CS01); });

                            ul.unlock();
                        } else if (x < 760) { //ENTROU NA CS05
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT3CS05 = false;
                                criticalSection.notify_all();
                            mutexCS05.unlock();
                        }

                        if (x == 630) { //ESPERA T2 SAIR DO CS01
                            std::unique_lock<std::mutex> ul(mutexCS01);

                            criticalSection.wait(ul, [] { return (freeCS01); });

                            ul.unlock();
                        } else if (x < 630 && x >= 600) {
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                        x -= 10;

                    } else if (x == 600 && y > 30) {
                        if (y >= 130) {
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT3CS05 = false;
                                criticalSection.notify_all();
                            mutexCS05.unlock();
                        } else {
                            mutexCS05.lock();
                                freeCS05 = true;
                                readyT3CS05 = true;
                                criticalSection.notify_all();
                            mutexCS05.unlock();
                        }

                        y -= 10;

                        if (y >= 30) { //T3 CS01
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                    } else {

                        if (x > 620) {
                            freeCS01 = true;
                            readyT3CS01 = true;
                            criticalSection.notify_all();
                        } else {
                            mutexCS01.lock();
                                freeCS01 = false;
                                readyT3CS01 = false;
                                criticalSection.notify_all();
                            mutexCS01.unlock();
                        }

                        x += 10;

                    }

                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 4: //Trem 4
                if (velocidade != -1) {
                    if (x == 195 && y > 150) {

                        if (y == 156) {
                            y -= 6;
                        } else if (y == 157) {
                            y -= 7;
                        } else {
                            y -= 10;
                        }

                        if (y == 166 || y == 167 || y == 160) { // ESPERA T1 SAIR DA CS02 e (T1 SAIR DO CS00 && T2 SAIR DO CS03 --> EVITAR DEADLOCK)

                            std::unique_lock<std::mutex> ul(mutexCS02);

                            criticalSection.wait(ul, [] { return (freeCS02) &&
                                                (readyT1CS00 || readyT2CS03) &&
                                                (readyT2CS04 || readyT5CS06); });
                            ul.unlock();

                        } else if (y < 160 && y > 150) { //INÍCIO CS02
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT4CS02  = false;
                                criticalSection.notify_all();
                            mutexCS02.unlock();
                        }

                    } else if (y == 150 && x < 465) {


                        if (x <= 350) { //CRITICAL SECTION 02 (T1 vs T4)
                            mutexCS02.lock();
                                freeCS02 = false;
                                readyT4CS02 = false;
                                criticalSection.notify_all();
                            mutexCS02.unlock();
                        } else {
                            freeCS02 = true;
                            readyT4CS02 = true; //terminou de utilizar
                            criticalSection.notify_all();
                        }

                        if (x == 305) { //ESPERA T2 SAIR DO CS03
                            std::unique_lock<std::mutex> ul(mutexCS03);

                            criticalSection.wait(ul, [] { return (freeCS03) &&
                                                            (readyT2CS04 || readyT5CS06) &&
                                                            (readyT2CS01 || readyT5CS06); });

                            ul.unlock();
                        } else if (x > 305 && x <= 465) { //CRITICAL SECTION 03 (T2 VS T4)
                           mutexCS03.lock();
                               freeCS03 = false;
                               readyT4CS03 = false;
                               criticalSection.notify_all();
                           mutexCS03.unlock();
                       }

                        if (x == 435) {
                            std::unique_lock<std::mutex> ul(mutexCS06);

                            criticalSection.wait(ul, [] { return (freeCS06); });

                            ul.unlock();
                        } else if (x > 435 && x < 465) {
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT4CS06 = false;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        x += 10;

                    } else if (x == 465 && y < 287) {

                        if (y < 287) {
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT4CS06 = false;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        if (y >= 150 && y < 167) {
                            mutexCS03.lock();
                                freeCS03 = false;
                                readyT4CS03 = false;
                                criticalSection.notify_all();
                            mutexCS03.unlock();
                        }

                        if (y == 160) {
                            y += 7;
                            freeCS03 = true;
                            readyT4CS03 = true; // terminou de utilizar a CS03
                            criticalSection.notify_all();
                        } else {
                            y += 10;
                        }

                    } else {

                        if (x == 435) { //PAROU DE USAR CS06
                            mutexCS06.lock();
                                freeCS06 = true;
                                readyT4CS06 = true;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        x -= 10;
                    }

                }
                emit updateGUI(ID, x, y);    //Emite um sinal
                break;
            case 5: //Trem 5
                if (velocidade != -1) {
                    if (y == 287 && x > 465) {

                        if (x == 490) {
                            std::unique_lock<std::mutex> ul(mutexCS06);

                            criticalSection.wait(ul, [] { return (freeCS06) &&
                                                        (readyT2CS01 || readyT3CS05) &&
                                                        (readyT4CS03 || readyT2CS04); });

                            ul.unlock();
                        } else if (x < 490 && x > 465) { //INICIO CS06
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT5CS06 = false;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        if (x == 470) x -= 5;
                        else x -= 10;

                    } else if (x == 465 && y > 150) {

                        if (y > 150) { //CS06
                            mutexCS06.lock();
                                freeCS06 = false;
                                readyT5CS06 = false;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        if (y == 167) {
                            std::unique_lock<std::mutex> ul(mutexCS04);

                            criticalSection.wait(ul, [] { return (freeCS04) && (readyT2CS01 || readyT3CS05); });

                            ul.unlock();
                        }

                        if (y < 167 && y >= 150) { // INICIO CS04
                            mutexCS03.lock();
                                freeCS04 = false;
                                readyT5CS04 = false;
                                criticalSection.notify_all();
                            mutexCS03.unlock();
                        }

                        if (y == 157) y -= 7;
                        else y -= 10;

                    } else if (y == 150 && x < 735) {

                        if (x == 495) {
                            mutexCS06.lock();
                                freeCS06 = true;
                                readyT5CS06 = true;
                                criticalSection.notify_all();
                            mutexCS06.unlock();
                        }

                        if (x <= 625) { //CS 04
                            mutexCS04.lock();
                                freeCS04 = false;
                                readyT5CS04 = false;
                                criticalSection.notify_all();
                            mutexCS04.unlock();
                        } else { // fora CS04 entrou na CS05
                            freeCS04 = true;
                            readyT5CS04 = true;
                            criticalSection.notify_all();
                        }

                        if (x == 575) {
                            std::unique_lock<std::mutex> ul(mutexCS05);

                            criticalSection.wait(ul, [] { return (freeCS05); });

                            ul.unlock();
                        } else if (x > 575) {
                            mutexCS05.lock();
                                freeCS05 = false;
                                readyT5CS05 = false;
                                criticalSection.notify_all();
                            mutexCS05.unlock();
                        }

                        x += 10;

                    } else {

                    if (y <= 167) {
                        mutexCS05.lock();
                            freeCS05 = false;
                            readyT5CS05 = false;
                            criticalSection.notify_all();
                        mutexCS05.unlock();
                    } else {
                        mutexCS05.lock();
                            freeCS05 = true;
                            readyT5CS05 = true;
                            criticalSection.notify_all();
                        mutexCS05.unlock();
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





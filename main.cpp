/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */


#include "InterruptIn.h"
#include "Mutex.h"
#include "PinNames.h"
#include "Semaphore.h"
#include "ThisThread.h"
#include "Thread.h"
#include "UnbufferedSerial.h"
#include "mbed.h"

// Blinking rate in milliseconds
#define TIEMPO_LECTURA     1s

UnbufferedSerial hc04 (PA_11, PA_12);

char c;


InterruptIn enconder (D2);

Thread hilo_incrementar;
Thread hilo_contabilizar;
Thread hilo_recibir_rc;

void int_isr(void);
void rc_isr(void);
void incrementar(void);
void contabilizar(void);
void recibir_rc(void);

Semaphore smp_int(0);
Semaphore smp_rc(0);
Mutex hc04_rc;

Timer t;

static long tiempo=0;
static float frecuencia = 0.0;
int entero;
int decimal;


int main()
{
    
    enconder.rise(int_isr);
    hc04.baud(115200);
    hc04.attach(&rc_isr, SerialBase::RxIrq);
    hilo_incrementar.start(incrementar);
    hilo_contabilizar.start(contabilizar);
    hilo_recibir_rc.start(recibir_rc);
    t.reset();
    t.start();
    while (true) {
        
    }
}

void int_isr(void)
{
    smp_int.release();
}


void incrementar (void)
{
    while(true)
    {
        smp_int.acquire();
        t.stop();
        tiempo = t.elapsed_time().count();
        t.reset();
        t.start();
        

    }
}

void contabilizar (void)
{
    while(true)
    {
        frecuencia = float(1e6/tiempo);
        entero = int(frecuencia);
        decimal = int(frecuencia-float(decimal))*10000;
        printf("La frec: %05u.%05u\n\r", entero, decimal);

        //printf("Tiempo: %lu \n\r", tiempo);
        ThisThread::sleep_for(TIEMPO_LECTURA);
    }
}

void rc_isr (void)
{
    smp_rc.release();
}

void recibir_rc(void)
{
    while(true)
    {

        smp_rc.acquire();
        hc04_rc.lock();
        hc04.read(&c, 1);
        if (c=='1')
        hc04_rc.unlock();
        hc04.write(&c, 1);
        


    }

}

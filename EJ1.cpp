#include <iostream>
#include <pthread.h>
#include <vector>
#include <csignal>

void Ejercicio1(int n, int m);
void* hiloT1(void* arg);
void* hiloT2(void* arg);

int COMPARTIDA1 = 0;

void Ejercicio1(int n, int m) {
    std::vector<pthread_t> hilos_t1, hilos_t2;
    std::vector<int> ids1;
    std::vector<int> ids2;
    int i;
    pthread_t hilo1;
    pthread_t hilo2;
    for(i=0;i<n;i++) {
        ids1.push_back(i);
        pthread_create(&hilo1, nullptr, hiloT1,(void*) &ids1[i]);
        hilos_t1.push_back(hilo1);
    }
    for(i=0;i<m;i++) {
        ids2.push_back(i);
        pthread_create(&hilo2, nullptr, hiloT2,(void*) &ids2[i]);
        hilos_t2.push_back(hilo2);
    }
    for(i=0;i<n;i++) {
        pthread_join(hilos_t1[i], nullptr);
    }
    for(i=0;i<m;i++) {
        pthread_join(hilos_t2[i], nullptr);
    }
    printf("Fin del programa");
}

void* hiloT1(void* arg) {
    auto id = (int*) arg;
    printf("Thread1 instancia %d \n",*id);
    usleep(rand() % 1000000);
    COMPARTIDA1++;
    return nullptr;
}

void* hiloT2(void* arg) {
    auto id = (int*) arg;
    printf("Thread2 instancia %d \n",*id);
    usleep(rand() % 1000000);
    printf("Valor de la variable compartida: %d \n",COMPARTIDA1);
    return nullptr;
}


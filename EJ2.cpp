#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>


void Ejercicio2(int n, int m);
void procesoT1(int id);
void procesoT2(int id);

int COMPARTIDA2 = 0;

void Ejercicio2(int n, int m) {
    std::vector<pid_t> procesos_t1, procesos_t2;
    int i;

    for (i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            procesoT1(i);
            exit(0); // Importante: el proceso hijo debe terminar con exit()
        } else { // Proceso padre
            procesos_t1.push_back(pid);
        }
    }

    for (i = 0; i < m; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            procesoT2(i);
            exit(0); // Importante: el proceso hijo debe terminar con exit()
        } else { // Proceso padre
            procesos_t2.push_back(pid);
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (pid_t pid : procesos_t1) {
        waitpid(pid, nullptr, 0);
    }

    for (pid_t pid : procesos_t2) {
        waitpid(pid, nullptr, 0);
    }

    std::cout << "Fin del programa" << std::endl;
}

void procesoT1(int id) {
    std::cout << "Proceso1 instancia " << id << std::endl;
    usleep(rand() % 1000000);
    COMPARTIDA2++;
}

void procesoT2(int id) {
    std::cout << "Proceso2 instancia " << id << std::endl;
    usleep(rand() % 1000000);
    std::cout << "Valor de la variable compartida: " << COMPARTIDA2 << std::endl;
}

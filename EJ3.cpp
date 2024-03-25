#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using std::rand, std::cout, std::endl;

int RangoAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

void jugador(int fd[2][2]) {
    int carta;
    float puntos = 0;
    bool abandonar = false;

    while (!abandonar && puntos < 7.5) {
        read(fd[0][0], &carta, sizeof(int));  // Leer la carta desde el padre

        int decision = RangoAleatorio(1, 3);  // Generar una decisión aleatoria (1: Pedir carta, 2: Plantarse, 3: Abandonar)
        if (decision == 1) {
            // Pedir carta
            if (carta > 7) {
                puntos += 0.5;  // Las figuras valen 0.5 puntos
            } else {
                puntos += (float) carta;
            }
            write(fd[1][1], &decision, sizeof(int));  // Enviar la decisión al padre
        } else if (decision == 2) {
            // Plantarse
            cout << "Jugador se planta. Puntos finales: " << puntos << endl;
            abandonar = true;
            write(fd[1][1], &decision, sizeof(int));  // Enviar la decisión al padre
        } else {
            // Abandonar
            cout << "Jugador se retira." << endl;
            abandonar = true;
            write(fd[1][1], &decision, sizeof(int));  // Enviar la decisión al padre
        }
    }
}

void repartirCartas(int numJugadores) {
    int fd[numJugadores][2][2];

    // Crear pipes para la comunicación con los jugadores
    for (int i = 0; i < numJugadores; ++i) {
        if (pipe(fd[i][0]) == -1 || pipe(fd[i][1]) == -1) {
            cout << "Error al crear los pipes para el jugador " << i + 1 << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos jugadores
    for (int i = 0; i < numJugadores; ++i) {
        pid_t pid = fork();
        if (pid == 0) {  // Proceso hijo (jugador)
            jugador(fd[i]);
            exit(EXIT_SUCCESS);
        } else {  // Proceso padre (repartidor)
            // Enviar la carta al proceso hijo (jugador)
            int carta = RangoAleatorio(1, 10);  // Generar una carta aleatoria entre 1 y 10
            write(fd[i][1][1], &carta, sizeof(int));
            int decision;
            read(fd[i][0][0], &decision, sizeof(int));  // Leer la decisión del hijo
            while (decision == 1) {
                // El jugador pide otra carta
                carta = RangoAleatorio(1, 10);  // Generar una carta aleatoria entre 1 y 10
                write(fd[i][1][1], &carta, sizeof(int));
            }
            wait(nullptr);
        }
    }

    // Esperar a que todos los jugadores terminen
    for (int i = 0; i < numJugadores; ++i) {
        wait(nullptr);
    }
}


void Ejercicio3(int numJugadores) {
    if (numJugadores > 0) {
        repartirCartas(numJugadores);
    } else {
        cout << "Número de jugadores inválido." << endl;
    }
}


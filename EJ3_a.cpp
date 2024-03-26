#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using std::rand, std::cout, std::endl;
using std::string;

int RangoAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

void jugador(int fd_lectura[2], int fd_escritura[2], int numJugador) {
    int carta;
    float puntos = 0;
    bool abandona = false;
    string nombreCarta;

    while (!abandona && puntos <= 7.5) {
        read(fd_lectura[0], &carta, sizeof(int));  // Leer la carta desde el padre

        switch (carta) {
            case 8:
                nombreCarta = "Sota";
                break;
            case 9:
                nombreCarta = "Caballo";
                break;
            case 10:
                nombreCarta = "Rey";
                break;
            default:
                nombreCarta = std::to_string(carta);
                break;
        }

        if (carta > 7) {  // Sota, caballo o rey == 8, 9 y 10
            puntos += 0.5;

        } else {
            puntos += (float) carta;
        }

        cout << "Jugador " << numJugador << " recibe carta: " << nombreCarta << ". Puntos: " << puntos << endl;
        if (puntos > 7.5) {
            cout << "Jugador " << numJugador << " se pasa de 7.5. Puntos finales: " << puntos << endl;
            cout << "----------------------------------------" << endl;
            abandona = true;
            write(fd_escritura[1], &puntos, sizeof(float));
        }
        else if (puntos == 7.5) {
            cout << "Jugador " << numJugador << " llega a 7.5. Puntos finales: " << puntos << endl;
            cout << "----------------------------------------" << endl;
            abandona = true;
            write(fd_escritura[1], &puntos, sizeof(float));
        }
        else if (puntos < 7.5 && puntos > 4) {
            cout << "Jugador " << numJugador << " se planta. Puntos finales: " << puntos << endl;
            cout << "----------------------------------------" << endl;
            abandona = true;
            write(fd_escritura[1], &puntos, sizeof(float));
        }
    }
}

void repartirCartas(int numJugadores) {
    int fd[numJugadores][2][2];
    pid_t pids[numJugadores];
    float puntos[numJugadores];

    // Crear los pipes
    for (int i = 0; i < numJugadores; ++i) {
        if (pipe(fd[i][0]) == -1) {
            cout << "Error al crear los pipes para el jugador " << i + 1 << endl;
            exit(EXIT_FAILURE);
        }
        if (pipe(fd[i][1]) == -1) {
            cout << "Error al crear los pipes para el jugador " << i + 1 << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Crear los procesos jugadoes
    for (int i = 0; i < numJugadores; ++i) {
        pid_t pid = fork();
        if (pid == 0) {  // (jugador)
            jugador(fd[i][0],fd[i][1],  i);
            exit(EXIT_SUCCESS);  // Terminar el proceso (jugador)
        } else {  // (padre)
            pids[i] = pid;  // Guardar el pid del proceso hijo
        }
    }

    // Envio las cartas a los jugadores
    float puntosJugador;
    for (int i = 0; i < numJugadores; ++i) {
        while (true) {
            int status;
            pid_t result = waitpid(pids[i], &status, WNOHANG);
            if (result == 0) {
                // Proceso hijo sigue vivo
                int carta = RangoAleatorio(1, 10); // Carta aleatoria entre 1 y 10
                write(fd[i][0][1], &carta, sizeof(int));
            } else if (result == pids[i]) {
                // Proceso hijo terminó
                break;
            } else {
                cout << "Error al esperar al jugador " << i + 1 << endl;
                exit(EXIT_FAILURE);
            }
        }
        read(fd[i][1][0], &puntosJugador, sizeof(float));
        puntos[i] = puntosJugador;
    }

    // Esperar a que todos los jugadores terminen
    for (int i = 0; i < numJugadores; ++i) {
        wait(nullptr);
    }
    for (int i = 0; i < numJugadores; ++i) {
        cout << "Jugador " << i << " puntos: " << puntos[i] << endl;
    }
}

void Ejercicio3_a(int numJugadores) {
    if (numJugadores > 0) {
        repartirCartas(numJugadores);
    } else {
        cout << "Número de jugadores inválido." << endl;
    }
}
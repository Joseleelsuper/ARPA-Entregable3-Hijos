#include <mpi.h>
#include <iostream>

constexpr int MAX_SIZE = 100;
constexpr int TAG = 0;
constexpr int RANK_MENOR = 0;
constexpr int RANK_MASTER = 0;

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Comm intercom;
    MPI_Comm_get_parent(&intercom);

    if (intercom == MPI_COMM_NULL) {
        printf("Este proceso no tiene un padre.\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Mensaje de saludo al ser creado
    printf("<<HIJO %d>> Hola desde el hijo %d de %d\n", rank, rank, size);

    // Recibir mensaje de saludo del padre
    char mensaje[MAX_SIZE];
	MPI_Bcast(mensaje, MAX_SIZE, MPI_CHAR, RANK_MASTER, intercom);
    printf("<<HIJO %d>> Hijo %d recibió mensaje: %s\n",rank, rank, mensaje);

    // El hijo de menor rango envía un mensaje a los demás hijos
    if (rank == RANK_MENOR) {
		printf("\n<<HIJO MENOR>> Soy el hijo de menor rango (%d) y enviaré un mensaje a los demás hijos\n", rank);
        char mensaje_hijo[] = "<<HIJO MENOR>> Hola desde el hijo de menor rango";
		MPI_Bcast(mensaje_hijo, MAX_SIZE, MPI_CHAR, RANK_MENOR, MPI_COMM_WORLD);
    }
    else {
		MPI_Bcast(mensaje, MAX_SIZE, MPI_CHAR, RANK_MENOR, MPI_COMM_WORLD);
        printf("<<HIJO %d>> Hijo %d recibió mensaje del hijo de menor rango: %s\n", rank, rank, mensaje);
    }

    if (rank == RANK_MENOR) {
        double start_time = 0;
		double end_time = MPI_Wtime();
        // Recibir del proceso padre el start time.
		MPI_Recv(&start_time, 1, MPI_DOUBLE, 0, TAG, intercom, MPI_STATUS_IGNORE);

		// Calcular tiempo de ejecución
		double tiempo_ejecucion = end_time - start_time;
		printf("\n<<HIJO MENOR>> Tiempo de ejecución: %f (s)\n", tiempo_ejecucion);
    }

    MPI_Finalize();
    return 0;
}

/*
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Comm parentcomm;
    MPI_Comm_get_parent(&parentcomm);
    if (parentcomm == MPI_COMM_NULL) {
        printf("No hay proceso padre.\n");
        MPI_Finalize();
        return 1;
    }

    int rankHijo, sizeHijos;
    MPI_Comm_rank(MPI_COMM_WORLD, &rankHijo);
    MPI_Comm_size(MPI_COMM_WORLD, &sizeHijos);

    // Mensaje al ser creado
    printf("<<HIJO %d>> He sido creado.\n", rankHijo);

    // Recibir mensaje del padre
    char mensajePadre[100];
    MPI_Bcast(mensajePadre, 100, MPI_CHAR, 0, parentcomm);
    printf("<<HIJO %d>> Recibido del padre: %s\n", rankHijo, mensajePadre);

    // El hijo de menor rango envía un mensaje al resto
    char mensajeHijo[100];
    if (rankHijo == 0) {
        sprintf_s(mensajeHijo, "<<HIJO %d>> Saludo a mis hermanos", rankHijo);
        printf("<<HIJO %d>> Soy el proceso hijo con menor tamaño.\n", rankHijo);
    }
    MPI_Bcast(mensajeHijo, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (rankHijo != 0) {
        printf("<<HIJO %d>> Recibido del hijo 0: %s\n", rankHijo, mensajeHijo);
    }

    MPI_Finalize();
    return 0;
}
*/
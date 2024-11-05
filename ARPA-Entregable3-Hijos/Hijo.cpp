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
    MPI_Recv(mensaje, MAX_SIZE, MPI_CHAR, RANK_MASTER, TAG, intercom, MPI_STATUS_IGNORE);
    printf("<<HIJO %d>> Hijo %d recibió mensaje: %s\n",rank, rank, mensaje);

    // El hijo de menor rango envía un mensaje a los demás hijos
    if (rank == RANK_MENOR) {
		printf("\n<<HIJO MENOR>> Soy el hijo de menor rango (%d) y enviaré un mensaje a los demás hijos\n", rank);
        char mensaje_hijo[] = "<<HIJO MENOR>> Hola desde el hijo de menor rango";
        for (int i = 1; i < size; i++) {
            MPI_Send(mensaje_hijo, strlen(mensaje_hijo) + 1, MPI_CHAR, i, TAG+10, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(mensaje, MAX_SIZE, MPI_CHAR, RANK_MENOR, TAG+10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("<<HIJO %d>> Hijo %d recibió mensaje del hijo de menor rango: %s\n", rank, rank, mensaje);
    }

    if (rank == RANK_MENOR) {
        double start_time = 0;
		double end_time = MPI_Wtime();
        // Recibir del proceso padre el start time.
		MPI_Recv(&start_time, 1, MPI_DOUBLE, 0, TAG + 1, intercom, MPI_STATUS_IGNORE);

		// Calcular tiempo de ejecución
		double tiempo_ejecucion = end_time - start_time;
		printf("\n<<HIJO MENOR>> Tiempo de ejecución: %f (s)\n", tiempo_ejecucion);
    }

    MPI_Finalize();
    return 0;
}
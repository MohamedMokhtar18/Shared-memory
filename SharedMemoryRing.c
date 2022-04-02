#include <stdio.h>
#include <mpi.h>
#define to_right 201

int main(int argc, char *argv[])
{
    int right, left;
    int my_rank_world, size_world;
    int my_rank_sm, size_sm;
    int rcv_buf, sum, i;
    int *rcv_buf_ptr;
    int snd_buf;
    MPI_Comm comm_sm;

    MPI_Win win;

    MPI_Init(&argc, &argv);
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &comm_sm);
    MPI_Comm_rank(comm_sm, &my_rank_sm);
    MPI_Comm_size(comm_sm, &size_sm);
    if (my_rank_sm == 0)
    {
        if (size_sm == size_world)
        {
            printf("MPI_COMM_WORLD consists of only one shared memory region\n");
        }
        else
        {
            printf("MPI_COMM_WORLD is split into 2 or more shared memory islands\n");
        }
    }
    right = (my_rank_sm + 1) % size_sm;
    left = (my_rank_sm - 1 + size_sm) % size_sm;

    MPI_Win_allocate(sizeof(int), sizeof(int), MPI_INFO_NULL, comm_sm, &rcv_buf_ptr, &win);

    sum = 0;
    snd_buf = my_rank_sm;

    for (i = 0; i < size_sm; i++)
    {

        /* *** The following 3 lines shall be substituted by
         *** 1-sided communication and synchronization    */
        MPI_Win_fence(0, win);
        MPI_Put(&snd_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, win);
        MPI_Win_fence(0, win);

        snd_buf = *rcv_buf_ptr;

        sum += *rcv_buf_ptr;
    }

    printf("PE%i:\tSum = %i\n", my_rank_sm, sum);
    MPI_Win_free(&win);
    MPI_Finalize();
}
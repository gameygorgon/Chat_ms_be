#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

int main(int argc, char **argv)
{

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  // program input argument
  
  long prime = ins__args.arg;

  struct timeval ins__tstart, ins__tstop;

  int myrank, nproc;

  MPI_Init(&argc, &argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  if (!myrank)
    gettimeofday(&ins__tstart, NULL);

  // run your computations here (including MPI communication)

  MPI_Request request;
  MPI_Request request_1;

  MPI_Status status;

  int z = 0;
  int flag=0;
  int is_prime = 1;

  MPI_Irecv(&z, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request_1);

  

  for (int i = myrank + 2; i * i <= prime; i += nproc)
  {

    MPI_Test(&request_1, &flag, MPI_STATUS_IGNORE);
    if (flag==1)
    {
      is_prime = 0;
      break;
    }

    int r = prime % i;
    if (r == 0)
    {
      is_prime = 0;
      

      for (int i = 0; i < nproc; i++)
      {
        if (i != myrank)
          MPI_Isend(&is_prime, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
      }

      break;
    }
  }

  int is_prime_final = 0;

  MPI_Reduce(&is_prime, &is_prime_final, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myrank == 0)
  {
    if (is_prime_final == nproc)
    {
      printf("%ln is a prime number\n", &prime);
    }

    else
    {
      printf("%ln is not a prime number\n", &prime);
    }
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }

  MPI_Finalize();
}

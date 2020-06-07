#include <iostream>
#include <stdio.h>
#include <time.h>
#include "mpi.h"
using namespace std;

void Merge(int* a, int* b, int l, int m, int r) 
{
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while ((h <= m) && (j <= r)) 
	{
		if (a[h] <= a[j]) 
		{
			b[i] = a[h];
			h++;
		}
		else
		{
			b[i] = a[j];
			j++;
		}
		i++;
	}
	if (m < h) 
	{
		for (k = j; k <= r; k++) 
		{
			b[i] = a[k];
			i++;
		}
	}
	else 
	{
		for (k = h; k <= m; k++) 
		{
			b[i] = a[k];
			i++;
		}
	}
	for (k = l; k <= r; k++) 
	{
		a[k] = b[k];
	}

}

void Sort(int* a, int* b, int l, int r) 
{
	int m;
	if (l < r) 
	{
		m = (l + r) / 2;
		Sort(a, b, l, m);
		Sort(a, b, (m + 1), r);
		Merge(a, b, l, m, r);
	}
}

int main(int argc, char** argv) 
{
	int Size_M = atoi(argv[1]), PartSize, Current_Rank, Proc_N, Root = 0;
	double WTime_Start = 0, WTime_End = 0;
	srand(time(NULL));								// for random randomizer in each run
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &Current_Rank);
	MPI_Comm_size(MPI_COMM_WORLD, &Proc_N);
	if (Size_M % Proc_N != 0)
	{
		if (Current_Rank == Root)
		{
			cout << "\n\nThe size of massive must be division by total processes without residue! \nSize of massive % total processes == 0!\n\n*************\nInitial abort\n*************\n\n";	
		}
		MPI_Finalize();
		return 0;
	}
	PartSize = Size_M / Proc_N;
	int* Unsorted_Massive = new int[Size_M];
	int* Sorted_Massive = new int[Size_M];
	int* Middle_Massive_Before_Final = new int[Size_M];
	int* Part_Sorted_Massive = new int[PartSize];
	int* tmp_array = new int[PartSize];
	if (Current_Rank == Root)
	{
		cout << "\n\tMerge sort realization with Microsoft MPI\n\tTimofeev E.V. 381708-2.\n\n";
		cout << "\tSize of massive = " << Size_M << "\n\n";
		cout << "Generated unsorted massive: ";
		for (int i = 0; i < Size_M; i++) 
		{
			Unsorted_Massive[i] = rand() % Size_M;
			cout << Unsorted_Massive[i] << "|";
		}
		cout << "\n\n";
		WTime_Start = MPI_Wtime();
	}
	MPI_Scatter(Unsorted_Massive, PartSize, MPI_INT, Part_Sorted_Massive, PartSize, MPI_INT, Root, MPI_COMM_WORLD);
	Sort(Part_Sorted_Massive, tmp_array, 0, (PartSize - 1));

	
	if (Current_Rank == Root) {

		Sorted_Massive = new int[Size_M];

	}

	MPI_Gather(Part_Sorted_Massive, PartSize, MPI_INT, Sorted_Massive, PartSize, MPI_INT, 0, MPI_COMM_WORLD);

	if (Current_Rank == Root) 
	{
		Sort(Sorted_Massive, Middle_Massive_Before_Final, 0, (Size_M - 1));
		WTime_End = MPI_Wtime();	
		cout << "Sorted massive:\t\t    ";
		for (int i = 0; i < Size_M; i++)
		{
			cout << Sorted_Massive[i] << "|";
		}
		cout << "\n\n";
		cout << "Total time = " << WTime_End - WTime_Start << " seconds\n\n";
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}

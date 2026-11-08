#include <mpi.h>
#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>

typedef std::vector<int> Row;
typedef Row Col;
typedef std::vector<Row> Matrix;
typedef Matrix::size_type mat_size_type;
typedef Row::size_type row_size_type;

const mat_size_type MAT_SIZE = 8;

Matrix matrixFrom1DArray(int *array, mat_size_type size) {
	Matrix m(size, Row(size));

	for (mat_size_type j = 0; j < size; j++) {
		for (row_size_type i = 0; i < size; i++) {
			m[j][i] = array[size * j + i];
		}
	}

	return m;
}

Matrix generateSquareMatrix(row_size_type size) {
	Matrix m(size, Row(size));
	for (mat_size_type i = 0; i < size; ++i) {
		for (row_size_type j = 0; j < size; j++) {
			m[i][j] = rand() % 100;
		}
	}

	return m;
}

Matrix transpose(const Matrix& m) {
	Matrix t(m[0].size(), Row(m.size()));

	for (mat_size_type y = 0; y < m.size(); ++y) {
		for (row_size_type x = 0; x < m[0].size(); ++x) {
			t[x][y] = m[y][x];
		}
	}

	return t;
}

void printMat(const Matrix& m) {
	std::string display;
	
	for (Row row: m) {
		for (int elt: row) {
			display.append(std::to_string(elt) + " ");
		}
		display.append("\n");
	}

	std::cout << display << std::endl;
}

int process(Row a, Col b) {
	int res = 0;
	for (row_size_type i = 0; i < a.size(); ++i) {
		res += a[i] * b[i];
	}

	return res;
}

int main(void) {
	srand(time(NULL));
	Matrix a = generateSquareMatrix(MAT_SIZE);
	Matrix b = generateSquareMatrix(MAT_SIZE);
	Matrix t_b = transpose(b);
	Row resultRowPerRank(MAT_SIZE);
	int gather_recv[MAT_SIZE * MAT_SIZE];

	int world_rank;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == 0) {
		std::cout << "A = \n";
		printMat(a);
		std::cout << "B = \n";
		printMat(b);
	}

	for (row_size_type idx = 0; idx < a.size(); ++idx) {
		resultRowPerRank[idx] = process(a[world_rank], t_b[idx]);
	}

	MPI_Gather(resultRowPerRank.data(), MAT_SIZE, MPI_INT, gather_recv, MAT_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	if (world_rank == 0) {
		Matrix collect = matrixFrom1DArray(gather_recv, MAT_SIZE);

		std::cout << "A * B = \n";
		printMat(collect);
	}

	MPI_Finalize();

	return 0;
}

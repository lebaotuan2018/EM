// Lê Bảo Tuân - 1512627
#include<iostream>
#include"In.h"
using namespace std;


// PI = 3.14
// Z_MAX = 9.0
void main() 
{
	float **measurement;
	int num_measurement = 500; // Tổng số phần tử của measurement
	float z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short;
	// Khởi tạo các giá trị cho các tham số
	z_hit = 0.4;
	z_short = 0.4;
	z_rand = 0.1;
	z_max = 0.1;
	sigma_hit = 3;
	lambda_short = 0.8;

	// Tạo dữ liệu Measuremet (Z)
	Create_Data_Measurement(0.5, 0.3, 0.15, 0.15, measurement, num_measurement);
	// Tính các tham số với dữ liệu Measuremet (Z) mới
	EM(z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short, num_measurement, measurement);

	cout << "z_hit = " << z_hit << endl;
	cout << "z_short = " << z_short << endl;
	cout << "z_rand = " << z_rand << endl;
	cout << "z_max = " << z_max << endl;
	cout << "sigma_hit = " << sigma_hit << endl;
	cout << "lamda_short = " << lambda_short << endl << endl;
	system("pause");
}

/*
Hàm tạo dữ liệu measurements
Input:
	z_hit, z_short, z_rand, z_max
	Z: Khởi tạo dữ liệu measurements, chứa các zi
	num_Z: tống số phần tử của Z
Output:
	Dữ liệu map
*/
void Create_Data_Measurement(float z_hit, float z_short, float z_rand, float z_max, float** &Z, int num_Z)
{
	Z = new float*[num_Z];
	for (int i = 0; i < num_Z; i++) 
	{
		Z[i] = new float[2];
	}
	srand(time(NULL));
	int num_z_hit = num_Z * z_hit / (z_hit + z_short + z_rand + z_max);
	int num_z_short = num_Z * z_short / (z_hit + z_short + z_rand + z_max);
	int num_z_rand = num_Z * z_rand / (z_hit + z_short + z_rand + z_max);
	int num_z_max = num_Z - num_z_hit - num_z_short - num_z_rand;

	for (int i = 0; i < num_z_hit; i++) 
	{
		Z[i][0] = (rand() / (float)RAND_MAX)*(9 - 3) + 1;// 9 - 3: zmax , có thể thay 3 bằng số khác.
		Z[i][1] = Z[i][0] + (rand() / (float)RAND_MAX - 0.5f);
	}
	for (int i = num_z_hit; i < num_z_hit + num_z_rand; i++) 
	{
		Z[i][0] = (rand() / (float)RAND_MAX)*(9 - 3) + 1;
		Z[i][1] = Z[i][0];
	}
	for (int i = num_z_hit + num_z_rand; i < num_z_hit + num_z_rand + num_z_max; i++) 
	{
		Z[i][0] = 9.0;
		Z[i][1] = 9.0;
	}
	for (int i = num_z_hit + num_z_rand + num_z_max; i < num_Z; i++) 
	{
		Z[i][0] = (rand() / (float)RAND_MAX)*(9 - 3) + 1;
		Z[i][1] = Z[i][0] + (rand() / (float)RAND_MAX);
	}
}


/*
Hàm điều kiện của dừng của EM
*/
bool Done(float z_hit, float z_short, float z_rand, float z_max, float sigma_hit, float lambda_short, float prev_z_hit, float prev_z_short, float prev_z_rand, float prev_z_max, float prev_sigma_hit, float prev_lambda_short) 
{
	float max = abs(prev_z_hit - z_hit);
	(abs(prev_z_short - z_short) > max) ? max = abs(prev_z_short - z_short) : 0;
	(abs(prev_z_rand - z_rand) > max) ? max = abs(prev_z_rand - z_rand) : 0;
	(abs(prev_z_max - z_max) > max) ? max = abs(prev_z_max - z_max) : 0;
	(abs(prev_sigma_hit - sigma_hit) > max) ? max = abs(prev_sigma_hit - sigma_hit) : 0;
	(abs(prev_lambda_short - lambda_short) > max) ? max = abs(prev_lambda_short - lambda_short) : 0;
	return (max < 0.00001) ? true : false;
}


/*
Hàm tính các giá trị z (z_hit, z_short, z_rand, z_max)
Input:
	e: Dữ liệu e
	num_m: Tống số dữ liệu measurements (Z)
	type: Loại
		0: Tính z_hit
		1: Tính z_short
		2: Tính z_max
		3: Tính z_rand
Output:
	Các giá trị z_hit, z_short, z_rand, z_max tương ứng với type (0, 1, 2, 3)
*/
float Compute_z(float** e, int num_m, int type) 
{
	float z = 0;
	for (int i = 0; i < num_m; i++) 
	{
		z += e[i][type];
	}
	return z / num_m;
}


/*
Hàm tính Sigma Hit
Input:
	e: Dữ liệu e(i,hit).
	Z: Dữ liệu measurements, chứa các zi
	num_m: Tống số dữ liệu
Output:
	Sigma Hit
*/
float Compute_Sigma_Hit(float** e, float** Z, int num_m) 
{
	float sigma;
	float n = 0, m = 0;

	for (int i = 0; i < num_m; i++) 
	{
		// Tính tống các e_i_hit
		n = n + e[i][0];
		if (Z[i][1] != 0 && Z[i][1] != 1.0)
		{
			// Tổng các tích của e_i_hit và (bình phương hiệu giữa z_i và z_i_*)
			m = m + (e[i][0] * pow(Z[i][0] - Z[i][1], 2));
		}
	}
	// Tính sigma hit
	sigma = pow(m / n, 0.5);
	return sigma;
}


/*
Hàm tính Lambda Short
Input:
	e: Dữ liệu e(i,hit).
	Z: Dữ liệu measuremets, chứa các zi
	num_m: Tống số dữ liệu measurements (Z)
Output:
	Lambda Short
*/

float Compute_Lambda_Short(float **e, float **Z, int num_m)
{
	float n = 0;
	float m = 0;
	float lambda;
	for (int i = 0; i < num_m; i++) 
	{
		// Tính tống các e_i_short
		n = n + e[i][1];
		// Tính tống các tích (e_i_short và z_i)
		m = m + (e[i][1] * Z[i][0]);
	}
	// Tính lambda short
	lambda = n / m;
	return lambda;
}



/*
Hàm EM
Input:
	z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short (Giá trị khởi tạo ban đầu)
	num_m: Tống số dữ liệu measurements (Z)
	Z: Dữ liệu measuremets, chứa các zi
Output:
	z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short (Đã được tính lại)
*/
void EM(float& z_hit, float& z_short, float& z_rand, float& z_max, float& sigma_hit, float& lambda_short, int num_m, float** Z) 
{
	float prev_z_hit;
	float prev_z_short;
	float prev_z_rand;
	float prev_z_max;
	float prev_sigma_hit;
	float prev_lambda_short;
	float** e = new float*[num_m]; // Chứa e_hit, e_short, e_max, e_rand cho từng zi
	for (int i = 0; i < num_m; i++)
		e[i] = new float[4];

	do 
	{
		prev_z_hit = z_hit;
		prev_z_short = z_short;
		prev_z_rand = z_rand;
		prev_z_max = z_max;
		prev_sigma_hit = sigma_hit;
		prev_lambda_short = lambda_short;

		// Vòng for tính các giá trị e_(i,hit); e_(i,short);e_(i,max);e_(i,rand) với từng zi
		for (int i = 0; i < num_m; i++) 
		{
			float somu = -0.5*pow(Z[i][0] - Z[i][1], 2) / pow(sigma_hit, 2);
			float pmax, pshort;
			(Z[i][1] == 9.0) ? pmax = 1 : pmax = 0;
			(Z[i][0] < Z[i][1]) ? pshort = lambda_short * pow(2.72, -lambda_short * Z[i][0]) : pshort = 0;
			// Tính eta
			float eta = 1 / (1.0 / (pow(2 * 3.14*pow(sigma_hit, 2), 0.5))*pow(2.72, somu) + pshort + 1.0 / 9 + pmax);

			e[i][0] = eta * 1.0 / (pow(2 * 3.14*pow(sigma_hit, 2), 0.5))*pow(2.72, somu); // Tính e_i_hit
			e[i][1] = eta * pshort; // Tính e_i_short
			e[i][2] = eta * pmax; // Tính e_i_max
			e[i][3] = eta * 1.0 / 9; // Tính e_i_rand
		}

		// Tính z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short với dữ liệu measuremets (Z) mới được khởi tạo
		z_hit = Compute_z(e, num_m, 0);
		z_short = Compute_z(e, num_m, 1);
		z_max = Compute_z(e, num_m, 2);
		z_rand = Compute_z(e, num_m, 3);
		sigma_hit = Compute_Sigma_Hit(e, Z, num_m);
		lambda_short = Compute_Lambda_Short(e, Z, num_m);
	} while (!Done(prev_z_hit, prev_z_short, prev_z_rand, prev_z_max, prev_sigma_hit, prev_lambda_short, z_hit, z_short, z_rand, z_max, sigma_hit, lambda_short));
}

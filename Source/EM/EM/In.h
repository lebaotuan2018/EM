// Lê Bảo Tuân - 1512627
#pragma once
#include<iostream>
#include<time.h>

using namespace std;

bool Done(float z_hit, float z_short, float z_rand, float z_max, float sigma,
	float lambda, float prev_zhit, float prev_zshort, float prev_zrand, float prev_zmax,
	float prev_s, float prev_l);

void Create_Data_Measurement(float z_hit, float z_short, float z_rand, float z_max, float** &Z, int num_Z);
float Compute_Lambda_Short(float **e, float **Z, int num_m);
float Compute_Sigma_Hit(float** e, float** Z, int num_m);
float Compute_z(float** e, int num_m, int type);

void EM(float& z_hit, float& z_short, float& z_rand, float& z_max, float& sigma_hit, float& lambda_short, int num_m, float** Z);
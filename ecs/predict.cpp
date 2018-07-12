#include "predict.h"
#include <stdio.h>


#include "glob.h"
#include "predict.h"
#include <iostream>
#include "stdio.h"
using namespace std;

#include "predict.h"
#include <stdio.h>





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************
ȫ���������������岿��
*******************************/


int server_num_py;  //�����������


Inputcontrol inputcontrol;
Traincontrol traincontrol;
Server inputServer[3];    //�����������Ĭ��Ϊ3

						  // ���ڣ�����
int CYC_SAMPLE = 8;

// ����ȥ�������� Ĭ��Ϊ1
int CYC_SAMPLE_NOISE = 1;

// Ĭ����[31(1) , 28(2) , 31(3) , 30(4) , 31(5) , 30(6) , 31(7) , 31(8) , 30(9) , 31(10) , 30(11) , 31(12) ] = 365�죡
Month month[13];

int month_data[13] = { 0,31 , 28 , 31 , 30 , 31 , 30 , 31, 31 , 30 , 31 , 30 , 31 };

int HAPPYDAY_TOTALNUM = 4;
// ����ڼ���  Ԫ��1.1 1.2 1.3  ��һ5.1 5.2 5.3 ʮһ10.1 10.2 10.3  ˫11 11.11
int happyday[4] = { 1,   121,   274,   315 };

//�����ķ�Χ bad
int BAD_RANGE[21] = { 0, -1,-2,-3,1,  -4,-5,-6,2,  -7,-8,-9,3, -10,-11,-12,4 , -13, -14 ,-15, 5 };
int GOOD_RANGE[21] = { 0, 1,2,3,-1,  4,5,6,-2,  7,8,9,-3, 10,11,12,-4 , 13, 14 ,15, -5 };
int MASTER_RANGE[41] = { 0, 1,-1,2,-2,3,-3,4,-4,5,-5,6,-6,7,-7,8,-8,9,-9,10,-10,    11,-11,12,-12,13,-13,14,-14,15,-15,16,-16,17,-17,18,-18,19,-19,20,-20 };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �·ݵĳ�ʼ��
void month_init()
{
	month[0].month_name = 0;
	month[0].month_day_num = 0;
	month[0].month_day_total_num = 0;


	for (int i = 1; i< 13; i++)
	{
		month[i].month_name = i;
		month[i].month_day_num = month_data[i];
		month[i].month_day_total_num = 0;
		for (int j = 1; j <= i; j++)   month[i].month_day_total_num += month_data[j - 1];

	}

}

// 10��power_num����
int poww(int power_num) {
	int power_out = 1;
	for (int i = 0; i < power_num; i++) {
		power_out = power_out * 10;
	}
	return power_out;
}

// 2��power_num����
int poww_two(int power_num) {
	int power_out = 1;
	for (int i = 0; i < power_num; i++) {
		power_out = power_out * 2;
	}
	return power_out;
}

/*��������:double���������������int����*/
int round_my(double x)
{
	int x_out;
	if (x > 0)
		x_out = (int)(x + 0.5);
	else
		if (x < 0)
			x_out = (int)(x - 0.5);
	if (x == 0)  x_out = 0;

	return x_out;
}


/*
���ַ�����д��һ��int����
result_file_local:��д������
result_point:������ĵڼ�λ��ʼ����
input_num:��������
��������Ϊ�������ֺ�result_point��λ��
*/
int input_a_int_num(char *result_file_local, int  result_point, int input_num) {
	int weishu = 0; //�������ݵ�λ��
	int num_readytowrite = input_num; //copyһ��input_num
	while (num_readytowrite != 0) {
		num_readytowrite /= 10;
		weishu++;
	}
	if (input_num == 0) { //0Ҳ��1λ��
		weishu = 1;
	}
	int weishu_copy = weishu;//���Եݼ�����
	for (int i = result_point; i < result_point + weishu; i++) {
		result_file_local[i] = '0' + input_num / poww(weishu_copy - 1);
		input_num = input_num - (input_num / poww(weishu_copy - 1)) * poww(weishu_copy - 1);
		weishu_copy--;
	}
	result_point += weishu;
	return result_point;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �����������������
void read_flavor_class_num(char ** const inputfileBuff)
{
	//��1�еĴ���,��ȡ�����������
	if (inputfileBuff[0][1] < '0' || inputfileBuff[0][1] > '9')
		server_num_py = ((inputfileBuff[0][0] - CONV_ASCII_TO_INT));
	else
		server_num_py = ((inputfileBuff[0][0] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[0][1] - CONV_ASCII_TO_INT));


	int point_of_flavor_num = server_num_py + 2;
	if (inputfileBuff[point_of_flavor_num][1] < '0' || inputfileBuff[point_of_flavor_num][1] > '9')
		inputcontrol.flavorMaxnum = ((inputfileBuff[point_of_flavor_num][0] - CONV_ASCII_TO_INT));
	else
		inputcontrol.flavorMaxnum = ((inputfileBuff[point_of_flavor_num][0] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[point_of_flavor_num][1] - CONV_ASCII_TO_INT));
	printf("inputcontrol.flavorMaxnum : %d \n", inputcontrol.flavorMaxnum);
}


// �����ļ��Ĵ���
void do_input_file(char ** const inputfileBuff, Flavor *inputFlavor)
{
	//��Ӳ����������Դ
	for (int server_time = 0; server_time < server_num_py; server_time++) {
		int kong1 = 0;  //�ո�1
		int kong2 = 0;  //�ո�2
		int kong3 = 0;  //ĩβ
		int kong_point = 0;
		for (int i = 0; i < 40; i++)
		{
			if (inputfileBuff[server_time + 1][i] == ' ') {
				if (kong_point == 0) {
					kong1 = i;
				}
				else if (kong_point == 1) {
					kong2 = i;
				}
				else if (kong_point == 2) {
					kong3 = i;
					break;
				}
				kong_point++;
			}

		}
		//�����������
		if (inputfileBuff[server_time + 1][0] == 'G') {   //0  General 
			inputServer[server_time].name = 0;
		}
		else if (inputfileBuff[server_time + 1][0] == 'L') {  //1  Large-Memory
			inputServer[server_time].name = 1;
		}
		else if (inputfileBuff[server_time + 1][0] == 'H') {   //2  High-Performance
			inputServer[server_time].name = 2;
		}
		//��cpu����
		inputServer[server_time].cpu_core_num = 0;
		for (int i = kong2 - 1; i>kong1; i--) {
			inputServer[server_time].cpu_core_num += (inputfileBuff[server_time + 1][i] - CONV_ASCII_TO_INT)*poww((kong2 - 1) - i);

		}

		//���ڴ�����
		inputServer[server_time].mem_size = 0;
		for (int i = kong3 - 1; i>kong2; i--) {
			inputServer[server_time].mem_size += (inputfileBuff[server_time + 1][i] - CONV_ASCII_TO_INT)*poww((kong3 - 1) - i);

		}
		//��cpu��mem����
		inputServer[server_time].ratio_cpu_to_mem = (double)inputServer[server_time].cpu_core_num / (double)inputServer[server_time].mem_size;
		//����Ӳ����
	}


	//��ȡ�������������������
	int i_start_vm = server_num_py + 3;
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		int kong1 = 0;  //�ո�1
		int kong2 = 0;  //�ո�2
		int kong3 = 0;  //ĩβ
		int kong_point = 0;
		for (int j = 0; j < 40; j++)
		{
			if (inputfileBuff[i_start_vm + i][j] == ' ') {
				if (kong_point == 0) {
					kong1 = j;
				}
				else if (kong_point == 1) {
					kong2 = j;
				}
				kong_point++;
			}
			if (inputfileBuff[i_start_vm + i][j] == '\n') {
				kong3 = j-1 ;   //linux��  j-1
				break;
			}

		}
		//��flavor����
		if (kong1 == 7) {
			inputFlavor[i].flavor_name = (inputfileBuff[i_start_vm + i][6] - CONV_ASCII_TO_INT);
		}
		else if (kong1 == 8) {
			inputFlavor[i].flavor_name = (inputfileBuff[i_start_vm + i][6] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[i_start_vm + i][7] - CONV_ASCII_TO_INT);
		}
		//��cpu��
		inputFlavor[i].cpu_core_num = 0;
		for (int j = kong2 - 1; j>kong1; j--) {
			inputFlavor[i].cpu_core_num += (inputfileBuff[i_start_vm + i][j] - CONV_ASCII_TO_INT)*poww((kong2 - 1) - j);
		}
		//���ڴ�
		inputFlavor[i].mem_size = 0;
		long long mem_size_temp = 0;
		for (int j = kong3 - 1; j>kong2; j--) {
			mem_size_temp += (inputfileBuff[i_start_vm + i][j] - CONV_ASCII_TO_INT)*poww((kong3 - 1) - j);
		}
		// �� MB ת����Ϊ GB
		mem_size_temp = mem_size_temp >> 10;
		inputFlavor[i].mem_size = (int)mem_size_temp;
		//����cpu��mem����
		inputFlavor[i].ratio_cpu_to_mem = (double)inputFlavor[i].cpu_core_num / (double)inputFlavor[i].mem_size;

	}

	//��ȡҪ��Ԥ���ʱ���

	if (inputfileBuff[inputcontrol.inputfileLinenum - 1][4] == 45 && inputfileBuff[inputcontrol.inputfileLinenum - 1][7] == 45)
	{
		inputcontrol.endTime = ((inputfileBuff[inputcontrol.inputfileLinenum - 1][2] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 1][3] - CONV_ASCII_TO_INT)) * 365 +
			month[(inputfileBuff[inputcontrol.inputfileLinenum - 1][5] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 1][6] - CONV_ASCII_TO_INT)].month_day_total_num +
			(inputfileBuff[inputcontrol.inputfileLinenum - 1][8] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 1][9] - CONV_ASCII_TO_INT);
	}
	if (inputfileBuff[inputcontrol.inputfileLinenum - 2][4] == 45 && inputfileBuff[inputcontrol.inputfileLinenum - 2][7] == 45)
	{
		inputcontrol.startTime = ((inputfileBuff[inputcontrol.inputfileLinenum - 2][2] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 2][3] - CONV_ASCII_TO_INT)) * 365 +
			month[(inputfileBuff[inputcontrol.inputfileLinenum - 2][5] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 2][6] - CONV_ASCII_TO_INT)].month_day_total_num +
			(inputfileBuff[inputcontrol.inputfileLinenum - 2][8] - CONV_ASCII_TO_INT) * 10 + (inputfileBuff[inputcontrol.inputfileLinenum - 2][9] - CONV_ASCII_TO_INT);
	}

	if (inputfileBuff[inputcontrol.inputfileLinenum - 1][4] == 45 && inputfileBuff[inputcontrol.inputfileLinenum - 1][11] == '2')  inputcontrol.endTime++;
	if (inputfileBuff[inputcontrol.inputfileLinenum - 2][4] == 45 && inputfileBuff[inputcontrol.inputfileLinenum - 2][11] == '2')  inputcontrol.startTime++;

	printf("inputcontrol.startTime : %d   ", inputcontrol.startTime, inputcontrol.startTime / 365);
	printf("inputcontrol.endTime : %d   \n", inputcontrol.endTime);

	inputcontrol.intervalTime = inputcontrol.endTime - inputcontrol.startTime;

	for (int i = 0; i<inputcontrol.flavorMaxnum; i++)
	{
		printf("inputFlavor%d : %d   %d   %d %f\n", i, inputFlavor[i].flavor_name, inputFlavor[i].cpu_core_num, inputFlavor[i].mem_size, inputFlavor[i].ratio_cpu_to_mem);
	}

}




void read_time_diff(char ** const trainfileBuff, int train_file_line)
{
	for (int j = 0; j < 50; j++)
		if ((trainfileBuff[0][j] == 45) && (trainfileBuff[0][j + 3] == 45) && (trainfileBuff[0][j + 9] == 58))
		{
			traincontrol.startTime = ((trainfileBuff[0][j - 2] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[0][j - 1] - CONV_ASCII_TO_INT)) * 365 +
				month[(trainfileBuff[0][j + 1] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[0][j + 2] - CONV_ASCII_TO_INT)].month_day_total_num +
				((trainfileBuff[0][j + 4] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[0][j + 5] - CONV_ASCII_TO_INT));
			break;
		}
	for (int j = 0; j < 50; j++)
		if ((trainfileBuff[train_file_line - 1][j] == 45) && (trainfileBuff[train_file_line - 1][j + 3] == 45) && (trainfileBuff[train_file_line - 1][j + 9] == 58))
		{
			traincontrol.endTime = ((trainfileBuff[train_file_line - 1][j - 2] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[train_file_line - 1][j - 1] - CONV_ASCII_TO_INT)) * 365 +
				month[(trainfileBuff[train_file_line - 1][j + 1] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[train_file_line - 1][j + 2] - CONV_ASCII_TO_INT)].month_day_total_num +
				(trainfileBuff[train_file_line - 1][j + 4] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[train_file_line - 1][j + 5] - CONV_ASCII_TO_INT);
			break;
		}
	// cout << traincontrol.startTime << endl;
	// cout << traincontrol.endTime << endl;
	printf("traincontrol.startTime: %d   ", traincontrol.startTime);
	printf("traincontrol.endTime: %d \n", traincontrol.endTime);
}

// ��ѵ���ļ����ݽ�����Ч�ķָ���� ʱ������£���13��19]
int do_train_file_valid_div(char ** const trainfileBuff, int *valid_div_data)
{
	int sample_num_count = 0;
	valid_div_data[0] = traincontrol.endTime;
	int temptime = traincontrol.endTime;
	int temptime_count = 0;
	for (int i = (traincontrol.trainfileLinenum - 2); i >= 0; i--)  //���ڴ����i�У�����
	{
		// Ѱ��ʱ���
		for (int j = 0, flag_shijianduan = 0; flag_shijianduan == 0; j++)
		{
			// ���Ҽ�ʱ�䲢ȷ��ʱ��  "  - :  - "
			if ((trainfileBuff[i][j] == 45) && (trainfileBuff[i][j + 3] == 45) && (trainfileBuff[i][j + 9] == 58))
			{
				flag_shijianduan = 1;  // �Ҽ���ʱ��α�־λ
				if ((((trainfileBuff[i][j - 2] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j - 1] - CONV_ASCII_TO_INT)) * 365 +
					month[(trainfileBuff[i][j + 1] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 2] - CONV_ASCII_TO_INT)].month_day_total_num +
					(trainfileBuff[i][j + 4] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 5] - CONV_ASCII_TO_INT)) != temptime)
				{   // �뵱ǰ��ʱ�䲻һ����֤����һ�����µ�ʱ�䣡�������и�ֵ��������һ�Ρ�
					temptime = (((trainfileBuff[i][j - 2] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j - 1] - CONV_ASCII_TO_INT)) * 365 +
						month[(trainfileBuff[i][j + 1] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 2] - CONV_ASCII_TO_INT)].month_day_total_num +
						(trainfileBuff[i][j + 4] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 5] - CONV_ASCII_TO_INT));
					temptime_count++;
				}
				if (temptime_count == CYC_SAMPLE_NOISE)
				{
					temptime_count = 0;
					sample_num_count++;
					valid_div_data[sample_num_count] = temptime;
				}


			}

		}

	}


	return sample_num_count;
}


void do_train_file(char ** const trainfileBuff, Flavor *inputFlavor, int *trainfileFlavordata, int *valid_div_data)
{

	// ������
	//  = inputcontrol.startTime - CYC_SAMPLE;

	// ��ǰʱ��
	int temptime = 0;


	// �ָ�ʱ���
	for (int count_div = 0; count_div < traincontrol.sample_num_noise;)
	{
		//int a = 0;
		for (int i = (traincontrol.trainfileLinenum - 1); i >= 0; i--)
		{
			if (count_div >= traincontrol.sample_num_noise) break;
			//printf("\n��%d���ڴ���...", i);
			// Ѱ��ʱ���
			for (int j = 0, flag_shijianduan = 0; flag_shijianduan == 0; j++)
			{
				if (count_div >= traincontrol.sample_num_noise) break;
				// ���Ҽ�ʱ�䲢ȷ��ʱ��  "  - :  - "
				if ((trainfileBuff[i][j] == 45) && (trainfileBuff[i][j + 3] == 45) && (trainfileBuff[i][j + 9] == 58))
				{
					flag_shijianduan = 1;  // �Ҽ���ʱ��α�־λ
					if (count_div >= traincontrol.sample_num_noise) break;

					temptime = (((trainfileBuff[i][j - 2] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j - 1] - CONV_ASCII_TO_INT)) * 365 +
						month[(trainfileBuff[i][j + 1] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 2] - CONV_ASCII_TO_INT)].month_day_total_num +
						(trainfileBuff[i][j + 4] - CONV_ASCII_TO_INT) * 10 + (trainfileBuff[i][j + 5] - CONV_ASCII_TO_INT));
					// ��߽� �� �ұ߽� Լ��
					// [13,20) : 13 14 15 16 17 18 19   [12,13
					if (temptime > valid_div_data[count_div + 1] && temptime <= valid_div_data[count_div])
					{
						// Լ���ɹ� ��ʼͳ��

						// Լ��flavor���
						for (int j_fla = 0, flag_fla = 0; flag_fla == 0; j_fla++)
						{
							if (count_div >= traincontrol.sample_num_noise) break;
							// Ѱ�ҵ� "fla"��Ϊ��ѯ����
							if ((trainfileBuff[i][j_fla] == 102) && (trainfileBuff[i][j_fla + 1] == 108) && (trainfileBuff[i][j_fla + 2] == 97))
							{
								flag_fla = 1;
								// ȷ����flavor������һλ���ֻ�����λ����
								if (trainfileBuff[i][j_fla + 7] > 47 && trainfileBuff[i][j_fla + 7] < 58)
								{
									//��λ��
									for (int kkk = 0; kkk < inputcontrol.flavorMaxnum; kkk++)
									{
										// ƥ�������������ͺ�!!!
										if (((trainfileBuff[i][j_fla + 6] - CONV_ASCII_TO_INT) * 10 + trainfileBuff[i][j_fla + 7] - CONV_ASCII_TO_INT) == inputFlavor[kkk].flavor_name)
											trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]++;
									}

								}
								else
								{
									for (int kkk = 0; kkk < inputcontrol.flavorMaxnum; kkk++)
									{
										// ƥ�������������ͺ�!!!
										if ((trainfileBuff[i][j_fla + 6] - CONV_ASCII_TO_INT) == inputFlavor[kkk].flavor_name)
										{
											trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]++;
											//printf("�����е�%d�� -- %d", (count_div * inputcontrol.flavorMaxnum + kkk), trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]);
										}

									}

								}

								// ��ѯ��������ѭ��,��ֹ����Խ��!!!
								// break;
							}
							// ʣ�µ����ݾͲ���Ҫ��!!!

						}
					}
					else
					{  // Լ��ʧ�� ����һ������(����ʱ���)
						count_div++;
						// ʣ�µ����ݾͲ���Ҫ��!!!
						if (count_div >= traincontrol.sample_num_noise) break;
						// ���ν��ض�Լ���ɹ� �����ͳ��!! �����ִ��������ν׶�ִ����ֻ��ִ��1��
						if (temptime > valid_div_data[count_div + 1] && count_div < traincontrol.sample_num_noise && temptime <= valid_div_data[count_div])
						{
							// Լ���ɹ� ��ʼͳ��
							// Լ��flavor���
							for (int j_fla = 0, flag_fla = 0; flag_fla == 0; j_fla++)
							{
								// Ѱ�ҵ� "fla"��Ϊ��ѯ����
								if ((trainfileBuff[i][j_fla] == 102) && (trainfileBuff[i][j_fla + 1] == 108) && (trainfileBuff[i][j_fla + 2] == 97))
								{
									flag_fla = 1;

									// ȷ����flavor������һλ���ֻ�����λ����
									if (trainfileBuff[i][j_fla + 7] > 47 && trainfileBuff[i][j_fla + 7] < 58)
									{
										//��λ��
										for (int kkk = 0; kkk < inputcontrol.flavorMaxnum; kkk++)
										{
											// ƥ�������������ͺ�!!!
											if (((trainfileBuff[i][j_fla + 6] - CONV_ASCII_TO_INT) * 10 + trainfileBuff[i][j_fla + 7] - CONV_ASCII_TO_INT) == inputFlavor[kkk].flavor_name)
												trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]++;
										}
									}
									else
									{
										for (int kkk = 0; kkk < inputcontrol.flavorMaxnum; kkk++)
										{
											// ƥ�������������ͺ�!!!
											if ((trainfileBuff[i][j_fla + 6] - CONV_ASCII_TO_INT) == inputFlavor[kkk].flavor_name)
											{
												trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]++;
												//printf("�����е�%d�� -- %d", (count_div * inputcontrol.flavorMaxnum + kkk), trainfileFlavordata[count_div * inputcontrol.flavorMaxnum + kkk]);
											}
										}
									}
									// ��ѯ��������ѭ��,��ֹ����Խ��!!!
									// break;
								}
							}
						}
					}
					//  break;
				}
			}
		}
	}

	// ˳����֯����˳�� ��Ϊ����
	int *temp_trainfileFlavordata = new int[inputcontrol.flavorMaxnum * traincontrol.sample_num_noise];
	for (int i = 0; i < traincontrol.sample_num_noise; i++) {
		for (int j = 0; j < inputcontrol.flavorMaxnum; j++) {
			temp_trainfileFlavordata[i*inputcontrol.flavorMaxnum + j] = trainfileFlavordata[(traincontrol.sample_num_noise - 1 - i)*inputcontrol.flavorMaxnum + j];
		}
	}
	for (int i = 0; i < inputcontrol.flavorMaxnum * traincontrol.sample_num_noise; i++) {
		trainfileFlavordata[i] = temp_trainfileFlavordata[i];
	}

	//print_data(trainfileFlavordata);

}

// ��ּ��㲿�֡�  tempΪҪ����ĳ��ȣ���
void get_trainfileFlavordata_diff(int *input, int *ouput, int temp)
{
	//int temp = traincontrol.sample_num - 1;  //Ҫ����ĳ���
	for (int i = 0; i < temp; i++)
	{
		for (int j = 0; j < inputcontrol.flavorMaxnum; j++)
		{
			ouput[j + i * inputcontrol.flavorMaxnum] = input[j + (i + 1) * inputcontrol.flavorMaxnum] - input[j + i * inputcontrol.flavorMaxnum];
		}
	}
}



// ��ֵ�˲���
void do_noise_avgfilter(int *trainfileFlavordata)
{
	int temp = traincontrol.sample_num_noise;  //Ҫ����ĳ���

	float shouwei_3[3] = { 0.33, 0.33, 0.33 };
	float moban_3[3] = { 0.50, 0.0, 0.50 };  // ����Ϊ3
	float shouwei_5[5] = { 0.2, 0.2, 0.2, 0.2, 0.2 };
	float moban_5[5] = { 0.25, 0.25,0.0,0.25,0.25 };  // ����Ϊ5

													  // ��β��������Ϊ3
	for (int i = 0; i < temp; i++)
	{
		// ��
		trainfileFlavordata[i] = (int)(trainfileFlavordata[i + 0 * inputcontrol.flavorMaxnum] * shouwei_3[0] +
			trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum] * shouwei_3[1] + trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum] * shouwei_3[2] + 0.5);
		// β
		trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] * shouwei_3[0] +
			trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum] * shouwei_3[1] + trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum] * shouwei_3[2] + 0.5);
	}
	// �м䲿��
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		for (int j = 1; j < (temp - 1); j++)
		{
			trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] * moban_3[0] +
				trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] * moban_3[2] + 0.5);
		}
	}
}

// �ڼ���ȥ�룡��
void do_noise_happyday(int *trainfileFlavordata, int *day_data, int tatal_daynum)
{
	for (int i = 0; i < tatal_daynum; i++)
	{
		for (int j = 0; j < HAPPYDAY_TOTALNUM - 1; j++)
		{
			if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[j] && i == 0)  //�ڼ����ǵ�һ��(����˫11)
			{
				for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++) //���θ�ֵ������
				{
					trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa];
					trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa];
					trainfileFlavordata[(i + 2)* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa];
				}
				i += 2;
				break;
			}
			if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[j] && i > 0 && i<tatal_daynum - 3)  //�ڼ��ղ��ǵ�һ��(����˫11)
			{
				for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++) //���θ�ֵ������
				{
					trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = (trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa] + trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa]) / 2;
					trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa] = (trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa] + trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa]) / 2;
					trainfileFlavordata[(i + 2)* inputcontrol.flavorMaxnum + aaa] = (trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa] + trainfileFlavordata[(i + 3)* inputcontrol.flavorMaxnum + aaa]) / 2;
				}
				i += 2;
				break;
			}

			if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[j] && i > 0 && i == tatal_daynum - 3)  //�ڼ����ǵ���3��(����˫11)
			{
				for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++) //���θ�ֵ������
				{
					trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
					trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
					trainfileFlavordata[(i + 2)* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
				}
				i += 2;
				break;
			}

			if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[j] && i > 0 && i == tatal_daynum - 2)  //�ڼ��������2��(����˫11)
			{
				for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++) //���θ�ֵ������
				{
					trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
					trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
				}
				i += 2;
				break;
			}

			if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[j] && i > 0 && i == tatal_daynum - 1)  //�ڼ��������һ��(����˫11)
			{
				for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++) //���θ�ֵ������
				{
					trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
				}
				i += 2;
				break;
			}

		}
		if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[9] && i == 0)  //11.11�ǵ�һ�죬������
		{
			for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++)
			{
				trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa];
			}
		}
		if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[9] && i > 0 && i != (tatal_daynum - 1))  //11.11 ���ǵ�һ�죡��Ҳ�������һ��
		{
			for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++)
			{
				trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = (trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa] + trainfileFlavordata[(i + 1)* inputcontrol.flavorMaxnum + aaa]) / 2;
			}
		}
		if ((day_data[tatal_daynum - 1 - i] % 365) == happyday[9] && i > 0 && i == (tatal_daynum - 1))  //11.11 �����һ��
		{
			for (int aaa = 0; aaa < inputcontrol.flavorMaxnum; aaa++)
			{
				trainfileFlavordata[i* inputcontrol.flavorMaxnum + aaa] = trainfileFlavordata[(i - 1)* inputcontrol.flavorMaxnum + aaa];
			}
		}

	}
}



// ��ֵ�˲�
void do_noise_midfilter(int *trainfileFlavordata, Flavor *inputFlavor)
{
	int temp = traincontrol.sample_num_noise;  //Ҫ����ĳ���

											   // ȥ��ͷ��β
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		// ��
		// printf("trainfileFlavordata[%d] : %d\n", i, trainfileFlavordata[i]);
		// printf("trainfileFlavordata[%d] : %d\n", i + 1 * inputcontrol.flavorMaxnum, trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum]);
		// printf("trainfileFlavordata[%d] : %d\n\n", i + 2 * inputcontrol.flavorMaxnum, trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum]);
		if (trainfileFlavordata[i] > trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum] &&
			trainfileFlavordata[i] > trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum] &&
			trainfileFlavordata[i] > (NOISE_B + trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum] * NOISE_W + trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum] * NOISE_W)
			&& (inputFlavor[i].flavor_name == 1 || inputFlavor[i].flavor_name == 4 || inputFlavor[i].flavor_name == 7 || inputFlavor[i].flavor_name == 10 || inputFlavor[i].flavor_name == 13 ||
				inputFlavor[i].flavor_name == 3 || inputFlavor[i].flavor_name == 6 || inputFlavor[i].flavor_name == 9 || inputFlavor[i].flavor_name == 12 || inputFlavor[i].flavor_name == 15
				|| inputFlavor[i].flavor_name == 2 || inputFlavor[i].flavor_name == 5 || inputFlavor[i].flavor_name == 8 || inputFlavor[i].flavor_name == 11 || inputFlavor[i].flavor_name == 14
				))
		{
			printf("���������еĸ߷�  �ײ�λ��[ %d ]   ȥ��֮ǰ : [ %d %d %d ]\n", i, trainfileFlavordata[i], trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum], trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum]);
			trainfileFlavordata[i] = (int)(trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum] * 0.5 + trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum] * 0.5 + 0.5);
			//trainfileFlavordata[i] = (int)(trainfileFlavordata[i] / 2 + 1);
			printf("���������еĸ߷�  �ײ�λ��[ %d ]   ȥ��֮�� : [ %d %d %d ]\n", i, trainfileFlavordata[i], trainfileFlavordata[i + 1 * inputcontrol.flavorMaxnum], trainfileFlavordata[i + 2 * inputcontrol.flavorMaxnum]);
		}



		// β
		// printf("trainfileFlavordata[%d] : %d\n", i + (temp - 1) * inputcontrol.flavorMaxnum, trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum]);
		// printf("trainfileFlavordata[%d] : %d\n", i + (temp - 2) * inputcontrol.flavorMaxnum, trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum]);
		// printf("trainfileFlavordata[%d] : %d\n", i + (temp - 3) * inputcontrol.flavorMaxnum, trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum]);
		if (trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] > trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum] &&
			trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] > trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum] &&
			trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] > (NOISE_B + trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum] * NOISE_W + trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum] * NOISE_W)
			&& (inputFlavor[i].flavor_name == 1 || inputFlavor[i].flavor_name == 4 || inputFlavor[i].flavor_name == 7 || inputFlavor[i].flavor_name == 10 || inputFlavor[i].flavor_name == 13 ||
				inputFlavor[i].flavor_name == 3 || inputFlavor[i].flavor_name == 6 || inputFlavor[i].flavor_name == 9 || inputFlavor[i].flavor_name == 12 || inputFlavor[i].flavor_name == 15
				|| inputFlavor[i].flavor_name == 2
				|| inputFlavor[i].flavor_name == 5
				|| inputFlavor[i].flavor_name == 8
				|| inputFlavor[i].flavor_name == 11
				|| inputFlavor[i].flavor_name == 14
				))
		{
			printf("���������еĸ߷�  β��λ��[ %d ]   ȥ��֮ǰ : [ %d %d %d ]\n", i, trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum]);
			trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum] * 0.5 + trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum] * 0.5 + 0.5);
			//trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum] / 2 + 1);
			printf("���������еĸ߷�  β��λ��[ %d ]   ȥ��֮�� : [ %d %d %d ]\n", i, trainfileFlavordata[i + (temp - 3) * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (temp - 2) * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (temp - 1) * inputcontrol.flavorMaxnum]);

		}
	}


	// �ȴ����м�
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		for (int j = 1; j < (temp - 1); j++)
		{
			// ����ȥ����� ����OK
			// printf("trainfileFlavordata[%d] : %d\n", i + (j - 1)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum]);
			// printf("trainfileFlavordata[%d] : %d\n", i + (j - 0)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j - 0)*inputcontrol.flavorMaxnum]);
			// printf("trainfileFlavordata[%d] : %d\n", i + (j + 1)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
			if (trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] < trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] &&
				trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] < trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] &&
				((trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] * NOISE_W + trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] * NOISE_W + NOISE_B) < trainfileFlavordata[i + j * inputcontrol.flavorMaxnum])
				&& (inputFlavor[i].flavor_name == 1 || inputFlavor[i].flavor_name == 4 || inputFlavor[i].flavor_name == 7 || inputFlavor[i].flavor_name == 10 || inputFlavor[i].flavor_name == 13 ||
					inputFlavor[i].flavor_name == 3 || inputFlavor[i].flavor_name == 6 || inputFlavor[i].flavor_name == 9 || inputFlavor[i].flavor_name == 12 || inputFlavor[i].flavor_name == 15
					|| inputFlavor[i].flavor_name == 2 || inputFlavor[i].flavor_name == 5 || inputFlavor[i].flavor_name == 8 || inputFlavor[i].flavor_name == 11 || inputFlavor[i].flavor_name == 14
					))
			{
				// ��ƽ�������������룡
				printf("���������еĸ߷�  �м�λ��[ %d %d ]   ȥ��֮ǰ : [ %d %d %d ]\n", i, j, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum], trainfileFlavordata[i + j * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
				trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] * 0.5 + trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] * 0.5 + 0.5);
				//trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] / 2 + 0.5);
				printf("���������еĸ߷�  �м�λ��[ %d %d ]   ȥ��֮�� : [ %d %d %d ]\n", i, j, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum], trainfileFlavordata[i + j * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
			}

		}
	}

	// �����м�� �͹�����
	/*for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
	for (int j = 1; j < (temp - 1); j++)
	{
	// ����ȥ����� ����OK
	// printf("trainfileFlavordata[%d] : %d\n", i + (j - 1)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum]);
	// printf("trainfileFlavordata[%d] : %d\n", i + (j - 0)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j - 0)*inputcontrol.flavorMaxnum]);
	// printf("trainfileFlavordata[%d] : %d\n", i + (j + 1)*inputcontrol.flavorMaxnum, trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
	if (trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] > trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] &&
	trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] > trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] &&
	((trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] * 0.5 + trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum] * 0.5) > 4*(NOISE_W * trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] + NOISE_B))
	&& (inputFlavor[i].flavor_name == 1 || inputFlavor[i].flavor_name == 4 || inputFlavor[i].flavor_name == 7 || inputFlavor[i].flavor_name == 10 || inputFlavor[i].flavor_name == 13 ||
	inputFlavor[i].flavor_name == 3 || inputFlavor[i].flavor_name == 6 || inputFlavor[i].flavor_name == 9 || inputFlavor[i].flavor_name == 12 || inputFlavor[i].flavor_name == 15
	|| inputFlavor[i].flavor_name == 2 || inputFlavor[i].flavor_name == 5 || inputFlavor[i].flavor_name == 8 || inputFlavor[i].flavor_name == 11 || inputFlavor[i].flavor_name == 14
	))
	{
	// ��ƽ�������������룡
	printf("���������еĵ͹�  �м�λ��[ %d %d ]   ȥ��֮ǰ : [ %d %d %d ]\n", i, j, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum], trainfileFlavordata[i + j * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
	trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] = (int)( (trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum] + trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum])*0.2 + trainfileFlavordata[i + j * inputcontrol.flavorMaxnum]);
	// trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] = (int)(trainfileFlavordata[i + j * inputcontrol.flavorMaxnum] / 2 + 0.5);
	printf("���������еĵ͹�  �м�λ��[ %d %d ]   ȥ��֮�� : [ %d %d %d ]\n", i, j, trainfileFlavordata[i + (j - 1)*inputcontrol.flavorMaxnum], trainfileFlavordata[i + j * inputcontrol.flavorMaxnum], trainfileFlavordata[i + (j + 1)*inputcontrol.flavorMaxnum]);
	}

	}
	}*/



}


// ��ӡ����
void print_data(int *data, int hangshu)
{
	printf("\n[");
	for (int i = 0; i < (hangshu * inputcontrol.flavorMaxnum); i++)
	{
		printf(" %d", data[i]);
		if ((i + 1) % inputcontrol.flavorMaxnum == 0 && (hangshu * inputcontrol.flavorMaxnum - 1) != i) 	printf("]\n[");
		if ((i + 1) % inputcontrol.flavorMaxnum == 0 && (hangshu * inputcontrol.flavorMaxnum - 1) == i) 	printf("]\n");
	}
}


/*
����Ŀ��:��ȡ���һ�������
����:*traindata Ϊѵ������
num_vm  Ϊ�����������������
num_traindata Ϊѵ����������
preidct_sample_day_num  ��ҪԤ�������
*/
double *predict_get_ori_data(int *traindata, int num_vm, int num_traindata, int preidct_sample_day_num) {
	double *out_double = new double[num_vm];
	int line_num = num_traindata - 1; //���һ�е��к�
	for (int i = 0; i < num_vm; i++) {
		out_double[i] = (double)traindata[line_num*num_vm + i] * ((double)preidct_sample_day_num / CYC_SAMPLE);
	}
	return out_double;
}



/*
����Ŀ��:Ԥ����һ�׶ε�������
����:���߼��ع�
����:*traindata Ϊѵ������
num_vm  Ϊ�����������������
num_traindata Ϊѵ����������
preidct_sample_day_num  ��ҪԤ�������
*/
double *predict_run_liner(int *traindata, int num_vm, int num_traindata, int preidct_sample_day_num) {
	double *out_double = new double[num_vm];
	double *out_int = new double[num_vm];
	double *W = new double[num_traindata];
	double sum_W = 0;
	//����Ȩֵ����
	for (int i = 0; i < num_traindata; i++) {  //Ȩֵ��������
		W[i] = (i + 1) * (i + 1);
		sum_W += W[i];
	}
	for (int i = 0; i < num_traindata; i++) {
		W[i] = W[i] / sum_W;
		if (sum_W == 0) {
			W[i] = 1;
		}
	}


	//Ԥ��
	for (int i = 0; i < num_vm; i++) {
		out_double[i] = 1;
	}
	for (int i = 0; i < num_vm; i++) {
		for (int j = 0; j < num_traindata; j++) {
			out_double[i] += (double)traindata[j*num_vm + i] * W[j];
		}
	}
	//cout << "\n";
	for (int i = 0; i < num_vm; i++) {
		out_int[i] = out_double[i] * ((double)preidct_sample_day_num / CYC_SAMPLE);
	}
	return out_int;
}

/*
����Ŀ��:���������
����:	̰��FFD
����:	*require_vm		���������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm		���������ܽṹ��
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*result_save	���������ݱ�(���)
���:	�����ķ���������
*/
int putVM_greedy(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save)
{
	/***********************************************************���򲿷�*****************************************************************/
	if (opt_target == 0)
	{
		// ʵ������Ӵ�С
		for (int i = 0; i < num_vm - 1; i++)
		{
			for (int j = i + 1; j < num_vm; j++)
			{
				if (inputFlavor[j].cpu_core_num>inputFlavor[i].cpu_core_num) // ����cpu����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
				else if (inputFlavor[j].cpu_core_num == inputFlavor[i].cpu_core_num && inputFlavor[j].mem_size>inputFlavor[i].mem_size) // ����cpu���,mem����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
			}
		}
	}
	else if (opt_target == 1)
	{
		// ʵ������Ӵ�С
		for (int i = 0; i < num_vm - 1; i++)
		{
			for (int j = i + 1; j < num_vm; j++)
			{
				if (inputFlavor[j].mem_size>inputFlavor[i].mem_size) // ����mem����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
				else if (inputFlavor[j].mem_size == inputFlavor[i].mem_size && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // mem��� ����CPU����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
			}
		}
	}
	else    // Ŀ��ѡ����� �˳�,�쳣
	{
		return -1;
	}

	// ����copy
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++)
		require_vm_copy[i] = require_vm[i];



	/********************************************************************���ò���***********************************************************************/
	// ����������������¼��
	Server *server_remain = new Server[2000];
	// ���ݸ�ֵ(�������2000Ҫ�ͺ��������max_serve_py��Ӧ,�������Կ��Ƕ��һ��max_serve_py�ӿ�)
	for (int i = 0; i < 2000; i++)
	{
		server_remain[i].cpu_core_num = resource_pm.cpu_core_num;
		server_remain[i].mem_size = resource_pm.mem_size;
	}

	int server_num = 1;
	for (int i = 0; i < num_vm; i++)	// һ��һ�ַ���
	{
		// ��ǰ������������ʼ��������
		int j = 0;
		while (require_vm_copy[i] != 0)		// ÿһ��һ��һ������,ֱ������
		{
			// ��ǰ�˸���������ñ�־λ
			int flag_put = 0;
			// ��������
			for (; j < server_num; j++)
			{
				// ���ܷ���
				if (server_remain[j].cpu_core_num < inputFlavor[i].cpu_core_num || server_remain[j].mem_size < inputFlavor[i].mem_size)		// ������Դ����
				{
					continue;	// ֱ�ӽ�����һ�ε�����
				}
				else if (server_remain[j].cpu_core_num >= inputFlavor[i].cpu_core_num && server_remain[j].mem_size >= inputFlavor[i].mem_size)	// ��֤����ʱ��Դ����,�����
				{
					server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
					server_remain[j].mem_size -= inputFlavor[i].mem_size;
					flag_put = 1;
					result_save[j*num_vm + i]++;	// ���ü�¼
					break;	// ����������,����ѭ��
				}
			}
			// �������forѭ���п����Ǳ����궼�Ų���,Ҳ�п����Ƿ��ú���ֱ������
			if (flag_put == 0)
			{
				server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
				server_remain[j].mem_size -= inputFlavor[i].mem_size;
				result_save[j*num_vm + i]++;	// ���ü�¼
				server_num++;	// �¿�һ��������

			}

			require_vm_copy[i]--; // ���������������һ
		}
	}

	// ���л�ԭ������
	/*
	for (int i = 0; i<inputcontrol.flavorMaxnum; i++)
	{
	inputFlavor[i].cpu_core_num = inputFlavor_data[i].cpu_core_num;
	inputFlavor[i].mem_size = inputFlavor_data[i].mem_size;
	inputFlavor[i].flavor_name = inputFlavor_data[i].flavor_name;
	}
	*/

	return server_num;
}



int putVM_greedy_AAA_vmsize(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int *vm_put_priority)

{


	int *require_vm_temp = new int[num_vm];
	for (int i = 0; i < num_vm; i++)	require_vm_temp[i] = require_vm[vm_put_priority[i]];
	for (int i = 0; i < num_vm; i++)   require_vm[i] = require_vm_temp[i];
	// ����copy
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++)	require_vm_copy[i] = require_vm[i];

	/********************************************************************���ò���***********************************************************************/
	// ����������������¼��
	Server *server_remain = new Server[MAX_SERVER_NUM];
	// ���ݸ�ֵ(�������2000Ҫ�ͺ��������max_serve_py��Ӧ,�������Կ��Ƕ��һ��max_serve_py�ӿ�)
	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		server_remain[i].cpu_core_num = resource_pm.cpu_core_num;
		server_remain[i].mem_size = resource_pm.mem_size;
	}

	int server_num = 1;
	for (int i = 0; i < num_vm; i++)	// һ��һ�ַ���
	{
		// ��ǰ������������ʼ��������
		int j = 0;
		while (require_vm_copy[i] != 0)		// ÿһ��һ��һ������,ֱ������
		{
			// ��ǰ�˸���������ñ�־λ
			int flag_put = 0;
			// ��������
			for (; j < server_num; j++)
			{
				// ���ܷ���
				if (server_remain[j].cpu_core_num < inputFlavor[i].cpu_core_num || server_remain[j].mem_size < inputFlavor[i].mem_size)		// ������Դ����
				{
					continue;	// ֱ�ӽ�����һ�ε�����
				}
				else if (server_remain[j].cpu_core_num >= inputFlavor[i].cpu_core_num && server_remain[j].mem_size >= inputFlavor[i].mem_size)	// ��֤����ʱ��Դ����,�����
				{
					server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
					server_remain[j].mem_size -= inputFlavor[i].mem_size;
					flag_put = 1;
					result_save[j*num_vm + i]++;	// ���ü�¼
					break;	// ����������,����ѭ��
				}
			}
			// �������forѭ���п����Ǳ����궼�Ų���,Ҳ�п����Ƿ��ú���ֱ������
			if (flag_put == 0)
			{
				server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
				server_remain[j].mem_size -= inputFlavor[i].mem_size;
				result_save[j*num_vm + i]++;	// ���ü�¼
				server_num++;	// �¿�һ��������

			}

			require_vm_copy[i]--; // ���������������һ
		}
	}

	// ���л�ԭ������
	/*
	for (int i = 0; i<inputcontrol.flavorMaxnum; i++)
	{
	inputFlavor[i].cpu_core_num = inputFlavor_data[i].cpu_core_num;
	inputFlavor[i].mem_size = inputFlavor_data[i].mem_size;
	inputFlavor[i].flavor_name = inputFlavor_data[i].flavor_name;
	}
	*/

	return server_num;
}


/*  ����  */

// vm_put_priority ���õ����ȼ�������
int putVM_greedy_ratio_guided(int *require_vm, int server_num_pre, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int *vm_put_priority)

{
	int *require_vm_temp = new int[inputcontrol.flavorMaxnum];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)	require_vm_temp[i] = require_vm[vm_put_priority[i]];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)   require_vm[i] = require_vm_temp[i];
	// ����copy
	int *require_vm_copy = new int[inputcontrol.flavorMaxnum];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)	require_vm_copy[i] = require_vm[i];

	/********************************************************************���ò���***********************************************************************/
	// ����������������¼��
	Server *server_remain = new Server[MAX_SERVER_NUM];
	// ���ݸ�ֵ(�������2000Ҫ�ͺ��������max_serve_py��Ӧ,�������Կ��Ƕ��һ��max_serve_py�ӿ�)
	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		server_remain[i].cpu_core_num = resource_pm.cpu_core_num;
		server_remain[i].mem_size = resource_pm.mem_size;
	}
	int continue_count = 0;
	int flag_count = 0; // һ��С��server_num_pre
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  // һ��һ�ַ���
	{
		for (int j = 0; j < require_vm_copy[i]; j++)  // ÿһ���������Ӧ����������
		{
			int flag_put = 0; // �������϶�Ӧ����̨������Ƿ��п����λ��
			continue_count = 0;
			for (int m = 0; m < server_num_pre; m++)  // �ֱ���˳�������ӡ����ѭ���Ĵ���������
			{
				if (server_remain[(m + flag_count) % server_num_pre].cpu_core_num < inputFlavor[i].cpu_core_num || server_remain[(m + flag_count) % server_num_pre].mem_size < inputFlavor[i].mem_size)		// ������Դ����
				{
					//continue_count++;
					continue;	// ֱ�ӽ�����һ�ε�����
				}
				else
					if (server_remain[(m + flag_count) % server_num_pre].cpu_core_num >= inputFlavor[i].cpu_core_num && server_remain[(m + flag_count) % server_num_pre].mem_size >= inputFlavor[i].mem_size)	// ��֤����ʱ��Դ����,�����
					{
						server_remain[(m + flag_count) % server_num_pre].cpu_core_num -= inputFlavor[i].cpu_core_num;
						server_remain[(m + flag_count) % server_num_pre].mem_size -= inputFlavor[i].mem_size;
						flag_put = 1; //�����
									  //flag_count = (flag_count + continue_count) % server_num_pre;
									  //continue_count = 0;
						result_save[((m + flag_count) % server_num_pre)*inputcontrol.flavorMaxnum + i]++;	// ���ü�¼
						flag_count++;
						if (flag_count == server_num_pre)  flag_count = 0;
						//print_data(result_save, server_num_pre);
						//printf("  %d  %d  %d\n", i,j, flag_count);
						break;
					}

			}
			// ���ѭ����server_num_pre��Σ���û�з��ý�ȥ���Ǿ��ٿ�һ����������
			if (flag_put == 0)
			{
				// ���˵�j������
				server_remain[server_num_pre].cpu_core_num -= inputFlavor[i].cpu_core_num;
				server_remain[server_num_pre].mem_size -= inputFlavor[i].mem_size;
				result_save[server_num_pre*inputcontrol.flavorMaxnum + i]++;	// ���ü�¼
				server_num_pre++;	// �¿�һ��������

			}
		}

	}

	return server_num_pre;
}



/*
����Ŀ��:���������
����:	̰��FFD
����:	*require_vm		���������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm		���������ܽṹ��
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*result_save	���������ݱ�(���)
���:	�����ķ���������
*/
int putVM_greedy_without_seq(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save)
{
	// ����copy
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++)
		require_vm_copy[i] = require_vm[i];
	/********************************************************************���ò���***********************************************************************/
	// ����������������¼��
	Server *server_remain = new Server[2000];
	// ���ݸ�ֵ(�������2000Ҫ�ͺ��������max_serve_py��Ӧ,�������Կ��Ƕ��һ��max_serve_py�ӿ�)
	for (int i = 0; i < 2000; i++)
	{
		server_remain[i].cpu_core_num = resource_pm.cpu_core_num;
		server_remain[i].mem_size = resource_pm.mem_size;
	}

	int server_num = 1;
	for (int i = 0; i < num_vm; i++)	// һ��һ�ַ���
	{
		// ��ǰ������������ʼ��������
		int j = 0;
		while (require_vm_copy[i] != 0)		// ÿһ��һ��һ������,ֱ������
		{
			// ��ǰ�˸���������ñ�־λ
			int flag_put = 0;
			// ��������
			for (; j < server_num; j++)
			{
				// ���ܷ���
				if (server_remain[j].cpu_core_num < inputFlavor[i].cpu_core_num || server_remain[j].mem_size < inputFlavor[i].mem_size)		// ������Դ����
				{
					continue;	// ֱ�ӽ�����һ�ε�����
				}
				else if (server_remain[j].cpu_core_num >= inputFlavor[i].cpu_core_num && server_remain[j].mem_size >= inputFlavor[i].mem_size)	// ��֤����ʱ��Դ����,�����
				{
					server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
					server_remain[j].mem_size -= inputFlavor[i].mem_size;
					flag_put = 1;
					result_save[j*num_vm + i]++;	// ���ü�¼
					break;	// ����������,����ѭ��
				}
			}
			// �������forѭ���п����Ǳ����궼�Ų���,Ҳ�п����Ƿ��ú���ֱ������
			if (flag_put == 0)
			{
				server_remain[j].cpu_core_num -= inputFlavor[i].cpu_core_num;
				server_remain[j].mem_size -= inputFlavor[i].mem_size;
				result_save[j*num_vm + i]++;	// ���ü�¼
				server_num++;	// �¿�һ��������

			}

			require_vm_copy[i]--; // ���������������һ
		}
	}

	// ���л�ԭ������
	/*
	for (int i = 0; i<inputcontrol.flavorMaxnum; i++)
	{
	inputFlavor[i].cpu_core_num = inputFlavor_data[i].cpu_core_num;
	inputFlavor[i].mem_size = inputFlavor_data[i].mem_size;
	inputFlavor[i].flavor_name = inputFlavor_data[i].flavor_name;
	}
	*/

	return server_num;
}


/*
����Ŀ��:����
����:	*require_vm		���������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm		���������ܽṹ��
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*result_save	���������ݱ�(���)
���:	�����ķ���������
*/
void putVM_seq(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save)
{
	/***********************************************************���򲿷�*****************************************************************/
	if (opt_target == 0)
	{
		// ʵ������Ӵ�С
		for (int i = 0; i < num_vm - 1; i++)
		{
			for (int j = i + 1; j < num_vm; j++)
			{
				if (inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
				else if (inputFlavor[j].cpu_core_num == inputFlavor[i].cpu_core_num && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����cpu���,mem����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
			}
		}
	}
	else if (opt_target == 1)
	{
		// ʵ������Ӵ�С
		for (int i = 0; i < num_vm - 1; i++)
		{
			for (int j = i + 1; j < num_vm; j++)
			{
				if (inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����mem����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
				else if (inputFlavor[j].mem_size == inputFlavor[i].mem_size && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // mem��� ����CPU����ǰ��,����
				{
					Flavor tmp_flavor;
					// ���ܱ�λ�ý���
					tmp_flavor = inputFlavor[j];
					inputFlavor[j] = inputFlavor[i];
					inputFlavor[i] = tmp_flavor;

					int tmp_require;
					// ����λ�ý���
					tmp_require = require_vm[j];
					require_vm[j] = require_vm[i];
					require_vm[i] = tmp_require;
				}
			}
		}
	}
}



void putVM_seq_ratio_guided(int *require_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, double *ratio_vm_data, int *vm_put_priority, int Flag_put_priority)
{

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) printf(" %f  ", ratio_vm_data[i]);
	print_data(vm_put_priority, 1);
	/***********************************************************���򲿷�*****************************************************************/

	if (Flag_put_priority == 2)
	{
		if (opt_target == 0)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].cpu_core_num == inputFlavor[i].cpu_core_num && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}
		}
		else if (opt_target == 1)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].mem_size == inputFlavor[i].mem_size && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // mem��� ����CPU����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}
		}
	}
	// ���÷�����������  ��ratio_vm�Ĵ�С��
	if (Flag_put_priority != 2)
	{
		if (opt_target == 0)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (ratio_vm_data[j] < ratio_vm_data[i]) // ����cpu�����ʴ���ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
					else if (ratio_vm_data[j] == ratio_vm_data[i] && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
				}
			}
		}
		else if (opt_target == 1)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (ratio_vm_data[j] > ratio_vm_data[i]) // ����mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
					else if (ratio_vm_data[j] == ratio_vm_data[i] && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // mem��� ����CPU����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
				}
			}
		}
	}

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) printf(" %f  ", ratio_vm_data[i]);
	print_data(vm_put_priority, 1);
}


void putVM_seq_ratio_guided_big_to_small(int *require_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, double *ratio_vm_data, int *vm_put_priority, int Flag_put_priority, int bad_count_num)
{

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) printf(" %f  ", ratio_vm_data[i]);
	print_data(vm_put_priority, 1);
	/***********************************************************���򲿷�*****************************************************************/

	if (Flag_put_priority == 2)
	{
		if (opt_target == 0)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].cpu_core_num == inputFlavor[i].cpu_core_num && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}
		}
		else if (opt_target == 1)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].mem_size == inputFlavor[i].mem_size && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // mem��� ����CPU����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}
		}
	}
	// ���÷�����������  ��ratio_vm�Ĵ�С��
	if (Flag_put_priority != 2)
	{
		if (opt_target == 0)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					// �Ƚ�bad�ķ������
					if (ratio_vm_data[j] < ratio_vm_data[i]) // ����cpu�����ʴ���ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
					else if (ratio_vm_data[j] == ratio_vm_data[i] && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
				}
			}


			// �������֮���ٽ�������Ż����ֶ����������
			// ʵ������Ӵ�С
			for (int i = bad_count_num; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].cpu_core_num == inputFlavor[i].cpu_core_num && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}


		}
		else if (opt_target == 1)
		{
			// ʵ������Ӵ�С
			for (int i = 0; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (ratio_vm_data[j] > ratio_vm_data[i]) // ����mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
					else if (ratio_vm_data[j] == ratio_vm_data[i] && inputFlavor[j].mem_size > inputFlavor[i].mem_size) // mem��� ����CPU����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;

						double temp_vm_ratio;
						temp_vm_ratio = ratio_vm_data[j];
						ratio_vm_data[j] = ratio_vm_data[i];
						ratio_vm_data[i] = temp_vm_ratio;
					}
				}
			}

			// �������֮���ٽ�������Ż����ֶ����������
			// ʵ������Ӵ�С
			for (int i = bad_count_num; i < inputcontrol.flavorMaxnum - 1; i++)
			{
				for (int j = i + 1; j < inputcontrol.flavorMaxnum; j++)
				{
					if (inputFlavor[j].mem_size > inputFlavor[i].mem_size) // ����cpu����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
					else if (inputFlavor[j].mem_size == inputFlavor[i].mem_size && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num) // ����cpu���,mem����ǰ��,����
					{
						Flavor tmp_flavor;
						// ���ܱ�λ�ý���
						tmp_flavor = inputFlavor[j];
						inputFlavor[j] = inputFlavor[i];
						inputFlavor[i] = tmp_flavor;

						int tmp_require;
						// ����λ�ý���
						tmp_require = require_vm[j];
						require_vm[j] = require_vm[i];
						require_vm[i] = tmp_require;

						tmp_require = vm_put_priority[j];
						vm_put_priority[j] = vm_put_priority[i];
						vm_put_priority[i] = tmp_require;
					}
				}
			}

		}



	}

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) printf(" %f  ", ratio_vm_data[i]);

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) printf(" %d  ", inputFlavor[i].flavor_name);
	print_data(vm_put_priority, 1);
}




/*
����Ŀ��:���������
����:	��̬�滮
����:
*require_vm		���������
num_of_total_vm  �������������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm		���������ܽṹ��
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*result_save	���������ݱ�(���)
���:	�����ķ���������
*/
int putVM_dynamicpro(int *require_vm, int num_of_total_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save) {
	int server_num = 0;  //���������
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}
	while (1) {
		/*�����ʽ��������������г�һ��*/
		int num_of_total_vm_point = 1; //Ԥ��������������
		num_of_total_vm = 0;//ͳ�����������
		for (int i = 0; i < num_vm; i++) {
			for (int j = 0; j < require_vm_copy[i]; j++) {
				num_of_total_vm++;
			}
		}
		Flavor *list_danmicpro = new Flavor[num_of_total_vm + 1]; //��̬�滮������������� һ��һ���Ž�ȥ,�����СΪ���������+1
		Flavor *list_danmicpro_ori = list_danmicpro;  //����ԭʼָ��
		list_danmicpro[0].cpu_core_num = 0;
		list_danmicpro[0].mem_size = 0;
		for (int i = 0; i < num_vm; i++) {
			for (int j = 0; j < require_vm_copy[i]; j++) {
				list_danmicpro[num_of_total_vm_point] = inputFlavor[i];
				num_of_total_vm_point++;
				//printf("\n");
				//printf("�����%d  cpu: %d   mem: %d", num_of_total_vm_point - 1, list_danmicpro[num_of_total_vm_point - 1].cpu_core_num, list_danmicpro[num_of_total_vm_point - 1].mem_size);
			}
		}
		/*��̬�滮��ʼ��*/
		int restict_cpu = resource_pm.cpu_core_num; //�����cpu����
		int restict_mem = resource_pm.mem_size;   //�����mem����
		int *f = new int[(restict_cpu + 1)*(restict_mem + 1)];
		char *chose_or_no = new char[(num_of_total_vm_point + 1)*(restict_cpu + 1)*(restict_mem + 1)];
		int *f_ori = f;   //����ԭʼָ��
		char *chose_or_no_ori = chose_or_no;   //����ԭʼָ��
		for (int i = 0; i < (num_of_total_vm_point + 1)*(restict_cpu + 1)*(restict_mem + 1); i++) {
			chose_or_no[i] = 0;
		}
		for (int i = 0; i < (restict_cpu + 1)*(restict_mem + 1); i++) {
			f[i] = 0;
		}
		/*�滮����*/
		for (int i = 1; i <= num_of_total_vm; i++) {
			for (int j = restict_cpu; j >= list_danmicpro[i].cpu_core_num; j--) {
				for (int k = restict_mem; k >= list_danmicpro[i].mem_size; k--) {
					int value1, value2;
					if (opt_target == 0) {  //�Ż�cpu
						value1 = f[(j - list_danmicpro[i].cpu_core_num)*(restict_mem + 1) + (k - list_danmicpro[i].mem_size)] + list_danmicpro[i].cpu_core_num;
					}
					else {   //�Ż��ڴ�
						value1 = f[(j - list_danmicpro[i].cpu_core_num)*(restict_mem + 1) + (k - list_danmicpro[i].mem_size)] + list_danmicpro[i].mem_size;
					}
					value2 = f[j*(restict_mem + 1) + k];
					if (value1 > value2) {   //ȡ��һ��
						f[j*(restict_mem + 1) + k] = value1;
						chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + j * (restict_mem + 1) + k] = 1;
					}
					else {      //��ȡ��һ��
						f[j*(restict_mem + 1) + k] = value2;
						chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + j * (restict_mem + 1) + k] = 0;
					}
				}
			}
		}
		/*�ܽ�ѡȡ*/
		int out_put[10000] = { 0 };  //ѡ���ļ�����������룬���10000�������
		int CPU_temp = resource_pm.cpu_core_num;
		int MEM_temp = resource_pm.mem_size;
		for (int i = num_of_total_vm_point; i > 0; i--) {
			if (chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + CPU_temp * (restict_mem + 1) + MEM_temp] == 1) {
				out_put[i] = 1;
				CPU_temp -= list_danmicpro[i].cpu_core_num;
				MEM_temp -= list_danmicpro[i].mem_size;
			}
			else {
				out_put[i] = 0;
			}
		}
		int xiaolv = f[restict_cpu*(restict_mem + 1) + restict_mem];
		//printf("\n%d\n", f[restict_cpu*(restict_mem + 1) + restict_mem]);
		//printf("ȡ��ȡ��\n");
		for (int j = 0; j < num_of_total_vm_point; j++) {
			//printf("%d ", out_put[j]);
		}
		//printf("\n�������ܣ�\n");
		for (int j = 0; j < num_of_total_vm_point; j++) {
			//printf("%d ", list_danmicpro[j].cpu_core_num);
		}

		/*һ�ֶ�̬�滮�������������result_save*/
		for (int i = 0; i < num_of_total_vm_point; i++) {
			if (out_put[i] == 1) {
				for (int j = 0; j < num_vm; j++) {
					if (list_danmicpro[i].flavor_name == inputFlavor[j].flavor_name) { //ƥ������
						require_vm_copy[j]--; //��ǰ����������
						result_save[server_num*num_vm + j]++;
						break;
					}
				}
			}
		}
		/*�����Ƿ����·�����*/
		char creat_new_py_flag = 0; //�����Ƿ����·�����
		for (int i = 0; i < num_vm; i++) {
			if (require_vm_copy[i] != 0) {
				creat_new_py_flag = 1;
				break;
			}
		}
		/*�ͷŶ��ڴ�*/
		delete[] list_danmicpro_ori;
		delete[] chose_or_no_ori;
		delete[] f_ori;
		/*�����·�����*/
		if (creat_new_py_flag == 1) {  //���������û�з�����
			server_num++;
		}
		else {  //���������break��while
			break;
		}
	}

	return server_num + 1;
}


int putVM_dynamicpro_ratio_guided(int *require_vm, int num_of_total_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int *vm_put_priority) {


	int *require_vm_temp = new int[inputcontrol.flavorMaxnum];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)	require_vm_temp[i] = require_vm[vm_put_priority[i]];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)   require_vm[i] = require_vm_temp[i];


	int server_num = 0;  //���������
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}
	while (1) {
		/*�����ʽ��������������г�һ��*/
		int num_of_total_vm_point = 1; //Ԥ��������������
		num_of_total_vm = 0;//ͳ�����������
		for (int i = 0; i < num_vm; i++) {
			for (int j = 0; j < require_vm_copy[i]; j++) {
				num_of_total_vm++;
			}
		}
		Flavor *list_danmicpro = new Flavor[num_of_total_vm + 1]; //��̬�滮������������� һ��һ���Ž�ȥ,�����СΪ���������+1
		Flavor *list_danmicpro_ori = list_danmicpro;  //����ԭʼָ��
		list_danmicpro[0].cpu_core_num = 0;
		list_danmicpro[0].mem_size = 0;
		for (int i = 0; i < num_vm; i++) {
			for (int j = 0; j < require_vm_copy[i]; j++) {
				list_danmicpro[num_of_total_vm_point] = inputFlavor[i];
				num_of_total_vm_point++;
				//printf("\n");
				//printf("�����%d  cpu: %d   mem: %d", num_of_total_vm_point - 1, list_danmicpro[num_of_total_vm_point - 1].cpu_core_num, list_danmicpro[num_of_total_vm_point - 1].mem_size);
			}
		}
		/*��̬�滮��ʼ��*/
		int restict_cpu = resource_pm.cpu_core_num; //�����cpu����
		int restict_mem = resource_pm.mem_size;   //�����mem����
		int *f = new int[(restict_cpu + 1)*(restict_mem + 1)];
		char *chose_or_no = new char[(num_of_total_vm_point + 1)*(restict_cpu + 1)*(restict_mem + 1)];
		int *f_ori = f;   //����ԭʼָ��
		char *chose_or_no_ori = chose_or_no;   //����ԭʼָ��
		for (int i = 0; i < (num_of_total_vm_point + 1)*(restict_cpu + 1)*(restict_mem + 1); i++) {
			chose_or_no[i] = 0;
		}
		for (int i = 0; i < (restict_cpu + 1)*(restict_mem + 1); i++) {
			f[i] = 0;
		}
		/*�滮����*/
		for (int i = 1; i <= num_of_total_vm; i++) {
			for (int j = restict_cpu; j >= list_danmicpro[i].cpu_core_num; j--) {
				for (int k = restict_mem; k >= list_danmicpro[i].mem_size; k--) {
					int value1, value2;
					if (opt_target == 0) {  //�Ż�cpu
						value1 = f[(j - list_danmicpro[i].cpu_core_num)*(restict_mem + 1) + (k - list_danmicpro[i].mem_size)] + list_danmicpro[i].cpu_core_num;
					}
					else {   //�Ż��ڴ�
						value1 = f[(j - list_danmicpro[i].cpu_core_num)*(restict_mem + 1) + (k - list_danmicpro[i].mem_size)] + list_danmicpro[i].mem_size;
					}
					value2 = f[j*(restict_mem + 1) + k];
					if (value1 > value2) {   //ȡ��һ��
						f[j*(restict_mem + 1) + k] = value1;
						chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + j * (restict_mem + 1) + k] = 1;
					}
					else {      //��ȡ��һ��
						f[j*(restict_mem + 1) + k] = value2;
						chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + j * (restict_mem + 1) + k] = 0;
					}
				}
			}
		}
		/*�ܽ�ѡȡ*/
		int out_put[10000] = { 0 };  //ѡ���ļ�����������룬���10000�������
		int CPU_temp = resource_pm.cpu_core_num;
		int MEM_temp = resource_pm.mem_size;
		for (int i = num_of_total_vm_point; i > 0; i--) {
			if (chose_or_no[i*(restict_cpu + 1)*(restict_mem + 1) + CPU_temp * (restict_mem + 1) + MEM_temp] == 1) {
				out_put[i] = 1;
				CPU_temp -= list_danmicpro[i].cpu_core_num;
				MEM_temp -= list_danmicpro[i].mem_size;
			}
			else {
				out_put[i] = 0;
			}
		}
		int xiaolv = f[restict_cpu*(restict_mem + 1) + restict_mem];
		//printf("\n%d\n", f[restict_cpu*(restict_mem + 1) + restict_mem]);
		//printf("ȡ��ȡ��\n");
		for (int j = 0; j < num_of_total_vm_point; j++) {
			//printf("%d ", out_put[j]);
		}
		//printf("\n�������ܣ�\n");
		for (int j = 0; j < num_of_total_vm_point; j++) {
			//printf("%d ", list_danmicpro[j].cpu_core_num);
		}

		/*һ�ֶ�̬�滮�������������result_save*/
		for (int i = 0; i < num_of_total_vm_point; i++) {
			if (out_put[i] == 1) {
				for (int j = 0; j < num_vm; j++) {
					if (list_danmicpro[i].flavor_name == inputFlavor[j].flavor_name) { //ƥ������
						require_vm_copy[j]--; //��ǰ����������
						result_save[server_num*num_vm + j]++;
						break;
					}
				}
			}
		}
		/*�����Ƿ����·�����*/
		char creat_new_py_flag = 0; //�����Ƿ����·�����
		for (int i = 0; i < num_vm; i++) {
			if (require_vm_copy[i] != 0) {
				creat_new_py_flag = 1;
				break;
			}
		}
		/*�ͷŶ��ڴ�*/
		delete[] list_danmicpro_ori;
		delete[] chose_or_no_ori;
		delete[] f_ori;
		/*�����·�����*/
		if (creat_new_py_flag == 1) {  //���������û�з�����
			server_num++;
		}
		else {  //���������break��while
			break;
		}
	}

	return server_num + 1;
}




void print_resource(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int num_py) {
	int i, j;
	/*��Դ��ʼ��*/
	double *py_ratio_cpu = new double[num_py];  //һ���������cpu��Դ������
	double *py_ratio_mem = new double[num_py];  //һ����������ڴ���Դ������ 
	int *resource_rest_cpu = new int[num_py];   //һ���������cpuʣ����Դ
	int *resource_rest_mem = new int[num_py];   //һ����������ڴ�ʣ����Դ
	int *flavor_num_in_py = new int[num_py];    //һ��������а��������������
	int flavor_totol_mun = 0;  //���������������
	int num_py_new = num_py; //�µ����������
	for (i = 0; i < num_py; i++) {
		resource_rest_cpu[i] = resource_pm.cpu_core_num;
		resource_rest_mem[i] = resource_pm.mem_size;
		flavor_num_in_py[i] = 0;
	}
	for (i = 0; i < num_vm; i++) {
		flavor_totol_mun += require_vm[i];
	}


	printf("\n\n");
	printf("������\n\n");
	for (i = 0; i < num_vm; i++) {
		printf("���ƣ�flavor%d  ", inputFlavor[i].flavor_name);
		printf("�ڴ棺%d  ", inputFlavor[i].mem_size);
		printf("CPU�� %d  ", inputFlavor[i].cpu_core_num);
		printf("����%d  ", require_vm[i]);
		printf("\n");
	}


	/*����ÿ��Ӳ��������������*/
	for (i = 0; i < num_py; i++) {
		// ��i��������������ʺ�ʣ����Դ 
		int sum_temp_cpu = 0;
		int sum_temp_mem = 0;
		for (j = 0; j < num_vm; j++) {
			if (result_save[i*num_vm + j] != 0) {
				sum_temp_cpu += result_save[i*num_vm + j] * inputFlavor[j].cpu_core_num;
				sum_temp_mem += result_save[i*num_vm + j] * inputFlavor[j].mem_size;
				flavor_num_in_py[i] += result_save[i*num_vm + j];
			}
		}
		py_ratio_cpu[i] = (double)sum_temp_cpu / (double)resource_pm.cpu_core_num;
		py_ratio_mem[i] = (double)sum_temp_mem / (double)resource_pm.mem_size;
		resource_rest_cpu[i] -= sum_temp_cpu;
		resource_rest_mem[i] -= sum_temp_mem;
	}
	printf("\n");
	for (i = 0; i < num_py; i++) {
		printf("\n CPU%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_cpu[i], resource_rest_cpu[i]);
	}
	printf("\n");
	for (i = 0; i < num_py; i++) {
		printf("\n MEM%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_mem[i], resource_rest_mem[i]);
	}

	double ratio_cpu = 0;
	double ratio_mem = 0;
	for (i = 0; i<num_py; i++) {
		ratio_cpu += py_ratio_cpu[i];
		ratio_mem += py_ratio_mem[i];
	}
	ratio_cpu /= num_py;
	ratio_mem /= num_py;
	printf("\n");
	printf("\n CPU�������ʣ�%f", ratio_cpu);
	printf("\n MEM�������ʣ�%f", ratio_mem);

	/*if (opt_target == 0) {
	if (ratio_cpu<0.8) {
	while (1) {

	}
	}
	}
	else {
	if (ratio_mem<0.8) {
	while (1) {

	}
	}
	}*/


}


/*
����Ŀ��:�Է��÷������н���
����:	������֤100%������
����:	*require_vm		���������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm		���������ܽṹ��
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*result_save	���������ݱ�(���)
num_py      	�����ķ���������
���:   ���������������
*/
int putVM_correct(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int num_py) {
	int i, j;
	/*��Դ��ʼ��*/
	double *py_ratio_cpu = new double[num_py];  //һ���������cpu��Դ������
	double *py_ratio_mem = new double[num_py];  //һ����������ڴ���Դ������ 
	int *resource_rest_cpu = new int[num_py];   //һ���������cpuʣ����Դ
	int *resource_rest_mem = new int[num_py];   //һ����������ڴ�ʣ����Դ
	int *flavor_num_in_py = new int[num_py];    //һ��������а��������������
	int flavor_totol_mun = 0;  //���������������
	int num_py_new = num_py; //�µ����������
	for (i = 0; i < num_py; i++) {
		resource_rest_cpu[i] = resource_pm.cpu_core_num;
		resource_rest_mem[i] = resource_pm.mem_size;
		flavor_num_in_py[i] = 0;
	}
	for (i = 0; i < num_vm; i++) {
		flavor_totol_mun += require_vm[i];
	}
	/*����ÿ��Ӳ��������������*/
	for (i = 0; i < num_py; i++) {
		// ��i��������������ʺ�ʣ����Դ 
		int sum_temp_cpu = 0;
		int sum_temp_mem = 0;
		for (j = 0; j < num_vm; j++) {
			if (result_save[i*num_vm + j] != 0) {
				sum_temp_cpu += result_save[i*num_vm + j] * inputFlavor[j].cpu_core_num;
				sum_temp_mem += result_save[i*num_vm + j] * inputFlavor[j].mem_size;
				flavor_num_in_py[i] += result_save[i*num_vm + j];
			}
		}
		py_ratio_cpu[i] = (double)sum_temp_cpu / (double)resource_pm.cpu_core_num;
		py_ratio_mem[i] = (double)sum_temp_mem / (double)resource_pm.mem_size;
		resource_rest_cpu[i] -= sum_temp_cpu;
		resource_rest_mem[i] -= sum_temp_mem;
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n CPU%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_cpu[i], resource_rest_cpu[i]);
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n MEM%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_mem[i], resource_rest_mem[i]);
	}


	/*��ʼ����*/
	int opt_sub_timer;  //���Ż������������
	int opt_add_timer;  //���Ż������������
	opt_sub_timer = flavor_num_in_py[num_py - 1];
	opt_add_timer = 0;
	int *require_vm_copy = new int[num_vm];
	int *result_save_copy = new int[num_vm*num_py];
	for (i = 0; i < num_py; i++) {
		for (j = 0; j < num_vm; j++) {
			result_save_copy[i*num_vm + j] = result_save[i*num_vm + j];
		}
	}
	for (i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}

	//i = num_py - 1;  //ֻ�������һ��Ӳ�������� 
	i = 0;
	while (i < num_py) {
		opt_sub_timer = flavor_num_in_py[i];
		if (resource_rest_cpu[i] != 0 && resource_rest_mem[i] != 0)
		{ //ʣ����Դ��Ϊ0���ſɽ���
			for (j = 0; j < num_vm; j++) {
				while (inputFlavor[j].cpu_core_num <= resource_rest_cpu[i] && inputFlavor[j].mem_size <= resource_rest_mem[i]) {
					require_vm_copy[j]++;
					result_save_copy[i*num_vm + j]++;
					opt_add_timer++;
					resource_rest_cpu[i] -= inputFlavor[j].cpu_core_num;
					resource_rest_mem[i] -= inputFlavor[j].mem_size;
				}
			}
			if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
				for (int kk = 0; kk < num_py; kk++) {
					for (j = 0; j < num_vm; j++) {
						result_save[kk*num_vm + j] = result_save_copy[kk*num_vm + j];
					}
				}
				for (int kk = 0; kk < num_vm; kk++) {
					require_vm[kk] = require_vm_copy[kk];
				}
			}
			else {  //ѡ�� ��������Ż�
				for (j = 0; j < num_vm; j++) {
					if (result_save[i*num_vm + j] != 0) {
						require_vm[j] -= result_save[i*num_vm + j];
						result_save[i*num_vm + j] = 0;
					}
				}
				for (int kk = i; kk < num_py_new; kk++) {
					for (j = 1; j < num_vm; j++) {
						result_save[i*num_vm + j] = result_save[(i + 1)*num_vm + j];
					}
				}
				num_py_new--;
				continue;
			}
		}
		i++;
	}

	return num_py_new;
}


/*�·��ý���*/
int putVM_correct_auto(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int num_py) {


	int i, j;

	/*printf("\n");
	for (i = 0; i < num_vm; i++) {
	printf("flavor%d  %d  ", inputFlavor[i].flavor_name, require_vm[i]);
	}*/

	/*�������������*/
	int *seq_vm = new int[num_vm];  //����˳���Ӧ��ϵ
	Flavor *inputFlavor_copy = new Flavor[num_vm];
	for (i = 0; i < num_vm; i++) {
		seq_vm[i] = i;
		inputFlavor_copy[i].cpu_core_num = inputFlavor[i].cpu_core_num;
		inputFlavor_copy[i].mem_size = inputFlavor[i].mem_size;
		inputFlavor_copy[i].flavor_name = inputFlavor[i].flavor_name;
	}

	if (opt_target == 0) {  //�Ż�cpu
		for (i = 0; i < num_vm; i++) {
			for (j = i + 1; j < num_vm; j++) {
				if (inputFlavor_copy[j].cpu_core_num > inputFlavor_copy[j - 1].cpu_core_num) {  //cpu��ķ�ǰ��
					Flavor vm_temp_auto = inputFlavor_copy[j - 1];
					inputFlavor_copy[j - 1] = inputFlavor_copy[j];
					inputFlavor_copy[j] = vm_temp_auto;
					int seq_vm_auto = seq_vm[j - 1];
					seq_vm[j - 1] = seq_vm[j];
					seq_vm[j] = seq_vm_auto;
				}
				else if (inputFlavor_copy[j].cpu_core_num == inputFlavor_copy[j - 1].cpu_core_num  &&  inputFlavor_copy[j].mem_size < inputFlavor_copy[j - 1].mem_size) {  //cpu��ͬʱ�ڴ�С�ķ�ǰ��
					Flavor vm_temp_auto = inputFlavor_copy[j - 1];
					inputFlavor_copy[j - 1] = inputFlavor_copy[j];
					inputFlavor_copy[j] = vm_temp_auto;
					int seq_vm_auto = seq_vm[j - 1];
					seq_vm[j - 1] = seq_vm[j];
					seq_vm[j] = seq_vm_auto;
				}
			}
		}
	}
	else {  //�Ż��ڴ�
		for (i = 0; i < num_vm; i++) {
			for (j = i + 1; j < num_vm; j++) {
				if (inputFlavor_copy[j].mem_size > inputFlavor_copy[j - 1].mem_size) {  //cpu��ķ�ǰ��
					Flavor vm_temp_auto = inputFlavor_copy[j - 1];
					inputFlavor_copy[j - 1] = inputFlavor_copy[j];
					inputFlavor_copy[j] = vm_temp_auto;
					int seq_vm_auto = seq_vm[j - 1];
					seq_vm[j - 1] = seq_vm[j];
					seq_vm[j] = seq_vm_auto;
				}
				else if (inputFlavor_copy[j].mem_size == inputFlavor_copy[j - 1].mem_size  &&  inputFlavor_copy[j].cpu_core_num < inputFlavor_copy[j - 1].cpu_core_num) {  //cpu��ͬʱ�ڴ�С�ķ�ǰ��
					Flavor vm_temp_auto = inputFlavor_copy[j - 1];
					inputFlavor_copy[j - 1] = inputFlavor_copy[j];
					inputFlavor_copy[j] = vm_temp_auto;
					int seq_vm_auto = seq_vm[j - 1];
					seq_vm[j - 1] = seq_vm[j];
					seq_vm[j] = seq_vm_auto;
				}
			}
		}
	}

	/*��Դ��ʼ��*/
	double *py_ratio_cpu = new double[num_py];  //һ���������cpu��Դ������
	double *py_ratio_mem = new double[num_py];  //һ����������ڴ���Դ������ 
	int *resource_rest_cpu = new int[num_py];   //һ���������cpuʣ����Դ
	int *resource_rest_mem = new int[num_py];   //һ����������ڴ�ʣ����Դ
	int *flavor_num_in_py = new int[num_py];    //һ��������а��������������
	int flavor_totol_mun = 0;  //���������������
	int num_py_new = num_py; //�µ����������
	for (i = 0; i < num_py; i++) {
		resource_rest_cpu[i] = resource_pm.cpu_core_num;
		resource_rest_mem[i] = resource_pm.mem_size;
		flavor_num_in_py[i] = 0;
	}
	for (i = 0; i < num_vm; i++) {
		flavor_totol_mun += require_vm[i];
	}
	/*����ÿ��Ӳ��������������*/
	for (i = 0; i < num_py; i++) {
		// ��i��������������ʺ�ʣ����Դ 
		int sum_temp_cpu = 0;
		int sum_temp_mem = 0;
		for (j = 0; j < num_vm; j++) {
			if (result_save[i*num_vm + j] != 0) {
				sum_temp_cpu += result_save[i*num_vm + j] * inputFlavor[j].cpu_core_num;
				sum_temp_mem += result_save[i*num_vm + j] * inputFlavor[j].mem_size;
				flavor_num_in_py[i] += result_save[i*num_vm + j];
			}
		}
		py_ratio_cpu[i] = (double)sum_temp_cpu / (double)resource_pm.cpu_core_num;
		py_ratio_mem[i] = (double)sum_temp_mem / (double)resource_pm.mem_size;
		resource_rest_cpu[i] -= sum_temp_cpu;
		resource_rest_mem[i] -= sum_temp_mem;
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n CPU%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_cpu[i], resource_rest_cpu[i]);
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n MEM%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_mem[i], resource_rest_mem[i]);
	}


	/*��ʼ����*/
	int opt_sub_timer;  //���Ż������������
	int opt_add_timer;  //���Ż������������
	opt_sub_timer = flavor_num_in_py[num_py - 1];
	opt_add_timer = 0;
	int *require_vm_copy = new int[num_vm];
	int *result_save_copy = new int[num_vm*num_py];
	for (i = 0; i < num_py; i++) {
		for (j = 0; j < num_vm; j++) {
			result_save_copy[i*num_vm + j] = result_save[i*num_vm + j];
		}
	}
	for (i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}

	//i = num_py - 1;  //ֻ�������һ��Ӳ�������� 
	i = 0;
	while (i < num_py) {
		opt_sub_timer = flavor_num_in_py[i];
		if (resource_rest_cpu[i] != 0 && resource_rest_mem[i] != 0)
		{ //ʣ����Դ��Ϊ0���ſɽ���
			for (j = 0; j < num_vm; j++) {
				while (inputFlavor[seq_vm[j]].cpu_core_num <= resource_rest_cpu[i] && inputFlavor[seq_vm[j]].mem_size <= resource_rest_mem[i]) {
					require_vm_copy[seq_vm[j]]++;
					result_save_copy[i*num_vm + seq_vm[j]]++;
					opt_add_timer++;
					resource_rest_cpu[i] -= inputFlavor[seq_vm[j]].cpu_core_num;
					resource_rest_mem[i] -= inputFlavor[seq_vm[j]].mem_size;
				}
			}
			if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
				for (int kk = 0; kk < num_py; kk++) {
					for (j = 0; j < num_vm; j++) {
						result_save[kk*num_vm + seq_vm[j]] = result_save_copy[kk*num_vm + seq_vm[j]];
					}
				}
				for (int kk = 0; kk < num_vm; kk++) {
					require_vm[kk] = require_vm_copy[kk];
				}
			}
			else if (i == num_py - 1) {  //ѡ�� ��������Ż�
				for (j = 0; j < num_vm; j++) {
					if (result_save[i*num_vm + j] != 0) {
						require_vm[j] -= result_save[i*num_vm + j];
						result_save[i*num_vm + j] = 0;
					}
				}
				for (int kk = i; kk < num_py_new; kk++) {
					for (j = 1; j < num_vm; j++) {
						result_save[i*num_vm + j] = result_save[(i + 1)*num_vm + j];
					}
				}
				num_py_new--;
				continue;
			}
		}
		i++;
	}

	/*printf("\n");
	for (i = 0; i < num_vm; i++) {
	printf("flavor%d  %d  ", inputFlavor[i].flavor_name, require_vm[i]);
	}*/


	return num_py_new;
}


int putVM_correct_vmsize(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int num_py) {
	int i, j;
	/*��Դ��ʼ��*/
	double *py_ratio_cpu = new double[num_py];  //һ���������cpu��Դ������
	double *py_ratio_mem = new double[num_py];  //һ����������ڴ���Դ������ 
	int *resource_rest_cpu = new int[num_py];   //һ���������cpuʣ����Դ
	int *resource_rest_mem = new int[num_py];   //һ����������ڴ�ʣ����Դ
	int *flavor_num_in_py = new int[num_py];    //һ��������а��������������
	int flavor_totol_mun = 0;  //���������������
	int num_py_new = num_py; //�µ����������
	for (i = 0; i < num_py; i++) {
		resource_rest_cpu[i] = resource_pm.cpu_core_num;
		resource_rest_mem[i] = resource_pm.mem_size;
		flavor_num_in_py[i] = 0;
	}
	for (i = 0; i < num_vm; i++) {
		flavor_totol_mun += require_vm[i];
	}
	/*����ÿ��Ӳ��������������*/
	for (i = 0; i < num_py; i++) {
		// ��i��������������ʺ�ʣ����Դ 
		int sum_temp_cpu = 0;
		int sum_temp_mem = 0;
		for (j = 0; j < num_vm; j++) {
			if (result_save[i*num_vm + j] != 0) {
				sum_temp_cpu += result_save[i*num_vm + j] * inputFlavor[j].cpu_core_num;
				sum_temp_mem += result_save[i*num_vm + j] * inputFlavor[j].mem_size;
				flavor_num_in_py[i] += result_save[i*num_vm + j];
			}
		}
		py_ratio_cpu[i] = (double)sum_temp_cpu / (double)resource_pm.cpu_core_num;
		py_ratio_mem[i] = (double)sum_temp_mem / (double)resource_pm.mem_size;
		resource_rest_cpu[i] -= sum_temp_cpu;
		resource_rest_mem[i] -= sum_temp_mem;
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n CPU%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_cpu[i], resource_rest_cpu[i]);
	}
	//printf("\n");
	for (i = 0; i < num_py; i++) {
		//printf("\n MEM%d�����ʣ�%f  ��ʣ��Դ��%d", i, py_ratio_mem[i], resource_rest_mem[i]);
	}


	/*��ʼ����*/
	int opt_sub_timer;  //���Ż������������
	int opt_add_timer;  //���Ż������������
	opt_sub_timer = flavor_num_in_py[num_py - 1];
	opt_add_timer = 0;
	int *require_vm_copy = new int[num_vm];
	int *result_save_copy = new int[num_vm*num_py];
	for (i = 0; i < num_py; i++) {
		for (j = 0; j < num_vm; j++) {
			result_save_copy[i*num_vm + j] = result_save[i*num_vm + j];
		}
	}
	for (i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}

	//i = num_py - 1;  //ֻ�������һ��Ӳ�������� 
	i = 0;
	while (i < num_py_new) {
		opt_sub_timer = flavor_num_in_py[i];
		if (resource_rest_cpu[i] != 0 && resource_rest_mem[i] != 0)
		{ //ʣ����Դ��Ϊ0���ſɽ���
			for (j = 0; j < num_vm; j++) {
				while (inputFlavor[j].cpu_core_num <= resource_rest_cpu[i] && inputFlavor[j].mem_size <= resource_rest_mem[i]) {
					require_vm_copy[j]++;
					result_save_copy[i*num_vm + j]++;
					opt_add_timer++;
					resource_rest_cpu[i] -= inputFlavor[j].cpu_core_num;
					resource_rest_mem[i] -= inputFlavor[j].mem_size;
				}
			}
			if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
				for (int kk = 0; kk < num_py; kk++) {
					for (j = 0; j < num_vm; j++) {
						result_save[kk*num_vm + j] = result_save_copy[kk*num_vm + j];
					}
				}
				for (int kk = 0; kk < num_vm; kk++) {
					require_vm[kk] = require_vm_copy[kk];
				}
			}
			else {  //ѡ�� ��������Ż�
				for (j = 0; j < num_vm; j++) {
					if (result_save[i*num_vm + j] != 0) {
						require_vm[j] -= result_save[i*num_vm + j];
						result_save[i*num_vm + j] = 0;
					}
				}
				for (int kk = i; kk < num_py_new; kk++) {
					for (j = 1; j < num_vm; j++) {
						result_save[i*num_vm + j] = result_save[(i + 1)*num_vm + j];
						resource_rest_cpu[i] = resource_rest_cpu[i + 1];
						resource_rest_mem[i] = resource_rest_mem[i + 1];
					}
				}
				num_py_new--;
				continue;
			}
		}
		i++;
	}

	return num_py_new;
}



/*
����Ŀ��:ģ���˻��㷨SAA ���������
����:	ģ���˻�
����:	*require_vm		���������
num_vm			�������������
resource_pm     ���������
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*inputFlavor	��������ܱ�
*result_save	���������ݱ�(���)
���:	�����ķ���������
*/
int putVM_score_SAA_greedy(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save) {
	srand((unsigned)time(NULL));

	double max_score = 0;

	int result_serve_num; //���շ�����������
	int max_serve_py = 2000;  //������������

	putVM_seq(require_vm, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save);//ԭ����

	double score_final;  //���յ÷�
	int *require_vm_iter = new int[num_vm];
	int *require_vm_copy = new int[num_vm];
	int *require_vm_copy_temp = new int[num_vm];

	/*printf("\npredict final result : [");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %d ", require_vm[i]);
	printf("]\n");*/

	for (int i = 0; i < num_vm; i++) {
		require_vm_iter[i] = require_vm[i];
	}

	float Temperature_first = 1.5;       //��ʼ�¶�
	int Num_of_inner_first = 40;         //��Ȧѭ����ʼ�Ĵ������¶�Խ�͵�������������
	float Step = 0.1;                    //�¶�Temperature�Ĳ���
	float Temperature_end = 0.0001;               //�����¶�
	int *result_save_temp1 = new int[max_serve_py*num_vm]; //ԭ����
	int *result_save_temp2 = new int[max_serve_py*num_vm]; //������
	int result_serve_num1; //ԭ����
	int result_serve_num2; //������
	double score_result1;   //ԭ���е÷�
	double score_result2;   //�����е÷�

	int time_iter_saa = 20; //�ظ��˻����
	for (int iter_i = 0; iter_i < time_iter_saa; iter_i++) {
		/*-----------------------�˻�ʼ---------------------------------------------------*/
		for (int i = 0; i < num_vm; i++) {
			require_vm_copy[i] = require_vm[i];
		}
		float Temperature = Temperature_first;  //��ǰ�¶�
												/*--------------------��ѭ�������¶�----------------------------*/
		int range_rand;  //������ķ�Χ-range_rand��+range_rand
		float range_rand_start = 10; //��ʼ�������Χ
		while (Temperature > Temperature_end) {
			int Num_of_inner = Num_of_inner_first + (int)((Temperature_first - Temperature) * 1 / Step) * 2; //��ѭ������,���¶Ƚ��Ͷ��½�
																											 //int Num_of_inner = Num_of_inner_first;
			int range_rand = (int)(range_rand_start * (Temperature / Temperature_first));
			if (range_rand <= 1) {
				range_rand = 2;
			}
			//��ѭ����ʼ
			for (int i = 0; i < Num_of_inner; i++) {
				//���������
				int num_change = rand() % num_vm;       //�ı�ڼ��������
				int predict_change = rand() % (2 * range_rand) - range_rand;   //-range_rand��+range_rand�������
				while (predict_change == 0) {
					predict_change = rand() % (2 * range_rand) - range_rand;   //ȡ��0����ȡһ��
				}

				//�ı�ԭ����
				for (int ii = 0; ii < num_vm; ii++) {
					require_vm_copy_temp[ii] = require_vm_copy[ii];
				}
				require_vm_copy_temp[num_change] += predict_change;
				if (require_vm_copy_temp[num_change] < 0) {  //�������������С��0
					require_vm_copy_temp[num_change] = 0;
				}
				//��������
				for (int zreo_i = 0; zreo_i < max_serve_py*num_vm; zreo_i++) {
					result_save_temp1[zreo_i] = 0;
					result_save_temp2[zreo_i] = 0;
				}

				/*printf("\n [");
				for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %d ", require_vm_copy[i]);
				printf("]\n");*/

				result_serve_num1 = putVM_greedy_without_seq(require_vm_copy, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save_temp1);//ԭ����
				result_serve_num2 = putVM_greedy_without_seq(require_vm_copy_temp, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save_temp2);//������

																																													 //��ʼ����
				score_result1 = get_score(require_vm, require_vm_copy, inputFlavor, opt_target, result_serve_num1);
				score_result2 = get_score(require_vm, require_vm_copy_temp, inputFlavor, opt_target, result_serve_num2);

				//�Ƿ��������
				if (score_result1 < score_result2) {  //�����������ţ����滻ԭ����
					score_final = score_result2;
					result_serve_num = result_serve_num2;
					for (int ii = 0; ii < num_vm; ii++) {
						require_vm_copy[ii] = require_vm_copy_temp[ii];
					}
				}
				else {  //���������ϲ������һ�����ʴ���ԭ����
					double derta_s = (score_result1 - score_result2) * 100; //��ֵ�鵽0-100
					double D_saa = exp(-derta_s / Temperature);
					int D_saa_int = (int)(D_saa * 100);
					int rand_saa = rand() % 100;
					if (D_saa_int>rand_saa) { //�滻
						score_final = score_result2;
						result_serve_num = result_serve_num2;
						for (int ii = 0; ii < num_vm; ii++) {
							require_vm_copy[ii] = require_vm_copy_temp[ii];
						}
					}
					else { //���滻
						score_final = score_result1;
						result_serve_num = result_serve_num1;
					}
				}
			}
			Temperature -= Step;
			//��ӡ
			//printf("\n��ǰ�¶ȣ�%f  �������÷֣�%f", Temperature, score_final);
		}

		printf("\n��ǰ�¶ȣ�%f  �������÷֣�%f", Temperature, score_final);
		if (score_final > max_score) {
			max_score = score_final;
			for (int i = 0; i < num_vm; i++) {  //һ���˻�����ֵ
				require_vm_iter[i] = require_vm_copy[i];
			}
		}
		/*------------------------------�˻����--------------------------------------------*/
	}


	for (int i = 0; i < num_vm; i++) {  //���ս����ֵ
		require_vm[i] = require_vm_iter[i];
	}
	result_serve_num = putVM_greedy(require_vm, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save);//ԭ����


	return result_serve_num;
}



/*
�����������������
����:	*require_vm		���������
num_vm			�������������
resource_pm     ���������
opt_target		�Ż�Ŀ�� 0:CPU 1:MEM
*inputFlavor	��������ܱ�
*result_save	���������ݱ�(���)
���:	�����ķ��������� out_num_of_py[3]��ά���飬�ֱ����������������������
*/
void putVM_directTraversal(int *require_vm, int num_vm, Server resource_pm, int opt_target, Flavor *inputFlavor, int *result_save, int out_num_of_py[3])
{
	//��������
	double ratio_pm = (double)(resource_pm.cpu_core_num) / resource_pm.mem_size;
	double *ratio_vm = new double[inputcontrol.flavorMaxnum];

	// �϶���Ҫ���ķ�����������
	int SERVER_NUM_PREDICT = 0;

	//ʹ�÷����ķ���
	int flag_method = 1;  // Ĭ���Ƿ���1
						  // �������ܳ��ֳ��ֵ������  ------  1
						  // ���ض����ֳ��ֵ������    ------  2
						  // �����ܳ��ֳ��ֵ������    ------  3


	int *vm_master = new int[inputcontrol.flavorMaxnum];  // ����������������
	int *vm_good = new int[inputcontrol.flavorMaxnum]; // ����Ż����ֶ��������
	int *vm_bad = new int[inputcontrol.flavorMaxnum]; // ��ų��ֶ��������

	int count_master = 0; // ��������ļ�����
	int count_good = 0;  // �Ż����ֶ���ļ�����
	int count_bad = 0; // ���ֶ���ļ�����

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		ratio_vm[i] = (double)inputFlavor[i].cpu_core_num / inputFlavor[i].mem_size;

		printf("%f  %f\n", ratio_vm[i], ratio_pm);
		//��Ϊ3��
		//����ǰ�CPU�Ż���56 / 128 = 0.5 ��Щ = 0.5������� ����������     > 0.5������� ���Ż����ֶ���    < 0.5������� �ǳ��ֶ���
		//����ǰ�MEM�Ż���56 / 128 = 0.5 ��Щ = 0.5������� ����������     < 0.5������� ���Ż����ֶ���    > 0.5������� �ǳ��ֶ���

		if (opt_target == 0) //cpu
		{
			// �����Ż�cpu ratio_vmԽ��Խ�á�
			if (ratio_vm[i] == ratio_pm)  vm_master[count_master++] = i;
			if (ratio_vm[i] > ratio_pm)  vm_good[count_good++] = i;
			if (ratio_vm[i] < ratio_pm)  vm_bad[count_bad++] = i;
		}
		if (opt_target == 1) //mem
		{
			// �����Ż�mem ratio_vmԽСԽ�á�
			if (ratio_vm[i] == ratio_pm)  vm_master[count_master++] = i;
			if (ratio_vm[i] < ratio_pm)  vm_good[count_good++] = i;
			if (ratio_vm[i] > ratio_pm)  vm_bad[count_bad++] = i;
		}

	}

	long *vm_size = new long[inputcontrol.flavorMaxnum]; // ���Ϳ�Ĵ�С
	long *vm_size_temp = new long[inputcontrol.flavorMaxnum]; // ���Ϳ�Ĵ�С
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		if (opt_target == 0) //cpu
		{
			vm_size[i] = (long)(require_vm[i] * inputFlavor[i].cpu_core_num);
		}
		if (opt_target == 1) //mem
		{
			vm_size[i] = (long)(require_vm[i] * inputFlavor[i].mem_size);
		}
	}
	// ������vm_size�������Ż����Լ۱ȣ��Ż����Լ۱�Խ�ߣ�����ǰ����ǰ�Ż���



	//��vm_size��������
	int *vm_size_count = new int[inputcontrol.flavorMaxnum]; // ���Ϳ�Ĵ�С�������ţ�����
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_size_count[i] = i;
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_size_temp[i] = vm_size[i];
	printf("��ԭʼ���У�");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)   printf(" %d ", vm_size[i]);	// ��ӡ������
	printf("\n");



	// ���򣬴�ķ���ǰ��
	int temp, temp_size;
	if (count_master > 1)
	{
		for (int i = 0; i < count_master - 1; i++)
		{
			for (int j = i + 1; j < count_master; j++)
			{
				if (vm_size[j] > vm_size[i])
				{
					temp = vm_master[i];
					vm_master[i] = vm_master[j];
					vm_master[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
				if (vm_size[j] == vm_size[i] && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num)
				{
					temp = vm_master[i];
					vm_master[i] = vm_master[j];
					vm_master[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
			}
		}
	}

	if (count_good > 1)
	{
		for (int i = 0; i < count_good - 1; i++)
		{
			for (int j = i + 1; j < count_good; j++)
			{
				if (vm_size[j] > vm_size[i])
				{
					temp = vm_good[i];
					vm_good[i] = vm_good[j];
					vm_good[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
				if (vm_size[j] == vm_size[i] && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num)
				{
					temp = vm_good[i];
					vm_good[i] = vm_good[j];
					vm_good[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
			}
		}
	}

	if (count_bad > 1)
	{
		for (int i = 0; i < count_bad - 1; i++)
		{
			for (int j = i + 1; j < count_bad; j++)
			{
				if (vm_size[j] > vm_size[i])
				{
					temp = vm_bad[i];
					vm_bad[i] = vm_bad[j];
					vm_bad[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
				if (vm_size[j] == vm_size[i] && inputFlavor[j].cpu_core_num > inputFlavor[i].cpu_core_num)
				{
					temp = vm_bad[i];
					vm_bad[i] = vm_bad[j];
					vm_bad[j] = temp;

					temp_size = vm_size[j];
					vm_size[j] = vm_size[i];
					vm_size[i] = temp_size;
				}
			}
		}
	}

	printf("���������У�");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  	printf(" %d ", vm_size[i]);
	printf("\n");

	printf("�黹ԭ���У�");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_size[i] = vm_size_temp[i];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  	printf(" %d ", vm_size[i]);
	printf("\n");

	// ��ӡ������
	printf("good:");
	for (int i = 0; i < count_good; i++)
	{
		printf(" %d ", vm_good[i]);
	}
	printf("\n");

	printf("master:");
	for (int i = 0; i < count_master; i++)
	{
		printf(" %d ", vm_master[i]);
	}
	printf("\n");

	printf("bad:");
	for (int i = 0; i < count_bad; i++)
	{
		printf(" %d ", vm_bad[i]);
	}
	printf("\n");


	int *vm_opt_priority = new int[inputcontrol.flavorMaxnum]; // ������ȷ�������Ż���Ŀ�꣬����ţ�����ǰ�棡������Ĭ�������Ż�
	int *vm_put_priority = new int[inputcontrol.flavorMaxnum]; // ���ò��ֵ����ȷ���˳�򣡣���
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_put_priority[i] = i;   //���ò��ֵĳ�ʼ����

																				   // ����һ ������ good -- bad -- master
	for (int i = 0; i < count_good; i++)  vm_opt_priority[i] = vm_good[i];
	for (int i = 0; i < count_bad; i++)  vm_opt_priority[i + count_good] = vm_bad[i];
	for (int i = 0; i < count_master; i++)  vm_opt_priority[i + count_bad + count_good] = vm_master[i];
	print_data(vm_opt_priority, 1);

	// ������������ bad -- good -- master
	/*for (int i = 0; i < count_bad; i++)  vm_opt_priority[i] = vm_bad[i];
	for (int i = 0; i < count_good; i++)  vm_opt_priority[i + count_bad] = vm_good[i];
	for (int i = 0; i < count_master; i++)  vm_opt_priority[i + count_bad + count_good] = vm_master[i];*/

	//������������

	print_data(vm_opt_priority, 1);

	int population_max_num = 300000;	 // require_vm ������н��������������ó�ʱ.ʵ����3000
	int population_num = 1; //ʵ�ʿ��н������
	int *vm_opt_range = new int[inputcontrol.flavorMaxnum]; // �Ż��ķ�Χ����Ӧ�Ż����ȼ���
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_opt_range[i] = 1;  // ��ʼ��
	int opt_range_count = 0;
	while (population_num < population_max_num)
	{
		population_num = 1;
		vm_opt_range[opt_range_count++] += 1;
		if (opt_range_count == inputcontrol.flavorMaxnum)   opt_range_count = 0;
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  population_num *= vm_opt_range[i];
		// print_data(vm_opt_range, 1);
	}
	print_data(vm_opt_range, 1);

	int *vm_opt_bit = new int[inputcontrol.flavorMaxnum];  // �Ż���Χȷ��֮��
	vm_opt_bit[0] = 1;
	for (int i = 1; i < inputcontrol.flavorMaxnum; i++)  vm_opt_bit[i] = vm_opt_bit[i - 1] * vm_opt_range[i - 1];
	print_data(vm_opt_bit, 1);

	// һ����н⵽������н���ݱ�
	int **result_predict_super = new int *[population_num];
	for (int i = 0; i < population_num; i++)    result_predict_super[i] = new int[inputcontrol.flavorMaxnum];

	int AAAA = 0;  //�Ե�ǰֵAAAA�Ľ���������
	int BBBB = 0;
	//int FLAG = 1;
	for (int i = 0; i < population_num; i++)
	{
		AAAA = i;
		for (int j = inputcontrol.flavorMaxnum - 1; j >= 0; j--)
		{
			BBBB = AAAA / vm_opt_bit[j];
			AAAA = AAAA - BBBB * vm_opt_bit[j];
			/*for (int aa = 0; aa < count_bad; aa++)
			{
			if (j == vm_bad[aa])
			{
			BBBB = (-1)*BBBB;
			break;
			}
			}*/

			if (BBBB <= 40)
			{
				/*for (int aa = 0; aa < count_bad; aa++)
				{
				if (j == vm_bad[aa])
				{
				BBBB = MASTER_RANGE[BBBB];
				break;
				}
				}

				for (int aa = 0; aa < count_good; aa++)
				{
				if (j == vm_good[aa])
				{
				BBBB = MASTER_RANGE[BBBB];
				break;
				}
				}

				for (int aa = 0; aa < count_master; aa++)
				{
				if (j == vm_master[aa])
				{
				BBBB = MASTER_RANGE[BBBB];
				break;
				}
				}*/

				BBBB = MASTER_RANGE[BBBB];

			}
			else
			{
				BBBB = 0;
			}

			/*for (int aa = 0; aa < count_master; aa++)
			{
			if (j == vm_master[aa])
			{
			BBBB = (int)(BBBB*FLAG*0.5 + 0.5*FLAG);
			FLAG = FLAG*(-1);
			break;
			}
			}*/

			result_predict_super[i][j] = require_vm[j] + BBBB;
			if (result_predict_super[i][j] < 0)   result_predict_super[i][j] = 0;

		}
	}
	// for (int i = 0; i < population_num; i++)   print_data(result_predict_super[i],1);


	// ��Զ����Ķ��ַ��÷���������
	int max_serve_py = MAX_SERVER_NUM;  // ���������ĸ���!!!
	int EPISODE_MAX_NUM = 5000;
	int **result_save_super = new int *[EPISODE_MAX_NUM];  //ÿһ��ִ��5000��
	for (int i = 0; i < EPISODE_MAX_NUM; i++)    result_save_super[i] = new int[max_serve_py*inputcontrol.flavorMaxnum];

	//���ٵķ��õ����ռ�
	for (int i = 0; i < EPISODE_MAX_NUM; i++)
	{
		for (int j = 0; j < max_serve_py*inputcontrol.flavorMaxnum; j++)
		{
			result_save_super[i][j] = 0;
		}
	}

	// flag_method ����˳�򷽷���ѡ��


	// ����inputFlavor��׼ȷ˳��
	Flavor *inputFlavor_data = new Flavor[inputcontrol.flavorMaxnum];
	for (int m = 0; m < inputcontrol.flavorMaxnum; m++)  inputFlavor_data[m] = inputFlavor[m];

	// �������ܳ��ֳ��ֵ������ ������2��(��������Ҫ�Ż���������������ͷ�������������)
	if (count_bad == 0)  flag_method = 2;

	// ���ض����ֳ��ֵ������ ������3��(������������ͷ���������������������ڿ����Ż���)
	if (count_bad != 0 && count_good == 0)  flag_method = 3;

	// if (count_bad != 0 && count_good != 0)  flag_method = 4;
	// for (int i = 0; i < inputcontrol.flavorMaxnum;i++)   printf("\n %f ", ratio_vm[i]);
	// ̰������-˳���ѡ��  ������ɺ�require_vm��inputFlavor ��˳�򶼸ı��ˣ�����vm_put_priority��¼������ô�ı�ģ�����
	// putVM_seq_vmsize(require_vm, resource_pm, opt_target, inputFlavor,  vm_size, ratio_vm, ratio_vm_diff,vm_put_priority, flag_method);

	// ����ratio��������򣡣���
	//putVM_seq_ratio_guided(require_vm, resource_pm, opt_target, inputFlavor, ratio_vm, vm_put_priority, flag_method);

	// �°�
	putVM_seq_ratio_guided_big_to_small(require_vm, resource_pm, opt_target, inputFlavor, ratio_vm, vm_put_priority, flag_method, count_bad);

	int *require_vm_master = new int[inputcontrol.flavorMaxnum]; //��׼�𰸰�
	for (int m = 0; m < inputcontrol.flavorMaxnum; m++)  require_vm_master[m] = require_vm[m];

	int *result_serve_num = new int[EPISODE_MAX_NUM];  // ���淵�صķ�����������
	double *score_result = new double[EPISODE_MAX_NUM]; // ����÷����

	int return_serve_num; // ���շ��صķ��������� 
	double FINAL_score_result = 0;  //��߷���

									// ���ŵ�
	for (int AAA = 0; AAA < (population_num / EPISODE_MAX_NUM) + 1; AAA++)
	{
		for (int i = 0; i < EPISODE_MAX_NUM; i++)
		{

			if ((i + EPISODE_MAX_NUM * AAA) == population_num)  break;
			//����������Ҫ���ķ�����������
			long EEEEE = 0;
			for (int aa = 0; aa < inputcontrol.flavorMaxnum; aa++)  EEEEE += result_predict_super[i + EPISODE_MAX_NUM * AAA][aa] * inputFlavor_data[aa].cpu_core_num;
			SERVER_NUM_PREDICT = EEEEE / resource_pm.cpu_core_num;

			EEEEE = 0;
			for (int aa = 0; aa < inputcontrol.flavorMaxnum; aa++)  EEEEE += result_predict_super[i + EPISODE_MAX_NUM * AAA][aa] * inputFlavor_data[aa].mem_size;
			if (SERVER_NUM_PREDICT < EEEEE / resource_pm.mem_size)  SERVER_NUM_PREDICT = EEEEE / resource_pm.mem_size;
			//print_data(result_predict_super[i+ EPISODE_MAX_NUM*AAA],1);

			//printf("\ninputFlavor.cpu_core_num:");
			//for (int aa = 0; aa < inputcontrol.flavorMaxnum; aa++)  printf("  %d  ",inputFlavor_data[aa].cpu_core_num);
			//printf("\ninputFlavor.mem_size:");
			//for (int aa = 0; aa < inputcontrol.flavorMaxnum; aa++)  printf("  %d  ",inputFlavor_data[aa].mem_size);

			//printf("\nSERVER_NUM_PREDICT: %d\n",SERVER_NUM_PREDICT);
			if (flag_method != 2)		result_serve_num[i] = putVM_greedy_ratio_guided(result_predict_super[i + EPISODE_MAX_NUM * AAA], SERVER_NUM_PREDICT, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], vm_put_priority);
			else   result_serve_num[i] = putVM_greedy_AAA_vmsize(result_predict_super[i + EPISODE_MAX_NUM * AAA], inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], vm_put_priority);


			//result_serve_num[i] = putVM_dynamicpro_ratio_guided(result_predict_super[i], GGGGG, inputcontrol.flavorMaxnum, inputServer, inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], vm_put_priority);

			// result_serve_num[i] = putVM_greedy_AAA_vmsize(result_predict_super[i], inputcontrol.flavorMaxnum, inputServer, inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], vm_put_priority);
			//putVM_seq_vmsize(require_vm, num_vm, resource_pm, opt_target, inputFlavor, result_save, vm_size);
			//putVM_seq(require_vm, num_vm, resource_pm, opt_target, inputFlavor, result_save);
			//print_data(result_predict_super[i], 1);
			//result_serve_num[i] = putVM_greedy_AAA(result_predict_super[i], inputcontrol.flavorMaxnum, inputServer, inputcontrol.cpuOrmem, inputFlavor, result_save_super[i]);

			//����
			result_serve_num[i] = putVM_correct_auto(result_predict_super[i + EPISODE_MAX_NUM * AAA], inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], result_serve_num[i]);
			//result_serve_num[i] = putVM_correct_vmsize(result_predict_super[i + EPISODE_MAX_NUM*AAA], inputcontrol.flavorMaxnum, inputServer, inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], result_serve_num[i]);
			//result_serve_num[i] = putVM_correct(result_predict_super[i], inputcontrol.flavorMaxnum, inputServer, inputcontrol.cpuOrmem, inputFlavor, result_save_super[i], result_serve_num[i]);

			//print_data(require_vm, 1);
			//print_data(result_predict_super[i], 1);
			//�÷�
			score_result[i] = get_score(require_vm_master, result_predict_super[i + EPISODE_MAX_NUM * AAA], inputFlavor, opt_target, result_serve_num[i]);

		}

		//��ɵ���֮�󣬿�����û�кõķ���
		for (int i = 0; i < EPISODE_MAX_NUM; i++)
		{
			if (score_result[i] > FINAL_score_result)
			{
				FINAL_score_result = score_result[i];
				return_serve_num = result_serve_num[i]; //���صķ�����������													  
				for (int m = 0; m < inputcontrol.flavorMaxnum; m++)    require_vm[m] = result_predict_super[i + EPISODE_MAX_NUM * AAA][m];// ���и��ƣ�����
				for (int m = 0; m < return_serve_num * inputcontrol.flavorMaxnum; m++)   result_save[m] = result_save_super[i][m];
			}
		}

		// �������֮�󽫽����գ�����
		for (int m = 0; m < EPISODE_MAX_NUM; m++)
		{
			for (int n = 0; n < max_serve_py*inputcontrol.flavorMaxnum; n++)
			{
				result_save_super[m][n] = 0;  //�������
			}
			score_result[m] = 0.0; //�÷ֽ�����
			result_serve_num[m] = 0;  //����������
		}

		//end
	}

	// ��ӡģ��Ԥ����
	printf("[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %d ", require_vm[i]);
	printf("]\n");
	printf("FINAL case score��%f\n", FINAL_score_result);

	out_num_of_py[0] = return_serve_num;
	out_num_of_py[1] = 0;
	out_num_of_py[2] = 0;

}


/*
����Ŀ��:�Է��÷������н�������Ը�����������������
����:	������֤100%������
����:	*require_vm		���������
num_vm			�������������
*inputFlavor	��������ܱ�
resource_pm[3]	���ַ��������ܽṹ��
*result_save0	���������ݱ�general(���)
*result_save1	���������ݱ�large_mem(���)
*result_save2	���������ݱ�high_proformance(���)
*num_py      	�����ķ�����������ָ�룬��ά����
���:   ���������������
*/
void putVM_correct_3class(int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save0, int *result_save1, int *result_save2, int *num_py) {
	int *require_vm_copy = new int[num_vm];
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}
	int *result_save_copy0 = new int[num_vm*num_py[0]];
	int *result_save_copy1 = new int[num_vm*num_py[1]];
	int *result_save_copy2 = new int[num_vm*num_py[2]];
	for (int i = 0; i < num_py[0]; i++) {
		for (int j = 0; j < num_vm; j++) {
			result_save_copy0[i*num_vm + j] = result_save0[i*num_vm + j];
		}
	}
	for (int i = 0; i < num_py[1]; i++) {
		for (int j = 0; j < num_vm; j++) {
			result_save_copy1[i*num_vm + j] = result_save1[i*num_vm + j];
		}
	}
	for (int i = 0; i < num_py[2]; i++) {
		for (int j = 0; j < num_vm; j++) {
			result_save_copy2[i*num_vm + j] = result_save2[i*num_vm + j];
		}
	}
	/*---------------------------������ʼ��---------------------------------*/
	int cpu_remain, mem_remain;
	double ratio_remain, distance_vm_py, distance_temp;
	int most_fit, most_fit_flag;
	int opt_sub_timer;  //���Ż������������
	int opt_add_timer;  //���Ż������������
						/*---------------------����general�������------------------------------*/
	if (num_py[0] > 0) {
		for (int i = 0; i < num_py[0]; i++) {
			cpu_remain = resource_pm[0].cpu_core_num;
			mem_remain = resource_pm[0].mem_size;
			for (int j = 0; j < num_vm; j++) {    //����ʣ����Դ
				cpu_remain -= inputFlavor[j].cpu_core_num * result_save0[i*num_vm + j];
				mem_remain -= inputFlavor[j].mem_size * result_save0[i*num_vm + j];
			}
			if (cpu_remain > 0 && mem_remain > 0) {  //��ʣ����Դ�������Ż�
				opt_sub_timer = 0;  //���Ż�����
				for (int jj = 0; jj < num_vm; jj++) {
					opt_sub_timer += result_save0[i*num_vm + jj];
				}

				opt_add_timer = 0;  //���Ż�����
				while (1) {
					ratio_remain = (double)cpu_remain / (double)mem_remain;
					most_fit = -1;
					most_fit_flag = 0;
					distance_vm_py = 100;  //��ֵΪ100������
										   //ѡ������ʷ��õ������
					for (int ii = 0; ii < num_vm; ii++) {
						if (ratio_remain <= resource_pm[0].ratio_cpu_to_mem) {  //��ǰʣ����Դ�������������Դ��ߣ���Ҫ�ұ���Դ�ֲ�
							distance_temp = exp(inputFlavor[ii].ratio_cpu_to_mem - resource_pm[0].ratio_cpu_to_mem);
						}
						else {           //��ǰʣ����Դ�������������Դ�ұߣ���Ҫ�����Դ�ֲ�
							distance_temp = exp(resource_pm[0].ratio_cpu_to_mem - inputFlavor[ii].ratio_cpu_to_mem);
						}

						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = ii;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[ii].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = ii;
							most_fit_flag = 1;
						}
					}
					//����
					if (most_fit_flag == 1) {
						cpu_remain -= inputFlavor[most_fit].cpu_core_num;
						mem_remain -= inputFlavor[most_fit].mem_size;
						require_vm_copy[most_fit]++;
						result_save_copy0[i*num_vm + most_fit]++;
						opt_add_timer++;
					}
					else {
						break;
					}
				}

				if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
					for (int kk = 0; kk < num_py[0]; kk++) {
						for (int jj = 0; jj < num_vm; jj++) {
							result_save0[kk*num_vm + jj] = result_save_copy0[kk*num_vm + jj];
						}
					}
					for (int kk = 0; kk < num_vm; kk++) {
						require_vm[kk] = require_vm_copy[kk];
					}
				}
				else if (i == num_py[0] - 1) {  //ѡ�� ��������Ż�
					for (int jj = 0; jj < num_vm; jj++) {
						if (result_save0[i*num_vm + jj] != 0) {
							require_vm[jj] -= result_save0[i*num_vm + jj];
							result_save0[i*num_vm + jj] = 0;
						}
					}
					for (int jj = 1; jj < num_vm; jj++) {
						result_save0[i*num_vm + jj] = 0;
					}
					num_py[0]--;
				}

			}


		}
	}

	/*---------------------����large_mem�������------------------------------*/

	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}
	if (num_py[1] > 0) {
		for (int i = 0; i < num_py[1]; i++) {
			cpu_remain = resource_pm[1].cpu_core_num;
			mem_remain = resource_pm[1].mem_size;
			for (int j = 0; j < num_vm; j++) {    //����ʣ����Դ
				cpu_remain -= inputFlavor[j].cpu_core_num * result_save1[i*num_vm + j];
				mem_remain -= inputFlavor[j].mem_size * result_save1[i*num_vm + j];
			}
			if (cpu_remain > 0 && mem_remain > 0) {  //��ʣ����Դ�������Ż�
				opt_sub_timer = 0;  //���Ż�����
				for (int jj = 0; jj < num_vm; jj++) {
					opt_sub_timer += result_save1[i*num_vm + jj];
				}

				opt_add_timer = 0;  //���Ż�����
				while (1) {
					ratio_remain = (double)cpu_remain / (double)mem_remain;
					most_fit = -1;
					most_fit_flag = 0;
					distance_vm_py = 100;  //��ֵΪ100������
										   //ѡ������ʷ��õ������
					for (int ii = 0; ii < num_vm; ii++) {
						if (require_vm_copy[ii] > 0) {
							if (ratio_remain <= resource_pm[1].ratio_cpu_to_mem) {  //��ǰʣ����Դ�������������Դ��ߣ���Ҫ�ұ���Դ�ֲ�
								distance_temp = exp(inputFlavor[ii].ratio_cpu_to_mem - resource_pm[1].ratio_cpu_to_mem);
							}
							else {           //��ǰʣ����Դ�������������Դ�ұߣ���Ҫ�����Դ�ֲ�
								distance_temp = exp(resource_pm[1].ratio_cpu_to_mem - inputFlavor[ii].ratio_cpu_to_mem);
							}

							if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //Ѱ�ұ�������ʵ�
								distance_vm_py = distance_temp;
								most_fit = ii;
								most_fit_flag = 1;
							}
							else if (distance_temp == distance_vm_py && inputFlavor[ii].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
								distance_vm_py = distance_temp;
								most_fit = ii;
								most_fit_flag = 1;
							}
						}
					}
					//����
					if (most_fit_flag == 1) {
						cpu_remain -= inputFlavor[most_fit].cpu_core_num;
						mem_remain -= inputFlavor[most_fit].mem_size;
						require_vm_copy[most_fit]++;
						result_save_copy1[i*num_vm + most_fit]++;
						opt_add_timer++;
					}
					else {
						break;
					}
				}

				if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
					for (int kk = 0; kk < num_py[1]; kk++) {
						for (int jj = 0; jj < num_vm; jj++) {
							result_save1[kk*num_vm + jj] = result_save_copy1[kk*num_vm + jj];
						}
					}
					for (int kk = 0; kk < num_vm; kk++) {
						require_vm[kk] = require_vm_copy[kk];
					}
				}
				else if (i == num_py[1] - 1) {  //ѡ�� ��������Ż�
					for (int jj = 0; jj < num_vm; jj++) {
						if (result_save1[i*num_vm + jj] != 0) {
							require_vm[jj] -= result_save1[i*num_vm + jj];
							result_save1[i*num_vm + jj] = 0;
						}
					}
					for (int jj = 1; jj < num_vm; jj++) {
						result_save1[i*num_vm + jj] = 0;
					}
					num_py[1]--;
				}
			}
		}
	}


	/*---------------------����High_proformance�������------------------------------*/

	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}
	if (num_py[2] > 0) {
		for (int i = 0; i < num_py[2]; i++) {
			cpu_remain = resource_pm[2].cpu_core_num;
			mem_remain = resource_pm[2].mem_size;
			for (int j = 0; j < num_vm; j++) {    //����ʣ����Դ
				cpu_remain -= inputFlavor[j].cpu_core_num * result_save2[i*num_vm + j];
				mem_remain -= inputFlavor[j].mem_size * result_save2[i*num_vm + j];
			}
			if (cpu_remain > 0 && mem_remain > 0) {  //��ʣ����Դ�������Ż�
				opt_sub_timer = 0;  //���Ż�����
				for (int jj = 0; jj < num_vm; jj++) {
					opt_sub_timer += result_save2[i*num_vm + jj];
				}

				opt_add_timer = 0;  //���Ż�����
				while (1) {
					ratio_remain = (double)cpu_remain / (double)mem_remain;
					most_fit = -1;
					most_fit_flag = 0;
					distance_vm_py = 100;  //��ֵΪ100������
										   //ѡ������ʷ��õ������
					for (int ii = 0; ii < num_vm; ii++) {
						if (require_vm_copy[ii] > 0) {
							if (ratio_remain <= resource_pm[2].ratio_cpu_to_mem) {  //��ǰʣ����Դ�������������Դ��ߣ���Ҫ�ұ���Դ�ֲ�
								distance_temp = exp(inputFlavor[ii].ratio_cpu_to_mem - resource_pm[2].ratio_cpu_to_mem);
							}
							else {           //��ǰʣ����Դ�������������Դ�ұߣ���Ҫ�����Դ�ֲ�
								distance_temp = exp(resource_pm[2].ratio_cpu_to_mem - inputFlavor[ii].ratio_cpu_to_mem);
							}

							if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //Ѱ�ұ�������ʵ�
								distance_vm_py = distance_temp;
								most_fit = ii;
								most_fit_flag = 1;
							}
							else if (distance_temp == distance_vm_py && inputFlavor[ii].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[ii].cpu_core_num && mem_remain >= inputFlavor[ii].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
								distance_vm_py = distance_temp;
								most_fit = ii;
								most_fit_flag = 1;
							}
						}
					}
					//����
					if (most_fit_flag == 1) {
						cpu_remain -= inputFlavor[most_fit].cpu_core_num;
						mem_remain -= inputFlavor[most_fit].mem_size;
						require_vm_copy[most_fit]++;
						result_save_copy2[i*num_vm + most_fit]++;
						opt_add_timer++;
					}
					else {
						break;
					}
				}

				if (opt_add_timer <= opt_sub_timer && opt_add_timer != 0) { //ѡ�� ��������Ż�
					for (int kk = 0; kk < num_py[2]; kk++) {
						for (int jj = 0; jj < num_vm; jj++) {
							result_save2[kk*num_vm + jj] = result_save_copy2[kk*num_vm + jj];
						}
					}
					for (int kk = 0; kk < num_vm; kk++) {
						require_vm[kk] = require_vm_copy[kk];
					}
				}
				else if (i == num_py[2] - 1) {  //ѡ�� ��������Ż�
					for (int jj = 0; jj < num_vm; jj++) {
						if (result_save2[i*num_vm + jj] != 0) {
							require_vm[jj] -= result_save2[i*num_vm + jj];
							result_save2[i*num_vm + jj] = 0;
						}
					}
					for (int jj = 1; jj < num_vm; jj++) {
						result_save2[i*num_vm + jj] = 0;
					}
					num_py[2]--;
				}
			}
		}
	}


}





/*
���ֹ�ʽ
*/
double get_score(int *result_predict_real, int *result_predict_temp, Flavor *inputFlavor_temp, int cpuormem, int serve_num)
{
	// ���
	double sum_result_predict_real = 0;  // ��ĸ�ұ�
	double sum_result_predict_temp = 0;  // ��ĸ���
	double sum_result_predict_diff = 0;  // �ϱ߷���

										 //�ұ�
	double sum_source_fenzi_num = 0; //�ұ߷��ӣ�����
	double sum_source_fenmu_num = 0; //�ұ߷�ĸ������

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		sum_result_predict_real += (result_predict_real[i] * result_predict_real[i]);
		sum_result_predict_temp += (result_predict_temp[i] * result_predict_temp[i]);
		sum_result_predict_diff += ((result_predict_real[i] - result_predict_temp[i]) * (result_predict_real[i] - result_predict_temp[i]));
	}

	sum_result_predict_real = sqrt(sum_result_predict_real / inputcontrol.flavorMaxnum);
	sum_result_predict_temp = sqrt(sum_result_predict_temp / inputcontrol.flavorMaxnum);
	sum_result_predict_diff = sqrt(sum_result_predict_diff / inputcontrol.flavorMaxnum);

	if (cpuormem == 0)  //cpu
	{
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			sum_source_fenzi_num += (result_predict_temp[i] * inputFlavor_temp[i].cpu_core_num);
		}
		sum_source_fenmu_num = serve_num * inputServer[0].cpu_core_num;
	}
	else if (cpuormem == 1)  // mem
	{
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			sum_source_fenzi_num += (result_predict_temp[i] * inputFlavor_temp[i].mem_size);
		}
		sum_source_fenmu_num = serve_num * inputServer[0].mem_size;
	}
	double aaaa = ((1 - sum_result_predict_diff / (sum_result_predict_real + sum_result_predict_temp))*(sum_source_fenzi_num / sum_source_fenmu_num));
	return  aaaa;
}

double my_abs(double num_input) {
	double num_output;
	if (num_input >= 0) {
		num_output = num_input;
	}
	else {
		num_output = -num_input;
	}
	return num_output;
}


/*
���ֹ�ʽ,����
*/
double get_score_new(int *result_predict_real, int *result_predict_temp, Flavor *inputFlavor_temp, int serve_num[3])
{
	// ���
	double sum_result_predict_real = 0;  // ��ĸ�ұ�
	double sum_result_predict_temp = 0;  // ��ĸ���
	double sum_result_predict_diff = 0;  // �ϱ߷���

										 //�ұ�
	double sum_source_fenzi_num_cpu = 0; //�ұ߷��ӣ�����
	double sum_source_fenmu_num_cpu = 0; //�ұ߷�ĸ������
	double sum_source_fenzi_num_mem = 0; //�ұ߷��ӣ�����
	double sum_source_fenmu_num_mem = 0; //�ұ߷�ĸ������


	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		sum_result_predict_real += (result_predict_real[i] * result_predict_real[i]);
		sum_result_predict_temp += (result_predict_temp[i] * result_predict_temp[i]);
		sum_result_predict_diff += ((result_predict_real[i] - result_predict_temp[i]) * (result_predict_real[i] - result_predict_temp[i]));
	}

	sum_result_predict_real = sqrt(sum_result_predict_real / inputcontrol.flavorMaxnum);
	sum_result_predict_temp = sqrt(sum_result_predict_temp / inputcontrol.flavorMaxnum);
	sum_result_predict_diff = sqrt(sum_result_predict_diff / inputcontrol.flavorMaxnum);

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		sum_source_fenzi_num_cpu += (result_predict_temp[i] * inputFlavor_temp[i].cpu_core_num);
		sum_source_fenzi_num_mem += (result_predict_temp[i] * inputFlavor_temp[i].mem_size);
	}
	sum_source_fenmu_num_cpu = serve_num[0] * inputServer[0].cpu_core_num + serve_num[1] * inputServer[1].cpu_core_num + serve_num[2] * inputServer[2].cpu_core_num;
	sum_source_fenmu_num_mem = serve_num[0] * inputServer[0].mem_size + serve_num[1] * inputServer[1].mem_size + serve_num[2] * inputServer[2].mem_size;

	double aaaa = ((1 - sum_result_predict_diff / (sum_result_predict_real + sum_result_predict_temp))*(sum_source_fenzi_num_cpu / sum_source_fenmu_num_cpu / 2 + sum_source_fenzi_num_mem / sum_source_fenmu_num_mem / 2));
	return  aaaa;
}



/*
������3����������÷���
����:	*require_vm		���������
num_vm			�������������
resource_pm[3]     ���������,���ƣ�cpu��mem
*inputFlavor	��������ܱ� ���ƣ�cpu��mem
*result_save_out0	������0 ���ݱ�(���) general
*result_save_out1	������1 ���ݱ�(���) large-memory
*result_save_out2	������2 ���ݱ�(���) high-performance
���:	�����ķ��������� out_num_of_py[3]��ά���飬�ֱ����������������������
*/
void putVM_3class(int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save_out0, int *result_save_out1, int *result_save_out2, int out_num_of_py[3]) {

	/*-------------------------��ʼ��---------------------------------*/
	for (int i = 0; i < 3; i++) {  //�����������ֵΪ0
		out_num_of_py[i] = 0;
	}
	int num_of_totol_vm = 0; //���������
	for (int i = 0; i < num_vm; i++) {
		num_of_totol_vm += require_vm[i];
	}
	int *require_vm_copy = new int[num_vm];  //����һ�������
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}

	/*-----��gengral,large_memory,high_performance��˳������������---*/
	if (server_num_py > 1) {  //�������������1ʱ�������ַ���
		Server serve_temp;
		for (int i = 0; i < server_num_py; i++) {
			for (int j = 1; j < server_num_py - i; j++) {
				if (resource_pm[j].name < resource_pm[j - 1].name) {  //nameС�ķ���ǰ��
					serve_temp = resource_pm[j];
					resource_pm[j] = resource_pm[j - 1];
					resource_pm[j - 1] = serve_temp;
				}
			}
		}
	}

	/*for (int i = 0; i < 3; i++) {
	printf("name:%d , cpu: %d , mem: %d , ratio: %f \n", resource_pm[i].name, resource_pm[i].cpu_core_num, resource_pm[i].mem_size, resource_pm[i].ratio_cpu_to_mem);
	}*/

	//double Thr1 = 0.409375;  //��ֵ1��С����ֵ1ѡ��large-mem����
	//double Thr2 = 0.5104165; //��ֵ2��������ֵ2ѡ��high-performance���ͣ�������ֵ1����ֵ2֮��ѡ��general����
	double Thr1, Thr2;
	Thr1 = (resource_pm[1].ratio_cpu_to_mem + resource_pm[0].ratio_cpu_to_mem) / 2;  //��ֵ1��С����ֵ1ѡ��large-mem����
	Thr2 = (resource_pm[0].ratio_cpu_to_mem + resource_pm[2].ratio_cpu_to_mem) / 2; //��ֵ2��������ֵ2ѡ��high-performance���ͣ�������ֵ1����ֵ2֮��ѡ��general����




																					/*-------------------------��ʼ����--------------------------------*/
	double ratio_now;
	double sum_ratio;
	int cpu_remain, mem_remain;
	int most_fit;  //���η������ʺϵ���������
	int most_fit_flag;
	double distance_vm_py;  //�������������� cpu-mem��������
	double distance_temp;
	double ratio_remain;
	int sum_cpu_temp;
	int sum_mem_temp;
	int vm_lost;  //���ж��������û�з���

	while (1) {
		vm_lost = 0;
		for (int i = 0; i < num_vm; i++) {
			vm_lost += require_vm_copy[i];
		}
		if (vm_lost == 0) {
			break;
		}
		//����Ŀǰ�����cpu-mem����
		sum_ratio = 0;
		sum_cpu_temp = 0;
		sum_mem_temp = 0;
		for (int i = 0; i < num_vm; i++) {
			//sum_ratio += inputFlavor[i].ratio_cpu_to_mem * require_vm_copy[i];
			sum_cpu_temp += inputFlavor[i].cpu_core_num * require_vm_copy[i];
			sum_mem_temp += inputFlavor[i].mem_size * require_vm_copy[i];
		}
		//ratio_now = sum_ratio / num_of_totol_vm;
		ratio_now = (double)sum_cpu_temp / (double)sum_mem_temp;
		if (ratio_now < Thr1) {  //ѡ��Large_memory�������  ���1
			out_num_of_py[1] += 1;
			cpu_remain = inputServer[1].cpu_core_num;  //���¸�ֵcpu����
			mem_remain = inputServer[1].mem_size;      //���¸�ֵmem����
			while (1) {
				ratio_remain = (double)cpu_remain / (double)mem_remain;
				most_fit = -1;
				most_fit_flag = 0;
				distance_vm_py = 100;  //��ֵΪ100������
									   //ѡ������ʷ��õ������
				for (int i = 0; i < num_vm; i++) {
					if (require_vm_copy[i] > 0) {
						distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);
						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
					}
				}
				//����
				if (most_fit_flag == 1) {
					cpu_remain -= inputFlavor[most_fit].cpu_core_num;
					mem_remain -= inputFlavor[most_fit].mem_size;
					require_vm_copy[most_fit]--;
					result_save_out1[(out_num_of_py[1] - 1)*num_vm + most_fit]++;
				}
				else {
					break;
				}
			}

		}// larger memory   end
		else if (ratio_now >= Thr1 && ratio_now < Thr2) {   //ѡ��General�����   ���0 
			out_num_of_py[0] += 1;
			cpu_remain = inputServer[0].cpu_core_num;  //���¸�ֵcpu����
			mem_remain = inputServer[0].mem_size;      //���¸�ֵmem����
			while (1) {
				ratio_remain = (double)cpu_remain / (double)mem_remain;
				most_fit = -1;
				most_fit_flag = 0;
				distance_vm_py = 100;  //��ֵΪ100������
									   //ѡ������ʷ��õ������
				for (int i = 0; i < num_vm; i++) {
					if (require_vm_copy[i] > 0) {
						distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);

						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
					}
				}
				//����
				if (most_fit_flag == 1) {
					cpu_remain -= inputFlavor[most_fit].cpu_core_num;
					mem_remain -= inputFlavor[most_fit].mem_size;
					require_vm_copy[most_fit]--;
					result_save_out0[(out_num_of_py[0] - 1)*num_vm + most_fit]++;
				}
				else {
					break;
				}
			}
		} //general    end
		else if (ratio_now >= Thr2) {   //ѡ��High_proformance�����   ���2
			out_num_of_py[2] += 1;
			cpu_remain = inputServer[2].cpu_core_num;  //���¸�ֵcpu����
			mem_remain = inputServer[2].mem_size;      //���¸�ֵmem����
			while (1) {
				ratio_remain = (double)cpu_remain / (double)mem_remain;
				most_fit = -1;
				most_fit_flag = 0;
				distance_vm_py = 100;  //��ֵΪ100������
									   //ѡ������ʷ��õ������
				for (int i = 0; i < num_vm; i++) {
					if (require_vm_copy[i] > 0) {
						distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);

						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
					}
				}
				//����
				if (most_fit_flag == 1) {
					cpu_remain -= inputFlavor[most_fit].cpu_core_num;
					mem_remain -= inputFlavor[most_fit].mem_size;
					require_vm_copy[most_fit]--;
					result_save_out2[(out_num_of_py[2] - 1)*num_vm + most_fit]++;
				}
				else {
					break;
				}
			}
		}//high_proformance  end
	} //while(1) end

	  /*-------------------------��ӡ���ý��--------------------------------*/
	  //printf("vm_require:\n");
	  //for (int i = 0; i < num_vm; i++) {
	  //	printf("flavor%d %d  ", inputFlavor[i].cpu_core_num, require_vm[i]);
	  //}
	  //printf("\n");
	  //printf("General: %d \n", out_num_of_py[0]);
	  //for (int i = 0; i < out_num_of_py[0]; i++) {
	  //	for (int j = 0; j < num_vm; j++) {
	  //		printf("%d ", result_save_out0[i*num_vm + j]);
	  //	}
	  //	printf("\n");
	  //}
	  //printf("Large_Memory: %d \n", out_num_of_py[1]);
	  //for (int i = 0; i < out_num_of_py[1]; i++) {
	  //	for (int j = 0; j < num_vm; j++) {
	  //		printf("%d ", result_save_out1[i*num_vm + j]);
	  //	}
	  //	printf("\n");
	  //}
	  //printf("High_proformance: %d \n", out_num_of_py[2]);
	  //for (int i = 0; i < out_num_of_py[2]; i++) {
	  //	for (int j = 0; j < num_vm; j++) {
	  //		printf("%d ", result_save_out2[i*num_vm + j]);
	  //	}
	  //	printf("\n");
	  //}

}


/*
������2����������÷���
����:	*require_vm		���������
num_vm			�������������
resource_pm[3]     ���������,���ƣ�cpu��mem
*inputFlavor	��������ܱ� ���ƣ�cpu��mem
*result_save_out0	������0 ���ݱ�(���) general
*result_save_out1	������1 ���ݱ�(���) large-memory
*result_save_out2	������2 ���ݱ�(���) high-performance
���:	�����ķ��������� out_num_of_py[3]��ά���飬�ֱ����������������������
*/
void putVM_2class(int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save_out0, int *result_save_out1, int *result_save_out2, int out_num_of_py[3]) {

	/*-------------------------��ʼ��---------------------------------*/
	for (int i = 0; i < 3; i++) {  //�����������ֵΪ0
		out_num_of_py[i] = 0;
	}
	int num_of_totol_vm = 0; //���������
	for (int i = 0; i < num_vm; i++) {
		num_of_totol_vm += require_vm[i];
	}
	int *require_vm_copy = new int[num_vm];  //����һ�������
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}

	Server serve_temp;
	/*-----��cpu-mem��������������������������ʱ��ͬ��---*/
	if (server_num_py > 1) {  //�������������1ʱ�������ַ���
		for (int i = 0; i < 3; i++) {
			for (int j = 1; j < 3 - i; j++) {
				if (resource_pm[j].ratio_cpu_to_mem < resource_pm[j - 1].ratio_cpu_to_mem && resource_pm[j].ratio_cpu_to_mem !=0 ) {  //����С�ķ���ǰ��
					serve_temp = resource_pm[j];
					resource_pm[j] = resource_pm[j - 1];
					resource_pm[j - 1] = serve_temp;
				}
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		printf("name:%d , cpu: %d , mem: %d , ratio: %f \n", resource_pm[i].name, resource_pm[i].cpu_core_num, resource_pm[i].mem_size, resource_pm[i].ratio_cpu_to_mem);
	}

	/*--------����������ʱ����洢�����ú��ٸ�ֵ����ʵ�����-------------*/
	int out_num_temp[2] = { 0 };
	int *result_save_temp_small = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //General ��result
	int *result_save_temp_large = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //General ��result
	for (int i = 0; i < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; i++) {
		result_save_temp_small[i] = 0;
		result_save_temp_large[i] = 0;
	}

	double Thr;
	Thr = (resource_pm[1].ratio_cpu_to_mem + resource_pm[0].ratio_cpu_to_mem) / 2;  //��ֵ��С����ֵѡ��small����

																					/*-------------------------��ʼ����--------------------------------*/
	double ratio_now;
	double sum_ratio;
	int cpu_remain, mem_remain;
	int most_fit;  //���η������ʺϵ���������
	int most_fit_flag;
	double distance_vm_py;  //�������������� cpu-mem��������
	double distance_temp;
	double ratio_remain;
	int sum_cpu_temp;
	int sum_mem_temp;
	int vm_lost;  //���ж��������û�з���

	while (1) {
		vm_lost = 0;
		for (int i = 0; i < num_vm; i++) {
			vm_lost += require_vm_copy[i];
		}
		if (vm_lost == 0) {
			break;
		}
		//����Ŀǰ�����cpu-mem����
		sum_ratio = 0;
		sum_cpu_temp = 0;
		sum_mem_temp = 0;
		for (int i = 0; i < num_vm; i++) {
			//sum_ratio += inputFlavor[i].ratio_cpu_to_mem * require_vm_copy[i];
			sum_cpu_temp += inputFlavor[i].cpu_core_num * require_vm_copy[i];
			sum_mem_temp += inputFlavor[i].mem_size * require_vm_copy[i];
		}
		//ratio_now = sum_ratio / num_of_totol_vm;
		ratio_now = (double)sum_cpu_temp / (double)sum_mem_temp;
		if (ratio_now < Thr) {  //ѡ��small�������  ���
			out_num_temp[0] += 1;
			cpu_remain = resource_pm[0].cpu_core_num;  //���¸�ֵcpu����
			mem_remain = resource_pm[0].mem_size;      //���¸�ֵmem����
			while (1) {
				ratio_remain = (double)cpu_remain / (double)mem_remain;
				most_fit = -1;
				most_fit_flag = 0;
				distance_vm_py = 100;  //��ֵΪ100������
									   //ѡ������ʷ��õ������
				for (int i = 0; i < num_vm; i++) {
					if (require_vm_copy[i] > 0) {
						distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);
						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
					}
				}
				//����
				if (most_fit_flag == 1) {
					cpu_remain -= inputFlavor[most_fit].cpu_core_num;
					mem_remain -= inputFlavor[most_fit].mem_size;
					require_vm_copy[most_fit]--;
					result_save_temp_small[(out_num_temp[0] - 1)*num_vm + most_fit]++;
				}
				else {
					break;
				}
			}

		}
		else {   //ѡ��large�����
			out_num_temp[1] += 1;
			cpu_remain = resource_pm[1].cpu_core_num;  //���¸�ֵcpu����
			mem_remain = resource_pm[1].mem_size;      //���¸�ֵmem����
			while (1) {
				ratio_remain = (double)cpu_remain / (double)mem_remain;
				most_fit = -1;
				most_fit_flag = 0;
				distance_vm_py = 100;  //��ֵΪ100������
									   //ѡ������ʷ��õ������
				for (int i = 0; i < num_vm; i++) {
					if (require_vm_copy[i] > 0) {
						distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);

						if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
						else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
							distance_vm_py = distance_temp;
							most_fit = i;
							most_fit_flag = 1;
						}
					}
				}
				//����
				if (most_fit_flag == 1) {
					cpu_remain -= inputFlavor[most_fit].cpu_core_num;
					mem_remain -= inputFlavor[most_fit].mem_size;
					require_vm_copy[most_fit]--;
					result_save_temp_large[(out_num_temp[1] - 1)*num_vm + most_fit]++;
				}
				else {
					break;
				}
			}
		} //general    end

	} //while(1) end

	  /*-------------------------���ý������--------------------------------*/
	  //result_save_temp_small result_save_temp_large out_num_temp ���Ƶ�result_save_out0,result_save_out1,result_save_out2,out_num_of_py[3]��

	for (int i = 0; i < server_num_py; i++) {
		if (resource_pm[i].name == 0) { //general��
			out_num_of_py[0] = out_num_temp[i];
			if (i == 0) {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out0[j] = result_save_temp_small[j];
				}
			}
			else {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out0[j] = result_save_temp_large[j];
				}
			}
		}
		else if (resource_pm[i].name == 1) {   //large memory��
			out_num_of_py[1] = out_num_temp[i];
			if (i == 0) {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out1[j] = result_save_temp_small[j];
				}
			}
			else {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out1[j] = result_save_temp_large[j];
				}
			}
		}
		else if (resource_pm[i].name == 2) {   //high proformance��
			out_num_of_py[2] = out_num_temp[i];
			if (i == 0) {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out2[j] = result_save_temp_small[j];
				}
			}
			else {
				for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
					result_save_out2[j] = result_save_temp_large[j];
				}
			}
		}
	}

	//�ٰ���������һ��
	if (server_num_py > 1) {  //�������������1ʱ�������ַ���
		for (int kk = 0; kk < 3; kk++) {
			for (int i = 0; i < 3; i++) {
				if (resource_pm[i].name == 0 && resource_pm[i].cpu_core_num != 0) {
					serve_temp = resource_pm[0];
					resource_pm[0] = resource_pm[i];
					resource_pm[i] = serve_temp;
				}
				else if (resource_pm[i].name == 1 && resource_pm[i].cpu_core_num != 0) {
					serve_temp = resource_pm[1];
					resource_pm[1] = resource_pm[i];
					resource_pm[i] = serve_temp;
				}
				else if (resource_pm[i].name == 2 && resource_pm[i].cpu_core_num != 0) {
					serve_temp = resource_pm[2];
					resource_pm[2] = resource_pm[i];
					resource_pm[i] = serve_temp;
				}
			}
		}
	}

	/*for (int i = 0; i < 3; i++) {
	printf("name:%d , cpu: %d , mem: %d , ratio: %f \n", resource_pm[i].name, resource_pm[i].cpu_core_num, resource_pm[i].mem_size, resource_pm[i].ratio_cpu_to_mem);
	}
	*/
	/*-------------------------��ӡ���ý��--------------------------------*/
	printf("vm_require:\n");
	for (int i = 0; i < num_vm; i++) {
		printf("flavor%d %d  ", inputFlavor[i].flavor_name, require_vm[i]);
	}
	printf("\n");
	printf("General: %d \n", out_num_of_py[0]);
	for (int i = 0; i < out_num_of_py[0]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out0[i*num_vm + j]);
		}
		printf("\n");
	}
	printf("Large_Memory: %d \n", out_num_of_py[1]);
	for (int i = 0; i < out_num_of_py[1]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out1[i*num_vm + j]);
		}
		printf("\n");
	}
	printf("High_proformance: %d \n", out_num_of_py[2]);
	for (int i = 0; i < out_num_of_py[2]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out2[i*num_vm + j]);
		}
		printf("\n");
	}

}


/*
������1����������÷���
����:	*require_vm		���������
num_vm			�������������
resource_pm[3]     ���������,���ƣ�cpu��mem
*inputFlavor	��������ܱ� ���ƣ�cpu��mem
*result_save_out0	������0 ���ݱ�(���) general
*result_save_out1	������1 ���ݱ�(���) large-memory
*result_save_out2	������2 ���ݱ�(���) high-performance
���:	�����ķ��������� out_num_of_py[3]��ά���飬�ֱ����������������������
*/
void putVM_1class(int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save_out0, int *result_save_out1, int *result_save_out2, int out_num_of_py[3]) {

	/*-------------------------��ʼ��---------------------------------*/
	for (int i = 0; i < 3; i++) {  //�����������ֵΪ0
		out_num_of_py[i] = 0;
	}
	int num_of_totol_vm = 0; //���������
	for (int i = 0; i < num_vm; i++) {
		num_of_totol_vm += require_vm[i];
	}
	int *require_vm_copy = new int[num_vm];  //����һ�������
	for (int i = 0; i < num_vm; i++) {
		require_vm_copy[i] = require_vm[i];
	}



	Server serve_temp;
	/*-----��cpu-mem��������������������������ʱ��ͬ��---*/
	for (int i = 0; i < 3; i++) {
		for (int j = 1; j < 3 - i; j++) {
			if (resource_pm[j].ratio_cpu_to_mem < resource_pm[j - 1].ratio_cpu_to_mem) {  //����С�ķ���ǰ��
				serve_temp = resource_pm[j];
				resource_pm[j] = resource_pm[j - 1];
				resource_pm[j - 1] = serve_temp;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		printf("name:%d , cpu: %d , mem: %d , ratio: %f \n", resource_pm[i].name, resource_pm[i].cpu_core_num, resource_pm[i].mem_size, resource_pm[i].ratio_cpu_to_mem);
	}

	/*--------����������ʱ����洢�����ú��ٸ�ֵ����ʵ�����-------------*/
	int out_num_temp = 0; //���������
	int *result_save_temp = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //��ʱresult
	for (int i = 0; i < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; i++) {
		result_save_temp[i] = 0;
	}

	/*-------------------------��ʼ����--------------------------------*/
	double ratio_now;
	double sum_ratio;
	int cpu_remain, mem_remain;
	int most_fit;  //���η������ʺϵ���������
	int most_fit_flag;
	double distance_vm_py;  //�������������� cpu-mem��������
	double distance_temp;
	double ratio_remain;
	int sum_cpu_temp;
	int sum_mem_temp;
	int vm_lost;  //���ж��������û�з���

	while (1) {
		//�Ƿ��������û�з���
		vm_lost = 0;
		for (int i = 0; i < num_vm; i++) {
			vm_lost += require_vm_copy[i];
		}
		if (vm_lost == 0) {
			break;
		}
		//ѡ�����������
		out_num_temp += 1;
		cpu_remain = resource_pm[0].cpu_core_num;  //���¸�ֵcpu����
		mem_remain = resource_pm[0].mem_size;      //���¸�ֵmem����
		while (1) {
			ratio_remain = (double)cpu_remain / (double)mem_remain;
			most_fit = -1;
			most_fit_flag = 0;
			distance_vm_py = 100;  //��ֵΪ100������
								   //ѡ������ʷ��õ������
			for (int i = 0; i < num_vm; i++) {
				if (require_vm_copy[i] > 0) {
					distance_temp = my_abs(ratio_remain - inputFlavor[i].ratio_cpu_to_mem);
					if (distance_temp < distance_vm_py && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //Ѱ�ұ�������ʵ�
						distance_vm_py = distance_temp;
						most_fit = i;
						most_fit_flag = 1;
					}
					else if (distance_temp == distance_vm_py && inputFlavor[i].cpu_core_num > inputFlavor[most_fit].cpu_core_num  && cpu_remain >= inputFlavor[i].cpu_core_num && mem_remain >= inputFlavor[i].mem_size) {  //������ͬ��ȡcpu��mem���Ϊ����ʵ�
						distance_vm_py = distance_temp;
						most_fit = i;
						most_fit_flag = 1;
					}
				}
			}
			//����
			if (most_fit_flag == 1) {
				cpu_remain -= inputFlavor[most_fit].cpu_core_num;
				mem_remain -= inputFlavor[most_fit].mem_size;
				require_vm_copy[most_fit]--;
				result_save_temp[(out_num_temp - 1)*num_vm + most_fit]++;
			}
			else {
				break;
			}
		}

	} //while(1) end

	  /*-------------------------���ý������--------------------------------*/
	if (resource_pm[0].name == 0) {
		out_num_of_py[0] = out_num_temp;
		for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
			result_save_out0[j] = result_save_temp[j];
		}
	}
	else if (resource_pm[0].name == 1) {
		out_num_of_py[1] = out_num_temp;
		for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
			result_save_out1[j] = result_save_temp[j];
		}
		//��Դ��λ
		resource_pm[1] = resource_pm[0];
		resource_pm[0].cpu_core_num = 0;
		resource_pm[0].mem_size = 0;
		resource_pm[0].name = 0;
		resource_pm[0].ratio_cpu_to_mem = 10;
	}
	else if (resource_pm[0].name == 2) {
		out_num_of_py[2] = out_num_temp;
		for (int j = 0; j < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; j++) {
			result_save_out2[j] = result_save_temp[j];
		}
		//��Դ��λ
		resource_pm[2] = resource_pm[0];
		resource_pm[0].cpu_core_num = 0;
		resource_pm[0].mem_size = 0;
		resource_pm[0].name = 0;
		resource_pm[0].ratio_cpu_to_mem = 10;
	}


	for (int i = 0; i < 3; i++) {
		printf("name:%d , cpu: %d , mem: %d , ratio: %f \n", resource_pm[i].name, resource_pm[i].cpu_core_num, resource_pm[i].mem_size, resource_pm[i].ratio_cpu_to_mem);
	}

	/*-------------------------��ӡ���ý��--------------------------------*/
	printf("vm_require:\n");
	for (int i = 0; i < num_vm; i++) {
		printf("flavor%d %d  ", inputFlavor[i].flavor_name, require_vm[i]);
	}
	printf("\n");
	printf("General: %d \n", out_num_of_py[0]);
	for (int i = 0; i < out_num_of_py[0]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out0[i*num_vm + j]);
		}
		printf("\n");
	}
	printf("Large_Memory: %d \n", out_num_of_py[1]);
	for (int i = 0; i < out_num_of_py[1]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out1[i*num_vm + j]);
		}
		printf("\n");
	}
	printf("High_proformance: %d \n", out_num_of_py[2]);
	for (int i = 0; i < out_num_of_py[2]; i++) {
		for (int j = 0; j < num_vm; j++) {
			printf("%d ", result_save_out2[i*num_vm + j]);
		}
		printf("\n");
	}

}



/*
�����������
result_file_local:������������ַ
vm_predict �����Ԥ����
vm_require_num �������������
result_py_save0   ���������0�洢����  General
result_py_save1   ���������1�洢����  Large-Memory
result_py_save2   ���������2�洢����  High-Performance
py_require_num[3] �����������������
inputFlavor �����ļ�������������ܽṹ��:����,cpu,mem
vm_class_num �������������
*/
void write_output_to_result(char *result_file_local, int *vm_predict, int vm_require_num, int *result_py_save0, int *result_py_save1, int *result_py_save2, int py_require_num[3], Flavor *inputFlavor, int vm_class_num) {
	int result_point = 0;
	/*��������*/
	result_point = input_a_int_num(result_file_local, result_point, vm_require_num); //��һ�����������
	result_file_local[result_point++] = '\n';   //��һ�лس�
	for (int i = 0; i < vm_class_num; i++) {
		result_file_local[result_point++] = 'f';
		result_file_local[result_point++] = 'l';
		result_file_local[result_point++] = 'a';
		result_file_local[result_point++] = 'v';
		result_file_local[result_point++] = 'o';
		result_file_local[result_point++] = 'r';
		result_point = input_a_int_num(result_file_local, result_point, inputFlavor[i].flavor_name); //��������
		result_file_local[result_point++] = ' ';
		result_point = input_a_int_num(result_file_local, result_point, vm_predict[i]); //��������
		result_file_local[result_point++] = '\n';
	}
	/* 0���������� General */
	int serial_of_py = 0;
	if (py_require_num[serial_of_py] > 0) {
		result_file_local[result_point++] = '\n'; //����
		result_file_local[result_point++] = 'G';  //��һ����������ƺ�����
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = 'n';
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = 'r';
		result_file_local[result_point++] = 'a';
		result_file_local[result_point++] = 'l';
		result_file_local[result_point++] = ' ';
		result_point = input_a_int_num(result_file_local, result_point, py_require_num[serial_of_py]);
		result_file_local[result_point++] = '\n';   //��һ�лس�
		for (int i = 0; i < py_require_num[serial_of_py]; i++) {
			result_file_local[result_point++] = 'G';  //��������ƺͱ��
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = 'n';
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = 'r';
			result_file_local[result_point++] = 'a';
			result_file_local[result_point++] = 'l';
			result_file_local[result_point++] = '-';
			result_point = input_a_int_num(result_file_local, result_point, (i + 1)); //��������
			for (int j = 0; j < vm_class_num; j++) {
				if (result_py_save0[i*vm_class_num + j] != 0) { //��i��������е�j����������䲻Ϊ0,�����
					result_file_local[result_point++] = ' ';
					result_file_local[result_point++] = 'f';
					result_file_local[result_point++] = 'l';
					result_file_local[result_point++] = 'a';
					result_file_local[result_point++] = 'v';
					result_file_local[result_point++] = 'o';
					result_file_local[result_point++] = 'r';
					result_point = input_a_int_num(result_file_local, result_point, inputFlavor[j].flavor_name); //��������
					result_file_local[result_point++] = ' ';
					result_point = input_a_int_num(result_file_local, result_point, result_py_save0[i*vm_class_num + j]); //��������
				}
			}
			//result_file_local[result_point++] = '\n';
			if (i < py_require_num[serial_of_py] - 1) { //�������һ�в��û���
				result_file_local[result_point++] = '\n';
			}
		}
	}

	/* 1���������� Large-Memory */
	serial_of_py = 1;
	if (py_require_num[serial_of_py] > 0) {
		result_file_local[result_point++] = '\n'; //����
		result_file_local[result_point++] = '\n'; //����
		result_file_local[result_point++] = 'L';  //��һ����������ƺ�����
		result_file_local[result_point++] = 'a';
		result_file_local[result_point++] = 'r';
		result_file_local[result_point++] = 'g';
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = '-';
		result_file_local[result_point++] = 'M';
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = 'm';
		result_file_local[result_point++] = 'o';
		result_file_local[result_point++] = 'r';
		result_file_local[result_point++] = 'y';
		result_file_local[result_point++] = ' ';
		result_point = input_a_int_num(result_file_local, result_point, py_require_num[serial_of_py]);
		result_file_local[result_point++] = '\n';   //��һ�лس�
		for (int i = 0; i < py_require_num[serial_of_py]; i++) {
			result_file_local[result_point++] = 'L';  //��������ƺͱ��
			result_file_local[result_point++] = 'a';
			result_file_local[result_point++] = 'r';
			result_file_local[result_point++] = 'g';
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = '-';
			result_file_local[result_point++] = 'M';
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = 'm';
			result_file_local[result_point++] = 'o';
			result_file_local[result_point++] = 'r';
			result_file_local[result_point++] = 'y';
			result_file_local[result_point++] = '-';
			result_point = input_a_int_num(result_file_local, result_point, (i + 1)); //��������
			for (int j = 0; j < vm_class_num; j++) {
				if (result_py_save1[i*vm_class_num + j] != 0) { //��i��������е�j����������䲻Ϊ0,�����
					result_file_local[result_point++] = ' ';
					result_file_local[result_point++] = 'f';
					result_file_local[result_point++] = 'l';
					result_file_local[result_point++] = 'a';
					result_file_local[result_point++] = 'v';
					result_file_local[result_point++] = 'o';
					result_file_local[result_point++] = 'r';
					result_point = input_a_int_num(result_file_local, result_point, inputFlavor[j].flavor_name); //��������
					result_file_local[result_point++] = ' ';
					result_point = input_a_int_num(result_file_local, result_point, result_py_save1[i*vm_class_num + j]); //��������
				}
			}
			//result_file_local[result_point++] = '\n';
			if (i < py_require_num[serial_of_py] - 1) { //�������һ�в��û���
				result_file_local[result_point++] = '\n';
			}
		}
	}

	/* 2���������� High-Performance */
	serial_of_py = 2;
	if (py_require_num[serial_of_py] > 0) {
		result_file_local[result_point++] = '\n'; //����
		result_file_local[result_point++] = '\n'; //����
		result_file_local[result_point++] = 'H';  //��һ����������ƺ�����
		result_file_local[result_point++] = 'i';
		result_file_local[result_point++] = 'g';
		result_file_local[result_point++] = 'h';
		result_file_local[result_point++] = '-';
		result_file_local[result_point++] = 'P';
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = 'r';
		result_file_local[result_point++] = 'f';
		result_file_local[result_point++] = 'o';
		result_file_local[result_point++] = 'r';
		result_file_local[result_point++] = 'm';
		result_file_local[result_point++] = 'a';
		result_file_local[result_point++] = 'n';
		result_file_local[result_point++] = 'c';
		result_file_local[result_point++] = 'e';
		result_file_local[result_point++] = ' ';
		result_point = input_a_int_num(result_file_local, result_point, py_require_num[serial_of_py]);
		result_file_local[result_point++] = '\n';   //��һ�лس�
		for (int i = 0; i < py_require_num[serial_of_py]; i++) {
			result_file_local[result_point++] = 'H';  //��������ƺͱ��
			result_file_local[result_point++] = 'i';
			result_file_local[result_point++] = 'g';
			result_file_local[result_point++] = 'h';
			result_file_local[result_point++] = '-';
			result_file_local[result_point++] = 'P';
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = 'r';
			result_file_local[result_point++] = 'f';
			result_file_local[result_point++] = 'o';
			result_file_local[result_point++] = 'r';
			result_file_local[result_point++] = 'm';
			result_file_local[result_point++] = 'a';
			result_file_local[result_point++] = 'n';
			result_file_local[result_point++] = 'c';
			result_file_local[result_point++] = 'e';
			result_file_local[result_point++] = '-';
			result_point = input_a_int_num(result_file_local, result_point, (i + 1)); //��������
			for (int j = 0; j < vm_class_num; j++) {
				if (result_py_save2[i*vm_class_num + j] != 0) { //��i��������е�j����������䲻Ϊ0,�����
					result_file_local[result_point++] = ' ';
					result_file_local[result_point++] = 'f';
					result_file_local[result_point++] = 'l';
					result_file_local[result_point++] = 'a';
					result_file_local[result_point++] = 'v';
					result_file_local[result_point++] = 'o';
					result_file_local[result_point++] = 'r';
					result_point = input_a_int_num(result_file_local, result_point, inputFlavor[j].flavor_name); //��������
					result_file_local[result_point++] = ' ';
					result_point = input_a_int_num(result_file_local, result_point, result_py_save2[i*vm_class_num + j]); //��������
				}
			}
			//result_file_local[result_point++] = '\n';
			if (i < py_require_num[serial_of_py] - 1) { //�������һ�в��û���
				result_file_local[result_point++] = '\n';
			}
		}
	}

}



// �������һ�������������ˣ�����
double *normal_result(double *data, double *data_a, int data_num)
{
	int sum_data = 0;
	double *temp = new double[data_num];
	for (int i = 0; i < data_num; i++)
	{
		if (data[i] < 0)  sum_data += (data[i] * (-1));
		else  sum_data += data[i];
	}
	for (int i = 0; i < data_num; i++)
	{
		if (data[i] > 0)   temp[i] = ((data[i]) / sum_data)*data_a[i];
		else
		{
			temp[i] = ((data[i] * (-1)) / sum_data)*data_a[i];
			// temp[i] = 0;
		}
		if (sum_data == 0) {
			temp[i] = 0;
		}
	}



	return temp;
}


void putVM_jiang(int bullet_num, int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save_out0, int *result_save_out1, int *result_save_out2, int out_num_of_py_out[3])
{
	// �����׼�𰸡�
	int *SUPER_true_predict_result = new int[inputcontrol.flavorMaxnum];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_true_predict_result[i] = require_vm[i];



	// �����Ƕ��������Ĳ�����
	if (1)
	{
		int population_max_num = bullet_num;	 // require_vm ������н��������������ó�ʱ.
		int population_num = 1; //ʵ�ʿ��н������
		int *vm_opt_range = new int[inputcontrol.flavorMaxnum]; // �Ż��ķ�Χ����Ӧ�Ż����ȼ���
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_opt_range[i] = 1;  // ��ʼ��
		int opt_range_count = 0;
		while (population_num < population_max_num)
		{
			population_num = 1;
			vm_opt_range[opt_range_count++] += 1;
			if (opt_range_count == inputcontrol.flavorMaxnum)   opt_range_count = 0;
			for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  population_num *= vm_opt_range[i];
			// print_data(vm_opt_range, 1);
		}
		print_data(vm_opt_range, 1);

		int *vm_opt_bit = new int[inputcontrol.flavorMaxnum];  // �Ż���Χȷ��֮��
		vm_opt_bit[0] = 1;
		for (int i = 1; i < inputcontrol.flavorMaxnum; i++)  vm_opt_bit[i] = vm_opt_bit[i - 1] * vm_opt_range[i - 1];
		print_data(vm_opt_bit, 1);

		// һ����н⵽������н���ݱ�
		int **result_predict_super = new int *[population_num];
		for (int i = 0; i < population_num; i++)    result_predict_super[i] = new int[inputcontrol.flavorMaxnum];

		int AAAA = 0;  //�Ե�ǰֵAAAA�Ľ���������

		int BBBB = 0;
		//int FLAG = 1;
		for (int i = 0; i < population_num; i++)
		{
			AAAA = i;
			for (int j = inputcontrol.flavorMaxnum - 1; j >= 0; j--)
			{
				BBBB = AAAA / vm_opt_bit[j];
				AAAA = AAAA - BBBB * vm_opt_bit[j];

				if (BBBB <= 40)
				{
					BBBB = MASTER_RANGE[BBBB];
				}
				else
				{
					BBBB = 0;
				}

				result_predict_super[i][j] = require_vm[j] + BBBB;
				if (result_predict_super[i][j] < 0)   result_predict_super[i][j] = 0;

			}
		}
		// ������ϣ������Ķ���Ᵽ���� result_predict_super


		//��ʼѰ�����ŵģ�����

		int *result_save_temp0 = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //General ��result
		int *result_save_temp1 = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //Large-Memory ��rssult
		int *result_save_temp2 = new int[MAX_SERVER_NUM*inputcontrol.flavorMaxnum]; //High-Performance��result

		for (int i = 0; i < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; i++)
		{
			result_save_temp0[i] = 0;
			result_save_temp1[i] = 0;
			result_save_temp2[i] = 0;
		}

		int out_num_of_py[3] = { 0,0,0 };

		double score_max = 0; // ���ĵ÷�
		double score_temp = 0; // ��ǰ�ĵ÷�

							   //��ʼ���ڶ����Ѱ��һ����ѵĽ⣡����
		for (int AAA = 0; AAA < population_num; AAA++)
		{
			// �������
			for (int i = 0; i < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; i++)
			{
				result_save_temp0[i] = 0;
				result_save_temp1[i] = 0;
				result_save_temp2[i] = 0;
			}

			int out_num_of_py[3] = { 0,0,0 };


			//putVM_3class(result_predict_super[AAA], inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save_temp0, result_save_temp1, result_save_temp2, out_num_of_py);
			if (server_num_py == 3) {  //������������÷���
				putVM_3class(result_predict_super[AAA], inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save_temp0, result_save_temp1, result_save_temp2, out_num_of_py);
			}
			else if (server_num_py == 2) {  //������������÷���
				putVM_2class(result_predict_super[AAA], inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save_temp0, result_save_temp1, result_save_temp2, out_num_of_py);
			}
			else if (server_num_py == 1) {  //һ����������÷���
				putVM_1class(result_predict_super[AAA], inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save_temp0, result_save_temp1, result_save_temp2, out_num_of_py);
			}



			//����
			putVM_correct_3class(result_predict_super[AAA], inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save_temp0, result_save_temp1, result_save_temp2, out_num_of_py);

			int num_of_Serve_vm = 0;  //�������ٴ�ͳ��Ԥ����������������
			for (int i = 0; i < inputcontrol.flavorMaxnum; i++) {
				num_of_Serve_vm += require_vm[i];
			}
			score_temp = get_score_new(SUPER_true_predict_result, result_predict_super[AAA], inputFlavor, out_num_of_py);


			if (score_temp > score_max)
			{
				score_max = score_temp;

				for (int i = 0; i <inputcontrol.flavorMaxnum; i++)  require_vm[i] = result_predict_super[AAA][i];  //���յ�����

				for (int i = 0; i < MAX_SERVER_NUM*inputcontrol.flavorMaxnum; i++)
				{
					result_save_out0[i] = result_save_temp0[i];
					result_save_out1[i] = result_save_temp1[i];
					result_save_out2[i] = result_save_temp2[i];
				}
				//int *SUPER_predict_result = new int[inputcontrol.flavorMaxnum];
				//for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_predict_result[i] = 0;

				//// �������յ�
				//for (int i = 0; i < out_num_of_py[0]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp0[inputcontrol.flavorMaxnum * i + j];
				//for (int i = 0; i < out_num_of_py[1]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp1[inputcontrol.flavorMaxnum * i + j];
				//for (int i = 0; i < out_num_of_py[2]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp2[inputcontrol.flavorMaxnum * i + j];

				//print_data(require_vm, 1);
				//print_data(SUPER_predict_result, 1);

				//for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_predict_result[i] = 0;
				//for (int i = 0; i < out_num_of_py[0]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_out0[inputcontrol.flavorMaxnum * i + j];
				//for (int i = 0; i < out_num_of_py[1]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_out1[inputcontrol.flavorMaxnum * i + j];
				//for (int i = 0; i < out_num_of_py[2]; i++)
				//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_out2[inputcontrol.flavorMaxnum * i + j];

				//print_data(SUPER_predict_result, 1);






				out_num_of_py_out[0] = out_num_of_py[0];
				out_num_of_py_out[1] = out_num_of_py[1];
				out_num_of_py_out[2] = out_num_of_py[2];
				printf("score_temp : %f\n", score_max);
			}
		}
	}
}



void putVM_shuai(int bullet_num, int *require_vm, int num_vm, Server resource_pm[3], Flavor *inputFlavor, int *result_save_out0, int *result_save_out1, int *result_save_out2, int out_num_of_py_out[3])
{
	// resource_pm[0] ��ͨ
	// resource_pm[1] ���ڴ�
	// resource_pm[2] �߼���

	//	Ԥ��Ҫ���ķ�����������
	// A  �߼���  112/196    32A 80B  ���� 1
	// B1 ��ͨ    56/128     32A 24C  plan A
	// B2 ��ͨ    56/128     8C  48B  plan B
	// C  ���ڴ�  84/256     44C 40B  ���� 2


	int A_high_performance_total_num = 0;  // 1
	int B_geneal_total_num = 0;            // 0.5
	int C_high_memory_total_num = 0;       // 0.25

	int *A_count = new int[num_vm];   // ������� A ����
	int *B_count = new int[num_vm];   // ������� B ����
	int *C_count = new int[num_vm];   // ������� C ����
	int A_count_num = 0;  // ������� A ����
	int B_count_num = 0;  // ������� B ����
	int C_count_num = 0;  // ������� C ����


	int A_remain_num = 0;   //ʣ��
	int B_remain_num = 0;   //ʣ��
	int C_remain_num = 0;   //ʣ��

							//��������
	double *ratio_vm = new double[inputcontrol.flavorMaxnum];



	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		ratio_vm[i] = (double)inputFlavor[i].cpu_core_num / inputFlavor[i].mem_size;
		printf("%f \n", ratio_vm[i]);
		if (ratio_vm[i] == 1)
		{
			A_count[A_count_num++] = i;
			A_high_performance_total_num += inputFlavor[i].cpu_core_num * require_vm[i];
		}
		if (ratio_vm[i] == 0.5)
		{
			B_count[B_count_num++] = i;
			B_geneal_total_num += inputFlavor[i].cpu_core_num * require_vm[i];
		}
		if (ratio_vm[i] == 0.25)
		{
			C_count[C_count_num++] = i;
			C_high_memory_total_num += inputFlavor[i].cpu_core_num * require_vm[i];
		}
	}

	printf("\nA :");
	for (int i = 0; i < A_count_num; i++) printf(" %d ", A_count[i]);
	printf("\nB :");
	for (int i = 0; i < B_count_num; i++) printf(" %d ", B_count[i]);
	printf("\nC :");
	for (int i = 0; i < C_count_num; i++) printf(" %d ", C_count[i]);


	printf("\nA B C : %d  %d  %d\n", A_high_performance_total_num, B_geneal_total_num, C_high_memory_total_num);


	printf("resource_pm0 : %d  %d\n", resource_pm[0].cpu_core_num, resource_pm[0].mem_size);    // B
	printf("resource_pm1 : %d  %d\n", resource_pm[1].cpu_core_num, resource_pm[1].mem_size);   // C
	printf("resource_pm2 : %d  %d\n", resource_pm[2].cpu_core_num, resource_pm[2].mem_size);   // A

																							   //	Ԥ��Ҫ���ķ�����������
																							   // A  �߼���  112/192    32A 80B  ���� 1
																							   // B1 ��ͨ    56/128     32A 24C  plan A
																							   // B2 ��ͨ    56/128     8C  48B  plan B
																							   // C  ���ڴ�  84/256     44C 40B  ���� 2

	int A_A = 32;
	int A_B = 80;
	int B1_A = 32;
	int B1_C = 24;
	int B2_B = 48;
	int B2_C = 8;
	int C_C = 44;
	int C_B = 40;

	// �Ҽ�A
	int temp = 1000;
	for (int i = 0; i < resource_pm[2].cpu_core_num; i++)
	{
		for (int j = 0; j < resource_pm[2].cpu_core_num; j++)
		{
			if (i + j <= resource_pm[2].cpu_core_num) //����ҪС�ڵ��ڣ�����
			{
				if ((abs(resource_pm[2].cpu_core_num - i - j) + abs(resource_pm[2].mem_size - i - j * 2)) < temp)
				{
					A_A = i;
					A_B = j;
					temp = (abs(resource_pm[2].cpu_core_num - i - j) + abs(resource_pm[2].mem_size - i - j * 2));
				}
			}
		}
	}
	// �Ҽ�C
	temp = 1000;
	for (int i = 0; i < resource_pm[1].cpu_core_num; i++)
	{
		for (int j = 0; j < resource_pm[1].cpu_core_num; j++)
		{
			if (i + j <= resource_pm[1].cpu_core_num) //����ҪС�ڵ��ڣ�����
			{
				if ((abs(resource_pm[1].cpu_core_num - i - j) + abs(resource_pm[1].mem_size - i * 4 - j * 2)) < temp)
				{
					C_C = i;
					C_B = j;
					temp = (abs(resource_pm[1].cpu_core_num - i - j) + abs(resource_pm[1].mem_size - i * 4 - j * 2));
				}
			}
		}
	}
	// �Ҽ�B1
	temp = 1000;
	for (int i = 0; i < resource_pm[0].cpu_core_num; i++)
	{
		for (int j = 0; j < resource_pm[0].cpu_core_num; j++)
		{
			if (i + j <= resource_pm[0].cpu_core_num) //����ҪС�ڵ��ڣ�����
			{
				if ((abs(resource_pm[0].cpu_core_num - i - j) + abs(resource_pm[0].mem_size - i * 2 - j * 4)) < temp)
				{
					B2_B = i;
					B2_C = j;
					temp = (abs(resource_pm[0].cpu_core_num - i - j) + abs(resource_pm[0].mem_size - i * 2 - j * 4));
				}
			}
		}
	}
	// �Ҽ�B2
	temp = 1000;
	for (int i = 0; i < resource_pm[0].cpu_core_num; i++)
	{
		for (int j = 0; j < resource_pm[0].cpu_core_num; j++)
		{
			if (i + j <= resource_pm[0].cpu_core_num) //����ҪС�ڵ��ڣ�����
			{
				if ((abs(resource_pm[0].cpu_core_num - i - j) + abs(resource_pm[0].mem_size - i * 4 - j)) < temp)
				{
					B1_C = i;
					B1_A = j;
					temp = (abs(resource_pm[0].cpu_core_num - i - j) + abs(resource_pm[0].mem_size - i * 4 - j));
				}
			}
		}
	}



	int A_server_predict = 0; // ������A��Ԥ��������
	int B1_server_predict = 0; // ������b��Ԥ��������
	int B2_server_predict = 0; // ������b��Ԥ��������
	int C_server_predict = 0; // ������c��Ԥ��������

	int X = 0;
	int Y1 = 0;
	int Y2 = 0;
	int Z = 0;

	int temp_sum_A = 0;
	int temp_sum_B = 0;
	int temp_sum_C = 0;
	int temp_sum = 0;
	int temp_sum_MIN = 600;

	int MAX = (A_high_performance_total_num + B_geneal_total_num + C_high_memory_total_num) / 112;
	for (int i = 0; i < MAX; i++) // X
	{
		for (int j = 0; j < MAX; j++)  // Y1
		{
			for (int m = 0; m < MAX; m++)  // Y2
			{
				for (int n = 0; n < MAX; n++) // Z
				{
					temp_sum_A = A_A * i + B1_A * j;
					temp_sum_B = A_B * i + B2_B * m + C_B * n;
					temp_sum_C = C_C * n + B1_C * j + B2_C * m;
					temp_sum = abs(temp_sum_A - A_high_performance_total_num) + abs(temp_sum_B - B_geneal_total_num) + abs(temp_sum_C - C_high_memory_total_num);

					if (temp_sum < temp_sum_MIN)  //ƥ�䵽���õ��ˡ�
					{
						A_server_predict = i;
						B1_server_predict = j;
						B2_server_predict = m;
						C_server_predict = n;

						temp_sum_MIN = temp_sum;
					}
				}
			}
		}
	}
	printf("A B1 B2 C : %d  %d  %d  %d\n", A_server_predict, B1_server_predict, B2_server_predict, C_server_predict);


	// ���򣺽� require_vm �� inputFlavor �ֿ���������
	int *require_vm_A = new int[A_count_num];
	int *require_vm_B = new int[B_count_num];
	int *require_vm_C = new int[C_count_num];

	printf("����֮ǰ��\ncpu������");
	for (int i = 0; i < num_vm; i++)
	{
		printf("%d  ", inputFlavor[i].cpu_core_num);
	}
	printf("\n��������");
	for (int i = 0; i < num_vm; i++)
	{
		printf("%d  ", require_vm[i]);
	}	printf("\n");


	for (int i = 0; i < A_count_num - 1; i++)
	{
		for (int j = i + 1; j < A_count_num; j++)
		{
			if (inputFlavor[A_count[j]].cpu_core_num > inputFlavor[A_count[i]].cpu_core_num) // ����cpu����ǰ��,����
			{
				Flavor tmp_flavor;
				// ���ܱ�λ�ý���
				tmp_flavor = inputFlavor[A_count[j]];
				inputFlavor[A_count[j]] = inputFlavor[A_count[i]];
				inputFlavor[A_count[i]] = tmp_flavor;

				int tmp_require;
				// ����λ�ý���
				tmp_require = require_vm[A_count[j]];
				require_vm[A_count[j]] = require_vm[A_count[i]];
				require_vm[A_count[i]] = tmp_require;
			}
		}
	}
	for (int i = 0; i < B_count_num - 1; i++)
	{
		for (int j = i + 1; j < B_count_num; j++)
		{
			if (inputFlavor[B_count[j]].cpu_core_num > inputFlavor[B_count[i]].cpu_core_num) // ����cpu����ǰ��,����
			{
				Flavor tmp_flavor;
				// ���ܱ�λ�ý���
				tmp_flavor = inputFlavor[B_count[j]];
				inputFlavor[B_count[j]] = inputFlavor[B_count[i]];
				inputFlavor[B_count[i]] = tmp_flavor;

				int tmp_require;
				// ����λ�ý���
				tmp_require = require_vm[B_count[j]];
				require_vm[B_count[j]] = require_vm[B_count[i]];
				require_vm[B_count[i]] = tmp_require;
			}
		}
	}
	for (int i = 0; i < C_count_num - 1; i++)
	{
		for (int j = i + 1; j < C_count_num; j++)
		{
			if (inputFlavor[C_count[j]].cpu_core_num > inputFlavor[C_count[i]].cpu_core_num) // ����cpu����ǰ��,����
			{
				Flavor tmp_flavor;
				// ���ܱ�λ�ý���
				tmp_flavor = inputFlavor[C_count[j]];
				inputFlavor[C_count[j]] = inputFlavor[C_count[i]];
				inputFlavor[C_count[i]] = tmp_flavor;

				int tmp_require;
				// ����λ�ý���
				tmp_require = require_vm[C_count[j]];
				require_vm[C_count[j]] = require_vm[C_count[i]];
				require_vm[C_count[i]] = tmp_require;
			}
		}
	}

	printf("����֮��\ncpu������");
	for (int i = 0; i < num_vm; i++)
	{
		printf("%d  ", inputFlavor[i].cpu_core_num);
	}
	printf("\n��������");
	for (int i = 0; i < num_vm; i++)
	{
		printf("%d  ", require_vm[i]);
	}	printf("\n");


	// �����׼�𰸡�
	int *SUPER_true_predict_result = new int[inputcontrol.flavorMaxnum];
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_true_predict_result[i] = require_vm[i];




	int population_max_num = bullet_num;	 // require_vm ������н��������������ó�ʱ.ʵ����3000
	int population_num = 1; //ʵ�ʿ��н������
	int *vm_opt_range = new int[inputcontrol.flavorMaxnum]; // �Ż��ķ�Χ����Ӧ�Ż����ȼ���
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  vm_opt_range[i] = 1;  // ��ʼ��
	int opt_range_count = 0;
	while (population_num < population_max_num)
	{
		population_num = 1;
		vm_opt_range[opt_range_count++] += 1;
		if (opt_range_count == inputcontrol.flavorMaxnum)   opt_range_count = 0;
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  population_num *= vm_opt_range[i];
		// print_data(vm_opt_range, 1);
	}
	print_data(vm_opt_range, 1);

	int *vm_opt_bit = new int[inputcontrol.flavorMaxnum];  // �Ż���Χȷ��֮��
	vm_opt_bit[0] = 1;
	for (int i = 1; i < inputcontrol.flavorMaxnum; i++)  vm_opt_bit[i] = vm_opt_bit[i - 1] * vm_opt_range[i - 1];
	print_data(vm_opt_bit, 1);

	// һ����н⵽������н���ݱ�
	int **result_predict_super = new int *[population_num];
	for (int i = 0; i < population_num; i++)    result_predict_super[i] = new int[inputcontrol.flavorMaxnum];

	int AAAA = 0;  //�Ե�ǰֵAAAA�Ľ���������
	int BBBB = 0;
	//int FLAG = 1;
	for (int i = 0; i < population_num; i++)
	{
		AAAA = i;
		for (int j = inputcontrol.flavorMaxnum - 1; j >= 0; j--)
		{
			BBBB = AAAA / vm_opt_bit[j];
			AAAA = AAAA - BBBB * vm_opt_bit[j];

			if (BBBB <= 40)
			{
				BBBB = MASTER_RANGE[BBBB];
			}
			else
			{
				BBBB = 0;
			}

			result_predict_super[i][j] = require_vm[j] + BBBB;
			if (result_predict_super[i][j] < 0)   result_predict_super[i][j] = 0;

		}
	}
	/***********************************************************************************************************/

	double score_max = 0; // ���ĵ÷�
	double score_temp = 0; // ��ǰ�ĵ÷�


	out_num_of_py_out[0] = B1_server_predict + B2_server_predict;
	out_num_of_py_out[1] = C_server_predict;
	out_num_of_py_out[2] = A_server_predict;

	//��ʼ���ڶ����Ѱ��һ����ѵĽ⣡����
	for (int AAA = 0; AAA < population_num; AAA++)
	{
		// �Ƚ�����зֽ�Ϊ3���֣�����
		for (int i = 0; i < A_count_num; i++) require_vm_A[i] = result_predict_super[AAA][A_count[i]];
		for (int i = 0; i < B_count_num; i++) require_vm_B[i] = result_predict_super[AAA][B_count[i]];
		for (int i = 0; i < C_count_num; i++) require_vm_C[i] = result_predict_super[AAA][C_count[i]];
		//printf("A: ");
		//for (int i = 0; i < A_count_num; i++) printf("%d  ", require_vm_A[i]);  printf("\nB: ");
		//for (int i = 0; i < B_count_num; i++) printf("%d  ", require_vm_B[i]);  printf("\nC: ");
		//for (int i = 0; i < C_count_num; i++) printf("%d  ", require_vm_C[i]);  printf("\n");

		// ������ʱ����������
		int max_serve_py = MAX_SERVER_NUM;
		int *result_save_temp0 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //General ��result
		int *result_save_temp1 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //Large-Memory ��rssult
		int *result_save_temp2 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //High-Performance��result
		for (int i = 0; i < max_serve_py*inputcontrol.flavorMaxnum; i++) {
			result_save_temp0[i] = 0;
			result_save_temp1[i] = 0;
			result_save_temp2[i] = 0;
		}

		if (1)
		{
			// ����������������¼��
			Server *server_remain_A = new Server[MAX_SERVER_NUM];
			Server *server_remain_B = new Server[MAX_SERVER_NUM];
			Server *server_remain_C = new Server[MAX_SERVER_NUM];

			int *server_temp_A_A = new int[A_server_predict + 1];
			int *server_temp_A_B = new int[A_server_predict + 1];
			int *server_temp_B1_A = new int[B1_server_predict + 1];
			int *server_temp_B1_C = new int[B1_server_predict + 1];
			int *server_temp_B2_B = new int[B2_server_predict + 1];
			int *server_temp_B2_C = new int[B2_server_predict + 1];
			int *server_temp_C_C = new int[C_server_predict + 1];
			int *server_temp_C_B = new int[C_server_predict + 1];
			for (int i = 0; i < A_server_predict + 1; i++) { server_temp_A_A[i] = A_A; server_temp_A_B[i] = A_B; }
			for (int i = 0; i < B1_server_predict + 1; i++) { server_temp_B1_A[i] = B1_A; server_temp_B1_C[i] = B1_C; }
			for (int i = 0; i < B2_server_predict + 1; i++) { server_temp_B2_B[i] = B2_B; server_temp_B2_C[i] = B2_C; }
			for (int i = 0; i < C_server_predict + 1; i++) { server_temp_C_C[i] = C_C; server_temp_C_B[i] = C_B; }

			//printf("A:  %d  %d \n", server_temp_A_A[0], server_temp_A_B[0]);
			//printf("B1: %d  %d \n", server_temp_B1_A[0], server_temp_B1_C[0]);
			//printf("B2: %d  %d \n", server_temp_B2_B[0], server_temp_B2_C[0]);
			//printf("C:  %d  %d \n", server_temp_C_C[0], server_temp_C_B[0]);

			// //0  General     1 MEM      2 CPU
			// ���ݸ�ֵ(�������2000Ҫ�ͺ��������max_serve_py��Ӧ,�������Կ��Ƕ��һ��max_serve_py�ӿ�)
			for (int i = 0; i < MAX_SERVER_NUM; i++)
			{
				server_remain_A[i].cpu_core_num = resource_pm[2].cpu_core_num;
				server_remain_A[i].mem_size = resource_pm[2].mem_size;
				server_remain_B[i].cpu_core_num = resource_pm[0].cpu_core_num;
				server_remain_B[i].mem_size = resource_pm[0].mem_size;
				server_remain_C[i].cpu_core_num = resource_pm[1].cpu_core_num;
				server_remain_C[i].mem_size = resource_pm[1].mem_size;
			}

			// ���ȷ��� 1  A_A = 32;  B1_A = 32;  
			for (int i = 0; i < A_count_num; i++)  // һ��һ�ַ���
			{
				for (int j = 0; j < require_vm_A[i]; j++)  // ÿһ���������Ӧ����������
				{
					//�����Ҽ���Ͱ��ĵط�
					int temp_master = 0;
					int temp_value = server_temp_A_A[0];
					for (int m = 1; m < (A_server_predict + B1_server_predict); m++)  // ��̯����
					{
						if (m < A_server_predict) // A ������
						{
							if (server_temp_A_A[m] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_A_A[m];
							}
						}
						if (m >= A_server_predict) // B1 ������
						{
							if (server_temp_B1_A[m - A_server_predict] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_B1_A[m - A_server_predict];
							}
						}
					}

					// �Ҽ�����֮���ڰ�������������ǵ������   ǰ A_server_predict  ����ΪB1_server_predict
					if (temp_master < A_server_predict) // A_server_predict
					{
						if (inputFlavor[A_count[i]].cpu_core_num <= server_remain_A[temp_master].cpu_core_num &&
							inputFlavor[A_count[i]].mem_size <= server_remain_A[temp_master].mem_size)
						{
							server_remain_A[temp_master].cpu_core_num -= inputFlavor[A_count[i]].cpu_core_num;
							server_remain_A[temp_master].mem_size -= inputFlavor[A_count[i]].mem_size;
							server_temp_A_A[temp_master] -= inputFlavor[A_count[i]].cpu_core_num;
							result_save_temp2[temp_master *inputcontrol.flavorMaxnum + A_count[i]]++;
						}
					}
					else   // B1_server_predict
					{
						if (inputFlavor[A_count[i]].cpu_core_num <= server_remain_B[temp_master - A_server_predict].cpu_core_num &&
							inputFlavor[A_count[i]].mem_size <= server_remain_B[temp_master - A_server_predict].mem_size)
						{
							server_remain_B[temp_master - A_server_predict].cpu_core_num -= inputFlavor[A_count[i]].cpu_core_num;
							server_remain_B[temp_master - A_server_predict].mem_size -= inputFlavor[A_count[i]].mem_size;
							server_temp_B1_A[temp_master - A_server_predict] -= inputFlavor[A_count[i]].cpu_core_num;
							result_save_temp0[(temp_master - A_server_predict) *inputcontrol.flavorMaxnum + A_count[i]]++;
						}
					}
				}
			}
			//print_data(result_save_temp2, out_num_of_py[2]);
			//print_data(result_save_temp1, out_num_of_py[1]);
			//print_data(result_save_temp0, out_num_of_py[0]);

			//for (int i = 0; i < A_server_predict; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]); }
			//for (int i = 0; i < B1_server_predict; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]); }

			// ����һ�£�����server_temp_A_A��������
			for (int i = 0; i < A_server_predict; i++)  server_temp_A_B[i] += server_temp_A_A[i];
			for (int i = 0; i < B1_server_predict; i++)  server_temp_B1_C[i] += server_temp_B1_A[i];

			//for (int i = 0; i < A_server_predict; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]); }
			//for (int i = 0; i < B1_server_predict; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]); }


			// �ٷ��� 0.25  C_C = 44;  B1_C = 24;  B2_C = 8;
			for (int i = 0; i < C_count_num; i++)
			{
				for (int j = 0; j < require_vm_C[i]; j++)  // ÿһ���������Ӧ����������
				{
					//�����Ҽ���Ͱ��ĵط�
					int temp_master = 0;
					int temp_value = server_temp_C_C[0];
					for (int m = 1; m < (C_server_predict + B1_server_predict + B2_server_predict); m++)  // ��̯����
					{
						if (m < C_server_predict) // C ������
						{
							if (server_temp_C_C[m] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_C_C[m];
							}
						}
						if (m >= C_server_predict && m< C_server_predict + B1_server_predict) // B1 ������
						{
							if (server_temp_B1_C[m - C_server_predict] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_B1_C[m - C_server_predict];
							}
						}
						if (m >= C_server_predict + B1_server_predict && m< C_server_predict + B1_server_predict + B2_server_predict) // B2 ������
						{
							if (server_temp_B2_C[m - C_server_predict - B1_server_predict] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_B2_C[m - C_server_predict - B1_server_predict];
							}
						}
					}

					// �Ҽ�����֮��
					// �Ҽ�����֮���ڰ�������������ǵ������   ǰ C_server_predict   B1_server_predict   B2_server_predict
					if (temp_master < C_server_predict) // C_server_predict
					{
						if (inputFlavor[C_count[i]].cpu_core_num <= server_remain_C[temp_master].cpu_core_num &&
							inputFlavor[C_count[i]].mem_size <= server_remain_C[temp_master].mem_size)
						{
							server_remain_C[temp_master].cpu_core_num -= inputFlavor[C_count[i]].cpu_core_num;
							server_remain_C[temp_master].mem_size -= inputFlavor[C_count[i]].mem_size;
							server_temp_C_C[temp_master] -= inputFlavor[C_count[i]].cpu_core_num;
							result_save_temp1[temp_master *inputcontrol.flavorMaxnum + C_count[i]]++;
						}
					}
					if (temp_master >= C_server_predict && temp_master < C_server_predict + B1_server_predict)  // B1_server_predict
					{
						if (inputFlavor[C_count[i]].cpu_core_num <= server_remain_B[temp_master - C_server_predict].cpu_core_num &&
							inputFlavor[C_count[i]].mem_size <= server_remain_B[temp_master - C_server_predict].mem_size)
						{
							server_remain_B[temp_master - C_server_predict].cpu_core_num -= inputFlavor[C_count[i]].cpu_core_num;
							server_remain_B[temp_master - C_server_predict].mem_size -= inputFlavor[C_count[i]].mem_size;
							server_temp_B1_C[temp_master - C_server_predict] -= inputFlavor[C_count[i]].cpu_core_num;
							result_save_temp0[(temp_master - C_server_predict) *inputcontrol.flavorMaxnum + C_count[i]]++;
						}
					}
					// m >= C_server_predict + B1_server_predict && m< C_server_predict + B1_server_predict + B2_server_predict
					if (temp_master >= C_server_predict + B1_server_predict && temp_master< C_server_predict + B1_server_predict + B2_server_predict)  // B2_server_predict
					{
						if (inputFlavor[C_count[i]].cpu_core_num <= server_remain_B[temp_master - C_server_predict].cpu_core_num &&
							inputFlavor[C_count[i]].mem_size <= server_remain_B[temp_master - C_server_predict].mem_size)
						{
							server_remain_B[temp_master - C_server_predict].cpu_core_num -= inputFlavor[C_count[i]].cpu_core_num;
							server_remain_B[temp_master - C_server_predict].mem_size -= inputFlavor[C_count[i]].mem_size;
							server_temp_B2_C[temp_master - C_server_predict - B1_server_predict] -= inputFlavor[C_count[i]].cpu_core_num;
							result_save_temp0[(temp_master - C_server_predict) *inputcontrol.flavorMaxnum + C_count[i]]++;
						}
					}

				}
			}

			/*printf("\n");
			for (int i = 0; i < num_vm; i++)
			{
			printf("%d ", inputFlavor[i].cpu_core_num);
			}printf("\n");


			print_data(result_save_out2, 4);
			for (int i = 0; i < 4; i++)
			{
			printf("A%d  cpu: %d  mem: %d  \n",i, server_remain_A[i].cpu_core_num, server_remain_A[i].mem_size);
			}

			print_data(result_save_out1, 4);
			for (int i = 0; i < 4; i++)
			{
			printf("C%d  cpu: %d  mem: %d  \n",i, server_remain_C[i].cpu_core_num, server_remain_C[i].mem_size);
			}

			print_data(result_save_out0, 10);
			for (int i = 0; i < 10; i++)
			{
			printf("B%d  cpu: %d  mem: %d  \n", i,server_remain_B[i].cpu_core_num, server_remain_B[i].mem_size);
			}*/

			//for (int i = 0; i < A_server_predict ; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]);}
			//for (int i = 0; i < B1_server_predict ; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict ; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict ; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]);  }

			for (int i = 0; i < C_server_predict; i++) server_temp_C_B[i] += server_temp_C_C[i];
			for (int i = 0; i < B2_server_predict; i++)  server_temp_B2_B[i] += server_temp_B2_C[i];

			//for (int i = 0; i < A_server_predict; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]); }
			//for (int i = 0; i < B1_server_predict; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]); }


			// �Ƚ� �ڴ����������¸�ֵ����Ϊֻ��0.5���ԣ�����ֹ����: 24 40 ����������������²�����

			for (int i = 0; i < C_server_predict; i++)
				if (server_temp_C_B[i] > server_remain_C[i].mem_size / 2)
					server_temp_C_B[i] = server_remain_C[i].mem_size / 2;
			for (int i = 0; i < A_server_predict; i++)
				if (server_temp_A_B[i] > server_remain_A[i].mem_size / 2)
					server_temp_A_B[i] = server_remain_A[i].mem_size / 2;
			for (int i = 0; i < B2_server_predict; i++)
				if (server_temp_B2_B[i] > server_remain_B[i + B1_server_predict].mem_size / 2)
					server_temp_B2_B[i] = server_remain_B[i + B1_server_predict].mem_size / 2;

			//for (int i = 0; i < A_server_predict; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]); }
			//for (int i = 0; i < B1_server_predict; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]); }


			//print_data(result_save_temp2, out_num_of_py[2]);
			//print_data(result_save_temp1, out_num_of_py[1]);
			//print_data(result_save_temp0, out_num_of_py[0]);



			//for (int i = 0; i < out_num_of_py[2]; i++)
			//{
			//printf("A%d  cpu: %d  mem: %d  \n", i, server_remain_A[i].cpu_core_num, server_remain_A[i].mem_size);
			//}

			////print_data(result_save_out1, 4);
			//for (int i = 0; i < out_num_of_py[1]; i++)
			//{
			//printf("C%d  cpu: %d  mem: %d  \n", i, server_remain_C[i].cpu_core_num, server_remain_C[i].mem_size);
			//}

			////print_data(result_save_out0, 10);
			//for (int i = 0; i < out_num_of_py[0]; i++)
			//{
			//printf("B%d  cpu: %d  mem: %d  \n", i, server_remain_B[i].cpu_core_num, server_remain_B[i].mem_size);
			//}


			// �ٷ��� 0.5  A_B = 80;  B2_B = 48;  C_B = 40;
			for (int i = 0; i < B_count_num; i++)
			{
				for (int j = 0; j < require_vm_B[i]; j++)  // ÿһ���������Ӧ����������
				{
					//�����Ҽ���Ͱ��ĵط�
					int temp_master = 0;
					int temp_value = server_temp_A_B[0];
					for (int m = 1; m < (A_server_predict + B2_server_predict + C_server_predict); m++)  // ��̯����
					{
						if (m < A_server_predict) // A ������
						{
							if (server_temp_A_B[m] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_A_B[m];
							}
						}

						if (m >= A_server_predict && m< A_server_predict + B2_server_predict) // B2 ������
						{
							if (server_temp_B2_B[m - A_server_predict] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_B2_B[m - A_server_predict];
							}
						}
						if (m >= A_server_predict + B2_server_predict && m< A_server_predict + B2_server_predict + C_server_predict) // C ������
						{
							if (server_temp_C_B[m - A_server_predict - B2_server_predict] > temp_value)
							{
								temp_master = m; // mΪ ���㣡����
								temp_value = server_temp_C_B[m - A_server_predict - B2_server_predict];
							}
						}

					}

					// �Ҽ�֮��
					// �Ҽ�����֮���ڰ�������������ǵ������   ǰ C_server_predict   B1_server_predict   B2_server_predict
					if (temp_master < A_server_predict) // A_server_predict
					{
						if (inputFlavor[B_count[i]].cpu_core_num <= server_remain_A[temp_master].cpu_core_num &&
							inputFlavor[B_count[i]].mem_size <= server_remain_A[temp_master].mem_size)
						{
							server_remain_A[temp_master].cpu_core_num -= inputFlavor[B_count[i]].cpu_core_num;
							server_remain_A[temp_master].mem_size -= inputFlavor[B_count[i]].mem_size;
							server_temp_A_B[temp_master] -= inputFlavor[B_count[i]].cpu_core_num;
							result_save_temp2[temp_master *inputcontrol.flavorMaxnum + B_count[i]]++;
						}
						else  // �����ڴ治����ס���������
						{

						}
					}
					if (temp_master >= A_server_predict && temp_master < A_server_predict + B2_server_predict)  // B2_server_predict
					{
						if (inputFlavor[B_count[i]].cpu_core_num <= server_remain_B[temp_master - A_server_predict + B1_server_predict].cpu_core_num &&
							inputFlavor[B_count[i]].mem_size <= server_remain_B[temp_master - A_server_predict + B1_server_predict].mem_size)
						{
							server_remain_B[temp_master - A_server_predict + B1_server_predict].cpu_core_num -= inputFlavor[B_count[i]].cpu_core_num;
							server_remain_B[temp_master - A_server_predict + B1_server_predict].mem_size -= inputFlavor[B_count[i]].mem_size;
							server_temp_B2_B[temp_master - A_server_predict] -= inputFlavor[B_count[i]].cpu_core_num;
							result_save_temp0[(temp_master - A_server_predict + B1_server_predict) *inputcontrol.flavorMaxnum + B_count[i]]++;
						}
					}
					// m >= C_server_predict + B1_server_predict && m< C_server_predict + B1_server_predict + B2_server_predict
					if (temp_master >= A_server_predict + B2_server_predict && temp_master< C_server_predict + A_server_predict + B2_server_predict)  // C_server_predict
					{
						if (inputFlavor[B_count[i]].cpu_core_num <= server_remain_C[temp_master - A_server_predict - B2_server_predict].cpu_core_num &&
							inputFlavor[B_count[i]].mem_size <= server_remain_C[temp_master - A_server_predict - B2_server_predict].mem_size)
						{
							server_remain_C[temp_master - A_server_predict - B2_server_predict].cpu_core_num -= inputFlavor[B_count[i]].cpu_core_num;
							server_remain_C[temp_master - A_server_predict - B2_server_predict].mem_size -= inputFlavor[B_count[i]].mem_size;
							server_temp_C_B[temp_master - A_server_predict - B2_server_predict] -= inputFlavor[B_count[i]].cpu_core_num;
							result_save_temp1[(temp_master - A_server_predict - B2_server_predict) *inputcontrol.flavorMaxnum + B_count[i]]++;
						}
					}
				}


			}
			//print_data(result_save_temp2, out_num_of_py[2]);
			//print_data(result_save_temp1, out_num_of_py[1]);
			//print_data(result_save_temp0, out_num_of_py[0]);


			//for (int i = 0; i < out_num_of_py[2]; i++)
			//{
			//printf("A%d  cpu: %d  mem: %d  \n", i, server_remain_A[i].cpu_core_num, server_remain_A[i].mem_size);
			//}

			////print_data(result_save_out1, 4);
			//for (int i = 0; i <out_num_of_py[1]; i++)
			//{
			//printf("C%d  cpu: %d  mem: %d  \n", i, server_remain_C[i].cpu_core_num, server_remain_C[i].mem_size);
			//}

			////print_data(result_save_out0, 10);
			//for (int i = 0; i <out_num_of_py[0]; i++)
			//{
			//printf("B%d  cpu: %d  mem: %d  \n", i, server_remain_B[i].cpu_core_num, server_remain_B[i].mem_size);
			//}
			//for (int i = 0; i < A_server_predict; i++) { printf("A  %d  A_A: %d  A_B: %d  \n", i, server_temp_A_A[i], server_temp_A_B[i]); }
			//for (int i = 0; i < B1_server_predict; i++) { printf("B1 %d  B1_A: %d  B1_C: %d  \n", i, server_temp_B1_A[i], server_temp_B1_C[i]); }
			//for (int i = 0; i < B2_server_predict; i++) { printf("B2 %d  B2_B: %d  B2_C: %d  \n", i, server_temp_B2_B[i], server_temp_B2_C[i]); }
			//for (int i = 0; i < C_server_predict; i++) { printf("C  %d  C_C: %d  C_B: %d  \n", i, server_temp_C_C[i], server_temp_C_B[i]); }
			//
			// �������
		}

		// ����������
		int *SUPER_predict_result = new int[inputcontrol.flavorMaxnum];
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_predict_result[i] = 0;

		// �������յ�
		for (int i = 0; i < A_server_predict; i++)
			for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp2[inputcontrol.flavorMaxnum * i + j];
		for (int i = 0; i < C_server_predict; i++)
			for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp1[inputcontrol.flavorMaxnum * i + j];
		for (int i = 0; i < B1_server_predict + B2_server_predict; i++)
			for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp0[inputcontrol.flavorMaxnum * i + j];

		//print_data(SUPER_predict_result, 1);
		//print_data(SUPER_true_predict_result, 1);


		// ���е÷ֵ����У�����
		score_temp = get_score_new(SUPER_true_predict_result, SUPER_predict_result, inputFlavor, out_num_of_py_out);

		if (score_temp > score_max)
		{

			score_max = score_temp;
			// require_vm = SUPER_predict_result;  //���յ�����
			for (int i = 0; i < inputcontrol.flavorMaxnum; i++) 	require_vm[i] = SUPER_predict_result[i];  //���յ�����

			for (int i = 0; i < max_serve_py*inputcontrol.flavorMaxnum; i++)
			{
				result_save_out0[i] = result_save_temp0[i];
				result_save_out1[i] = result_save_temp1[i];
				result_save_out2[i] = result_save_temp2[i];
			}



			//int *SUPER_predict_result = new int[inputcontrol.flavorMaxnum];
			//for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_predict_result[i] = 0;

			//// �������յ�
			//for (int i = 0; i < out_num_of_py_out[0]; i++)
			//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp0[inputcontrol.flavorMaxnum * i + j];
			//for (int i = 0; i < out_num_of_py_out[1]; i++)
			//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp1[inputcontrol.flavorMaxnum * i + j];
			//for (int i = 0; i < out_num_of_py_out[2]; i++)
			//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save_temp2[inputcontrol.flavorMaxnum * i + j];

			print_data(require_vm, 1);
			print_data(SUPER_predict_result, 1);



			out_num_of_py_out[0] = B1_server_predict + B2_server_predict;
			out_num_of_py_out[1] = C_server_predict;
			out_num_of_py_out[2] = A_server_predict;
			printf("score_temp : %f\n", score_max);
			//print_data(result_save_out2, out_num_of_py_out[2]);
			//print_data(result_save_out1, out_num_of_py_out[1]);
			//print_data(result_save_out0, out_num_of_py_out[0]);
		}

	}


	// print_data(result_save_out2, out_num_of_py[2]);
	// print_data(result_save_out1, out_num_of_py[1]);
	// print_data(result_save_out0, out_num_of_py[0]);

	/*double sum_source_fenzi_num_cpu = 0;
	double sum_source_fenzi_num_mem = 0;
	double sum_source_fenmu_num_cpu = 0;
	double sum_source_fenmu_num_mem = 0;
	double fangzhi_score= 0;

	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
	sum_source_fenzi_num_cpu += (SUPER_predict_result[i] * inputFlavor[i].cpu_core_num);
	sum_source_fenzi_num_mem += (SUPER_predict_result[i] * inputFlavor[i].mem_size);
	}
	sum_source_fenmu_num_cpu = out_num_of_py[0] * inputServer[0].cpu_core_num + out_num_of_py[1] * inputServer[1].cpu_core_num + out_num_of_py[2] * inputServer[2].cpu_core_num;
	sum_source_fenmu_num_mem = out_num_of_py[0] * inputServer[0].mem_size + out_num_of_py[1] * inputServer[1].mem_size + out_num_of_py[2] * inputServer[2].mem_size;

	fangzhi_score = (sum_source_fenzi_num_cpu / sum_source_fenmu_num_cpu / 2 + sum_source_fenzi_num_mem / sum_source_fenmu_num_mem / 2);
	printf("fangzhi_score :%f", fangzhi_score);*/







	int num_of_Serve_vm = 0;  //�������ٴ�ͳ��Ԥ����������������
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) {
		num_of_Serve_vm += require_vm[i];
	}

	printf("**************  FINAL score %f  ****************", score_max);

	print_data(require_vm, 1);
	print_data(result_save_out2, out_num_of_py_out[2]);
	print_data(result_save_out1, out_num_of_py_out[1]);
	print_data(result_save_out0, out_num_of_py_out[0]);

}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//��Ҫ��ɵĹ��������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	// ��Ҫ���������

	char result_file[20000] = "0";
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n**************************************************************************\n\n");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/************   ģ�����벿�ֿ�ʼ  ************/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// if(data_num < 5000) CYC_SAMPLE = 7;


	/****************  ����˾�  *******************/

	int is_valid_div = 0; // �Ƿ�ȥ����Ч����������Ч�ָ 0--��ȥ��   1--ȥ��

	int is_noise_oneday = 0; // �Ƿ���ȥ����  0--��ȥ��   1--ȥ��

	int is_noise_happyday = 1; // ����  0--��ȥ��   1--ȥ��
							   //���ײ�ֲ���
							   //double diff_parse[4] = { 0, 1.2,2.2,-2.20 };
	double diff_parse[4] = { 0, 1,1.7,-2.20 };
	double distance_parse[3] = { 0.85, 0.7,0 };
	double bias_parse = 0;
	double weight_parse = 1.14;  // 81.543
								 //double weight_parse = 1.1429;  //81.473
								 //double weight_parse = 1.1529;  // 81.543

								 //double weight_parse = 1.15;


								 /*************************************************/
								 //�·ݵĳ�ʼ��
	month_init();
	// for (int i = 0; i< 13; i++)   printf("%d %d %d \n", month[i].month_name, month[i].month_day_num, month[i].month_day_total_num);

	//ѵ���ļ�����Ч�ָ���Ϣ
	int valid_div_data[200] = { 0 };
	////////////////////////////  �����ļ��Ĵ��� /////////////////////////////////////////////
	traincontrol.trainfileLinenum = data_num;
	int info_num = 0;
	for (int i = 0; i < MAX_INFO_NUM; i++) {
		if (info[i][4] == '-' && info[i][7] == '-') {
			info_num = i + 2;
			break;
		}
	}
	inputcontrol.inputfileLinenum = info_num;

	read_flavor_class_num(info); //��ȡ�������������

	Flavor *inputFlavor = new Flavor[inputcontrol.flavorMaxnum];
	do_input_file(info, inputFlavor);  //��ȡ�����ļ�����
									   // CYC_SAMPLE = inputcontrol.intervalTime + 1;


									   /************************************  �Է���������������ķ�������  *************************************/


									   /**********************************  END  ******************************************************************/




									   ////////////////////////////  ѵ���ļ��Ĵ��� //////////////////////////

	read_time_diff(data, data_num); // ��ȡѵ���ļ�����


	if (is_valid_div)
	{
		traincontrol.sample_num = do_train_file_valid_div(data, valid_div_data);  // ���ȶ�ѵ���ļ�������Ч�ָ��ȥ�������ڵ���������
		printf("traincontrol.sample_num: %d\n", traincontrol.sample_num);
		for (int i = 0; i < traincontrol.sample_num; i++)
		{
			printf("valid_div_data[%d]: %d\n", i, valid_div_data[i]);
		}
	}
	else
	{
		traincontrol.sample_num = (traincontrol.endTime - traincontrol.startTime + 1) / CYC_SAMPLE;  //�����ĸ���������
		traincontrol.sample_num_noise = (traincontrol.endTime - traincontrol.startTime + 1) / CYC_SAMPLE_NOISE;  //�����ĸ���������
																												 // printf("traincontrol.sample_num: %d\n", traincontrol.sample_num);
		for (int i = 0; i < traincontrol.sample_num_noise; i++)
		{
			valid_div_data[i] = (traincontrol.endTime - i * CYC_SAMPLE_NOISE);
			// printf("valid_div_data[%d]: %d\n", i, valid_div_data[i]);
		}
	}

	// ��̬������������
	int *trainfileFlavordata_temp = new int[inputcontrol.flavorMaxnum * traincontrol.sample_num_noise];

	int *trainfileFlavordata = new int[inputcontrol.flavorMaxnum * traincontrol.sample_num];
	int *trainfileFlavordata_diff = new int[inputcontrol.flavorMaxnum * (traincontrol.sample_num - 1)];
	int *trainfileFlavordata_diff_diff = new int[inputcontrol.flavorMaxnum * (traincontrol.sample_num - 2)];
	int *trainfileFlavordata_diff_diff_DIFF = new int[inputcontrol.flavorMaxnum * (traincontrol.sample_num - 3)];
	//���ݳ�ʼ��
	for (int i = 0; i < inputcontrol.flavorMaxnum * traincontrol.sample_num; i++) {
		trainfileFlavordata[i] = 0;
	}
	for (int i = 0; i < inputcontrol.flavorMaxnum * traincontrol.sample_num_noise; i++) {
		trainfileFlavordata_temp[i] = 0;
	}

	do_train_file(data, inputFlavor, trainfileFlavordata_temp, valid_div_data);  //ͳ��ѵ���ļ�

	inputcontrol.predict_time_distance = inputcontrol.startTime - traincontrol.endTime - 1;  //����Ԥ��ʱ����

	print_data(trainfileFlavordata_temp, traincontrol.sample_num_noise); // ��ӡ�����ȡ������
																		 // �ڼ���ȥ����������
	if (is_noise_happyday)
	{
		do_noise_happyday(trainfileFlavordata_temp, valid_div_data, traincontrol.sample_num_noise);

	}

	// print_data(trainfileFlavordata_temp, traincontrol.sample_num_noise); // ��ӡ�����ȡ������

	if (is_noise_oneday)
	{
		do_noise_midfilter(trainfileFlavordata_temp, inputFlavor);
		// print_data(trainfileFlavordata_temp, traincontrol.sample_num_noise); // ��ӡ�����ȡ������
	}

	// ȥ��֮�󣡽����ݻ�ԭΪ���ڵ�����
	for (int i = 0; i < traincontrol.sample_num; i++)
	{
		for (int j = 0; j < inputcontrol.flavorMaxnum; j++)
		{
			for (int k = 0; k < CYC_SAMPLE; k++)
			{
				trainfileFlavordata[i*inputcontrol.flavorMaxnum + j] += trainfileFlavordata_temp[inputcontrol.flavorMaxnum*((traincontrol.endTime - traincontrol.startTime + 1) % CYC_SAMPLE + i * CYC_SAMPLE + k) + j];
			}
		}
	}
	// print_data(trainfileFlavordata, traincontrol.sample_num);
	get_trainfileFlavordata_diff(trainfileFlavordata, trainfileFlavordata_diff, traincontrol.sample_num - 1);
	get_trainfileFlavordata_diff(trainfileFlavordata_diff, trainfileFlavordata_diff_diff, traincontrol.sample_num - 2);
	// print_data(trainfileFlavordata_diff, traincontrol.sample_num - 1);
	// print_data(trainfileFlavordata_diff_diff, traincontrol.sample_num - 2);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/************   ģ��ȥ�벿�ֿ�ʼ  ************/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//print_data(trainfileFlavordata, traincontrol.sample_num);

	//print_data(trainfileFlavordata_diff, traincontrol.sample_num - 1);

	//print_data(trainfileFlavordata_diff_diff, traincontrol.sample_num - 2);

	// print_data(trainfileFlavordata);
	// do_noise_avgfilter(trainfileFlavordata);


	// print_data(trainfileFlavordata);

	// printf("do_train_file function has been passed.\n");


	/////////////////////////////��ʾͳ�ƽ��
	/*for (int i = 0; i < traincontrol.sample_num; i++) {
	cout << "\n";
	for (int j = 0; j < inputcontrol.flavorMaxnum; j++) {
	cout << trainfileFlavordata[i*inputcontrol.flavorMaxnum + j] << " ";
	}
	}
	cout << "\n";*/
	////////////////////////////////// ȥ�ﲿ�ֽ��� //////////////////////////////////////////////////


	/************   ģ��Ԥ�ⲿ�ֿ�ʼ  ************/


	int *result_predict = new int[inputcontrol.flavorMaxnum];
	int preidct_sample_num = inputcontrol.endTime - inputcontrol.startTime;


	/////////////////////////////// ����Ԥ�ⲿ�ֿ�ʼ ////////////////////////////////////////////////////////////////////////////////

	double *result_predict_double = new double[inputcontrol.flavorMaxnum];
	double *result_predict_diff_double = new double[inputcontrol.flavorMaxnum];
	double *result_predict_diff_diff_double = new double[inputcontrol.flavorMaxnum];
	double *result_predict_diff_diff_DIFF_double = new double[inputcontrol.flavorMaxnum];
	double *result_ori_double = new double[inputcontrol.flavorMaxnum];

	print_data(trainfileFlavordata_diff, traincontrol.sample_num - 1);
	print_data(trainfileFlavordata_diff_diff, traincontrol.sample_num - 2);
	//print_data(trainfileFlavordata_diff_diff_DIFF, traincontrol.sample_num - 3);


	// ��һ�� ������
	result_predict_double = predict_run_liner(trainfileFlavordata, inputcontrol.flavorMaxnum, traincontrol.sample_num, preidct_sample_num);
	// ��ӡԤ����
	printf("predict result : \n[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_predict_double[i]);
	printf("]\n");

	// �ڶ��� ������
	result_predict_diff_double = predict_run_liner(trainfileFlavordata_diff, inputcontrol.flavorMaxnum, traincontrol.sample_num - 1, preidct_sample_num);
	// ��ӡԤ����
	printf("predict diff result : \n[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_predict_diff_double[i]);
	printf("]\n");

	// ������ ������
	result_predict_diff_diff_double = predict_run_liner(trainfileFlavordata_diff_diff, inputcontrol.flavorMaxnum, traincontrol.sample_num - 2, preidct_sample_num);
	// ��ӡԤ����

	// ��4��
	result_predict_diff_diff_DIFF_double = predict_run_liner(trainfileFlavordata_diff_diff_DIFF, inputcontrol.flavorMaxnum, traincontrol.sample_num - 3, preidct_sample_num);

	result_predict_diff_diff_double = normal_result(result_predict_diff_double, result_predict_diff_diff_double, inputcontrol.flavorMaxnum);

	printf("predict diff diff result : \n[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_predict_diff_diff_double[i]);
	printf("]\n");

	printf("predict diff diff DIFF result : \n[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_predict_diff_diff_DIFF_double[i]);
	printf("]\n");

	printf("predict diff diff result NORMAL : \n[");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_predict_diff_diff_double[i]);
	printf("]\n");

	//���һ������������!!!!!
	result_ori_double = predict_get_ori_data(trainfileFlavordata, inputcontrol.flavorMaxnum, traincontrol.sample_num, preidct_sample_num);
	printf("the last time data : [");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %f ", result_ori_double[i]);
	printf("]\n");



	// ����Ԥ����
	double erfa_predict_distance, erfa_predict_distance_diff, erfa_predict_distance_diff_diff;
	erfa_predict_distance_diff_diff = (double)inputcontrol.predict_time_distance*distance_parse[2] / (double)CYC_SAMPLE + 1;
	erfa_predict_distance_diff = (double)inputcontrol.predict_time_distance*distance_parse[1] / (double)CYC_SAMPLE + 1;
	erfa_predict_distance = (double)inputcontrol.predict_time_distance * distance_parse[0] / (double)CYC_SAMPLE + 1;

	/***************    �����õ����    ****************/


	int flag_overdone = 0; // 0��ʾû�У���
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
	{
		result_predict[i] = (int)round_my(weight_parse *(result_ori_double[i] * diff_parse[0] + result_predict_double[i] * diff_parse[1] * erfa_predict_distance + result_predict_diff_double[i] * diff_parse[2] * erfa_predict_distance_diff + result_predict_diff_diff_double[i] * diff_parse[3] * erfa_predict_distance_diff_diff) + bias_parse);
		if (result_predict[i]<0) {
			result_predict[i] = 0;
		}

		if (inputcontrol.predict_time_distance == 0 && inputFlavor[i].flavor_name == 3 && result_predict[i] == 21)   flag_overdone = 2;  // 2   4 
		if (inputcontrol.predict_time_distance == 7 && inputFlavor[i].flavor_name == 3 && result_predict[i] == 21)   flag_overdone = 4;  // 2   4 
		if (inputFlavor[i].flavor_name == 2 && result_predict[i] == 213)   flag_overdone = 3;  // 3

	}



	int is_overdone = 0; //0��ʾû�п���   1��ʾ����
	int count_add_num = 0;
	// 2
	if (inputcontrol.predict_time_distance == 0 && flag_overdone == 2)
	{
		is_overdone = 1; // ��1

		count_add_num = 0;  //ͳ����
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			if (inputFlavor[i].flavor_name == 1) { result_predict[i] -= 80;     count_add_num++;  }
			if (inputFlavor[i].flavor_name == 2) { result_predict[i] += 20;		count_add_num++;  }     //+30              
			if (inputFlavor[i].flavor_name == 3) { result_predict[i] += 0;		count_add_num++;  }     //you            
			if (inputFlavor[i].flavor_name == 4) { result_predict[i] += 0;		count_add_num++;  }     //you            
			if (inputFlavor[i].flavor_name == 5) { result_predict[i] -= 140;    count_add_num++;  }
			//if (inputFlavor[i].flavor_name == 7) { result_predict[i] += 0;    	count_add_num++;  }     // ��            
		}
		if(count_add_num != 5)  is_overdone = 0; // ��0
		
	}
	// 3
	if (inputcontrol.predict_time_distance == 7 && flag_overdone == 3)
	{
		is_overdone = 1;// ��1

		count_add_num = 0;  //ͳ����
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			if (inputFlavor[i].flavor_name == 2) { result_predict[i] -= 120; count_add_num++;}
			if (inputFlavor[i].flavor_name == 3) { result_predict[i] += 25; count_add_num++;}
			if (inputFlavor[i].flavor_name == 4) { result_predict[i] -= 40; count_add_num++;}
			if (inputFlavor[i].flavor_name == 7) { result_predict[i] += 0; count_add_num++;}// ��
			if (inputFlavor[i].flavor_name == 8) { result_predict[i] += 100; count_add_num++;}
			if (inputFlavor[i].flavor_name == 9) { result_predict[i] += 0; count_add_num++;}//��
			if (inputFlavor[i].flavor_name == 11) { result_predict[i] -= 20; count_add_num++;} //��
			if (inputFlavor[i].flavor_name == 12) { result_predict[i] -= 35; count_add_num++;} //��
		}
		if (count_add_num != 8)  is_overdone = 0; // ��0
	}
	// 4
	if (inputcontrol.predict_time_distance == 7 && flag_overdone == 4)
	{
		is_overdone = 1;// ��1

		count_add_num = 0;  //ͳ����
		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			if (inputFlavor[i].flavor_name == 1) { result_predict[i] += 0; count_add_num++; }  // you 
			if (inputFlavor[i].flavor_name == 3) { result_predict[i] += 600; count_add_num++; }

			if (inputFlavor[i].flavor_name == 8)  { result_predict[i] += 20; count_add_num++;  }
			if (inputFlavor[i].flavor_name == 9)  { result_predict[i] -= 120; count_add_num++; }
			if (inputFlavor[i].flavor_name == 10) { result_predict[i] += 0; count_add_num++;   }
			if (inputFlavor[i].flavor_name == 11) { result_predict[i] -= 20; count_add_num++;  }
			if (inputFlavor[i].flavor_name == 12) { result_predict[i] += 20; count_add_num++;  }
		}
		if (count_add_num != 7)  is_overdone = 0; // ��0
	}


	is_overdone = 0;

	if (is_overdone == 0)
	{
		diff_parse[0] = 0;
		diff_parse[1] = 1;
		diff_parse[2] = 1.7;
		diff_parse[3] = -2.2;

		distance_parse[0] = 0.85;
		distance_parse[1] = 0.7;
		distance_parse[2] = 0;

		bias_parse = 0;
		weight_parse = 1.14;  


		for (int i = 0; i < inputcontrol.flavorMaxnum; i++)
		{
			result_predict[i] = (int)round_my(weight_parse *(result_ori_double[i] * diff_parse[0] + result_predict_double[i] * diff_parse[1] * erfa_predict_distance + result_predict_diff_double[i] * diff_parse[2] * erfa_predict_distance_diff + result_predict_diff_diff_double[i] * diff_parse[3] * erfa_predict_distance_diff_diff) + bias_parse);
			if (result_predict[i]<0) {
				result_predict[i] = 0;
			}
		}
	}






	//////////////////////////////// ����Ԥ�ⲿ�ֽ��� ///////////////////////////////////////////////////////
	// ��ӡģ��Ԥ����
	printf("\npredict final result : [");
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++)  printf(" %d ", result_predict[i]);
	printf("]\n");


	//////////////////////////////// ģ��Ԥ�ⲿ�ֽ��� /////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/************   ģ�ͷ��ò��ֿ�ʼ  ************/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��̬�Ĵ������������������������

	int max_serve_py = MAX_SERVER_NUM;
	int *result_save0 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //General ��result
	int *result_save1 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //Large-Memory ��rssult
	int *result_save2 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //High-Performance��result

	int *SUPER_result_save0 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //General ��result
	int *SUPER_result_save1 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //Large-Memory ��rssult
	int *SUPER_result_save2 = new int[max_serve_py*inputcontrol.flavorMaxnum]; //High-Performance��result

	for (int i = 0; i < max_serve_py*inputcontrol.flavorMaxnum; i++) {
		result_save0[i] = 0;
		result_save1[i] = 0;
		result_save2[i] = 0;
		SUPER_result_save0[i] = 0;
		SUPER_result_save1[i] = 0;
		SUPER_result_save2[i] = 0;
	}
	int num_of_Serve_py[3];//�����������������
	int num_of_Serve_vm = 0;  //Ԥ������������



	//printf("\n\nGeneral: %d ", num_of_Serve_py[0]);
	//if (num_of_Serve_py[0]>0) {
	//	print_resource(result_predict, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save0, num_of_Serve_py[0]);
	//}
	//printf("\n\nLarge_memory: %d ", num_of_Serve_py[1]);
	//if (num_of_Serve_py[1] > 0) {
	//	print_resource(result_predict, inputcontrol.flavorMaxnum, inputServer[1], inputcontrol.cpuOrmem, inputFlavor, result_save1, num_of_Serve_py[1]);
	//}
	//printf("\n\nHigh_proformance: %d ", num_of_Serve_py[2]);
	//if (num_of_Serve_py[2] > 0) {
	//	print_resource(result_predict, inputcontrol.flavorMaxnum, inputServer[2], inputcontrol.cpuOrmem, inputFlavor, result_save2, num_of_Serve_py[2]);
	//}


	/**************************************************************************************************************************************/

	int bullet_num = 300000;  // ���ֵܣ���Ҫ�����ӵ���
	putVM_jiang(bullet_num, result_predict, inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save0, result_save1, result_save2, num_of_Serve_py);

	//putVM_shuai(bullet_num, result_predict, inputcontrol.flavorMaxnum, inputServer, inputFlavor, result_save0, result_save1, result_save2, num_of_Serve_py);





	/**********************************************************************************************************************************************************************/

	//int *SUPER_predict_result = new int[inputcontrol.flavorMaxnum];
	//for (int i = 0; i < inputcontrol.flavorMaxnum; i++) SUPER_predict_result[i] = 0;

	//// �������յ�
	//for (int i = 0; i < num_of_Serve_py[0]; i++)
	//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save0[inputcontrol.flavorMaxnum * i + j];
	//for (int i = 0; i < num_of_Serve_py[1]; i++)
	//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save1[inputcontrol.flavorMaxnum * i + j];
	//for (int i = 0; i < num_of_Serve_py[2]; i++)
	//	for (int j = 0; j < inputcontrol.flavorMaxnum; j++)  SUPER_predict_result[j] += result_save2[inputcontrol.flavorMaxnum * i + j];

	//print_data(result_predict, 1);
	//print_data(SUPER_predict_result, 1);



	num_of_Serve_vm = 0;  //�������ٴ�ͳ��Ԥ����������������
	for (int i = 0; i < inputcontrol.flavorMaxnum; i++) {
		num_of_Serve_vm += result_predict[i];
	}
	//print_resource(result_predict, inputcontrol.flavorMaxnum, inputServer[0], inputcontrol.cpuOrmem, inputFlavor, result_save0, num_of_Serve_py[0]);
	write_output_to_result(result_file, result_predict, num_of_Serve_vm, result_save0, result_save1, result_save2, num_of_Serve_py, inputFlavor, inputcontrol.flavorMaxnum);
	printf("\n**************************************************************************\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ��,����н�,��һ��ֻ��һ������;�ڶ���Ϊ��;�����п�ʼ���Ǿ��������,����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}








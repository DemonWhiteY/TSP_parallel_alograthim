#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#define ZUIDADIANSHU 200	//???????
#define QISHIDIAN 4			//?????
#define CHUSHIWENDU 1400000 //??????
#define WENDUXISHU 0.99		//???????
#define T_MIN 0.000001

//??????
char point_list[ZUIDADIANSHU][20];		  //?��??????
double point_coordinate[ZUIDADIANSHU][2]; //?��??x y????

int *simulation(int *x, int point_number);
double P(int *i, int *j, double t, int point_number);
int *simulation(int *x, int point_number);
double distance_sum(int *x, int point_number);
void Neighbour(int *father, int *result, int point_number);
double distance(int x, int y);
double random0_1(void);
int *random2(int point_number);
void print_coordinate(int *list, int point_number);
double mytime();

int main()
{
	//??????
	double tstart, tstop;

	char filename[81];
	double x_position, y_position;
	FILE *pfile;
	int *sequence;	  //???????
	int point_number; //??????  ????????��??�G?????????
	int i;			  // for?????????
	int *endlist;
	double sumlen;
	tstart = mytime();

	strcpy(filename, "100.txt");
	if ((pfile = fopen(filename, "r")) == NULL)
	{
		printf("?????????????%s!\n", filename);
		return 0;
	}

	/*????????????????*/
	fscanf(pfile, "%d", &point_number);
	for (i = 0; i < point_number; i++)
	{
		fscanf(pfile, "%s\t%lf\t%lf", &point_list[i], &x_position, &y_position);
		point_coordinate[i][0] = x_position;
		point_coordinate[i][1] = y_position;
	}
	fclose(pfile);

	srand((unsigned)time(NULL)); //???????? ??????????????
	sequence = random2(point_number);

	endlist = simulation(sequence, point_number);
	printf("last: ");
	for (i = 0; i < point_number; i++)
	{
		printf("%d ", endlist[i]);
	}
	sumlen = distance_sum(endlist, point_number);
	printf("\n sumlen:%lf\n", sumlen);
	free(endlist);
	free(sequence);

	tstop = mytime();
	printf("spend time=%lf\n", tstop - tstart);
	return 0;
}

/*?????????0????? 0-point_number??????????*/
int *random2(int point_number)
{
	int temp = 0, signal = 1, k = 0;
	int i, j;
	int *save = NULL;
	save = (int *)malloc(point_number * sizeof(int));

	//????????
	for (j = 0; j < point_number; j++)
		save[j] = -1;

	/*???????0-point_number??????????*/
	do
	{
		signal = 1;
		temp = rand() % point_number; // temp?0-point_number???
		for (i = 0; i < point_number; i++)
		{
			if (save[i] == temp)
			{
				signal = 0;
				break;
			}
		}
		if (signal != 0)
		{
			save[k] = temp;
			k++;
		}
	} while (signal == 0 || k != point_number);

	/*???save[0]=0??save[0]??????*/
	for (i = 1; i < point_number; i++)
	{
		if (save[i] == QISHIDIAN)
		{
			temp = save[i];
			save[i] = save[0];
			save[0] = temp;
		}
	}
	return save;
}

/*?????????,??��??0-1???*/
double random0_1(void)
{
	return (double)rand() / (double)RAND_MAX;
}

/*????x y 2?????????*/
double distance(int x, int y)
{
	double xy_distance = 0;
	//	printf("x=%d y=%d\t",x,y);
	xy_distance = sqrt((point_coordinate[x][0] - point_coordinate[y][0]) * (point_coordinate[x][0] - point_coordinate[y][0]) +
					   (point_coordinate[x][1] - point_coordinate[y][1]) * (point_coordinate[x][1] - point_coordinate[y][1]));
	return xy_distance;
}

/* ??????????
father?????????��?result??????????,point_number????????? ??????????��??result??*/
void Neighbour(int *father, int *result, int point_number)
{
	int n = 0, m = 0, temp = 0;
	int k;
	for (k = 0; k < point_number; k++)
		result[k] = father[k];

	do
	{
		n = rand() % (point_number - 1); //-1-point_number-1
		m = rand() % (point_number - 1);
	} while (n == m); //?????????????n??m
	n++;
	m++; // 0-point_number
	//????
	temp = result[n];
	result[n] = result[m];
	result[m] = temp;
}

/*?????????????��?????????????*/
double distance_sum(int *x, int point_number)
{
	double result = 0;
	int i;
	for (i = 0; i < point_number - 1; i++)
	{
		result += distance(x[i], x[i + 1]);
	}
	//	result += distance(x[point_number-1],x[0]);	//??????????
	return result;
}

/*???????????
???????T=CHUSHIWENDU ??????????rate=WENDUXISHU
x????? point_number?????*/
int *simulation(int *x, int point_number)
{

	int *temp = NULL;
	double random = 0;
	int *i = malloc(point_number * sizeof(int));
	int *j = malloc(point_number * sizeof(int));
	int m;
	double t = CHUSHIWENDU, rate = WENDUXISHU; //??????????????

	int L = 200 * point_number; //??????????????,??????????????????????
	int time = 0;				//??????????????????

	for (m = 0; m < point_number; m++)
	{
		i[m] = x[m];
		j[m] = 0;
	}
	double nextlen = 0.0, len = 0.0, local_len = 0.0, d_len = 0.0;
	do
	{
		do
		{
			for (m = 0; m < 10000; m++)
			{
				Neighbour(i, j, point_number);
				random = P(i, j, t, point_number);
				if ((random == 1.0) || (random > random0_1()))
				{
					temp = i;
					i = j;
					j = temp; //???i??????????????????
				}
			}
			local_len = distance_sum(i, point_number);
			nextlen = len;
			len = local_len;
			d_len = len - nextlen;
		} while (d_len > 0.1); //		f2 = distance_sum(i,point_number);
		t = rate * t;
	} while (t > T_MIN); //????????2??t>t_min
	free(j);
	return i;
}

// t??????????,i,j???????????????????,??????????????????
double P(int *i, int *j, double t, int point_number)
{
	double fi = 0, fj = 0;
	double result = 0;
	fi = distance_sum(i, point_number);
	fj = distance_sum(j, point_number);
	if (fj < fi)
		result = 1.0;
	else
		result = exp((fi - fj) / t);
	return result;
}
void print_coordinate(int *list, int point_number)
{
	int i;
	for (i = 0; i < point_number; i++)
	{
		printf("G00 X%.3lf Y%.3lf\n", point_coordinate[list[i]][0], point_coordinate[list[i]][1]);
	}
}

double mytime()
{
	double ts = 0.0;
	struct timeval mt;
	gettimeofday(&mt, (struct timezone *)0);
	ts = (double)(mt.tv_sec + mt.tv_usec * 1.0e-6);
	return (ts);
}


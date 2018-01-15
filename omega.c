/*************************************************************************
    > File Name: omega.c
    > Author: cgn
    > Func: 
    > Created Time: 一  1/15 21:26:25 2018
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX_LEN 128

struct sw //switch
{
	char in[2];
	short broadcast;
	short reduction;
};

struct cicle
{
	int len;
	char data[8];
};

struct permutation
{
	int nc;
	struct cicle cicles[8];
};

struct sw level0[4];
struct sw level1[4];
struct sw level2[4];


void init_level(struct sw* level)
{
	int i;
	for(i = 0;i < 4; ++i)
	{
		level[i].in[0] = -1;
		level[i].in[1] = -1;
		level[i].broadcast = 0;
		level[i].reduction = 0;
	}
}

void init_permutation(struct permutation* pm)
{
	pm->nc = 0;
	int i = 0;
	for(i = 0;i < 8; ++i)
	{
		pm->cicles[i].len = 0;
		memset(pm->cicles[i].data, 0, 8 * sizeof(char));
	}
}

void parse_permutation(char* str, struct permutation* pm)
{
	int i, j;
	int len = strlen(str);
	init_permutation(pm);
	for(i = 0; i < len; ++i)
	{
		if(str[i] == '(')
		{
			++ i;
			j = 0;
			while(str[i] && str[i] != ')')
			{
				if(str[i] >= '0' && str[i] <= '9')
				{
					pm->cicles[pm->nc].data[j] = str[i] - '0';
					++ j;
					pm->cicles[pm->nc].len ++;
				}
				++ i;
			}
			pm->nc ++;
		}
	}
}

void print_permutation(struct permutation* pm)
{
	int i, j;
	for(i = 0; i < pm->nc; ++ i)
	{
		printf("( ");
		for(j = 0; j < pm->cicles[i].len; ++ j)
		{
			printf("%d ", pm->cicles[i].data[j]+0);
		}
		printf(") ");
	}
	printf("\n");
}

void init_omega()
{
	//init
	init_level(level0);
	init_level(level1);
	init_level(level2);
}

int check_omega()
{
	int i;
	for(i = 0; i < 4; ++i)
	{
		if(level0[i].broadcast == 1 || level0[i].reduction == 1)
			return 0;
	}
	for(i = 0; i < 4; ++i)
	{
		if(level1[i].broadcast == 1 || level1[i].reduction == 1)
			return 0;
	}
	for(i = 0; i < 4; ++i)
	{
		if(level2[i].broadcast == 1 || level2[i].reduction == 1)
			return 0;
	}

	return 1;
}


void process_permutation(struct permutation* pm, int printflag)
{
	init_omega();
	int i, j, k, r, s, d;
	int from, to;
	for(i = 0;i < pm->nc; ++ i)
	{
		for(j = 0; j < pm->cicles[i].len; ++j)
		{
			from = pm->cicles[i].data[j];
			if(printflag)
				printf("%d ", from);
			to = (j != pm->cicles[i].len-1) ? pm->cicles[i].data[j+1] : pm->cicles[i].data[0];

			r = from % 4;
			s = from / 4;
			
			//level0
			d = (to >> 2) & 0x1;

			if(printflag)
				printf("%d->%d ", s, d);
			if(level0[r].in[s] == -1)
			{
				level0[r].in[s] = d;
				if(level0[r].in[1-s] == d)
					level0[r].reduction = 1;
			}
			else if(level0[r].in[s] != d) // another output pin
				level0[r].broadcast = 1;

			//level1
			from = r * 2 + d;
			r = from % 4;
			s = from / 4;
			d = (to >> 1) & 0x1;

			if(printflag)
				printf("%d->%d ", s, d);
			if(level1[r].in[s] == -1)
			{
				level1[r].in[s] = d;
				if(level1[r].in[1-s] == d)
					level1[r].reduction = 1;
			}
			else if(level1[r].in[s] != d)
				level1[r].broadcast = 1;


			//level2
			from = r * 2 + d;
			r = from % 4;
			s = from / 4;
			d = to & 0x1;

			if(printflag)
				printf("%d->%d ", s, d);
			if(level2[r].in[s] == -1)
			{
				level2[r].in[s] = d;
				if(level2[r].in[1-s] == d)
					level2[r].reduction = 1;
			}
			else if(level2[r].in[s] != d)
				level2[r].broadcast = 1;

			if(printflag)
				printf("%d\n", to);
			
		}
	}

	
}


void process(char* str)
{
	struct permutation pm;
	parse_permutation(str, &pm);
	if(pm.nc <= 0)
		return;
	process_permutation(&pm, 0);
	if(check_omega())
	{
		printf("\npass %s", str);
		process_permutation(&pm, 1);
	}
	else
	{
		printf("\ncongestion %s", str);
		int i;
		for(i = 0;i < 4; ++i)
			if(level0[i].broadcast == 1 || level0[i].reduction == 1)
				printf("level0 : switch %d\n", i);
		for(i = 0;i < 4; ++i)
			if(level1[i].broadcast == 1 || level1[i].reduction == 1)
				printf("level1 : switch %d\n", i);
		for(i = 0;i < 4; ++i)
			if(level2[i].broadcast == 1 || level2[i].reduction == 1)
				printf("level2 : switch %d\n", i);
	}
}


int main(int argc, char* argv[])
{

	if(argc < 2)
	{
		printf("usage : exeute [filename], such as : ./omege data.txt\n");
		return 0;
	}

	//char *str1 = "(0 7 6 4 2) (1 3) (5)";
	//char *str2 = "(0 6 4 7 3) (1 5) (2)";

	FILE* file = fopen(argv[1], "r");
	if(!file)
	{
		printf("no such file : %s\n", argv[1]);
		return 0;
	}
	char buffer[LINE_MAX_LEN];
	while(fgets(buffer, LINE_MAX_LEN, file))
	{
		process(buffer);
	}

	fclose(file);
	
	return 0;
}

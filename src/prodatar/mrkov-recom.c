/*
 ============================================================================
 Name        : lmc.c
 Author      : Yin Lu
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct prec{
	double pval;
	char c;
};

unsigned long rand_interval(unsigned long max) {
	srand(time(NULL ));
	return (rand() % max);
}

int random_in_range(unsigned int min, unsigned int max) {
	int base_random = rand(); /* in [0, RAND_MAX] */
	if (RAND_MAX == base_random)
		return random_in_range(min, max);
	/* now guaranteed to be in [0, RAND_MAX) */
	int range = max - min, remainder = RAND_MAX % range, bucket = RAND_MAX
			/ range;
	/* There are range buckets, plus one smaller interval
	 within remainder of RAND_MAX */
	if (base_random < RAND_MAX - remainder) {
		return min + base_random / bucket;
	} else {
		return random_in_range(min, max);
	}
}
int partition(struct prec arr[], int l, int r) {
	int  i, j;
	struct prec pivot, temp;
	pivot = arr[l];
	i = l; j = r+1;
	while (1) {
		do ++i; while (arr[i].pval <= pivot.pval && i <= r);
		do --j; while (arr[j].pval > pivot.pval);
		if(i >= j) break;
		temp = arr[i]; arr[i] = arr[j]; arr[j] = temp;
	}
	temp = arr[l]; arr[l] = arr[j]; arr[j] = temp;
	return j;
}

void quickSort(struct prec arr[], int l, int r){
	int j;
	if(l < r){
		j = partition (arr, l, r);
		quickSort(arr, l, j-1);
		quickSort(arr, j+1, r);
	}
}


int main(int argc, char **argv) {
	unsigned long sampleSize, querySize, poolSize;
	int cachesize;

	unsigned long sfmtrix[52][52];
	double spmtrix[52][52];

	struct prec psortarr[52];

	int i, j, k;
	char* samplearr;
	char* queryarr;
	if(argc<3){
	printf("run as ./lmc samplesize querysize cachesize\n"); 
	return 0;
	}
	srand(time(NULL ));

	sscanf(argv[1], "%ld", &sampleSize);
	sscanf(argv[2], "%ld", &querySize);
	sscanf(argv[3], "%d", &cachesize);

	samplearr = malloc(sampleSize * sizeof(char));
	queryarr = malloc(querySize * sizeof(char));

	for (i = 0; i < 52; i++) {
		psortarr[i].pval = 0;
		psortarr[i].c = ' ';
	}
	unsigned long ridx;
	//unsigned long ridx = rand_interval(sampleSize - querySize);
	printf("\n");
	//init fmtrix pmtrix qscore
	for (i = 0; i < 52; i++) {
		for (j = 0; j < 52; j++) {
			sfmtrix[i][j] = 0;
			spmtrix[i][j] = 0;
		}
	}

	for (i = 0; i < sampleSize; ++i) {
		char randomletter =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[random_in_range(
						0, 52)];
		//putchar(randomletter); /* test random letter */
		samplearr[i] = randomletter;
		if (i >= ridx && i < (ridx + querySize))
			queryarr[i] = randomletter;
	}
	for (i = 0, j = 1; j < sampleSize; ++i, ++j) {
		char rllft = samplearr[i];
		char rlrgt = samplearr[j];
		int row =
				(rllft >= 'A' && rllft <= 'Z') ? rllft - 'A' : rllft - 'a' + 26;
		int col =
				(rlrgt >= 'A' && rlrgt <= 'Z') ? rlrgt - 'A' : rlrgt - 'a' + 26;
		sfmtrix[row][col]++;
		/*		if (i >= ridx && i < (ridx + querySize))
		 qfmtrix[row][col]++;*/
	}
	printf("\n");

	//sample pmtrix
	for (i = 0; i < 52; i++) {
		double sumrow = 0;
		for (j = 0; j < 52; j++) {
			sumrow = sumrow + sfmtrix[i][j];
		}
		for (j = 0; j < 52; j++) {
			if (sfmtrix[i][j] != 0 && sumrow != 0)
				spmtrix[i][j] = sfmtrix[i][j] / sumrow;
		}
	}

	//query
	int hits = 0;
	ridx=poolSize;
	int queryi=0;
	int cachei=0;
	int cachepos=0;
	int totalcachepos=0;
	//for (i = ridx, j = ridx+1; j < (ridx + querySize); ++i, ++j) 
	for (;queryi<querySize;queryi++){
		char rllft = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[random_in_range(
						0, 52)];

		char rlrgt = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[random_in_range(
						0, 52)];
		cachepos=0;
		
		for(cachei=0;cachei<cachesize;cachei++){
			char pos = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[random_in_range(
						0, 52)];	
			if(pos == rlrgt)
			cachepos=1;	
		}
		totalcachepos+=cachepos;
		int row =
				(rllft >= 'A' && rllft <= 'Z') ? rllft - 'A' : rllft - 'a' + 26;
		for (k = 0; k < 52; ++k ) {
			psortarr[k].c = k < 26 ? 'A' + k : 'a' + k - 26;
			psortarr[k].pval = spmtrix[row][k];
		}
		int colnum = sizeof (psortarr) / sizeof (psortarr[0]);
		quickSort(psortarr, 0, colnum - 1);
		//display sorted pvalues
		char prediction = ' ';
		for (k = 51; k > (51 - cachesize); k--){
			prediction = psortarr[k].c;
			if (prediction == rlrgt) {
				hits++;
				//putchar(prediction);
				break;
			}
		}
	}

	//printf("\nhit times: %d \n",hits );
	double hitrate = (double) hits / (double) (querySize - 1);
	printf("Markov-Theory-Practi\t%.2lf\t%.2lf\t%.2lf\n", hitrate,(double) cachesize/52.0,(double) totalcachepos / (double) (querySize-1));
	//printf("Theory\t%.2lf\n",(double) cachesize/52.0);
	//printf("Practi\t%.2lf\n",(double) totalcachepos / (double) (querySize-1));
	printf("\n");

	return EXIT_SUCCESS;
}

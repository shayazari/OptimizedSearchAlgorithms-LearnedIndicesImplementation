/* A simplified learned index implementation:
 *
 * Skeleton code written by Jianzhong Qi, April 2023
 * Edited by: Shayan Azari Pour 1317598
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define DATASET_SIZE 100					  /* number of input integers */
#define DATA_OUTPUT_SIZE 10					  /* output size for stage 1 */

#define BS_NOT_FOUND (-1)					  /* used by binary search */
#define BS_FOUND 0

#define SEARCH_KEY "search key in data domain."
#define NOT_SEARCH_KEY "not found!"
#define MAX_DATA_VALUE 999

typedef int data_t; 				  		  /* data type */

/* struct for stage 2 mapping functions */ 
typedef struct {							 
	int a;
    int b;
	int max_elmnt;
	int count;
} segment_t;


/****************************************************************/

/* function prototypes */
void print_stage_header(int stage_num);
int cmp(data_t *x1, data_t *x2);
void swap_data(data_t *x1, data_t *x2);
void partition(data_t dataset[], int n, data_t *pivot, 
		int *first_eq, int *first_gt);
void quick_sort(data_t dataset[], int n);
int binary_search(data_t searching_in[], int lo, int hi, 
	data_t *key, int *locn, int *min_element_index);

void stage_one(data_t dataset[], int* max_err, data_t search_keys[]);
void stage_two(data_t dataset[]);
void stage_three(data_t dataset[], int max_err, segment_t* new_segment, 
	data_t max_elements[]);
void stage_four(data_t dataset[], int max_err, segment_t* new_segment, 
	data_t search_keys[],
	data_t max_elements[]);

/* add your own function prototypes here */

int f_key_calc(int a, int b, int key);
void a_b_calc(int* a, int* b, data_t dataset[], int i);
void add_new_segment(int first, int second, int third, segment_t* new_segment, 
	int n, data_t max_elements[]);
int min(int f_key, int max_err);
int max(int f_key, int max_err);
void step_2(data_t dataset[], data_t max_elements[], data_t search_keys[], 
	segment_t* new_segment, int i, int *min_element_index);
void step_3(data_t dataset[], data_t search_keys[], segment_t* new_segment, 
	int i, int min_element_index, int max_err);

/****************************************************************/

/* main function controls all the action */
int
main(int argc, char *argv[]) {
	/* to hold all input data */
	data_t dataset[DATASET_SIZE];
	int max_err;
	/* to hold all mapping functions */ 
	segment_t new_segment[DATASET_SIZE]; 
	/* to hold all search keys from last line of input */ 
	data_t search_keys[DATASET_SIZE];
	/* to hold each max element from each mapping function */ 
	data_t max_elements[DATASET_SIZE];

	/* stage 1: read and sort the input */
	stage_one(dataset, &max_err, search_keys);
	
	/* stage 2: compute the first mapping function */
	stage_two(dataset);
	
	/* stage 3: compute more mapping functions */ 
	stage_three(dataset, max_err, new_segment, max_elements);
	
	/* stage 4: perform exact-match queries */
	stage_four(dataset, max_err, new_segment, search_keys, max_elements);
	
	return 0;
}

/****************************************************************/
/* algorithms are fun */
/* i love algorithms */
/* i'd love it more if i got full marks */

/* stage 1: read and sort the input */
void 
stage_one(data_t dataset[], int* max_err, data_t search_keys[]){
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	int num, count = 0, i = 0, j = 0;

	/* assigns values to relevant "place" based on 
	the location of the integer scanf is looking at */
	while(scanf("%d", &num)!=EOF){
		count++;
		if(count<=100){
			dataset[i] = num;
		}
		else if(count==101){
			*max_err = num;
		}
		else if(count>101){
			search_keys[j] = num;
			j++;
		}
		i++;
	}

	search_keys[j] = 1000;	/* arbitrary indicator for end of array */
	quick_sort(dataset, DATASET_SIZE);
	

	/* stage 1 output */
	printf("First 10 numbers: ");
	for(i=0; i<DATA_OUTPUT_SIZE; i++){
		if(i==DATA_OUTPUT_SIZE-1){
			printf("%d", dataset[i]);
			break;
		}
		printf("%d ", dataset[i]);
	}
	printf("\n\n");
}

/****************************************************************/

/* stage 2: compute the first mapping function */
void 
stage_two(data_t dataset[]){
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	int a = 0, b = 0;	/* default if first 2 dataset values are equal */
	int f_key, err_f, max_key, max_position, max_err_f = 0;

	/* assigining relevant values when first 2 elements not equal  */
	if(dataset[0]!=dataset[1]){
		a = -dataset[0];
		b = dataset[1] - dataset[0];
		/* helper functions calculate values; 
		exiting loop with maximum predicion error */
		for(int i=0; i<DATASET_SIZE; i++){
			f_key = f_key_calc(a, b, dataset[i]);
			err_f = abs(i-f_key);
			if(err_f > max_err_f){
				max_position = i;
				max_key = dataset[i];	
				max_err_f = err_f;
			}
		}
	}

	/* when first 2 dataset values are equal, only one possible case */
	else{
		max_key = dataset[DATASET_SIZE-1];
		max_position = DATASET_SIZE-1;
		max_err_f = DATASET_SIZE-1;
	}

	/* stage 2 output */
	printf("Maximum prediction error: %d\n", max_err_f);
	printf("For key: %d\n", max_key);
	printf("At position: %d\n", max_position);
	printf("\n");
}

/****************************************************************/

/* stage 3: compute more mapping functions */ 
void 
stage_three(data_t dataset[], int max_err, segment_t* new_segment, 
	data_t max_elements[]){
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	/*  if first 2 dataset values are not equal */
	int a = -dataset[0], b = dataset[1] - dataset[0];

	int f_key, err_f, i = 2, n = 0, j = 0, count = 0;
	
	new_segment[0].count = 0;

	printf("Target maximum prediction error: %d\n", max_err);
	
	/*  calculating values and outputting for stage 3 */
	while(i<=DATASET_SIZE-1){
		/* first loop starts at i = 2 with pre-assigned a & b */
		if(count>0){	
			a_b_calc(&a, &b, dataset, i); 
		}
		/* manual assignment as i==2, but a & b based on first 2 values */
		else if(i==2 && dataset[i-2]==dataset[i-1]){
			a = 0;	
			b = 0;
		}
		/* whether or not dataset value is covered by errm */
		for(j=i; j<=DATASET_SIZE-1; j++){
			f_key = f_key_calc(a, b, dataset[j]);
			err_f = abs(j-f_key);

			/* when maximum element for mapping function found, calls up
			helper function then resets loop with updated indexes */
			if(err_f>max_err){
				add_new_segment(a, b, dataset[j-1], new_segment, n, max_elements);
				n++;
				i = j;
				break;
			}
			/* when dataset value is the last in the array, special condition*/
			if(j==DATASET_SIZE-1){
				add_new_segment(a, b, dataset[j], new_segment, n, max_elements);
				i = j;
				printf("\n");
				return;
			}
		}
		count++;
	}
}

/****************************************************************/

/* stage 4: perform exact-match queries */
void stage_four(data_t dataset[], int max_err, segment_t* new_segment, 
	data_t search_keys[], data_t max_elements[]){
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	int i = 0, min_element_index = 0;

	/* iterating through search keys array that was scanned 
	and running helper functions for stages 2 and 3 */
	while(search_keys[i]<=MAX_DATA_VALUE){
		printf("Searching for %d:\n", search_keys[i]);

		/* if within dataset boundary, can proceed to steps 2 and 3 */
		if(search_keys[i]>=dataset[0] && search_keys[i]<=dataset[DATASET_SIZE-1]){
			/* Step 1 */
			printf("Step 1: %s\n", SEARCH_KEY);
			/* Step 2 */
			step_2(dataset, max_elements, search_keys, new_segment, i, 
				&min_element_index);
			/* Step 3 */
			step_3(dataset, search_keys, new_segment, i, min_element_index, max_err);
		}
		
		/* Step 1 not found */
		else{
			printf("Step 1: %s\n", NOT_SEARCH_KEY);
		}
		i++;
	}
	printf("\n");
}

/****************************************************************/

/* calculates and returns the f(key) value; considers special case */
int f_key_calc(int a, int b, int key){
	if(b==0){
		return a;
	}
	return ceil((key + a)/(float)b);
}

/****************************************************************/

/* calculates a & b based on if value is at last location in array or 
if equal to the proceeding value */
void a_b_calc(int* a, int* b, data_t dataset[], int i){
	if(i==DATASET_SIZE-1){
		*a = i;
		*b = 0;
	}
	else if(dataset[i]==dataset[i+1]){
		*a = i;
		*b = 0;
	}
	else if(dataset[i]!=dataset[i+1]){
		*a = (dataset[i+1] * (i)) - (dataset[i] * (i+1));
		*b = dataset[i+1] - dataset[i];
	}
}

/****************************************************************/

/* for stage 3, whenever maximum element found or when final value not in 
previous mapping function, assigns relevant values into struct
and prints each function as output for stage 2*/
void add_new_segment(int first, int second, int third, segment_t* new_segment, 
	int n, data_t max_elements[]){
	new_segment[n].a = first;
	new_segment[n].b = second;
	new_segment[n].max_elmnt = third;
	max_elements[n] = third;
	new_segment[0].count++; /* for step_2 function to use as maximum 
							perimeter for binary search */
							
	/* stage 3 output (most of) */
	printf("Function%3d: a =%5d, b =%4d, max element =%4d\n",
	 n, new_segment[n].a, new_segment[n].b, new_segment[n].max_elmnt);
}

/****************************************************************/

/* for stage 4, calculates the maximum of two values; used as
minimum perimeter in step_3 function for binary search */
int min(int f_key, int max_err){
	int min = 0;
	if((f_key-max_err)>=0){
		min = (f_key-max_err);
	}
	return min;
}

/****************************************************************/

/* for stage 4, calculates the minimum of two values; used as
maximum perimeter in step_3 function for binary search */
int max(int f_key, int max_err){
	int max = DATASET_SIZE-1;
	if((f_key+max_err)<=DATASET_SIZE-1){
		max = (f_key+max_err);
	}

	/* binary_search function takes 'n' as maximum perimiter; current 
	max value represents 'n-1' */
	max++; 

	return max;
}

/****************************************************************/

/* step 2 of stage 4; calls binary search over array of mapping functions 
to locate the mapping function covering the relevant key */
void step_2(data_t dataset[], data_t max_elements[], data_t search_keys[], 
	segment_t* new_segment, int i, int *min_element_index){
	int locn = 0, function_count = new_segment[0].count;

	
	printf("Step 2:");
	/* as output for stage 4, binary_search function prints each 
	element that has been compared with during the search process */
	binary_search(max_elements, 0, function_count, &search_keys[i], 
		&locn, min_element_index);
	printf("\n");
}

/****************************************************************/

/* step 3 of stage 4; calls binary search over array of mapping functions 
to locate the mapping function covering the relevant key */
void step_3(data_t dataset[], data_t search_keys[], segment_t* new_segment, 
	int i, int min_element_index, int max_err){

	int a, b, f_key, range_min, range_max, locn = 0;
	printf("Step 3:");
	a_b_calc(&a, &b, dataset, i);

	/* a & b from the mapping function with lowest maximum element;
	min_element_index from step_2 (from binary_search function */
	a = new_segment[min_element_index].a;
	b = new_segment[min_element_index].b;
	
	f_key = f_key_calc(a, b, search_keys[i]);
	range_min = min(f_key, max_err);
	range_max = max(f_key, max_err);

	/* attempting to find actual location of key with (possibly) shortened
	perimiter; as output for stage 4, binary_search function prints each 
	element that has been compared with during the search process */
	binary_search(dataset, range_min, range_max, &search_keys[i], 
		&locn, &min_element_index);

	/* different outputs based on if key was found */
	if(dataset[locn]==search_keys[i]){
		printf(" @ dataset[%d]!", locn);
	}
	else{
		printf(" not found!");
	}
	printf("\n");

}

/****************************************************************/

/* print stage header given stage number */
void 
print_stage_header(int stage_num){
	printf(STAGE_HEADER, stage_num);
}

/* data swap function used by quick sort, adpated from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/sortscaffold.c
*/
void
swap_data(data_t *x1, data_t *x2){
	data_t t;
	t = *x1;
	*x1 = *x2;
	*x2 = t;
}

/* partition function used by quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void
partition(data_t dataset[], int n, data_t *pivot,
		int *first_eq, int *first_gt){
	int next=0, fe=0, fg=n, outcome;
	
	while (next<fg) {
		if ((outcome = cmp(dataset+next, pivot)) < 0) {
			swap_data(dataset+fe, dataset+next);
			fe += 1;
			next += 1;
		} else if (outcome > 0) {
			fg -= 1;
			swap_data(dataset+next, dataset+fg);
		} else {
			next += 1;
		}
	}
	
	*first_eq = fe;
	*first_gt = fg;
	return;
}

/* quick sort function, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void
quick_sort(data_t dataset[], int n){
	data_t pivot;
	int first_eq, first_gt;
	if (n<=1) {
		return;
	}
	/* array section is non-trivial */
	pivot = dataset[n/2]; // take the middle element as the pivot
	partition(dataset, n, &pivot, &first_eq, &first_gt);
	quick_sort(dataset, first_eq);
	quick_sort(dataset + first_gt, n - first_gt);
}

/* comparison function used by binary search and quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
*/
int
cmp(data_t *x1, data_t *x2){
	return (*x1-*x2);
}

/* binary search between dataset[lo] and dataset[hi-1], adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
   note: modifications made to take in an extra input and to print each 
   element that has been compared with during the search process 
*/
int
binary_search(data_t searching_in[], int lo, int hi, data_t *key, 
	int *locn, int *min_element_index){
	int mid = 0, outcome;

	if (lo>=hi) {
		return BS_NOT_FOUND;
	}

	mid = (lo+hi)/2;

	/* for steps 2 and 3, printing the element that has been 
	compared with during the search process each time */
	printf(" %d", searching_in[mid]);

	/* for the "step_2" call-up of the binary_search function, 
	re-assigning the index of the smallest maximum element  
	from mapping functions that is >= than the key (used
	in "step_3") */
	if(*key<=searching_in[mid]){
		*min_element_index = mid;	
	}

	if ((outcome = cmp(key, searching_in+mid)) < 0) {
		return binary_search(searching_in, lo, mid, key, locn, 
			min_element_index);
	} else if (outcome > 0) {
		return binary_search(searching_in, mid+1, hi, key, locn, 
			min_element_index);
	} else {
		*locn = mid;
		return BS_FOUND;
	}
}
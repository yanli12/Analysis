//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "../include/map_reduce.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

//Implement map_reduce.h functions here.

/**
 * Validates the command line arguments passed in by the user.
 * @param  argc The number of arguments.
 * @param  argv The arguments.
 * @return      Returns -1 if arguments are invalid (refer to hw document).
 *              Returns 0 if -h optional flag is selected. Returns 1 if analysis
 *              is chosen. Returns 2 if stats is chosen. If the -v optional flag
 *              has been selected, validateargs returns 3 if analysis
 *              is chosen and 4 if stats is chosen.
 */
int validateargs(int argc, char** argv){
	int i = 1;
	if (argv[1] == NULL)
		return -1;
	else if (strcmp(argv[i], "-h")==0){
		printf("$ ./mapreduce -h\n");
        printf("Usage: ./mapreduce [h|v] FUNC DIR\n");
        printf("    FUNC    Which operation you would like to run on the data:\n");
        printf("            ana - Analysis of various text files in a directory.\n");
        printf("            stats - Calculates stats on files which contain only numbers.\n");
        printf("    DIR     The directory in which the files are located.\n\n");
        printf("    Options:\n");
        printf("    -h Prints this help menu.\n");
        printf("    -v Prints the map function’s results, stating the file it’s from.\n");
        printf("$\n");
        return 0;
	}	
    else if (strcmp(argv[i], "-v")==0){
    	i++;
    	if (strcmp(argv[i], "stats")==0)
        	return 3;
        else if (strcmp(argv[i], "ana")==0)
        	return 4;
        else
        	return -1;
    }
    else
    	return -1;
}

/**
 * Counts the number of files in a directory EXCLUDING . and ..
 * @param  dir The directory for which number of files is desired.
 * @return     The number of files in the directory EXCLUDING . and ..
 *             If nfiles returns 0, then print "No files present in the
 *             directory." and the program should return EXIT_SUCCESS.
 *             Returns -1 if any sort of failure or error occurs.
 */
int nfiles(char* dir){
	int fcount = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir(dir);
	if (dirp == NULL){
		printf("ERROR! No such directory found!\n");
		return -1;
	}
	while((entry = readdir(dirp)) != NULL){
		if(entry->d_type == DT_REG){
			fcount++;
		}
	}
	closedir(dirp);
	return fcount;
}

/**
 * The map function goes through each file in a directory, performs some action on
 * the file and then stores the result.
 *
 * @param  dir     The directory that was specified by the user.
 * @param  results The space where map can store the result for each file.
 * @param  size    The size of struct containing result data for each file.
 * @param  act     The action (function map will call) that map will perform on
 *                 each file. Its argument f is the file stream for the specific
 *                 file. act assumes the filestream is valid, hence, map should
 *                 make sure of it. Its argument res is the space for it to store
 *                 the result for that particular file. Its argument fn is a
 *                 string describing the filename. On failure returns -1, on
 *                 sucess returns value specified in description for the act
 *                 function.
 *
 * @return        The map function returns -1 on failure, sum of act results on
 *                success.
 */
int map(char* dir, void* results, size_t size, int (*act)(FILE* f, void* res, char* fn)){
	DIR * dirp;
	struct dirent * entry;
	dirp = opendir(dir);
	int actcount = 0;
	total_byte_directory = 0;
	//int file_count = nfiles(dir);
	if (dirp == NULL){
		printf("ERROR! No such directory found!\n");
		return -1;
	}
	while((entry = readdir(dirp)) != NULL){
		if(entry->d_type == DT_REG){
			//char fn[1024], path[1024];
			memset(path, '\0', sizeof(path));
			strcpy(path,dir);
			strcat(path,"/");
			memset(fn,'\0',sizeof(fn));
			strcpy(fn,entry->d_name);
			strcat(path,fn);
			FILE *f=fopen(path, "r+");
			act(f, results, fn);
			actcount++;
			results = results + size;
			fclose(f);
		}
	}
	int i=0;
	//reset result pointer
	for(i=0;i<actcount;i++){
		results = results - size;
	}
	closedir(dirp);
	return actcount;
}


/**
 * This reduce function takes the results produced by map and cumulates all
 * the data to give one final Analysis struct. Final struct should contain 
 * filename of file which has longest line.
 *
 * @param  n       The number of files analyzed.
 * @param  results The results array that has been populated by map.
 * @return         The struct containing all the cumulated data.
 */
struct Analysis analysis_reduce(int n, void* results){
	struct Analysis final_ana = {};
	if (n <= 0){
		return final_ana;
	}
	int i,j,k;
	char* reduce_fn =0;
	int reduce_max =0 ;
	int reduce_line =0 ;
	for (i = 0; i< n; i++){
		if(((struct Analysis*)results)->lnlen > reduce_max){
			reduce_fn = ((struct Analysis*)results)->filename;
			reduce_max = ((struct Analysis*)results)->lnlen;
			reduce_line = ((struct Analysis*)results)->lnno;
		}
		for(k=0;k<128;k++){
			int s= ((struct Analysis*)results)->ascii[k];
			if (s > 0){
				for (j=0;j<s;j++){
					final_ana.ascii[k]++;
				}
			}
		}
		results = results+sizeof(struct Analysis);
	}
	final_ana.filename = reduce_fn;
	final_ana.lnno = reduce_line;
	final_ana.lnlen = reduce_max;


	return  final_ana;
}

/**
 * This reduce function takes the results produced by map and cumulates all
 * the data to give one final Stats struct. Filename field in the final struct 
 * should be set to NULL.
 *
 * @param  n       The number of files analyzed.
 * @param  results The results array that has been populated by map.
 * @return         The struct containing all the cumulated data.
 */
Stats stats_reduce(int n, void* results){
	Stats final_stats = {};
	if (n<=0){
		printf("ERROR!!! INVALID number of files analyzed.\n");
		return final_stats;
	}
	int i,j,k;
	int redcue_num = 0;
	int reduce_sum = 0;
	// combin histogram
	for(i=0;i<n;i++){
		for(j=0;j<NVAL;j++){
			if(((Stats*)results)->histogram[j] > 0){
				for(k=0;k<((Stats*)results)->histogram[j];k++){
					final_stats.histogram[j]++;
					redcue_num++;
					reduce_sum += j;
				}
			}
		}
		results = results + sizeof(Stats);
	}
	final_stats.sum = reduce_sum;
	final_stats.n = redcue_num;
	return final_stats;
}


/**
 * Always prints the following:
 * - The name of the file (for the final result the file with the longest line)
 * - The longest line in the directory's length.
 * - The longest line in the directory's line number.
 *
 * Prints only for the final result:
 * - The total number of bytes in the directory.
 *
 * If the hist parameter is non-zero print the histogram of ASCII character
 * occurrences. When printing out details for each file (i.e the -v option was
 * selected) you MUST NOT print the histogram. However, it MUST be printed for
 * the final result.
 *
 * Look at sample output for examples of how this should be print. You have to
 * match the sample output for full credit.
 *
 * @param res    The final result returned by analysis_reduce
 * @param nbytes The number of bytes in the directory.
 * @param hist   If this is non-zero, prints additional information. (Only non-
 *               zero for printing the final result.)
 * ONLY PRINT FINAL RESULT
 */
void analysis_print(struct Analysis res, int nbytes, int hist){
	printf("File: %s\n", res.filename);
    printf("Longest line length: %d\n", res.lnlen);
    printf("Longest line number: %d\n", res.lnno);
    printf("Total Bytes in directory: %d\n", nbytes);
	// if hist is not zero, print histogram
	if (hist != 0){
		printf("Histogram: \n");
		int i = 0;
		for(i=0;i<128;i++){
			if(res.ascii[i] != 0){
				int j = 0;
				printf(" %d: ", i);
				for(j=0;j<res.ascii[i];j++){
					printf("-");
				}
				printf("\n");
			}
		}
	}
    printf("$\n");
}

/**
 * Always prints the following:
 * Count (total number of numbers read), Mean, Mode, Median, Q1, Q3, Min, Max
 *
 * Prints only for each Map result:
 * The file name
 *
 * If the hist parameter is non-zero print the the histogram. When printing out
 * details for each file (i.e the -v option was selected) you MUST NOT print the
 * histogram. However, it MUST be printed for the final result.
 *
 * Look at sample output for examples of how this should be print. You have to
 * match the sample output for full credit.
 *
 * @param res  The final result returned by stats_reduce
 * @param hist If this is non-zero, prints additional information. (Only non-
 *             zero for printing the final result.)
 */
void stats_print(Stats res, int hist){
	// if hist is not zero, print histogram
	int i = 0;
	int j = 0;
	if (hist != 0){
		printf("Histogram: \n");
		for(i=0;i<NVAL;i++){
			if(res.histogram[i] != 0){
				printf(" %d: ", i);
				for(j=0;j<res.histogram[i];j++){
					printf("-");

				}
				printf("\n");
			}
		}
	}
	printf("\n");
	i = 0;
	int mode[100];
	int mode_count = 0;
	int most_occur = 0;
	int max = 0;
	int min = 0;
	double median = 0;
	double qone = 0;
	double qthree = 0;
	int sp_array[res.n];
	int sp_index=0;
	for(i = 1; i<NVAL;i++){
		// find most occured number
		if (res.histogram[i] > res.histogram[i-1]){
			most_occur = res.histogram[i];
		}
		// find max
		if (res.histogram[i] != 0){
			max = i;
		}
	}
	//find all mode
	for (i=0;i<NVAL;i++){
		if (res.histogram[i] == most_occur){
			mode[mode_count] = i;
			mode_count++;
		}
		
	}

	//put all data in an array
	for (i=0;i<NVAL;i++){
		if (res.histogram[i] != 0){
			for(j=0;j<res.histogram[i];j++){
				sp_array[sp_index] = i;
				sp_index++;
			}
		}
	}
	//  find min
	for(i = 1; i<NVAL;i++){
		if (res.histogram[i] != 0){
			min = i;
			break;
		}
	}
	median = ceil(res.n*0.5);
	//find Q1
	qone = ceil(res.n * 0.25);
	//find Q3
	qthree = ceil(res.n * 0.75);
	//print final result
	printf("Count: %d\n", res.n);
	printf("Mean: %f\n", (double)res.sum/res.n);
	printf("Mode: ");
	for(i=0;i<mode_count;i++){
		printf("%d ", mode[i]);
	}
	printf("\n");
	printf("Median: %f\n",(double)sp_array[(int)median-1]);
	printf("Q1: %f\n", (double)sp_array[(int)qone - 1]);
	printf("Q3: %f\n", (double)sp_array[(int)qthree - 1]);
	printf("Min: %d\n", min);
	printf("Max: %d\n", max);
	printf("$\n");
}


/**
 * This function performs various different analyses on a file. It
 * calculates the total number of bytes in the file, stores the longest line
 * length and the line number, and frequencies of ASCII characters in the file.
 *
 * @param  f        The filestream on which the action will be performed. You
 *                  you can assume the filestream passed by map will be valid.
 * @param  res      The slot in the results array in which the data will be
 *                  stored.
 * @param  filename The filename of the file currently being processed.
 * @return          Return the number of bytes read.
 */
int analysis(FILE* f, void* res, char* filename){
	memset(((struct Analysis*)res)->ascii, 0, 128);
	char c;
	int n = 0;
    int count_each_line = 0;
    int max_each_line = 0;
    int line_number = 1;
    int longest_line = 1;
    
    while((c = fgetc(f)) != EOF) {
    	if(c != '\n'){
    		count_each_line++;
    	}
    	else{
    		if (count_each_line > max_each_line){
    			max_each_line = count_each_line;
    			longest_line = line_number; 
    		}
    		count_each_line = 0;  //reset counter
    		line_number++;
    	}
        //printf("%c", c);
       	int i = c;			//convert char we just read to int
        //printf("%d\n", i);
        ((struct Analysis*)res)->ascii[i]++;	//store in the array, to make a histogram later
        total_byte_directory++;	// number of bytes in this whole directory
        n++; // number of byte in this file

    }
    ((struct Analysis*)res)->lnlen = max_each_line;
    ((struct Analysis*)res)->lnno = longest_line;
    ((struct Analysis*)res)->filename = filename;
    printf("File: %s\n", ((struct Analysis*)res)->filename);
    printf("Longest line length: %d\n", ((struct Analysis*)res)->lnlen);
    printf("Longest line number: %d\n", ((struct Analysis*)res)->lnno);
    printf("\n");
    /**
    *struct Analysis *pt = res;
	*analysis_print(*pt, total_byte_directory, 0); // print struct
	*/
    return n;
}

/**
 * This function counts the number of occurrences of each number in a file. It
 * also calculates the sum total of all numbers in the file and how many numbers
 * are in the file. If the file has an invalid entry return -1.
 *
 * @param  f        The filestream on which the action will be performed. You
 *                  you can assume the filestream passed by map will be valid.
 * @param  res      The slot in the results array in which the data will be
 *                  stored
 * @param  filename The filename of the file currently being processed.
 * @return          Return 0 on success and -1 on failure.
 */
int stats(FILE* f, void* res, char* filename){
	memset(((Stats*)res)->histogram, '\0', 32);
	int n = 0;
	int count = 0;
	int sum = 0;
	int i = 0;
	while((fscanf(f,"%d", &n)) != EOF){
		if (n < 0 || n > 32){
			printf("ERROR!!! Invaild number read from file.\n");
			return -1;
		}
		else{
			((Stats*)res)->histogram[n]++;
			count++;
			sum = sum + n;
		}
	}
	((Stats*)res)->filename = filename;
	((Stats*)res)->sum = sum;
	((Stats*)res)->n = count;
	printf("File: %s\n", (((Stats*)res))->filename);
	printf("Count: %d\n", ((Stats*)res)->n);
	printf("Mean: %f\n", (double)(((Stats*)res)->sum) / count) ;
	i = 0;
	int mode[100];
	int mode_count = 0;
	int most_occur = 0;
	int max = 0;
	int min = 0;
	double median = 0;
	double qone = 0;
	double qthree = 0;
	int j = 0;
	int summer[((Stats*)res)->n];
	int summer_count = 0;
	for(i = 1; i<NVAL;i++){
		// find most occured number
		if (((Stats*)res)->histogram[i] > ((Stats*)res)->histogram[i-1]){
			most_occur = ((Stats*)res)->histogram[i];
		}
		// find max
		if (((Stats*)res)->histogram[i] != 0){
			max = i;
		}
	}
	//find all mode
	for (i=0;i<NVAL;i++){
		if (((Stats*)res)->histogram[i] == most_occur){
			mode[mode_count] = i;
			mode_count++;
		}
		//put all data in an array
		if (((Stats*)res)->histogram[i] != 0){
			for(j=0;j<((Stats*)res)->histogram[i];j++){
				summer[summer_count] = i;
				summer_count++;
			}
		}
	}
	//  find min
	for(i = 1; i<NVAL;i++){
		if (((Stats*)res)->histogram[i] != 0){
			min = i;
			break;
		}
	}
	//find median 
	median = ceil(((Stats*)res)->n*0.5);
	//find Q1
	qone = ceil(((Stats*)res)->n * 0.25);
	//find Q3
	qthree = ceil(((Stats*)res)->n * 0.75);

	//print current file
	printf("Mode: ");
	for(i=0;i<mode_count;i++){
		printf("%d ", mode[i]);
	}
	printf("\n");
	printf("Median: %f\n", (double)median);
	printf("Q1: %f\n", (double)summer[(int)qone - 1]);
	printf("Q3: %f\n", (double)summer[(int)qthree - 1]);
	printf("Min: %d\n", min);
	printf("Max: %d\n", max);
	printf("\n");	
	return 0;
}


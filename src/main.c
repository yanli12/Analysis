#include "../include/map_reduce.h"

//Space to store the results for analysis map
struct Analysis analysis_space[NFILES];
//Space to store the results for stats map
Stats stats_space[NFILES];
//Sample Map function action: Print file contents to stdout and returns the number bytes in the file.
int cat(FILE* f, void* res, char* filename) {
    char c;
    int n = 0;
    printf("%s\n", filename);
    while((c = fgetc(f)) != EOF) {
        printf("%c", c);
        n++;
    }
    printf("\n");
    return n;
}
int main(int argc, char** argv) {
    int vali = validateargs(argc, argv);
    if (vali==0){
        if (argv[2]==NULL){
            //printf("ERROR!!! INVAILD ARGUMENT. PROGRAM TERMINATING.\n");
            return EXIT_SUCCESS;
        }
        printf("$ ./mapreduce %s %s %s\n", argv[1], argv[2], argv[3]);
        if (strcmp(argv[2], "stats")==0){
            printf("running stats\n");
            int map_stats = map(argv[3],stats_space,sizeof(Stats),stats);
            Stats ms1 = stats_reduce(map_stats, stats_space);
            stats_print(ms1, 1);
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[2], "ana")==0){
            printf("running ana\n");
            int map_ana = map(argv[3], analysis_space, sizeof(struct Analysis), analysis);
            struct Analysis ma1 = analysis_reduce(map_ana, analysis_space);
            analysis_print(ma1, total_byte_directory, 1);
            return EXIT_SUCCESS;
        }
        else{
            printf("ERROR!!! INVAILD ARGUMENT. PROGRAM TERMINATING.\n");
            return EXIT_FAILURE;
        }
    }
    else if (vali == -1){
    	printf("ERROR!!! Invaild argument. Program terminating.\n");
        return EXIT_FAILURE;
    }
    
    printf("$ ./mapreduce %s %s %s\n", argv[1], argv[2], argv[3]);
    if (vali == 3){
        int map_stats = map(argv[3],stats_space,sizeof(Stats),stats);
        Stats ms1 = stats_reduce(map_stats, stats_space);
        stats_print(ms1, 1);
    }
    else if (vali == 4){
        int map_ana = map(argv[3], analysis_space, sizeof(struct Analysis), analysis);
        struct Analysis ma1 = analysis_reduce(map_ana, analysis_space);
        analysis_print(ma1, total_byte_directory, 1);
    }
    
}

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>

#define MAX_INPUT_SIZE 1024
#define MAX_LINE_SIZE 256

void *executeCommand(void *command) {
    char *cmd = (char *)command;
    
    sleep(10);
    int result = system(cmd);
    
    if (result == 0) {
        printf("Command '%s' completed successfully.\n", cmd);
    } else {
        printf("Command '%s' failed with error code %d.\n", cmd, result);
    }

    free(cmd); // Free dynamically allocated memory for the command
    return NULL;
}

void internal(char* filename,int *vsize, int *hsize){

    char remain[100];
    strcpy(remain,filename+5);


    FILE *file = fopen(remain, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char line[MAX_LINE_SIZE];
    int lines_displayed = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lines_displayed++;
        
        // Trim the line to hsize characters if necessary
        int len = strlen(line);
        if (len > *hsize) {
            line[*hsize] = '\0';
            len = *hsize;
        }

        printf("%s \n", line);
        
        if (lines_displayed >= *vsize) {
            printf("Press Enter to see more, else enter any key to exit.\n");
            char user_input[MAX_LINE_SIZE];
            if (fgets(user_input, sizeof(user_input), stdin) == NULL || strcmp(user_input, "\n") != 0) {
                // Terminate if the user input is not an empty line
                break;
            }
            lines_displayed = 0;
        }
    }

    fclose(file);

    
}

void shconfigFile(int *vsize, int *hsize){
    // Creating shconfig.txt file
    FILE *file;
    *vsize = 40;
    *hsize = 75;
    bool vFound = false;
    bool hFound = false;
    

    const char *filename = "shconfig.txt";


    file = fopen(filename,"r");

    if (access(filename, F_OK) != -1) {
        
        // Read the file if it exists
        if (file != NULL){
        char key[32], value[32];
        
        while (fscanf(file, "%s %s", key, value) != EOF) {
            if(strcmp(key, "VSIZE") == 0) {
                *vsize = atoi(value); // Converting string argument str to int
                
                vFound = true;
            } else if (strcmp(key, "HSIZE") == 0) {
                *hsize = atoi(value);
                
                hFound = true;
            }
        }
        printf("Successfully Read shconfig.txt File!\n");
        fclose(file);
        }

        // If VSIZE and HSIZE doesn't exists :- append
        if (!vFound){
            
            file = fopen(filename,"a");
            fprintf(file,"VSIZE 40 \n");
            fclose(file);
        }
         if (!hFound){
            
            file = fopen(filename,"a");
            fprintf(file,"HSIZE 75 \n");
            fclose(file);
    }
    


    } else {
        // If file doesn't exists 
        if (!vFound){
            
            file = fopen(filename,"a");
            fprintf(file,"VSIZE 40 \n");
            fclose(file);
        }
         if (!hFound){
            
            file = fopen(filename,"a");
            fprintf(file,"HSIZE 75 \n");
            fclose(file);
    }
    printf("Successfully Created shconfig.txt File!\n");


}
}

int main(){

   int vsize, hsize;
   shconfigFile(&vsize,&hsize);

    

    char input[MAX_INPUT_SIZE];


    pthread_t bgThread; // Thread for background command
    
    while (1) {
        printf("shell>> ");
        fgets(input, sizeof(input), stdin);

        if (strlen(input) > 0) {
            // Remove the newline character
            input[strlen(input) - 1] = '\0';

            // Check if the command should run in the background
            int runInBackground = 0;


            if (input[0] == '&' && input[1] == ' ') {
                printf("Background command detected. going to sleep for 10sec... \n");
                runInBackground = 1;
                // memmove copies n(strlen(input)-1) characters from str2(input+2) to str1(input)
                memmove(input, input + 2, strlen(input) - 1);  
            }

            // Exit if the command is exit
            if (strcmp(input,"exit") == 0){
                break;
            } else if (strncmp(input,"more",4) == 0){ // Check if the command is more
                
                internal(input, &vsize, &hsize);

            } else if (runInBackground) {
                // Create a thread to execute the background command
                char *cmd = strdup(input); // Duplicate the command to avoid data race
                
                if (pthread_create(&bgThread, NULL, executeCommand, cmd) != 0) {
                    perror("Thread creation failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Execute the foreground command in the current process
                int result = system(input);
                
                if (result == 0) {
                    printf("Command '%s' completed successfully.\n", input);
                } else {
                    printf("Command '%s' failed with error code %d.\n", input, result);
                }
            }
        }
    }

    return 0;


}
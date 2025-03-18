#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Added for isdigit() function
#include <limits.h> // Added for INT_MAX constant

#define MAX_PROCESSES 100

typedef struct { // Structure to represent a process
    int id;              // Process ID
    int arrival_time;    // Arrival time of the process
    int burst_time;      // Original burst time of the process
    int burst_left;      // Remaining burst time of the process
    int wait_time;       // Wait time of the process
    int turnaround_time; // Turnaround time of the process
    int completed;       // Flag to indicate if the process is completed
} Process;

// Function prototypes
void simulateFCFS(Process processes[], int num_processes); // Simulate First Come First Served scheduling
void simulateSJF(Process processes[], int num_processes); // Simulate Shortest Job First scheduling
void simulateRR(Process processes[], int num_processes, int quantum); // Simulate Round Robin scheduling
int readProcessesFromFile(const char* filename, Process processes[]); // Read processes from a file
void displayResults(Process processes[], int num_processes); // Display the results of the simulation
void printTimeStep(int time, Process *current_process); // Print the details of a time step

int main(int argc, char *argv[]) { // Main function
    Process processes[MAX_PROCESSES]; // Array to store the processes
    int num_processes; // Number of processes
    char *filename; // Filename to read processes from
    int quantum = 0; // Time quantum for Round Robin scheduling
    
    if (argc < 3) { // Check if the correct number of command line arguments is provided
        printf("Usage: %s [-f | -s | -r <quantum>] <input_file>\n", argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    if (strcmp(argv[1], "-f") == 0) { // First Come First Served
        if (argc != 3) {
            printf("Usage: %s -f <input_file>\n", argv[0]);
            return 1;
        }
        filename = argv[2];
    } else if (strcmp(argv[1], "-s") == 0) { // Shortest Job First
        if (argc != 3) {
            printf("Usage: %s -s <input_file>\n", argv[0]);
            return 1;
        }
        filename = argv[2];
    } else if (strcmp(argv[1], "-r") == 0) { // Round Robin
        if (argc != 4) {
            printf("Usage: %s -r <quantum> <input_file>\n", argv[0]);
            return 1;
        }
        quantum = atoi(argv[2]); // Convert quantum from string to integer
        if (quantum <= 0) { // Check if the quantum is a positive integer
            printf("Error: Time quantum must be a positive integer\n");
            return 1;
        }
        filename = argv[3];
    } else {
        printf("Unknown option: %s\n", argv[1]);
        printf("Usage: %s [-f | -s | -r <quantum>] <input_file>\n", argv[0]);
        return 1;
    }

    num_processes = readProcessesFromFile(filename, processes); // Read processes from file
    if (num_processes <= 0) {
        printf("Error reading processes from file\n");
        return 1;
    }
    
    // Run the appropriate scheduling algorithm based on the command line argument
    if (strcmp(argv[1], "-f") == 0) {
        printf("First Come First Served\n");
        simulateFCFS(processes, num_processes);
    } else if (strcmp(argv[1], "-s") == 0) {
        printf("Shortest Job First\n");
        simulateSJF(processes, num_processes);
    } else if (strcmp(argv[1], "-r") == 0) {
        printf("Round Robin with Quantum %d\n", quantum);
        simulateRR(processes, num_processes, quantum);
    }
    
    displayResults(processes, num_processes); // Display the results
    
    return 0;
}

int readProcessesFromFile(const char* filename, Process processes[]) { // Read processes from input file
    FILE *file = fopen(filename, "r"); // Open the file in read mode
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }
    
    char line[256]; // Buffer to store a line from the file
    int num_processes = 0; // Number of processes read
    
    if (fgets(line, sizeof(line), file)) { // Skip header line if it exists
        if (line[0] != 'P' && !isdigit(line[0])) { // Check if this is a header or data line
            // Skip header
        } else {
            rewind(file); // Go back to the beginning of the file
        }
    }
    
    while (fgets(line, sizeof(line), file) && num_processes < MAX_PROCESSES) { // Read process data from the file
        int id, burst;
        if (sscanf(line, "P%d,%d", &id, &burst) == 2) { // Referenced https://www.geeksforgeeks.org/scanf-and-fscanf-in-c/
            processes[num_processes].id = id;
            processes[num_processes].arrival_time = id; // arrival time = process id
            processes[num_processes].burst_time = burst;
            processes[num_processes].burst_left = burst;
            processes[num_processes].wait_time = 0;
            processes[num_processes].turnaround_time = 0;
            processes[num_processes].completed = 0;
            num_processes++;
        }
    }
    
    fclose(file);
    return num_processes;
}

void printTimeStep(int time, Process *current_process) { // Function to print the details of a time step
    if (current_process != NULL) {
        printf("T%d : P%d  - Burst left %2d, Wait time %3d, Turnaround time %3d\n", 
               time, current_process->id, current_process->burst_left, 
               current_process->wait_time, current_process->turnaround_time);
    } else {
        printf("T%d : Idle\n", time);
    }
}

void simulateFCFS(Process processes[], int num_processes) { // Referenced https://www.geeksforgeeks.org/program-for-fcfs-cpu-scheduling-set-1/ and https://github.com/AugustineAykara/CPU-Scheduling-Algorithm-In-C/blob/master/FCFS.c
    int time = 0; // Current time
    int completed_processes = 0; // Number of completed processes
    Process *current_process = NULL; // Current process being executed
    
    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; i++) { // Check for newly arrived processes
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                if (processes[i].arrival_time <= time) { // Update wait and turnaround times for all arrived processes
                    if (&processes[i] != current_process && processes[i].burst_left > 0) {
                        processes[i].wait_time++;
                    }
                    if (processes[i].burst_left > 0) {
                        processes[i].turnaround_time++;
                    }
                }
            }
        }
        
        if (current_process == NULL || current_process->burst_left == 0) { // Select a process to run if none is currently running
            if (current_process != NULL && current_process->burst_left == 0) {
                current_process->completed = 1;
                completed_processes++;
                current_process = NULL;
            }
            
            for (int i = 0; i < num_processes; i++) { // Find the first process that has arrived and has burst left
                if (processes[i].arrival_time <= time && 
                    processes[i].burst_left > 0 && 
                    !processes[i].completed) {
                    current_process = &processes[i];
                    break;
                }
            }
        }
        
        if (current_process != NULL && current_process->burst_left > 0) { // Execute the current process for one time unit
            printTimeStep(time, current_process);
            current_process->burst_left--;
        } else {
            printTimeStep(time, NULL);
        }
        
        time++;
    }
}

void simulateSJF(Process processes[], int num_processes) { // Referenced https://www.edureka.co/blog/sjf-scheduling-in-c/ and https://www.sanfoundry.com/c-program-sjf-scheduling/#google_vignette
    int time = 0;
    int completed_processes = 0;
    Process *current_process = NULL;
    
    while (completed_processes < num_processes) {
        Process *shortest_job = NULL;
        int shortest_burst = INT_MAX;
        
        for (int i = 0; i < num_processes; i++) { // Update wait and turnaround times for all arrived processes
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                if (&processes[i] != current_process && processes[i].burst_left > 0) {
                    processes[i].wait_time++;
                }
                if (processes[i].burst_left > 0) {
                    processes[i].turnaround_time++;
                }
                
                if (processes[i].burst_left < shortest_burst) { // Find the process with shortest remaining burst time
                    shortest_burst = processes[i].burst_left;
                    shortest_job = &processes[i];
                } else if (processes[i].burst_left == shortest_burst && 
                           processes[i].arrival_time < shortest_job->arrival_time) {
                    shortest_job = &processes[i]; // If there's a tie, select the one that arrived earlier
                }
            }
        }
        
        if (current_process != NULL && current_process->burst_left > 0 && current_process->burst_left <= shortest_burst) { // If current process still has shortest burst, keep it running (ties go to current process)
            shortest_job = current_process;
        }
        
        if (current_process != NULL && current_process->burst_left == 0) { // Check if the current process has completed
            current_process->completed = 1;
            completed_processes++;
            current_process = NULL;
        }
        
        current_process = shortest_job; // Execute the shortest job for one time unit
        if (current_process != NULL) {
            printTimeStep(time, current_process);
            current_process->burst_left--;
            if (current_process->burst_left == 0) {
                current_process->completed = 1;
                completed_processes++;
                current_process = NULL;
            }
        } else {
            printTimeStep(time, NULL);
        }
        
        time++;
    }
}

void simulateRR(Process processes[], int num_processes, int quantum) { // Referenced https://www.geeksforgeeks.org/program-for-round-robin-scheduling-for-the-same-arrival-time/ and https://www.ccbp.in/blog/articles/round-robin-program-in-c
    int time = 0;
    int completed_processes = 0;
    Process *current_process = NULL;
    int time_in_quantum = 0;
    
    while (completed_processes < num_processes) { // Loop until all processes are completed
        for (int i = 0; i < num_processes; i++) { // Update wait and turnaround times for all arrived processes
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                if (&processes[i] != current_process && processes[i].burst_left > 0) {
                    processes[i].wait_time++; // Increment wait time for all arrived processes
                }
                if (processes[i].burst_left > 0) {
                    processes[i].turnaround_time++;
                }
            }
        }
        
        if (current_process == NULL || current_process->burst_left == 0 || time_in_quantum >= quantum) { // Check if we need to switch processes (quantum expired or process completed)
            if (current_process != NULL && current_process->burst_left == 0) { // Mark as completed if burst is done
                current_process->completed = 1;
                completed_processes++;
            }
            
            time_in_quantum = 0; // Reset quantum counter
            
            int start = (current_process == NULL) ? 0 : (current_process - processes + 1) % num_processes; // Find next process in round robin fashion
            current_process = NULL;
            
            for (int i = 0; i < num_processes; i++) { // Look for the next available process
                int idx = (start + i) % num_processes;
                if (processes[idx].arrival_time <= time && 
                    processes[idx].burst_left > 0 && 
                    !processes[idx].completed) {
                    current_process = &processes[idx];
                    break;
                }
            }
        }
        
        if (current_process != NULL && current_process->burst_left > 0) { // Execute the current process for one time unit
            printTimeStep(time, current_process);
            current_process->burst_left--;
            time_in_quantum++;
            
            if (current_process->burst_left == 0) { // Check if process just completed
                current_process->completed = 1;
                completed_processes++;
                time_in_quantum = 0;
                current_process = NULL;
            }
        } else {
            printTimeStep(time, NULL);
        }
        
        time++;
    }
}

void displayResults(Process processes[], int num_processes) { // Display the final results
    float total_wait_time = 0;
    float total_turnaround_time = 0;
    
    printf("\n");
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\n", processes[i].id);
        printf("\tWaiting time:         %3d\n", processes[i].wait_time);
        printf("\tTurnaround time:      %3d\n\n", processes[i].turnaround_time);
        
        total_wait_time += processes[i].wait_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    
    printf("Total average waiting time:     %.1f\n", total_wait_time / num_processes);
    printf("Total average turnaround time:  %.1f\n", total_turnaround_time / num_processes);
}

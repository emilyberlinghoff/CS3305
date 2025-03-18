#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Added for isdigit() function
#include <limits.h> // Added for INT_MAX constant

#define MAX_PROCESSES 100

typedef struct {
    int id;             // Process ID
    int arrival_time;   // Arrival time
    int burst_time;     // Original burst time
    int burst_left;     // Remaining burst time
    int wait_time;      // Wait time
    int turnaround_time; // Turnaround time
    int completed;      // Flag to mark completion
} Process;

// Function prototypes
void simulateFCFS(Process processes[], int num_processes);
void simulateSJF(Process processes[], int num_processes);
void simulateRR(Process processes[], int num_processes, int quantum);
int readProcessesFromFile(const char* filename, Process processes[]);
void displayResults(Process processes[], int num_processes);
void printTimeStep(int time, Process *current_process);

int main(int argc, char *argv[]) {
    Process processes[MAX_PROCESSES];
    int num_processes;
    char *filename;
    int quantum = 0;
    
    // Check command line arguments
    if (argc < 3) {
        printf("Usage: %s [-f | -s | -r <quantum>] <input_file>\n", argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: %s -f <input_file>\n", argv[0]);
            return 1;
        }
        filename = argv[2];
    } else if (strcmp(argv[1], "-s") == 0) {
        if (argc != 3) {
            printf("Usage: %s -s <input_file>\n", argv[0]);
            return 1;
        }
        filename = argv[2];
    } else if (strcmp(argv[1], "-r") == 0) {
        if (argc != 4) {
            printf("Usage: %s -r <quantum> <input_file>\n", argv[0]);
            return 1;
        }
        quantum = atoi(argv[2]);
        if (quantum <= 0) {
            printf("Error: Time quantum must be a positive integer\n");
            return 1;
        }
        filename = argv[3];
    } else {
        printf("Unknown option: %s\n", argv[1]);
        printf("Usage: %s [-f | -s | -r <quantum>] <input_file>\n", argv[0]);
        return 1;
    }
    
    // Read processes from file
    num_processes = readProcessesFromFile(filename, processes);
    if (num_processes <= 0) {
        printf("Error reading processes from file\n");
        return 1;
    }
    
    // Run the appropriate simulation
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
    
    // Display the results
    displayResults(processes, num_processes);
    
    return 0;
}

// Read processes from input file
int readProcessesFromFile(const char* filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }
    
    char line[256];
    int num_processes = 0;
    
    // Skip header line if it exists
    if (fgets(line, sizeof(line), file)) {
        // Check if this is a header or data
        if (line[0] != 'P' && !isdigit(line[0])) {
            // Skip header - do nothing here
        } else {
            // Go back to the beginning of the file
            rewind(file);
        }
    }
    
    // Read process data
    while (fgets(line, sizeof(line), file) && num_processes < MAX_PROCESSES) {
        int id, burst;
        if (sscanf(line, "P%d,%d", &id, &burst) == 2) {
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

// Print the current time step details
void printTimeStep(int time, Process *current_process) {
    if (current_process != NULL) {
        printf("T%d : P%d  - Burst left %2d, Wait time %3d, Turnaround time %3d\n", 
               time, current_process->id, current_process->burst_left, 
               current_process->wait_time, current_process->turnaround_time);
    } else {
        printf("T%d : Idle\n", time);
    }
}

// First Come First Served scheduling algorithm
void simulateFCFS(Process processes[], int num_processes) {
    int time = 0;
    int completed_processes = 0;
    Process *current_process = NULL;
    
    while (completed_processes < num_processes) {
        // Check for newly arrived processes
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                // Update wait and turnaround times for all arrived processes
                if (processes[i].arrival_time <= time) {
                    if (&processes[i] != current_process && processes[i].burst_left > 0) {
                        processes[i].wait_time++;
                    }
                    if (processes[i].burst_left > 0) {
                        processes[i].turnaround_time++;
                    }
                }
            }
        }
        
        // Select a process to run if none is currently running
        if (current_process == NULL || current_process->burst_left == 0) {
            if (current_process != NULL && current_process->burst_left == 0) {
                current_process->completed = 1;
                completed_processes++;
                current_process = NULL;
            }
            
            // Find the first process that has arrived and has burst left
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= time && 
                    processes[i].burst_left > 0 && 
                    !processes[i].completed) {
                    current_process = &processes[i];
                    break;
                }
            }
        }
        
        // Execute the current process for one time unit
        if (current_process != NULL && current_process->burst_left > 0) {
            printTimeStep(time, current_process);
            current_process->burst_left--;
        } else {
            printTimeStep(time, NULL);
        }
        
        time++;
    }
}

// Shortest Job First scheduling algorithm (preemptive)
void simulateSJF(Process processes[], int num_processes) {
    int time = 0;
    int completed_processes = 0;
    Process *current_process = NULL;
    
    while (completed_processes < num_processes) {
        Process *shortest_job = NULL;
        int shortest_burst = INT_MAX;
        
        // Update wait and turnaround times for all arrived processes
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                if (&processes[i] != current_process && processes[i].burst_left > 0) {
                    processes[i].wait_time++;
                }
                if (processes[i].burst_left > 0) {
                    processes[i].turnaround_time++;
                }
                
                // Find the process with shortest remaining burst time
                if (processes[i].burst_left < shortest_burst) {
                    shortest_burst = processes[i].burst_left;
                    shortest_job = &processes[i];
                } else if (processes[i].burst_left == shortest_burst && 
                           processes[i].arrival_time < shortest_job->arrival_time) {
                    // If there's a tie, select the one that arrived earlier
                    shortest_job = &processes[i];
                }
            }
        }
        
        // If current process still has shortest burst, keep it running
        // (ties go to current process)
        if (current_process != NULL && current_process->burst_left > 0 && 
            current_process->burst_left <= shortest_burst) {
            shortest_job = current_process;
        }
        
        // Check if the current process has completed
        if (current_process != NULL && current_process->burst_left == 0) {
            current_process->completed = 1;
            completed_processes++;
            current_process = NULL;
        }
        
        // Execute the shortest job for one time unit
        current_process = shortest_job;
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

// Round Robin scheduling algorithm
void simulateRR(Process processes[], int num_processes, int quantum) {
    int time = 0;
    int completed_processes = 0;
    Process *current_process = NULL;
    int time_in_quantum = 0;
    
    while (completed_processes < num_processes) {
        // Update wait and turnaround times for all arrived processes
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= time && processes[i].burst_left > 0) {
                if (&processes[i] != current_process && processes[i].burst_left > 0) {
                    processes[i].wait_time++;
                }
                if (processes[i].burst_left > 0) {
                    processes[i].turnaround_time++;
                }
            }
        }
        
        // Check if we need to switch processes (quantum expired or process completed)
        if (current_process == NULL || 
            current_process->burst_left == 0 || 
            time_in_quantum >= quantum) {
            
            // Mark as completed if burst is done
            if (current_process != NULL && current_process->burst_left == 0) {
                current_process->completed = 1;
                completed_processes++;
            }
            
            // Reset quantum counter
            time_in_quantum = 0;
            
            // Find next process in round robin fashion
            int start = (current_process == NULL) ? 0 : (current_process - processes + 1) % num_processes;
            current_process = NULL;
            
            // Look for the next available process
            for (int i = 0; i < num_processes; i++) {
                int idx = (start + i) % num_processes;
                if (processes[idx].arrival_time <= time && 
                    processes[idx].burst_left > 0 && 
                    !processes[idx].completed) {
                    current_process = &processes[idx];
                    break;
                }
            }
        }
        
        // Execute the current process for one time unit
        if (current_process != NULL && current_process->burst_left > 0) {
            printTimeStep(time, current_process);
            current_process->burst_left--;
            time_in_quantum++;
            
            // Check if process just completed
            if (current_process->burst_left == 0) {
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

// Display the final results
void displayResults(Process processes[], int num_processes) {
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

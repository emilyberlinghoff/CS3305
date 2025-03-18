# CPU Scheduling Simulator

This project simulates three fundamental CPU scheduling algorithms:

- First Come First Served (FCFS)
- Shortest Job First (SJF)
- Round Robin (RR)

Each algorithm calculates and displays individual and average waiting and turnaround times for processes defined in an input file.

## Compilation

Use the following command to compile the program:

```bash
gcc -o assignment-4 assignment-4.c
```

## Usage

Run the executable with the desired scheduling algorithm:

- FCFS:
  ```bash
  ./assignment-4 -f assignment-4-input.csv
  ```

- SJF (Preemptive):
  ```bash
  ./assignment-4 -s assignment-4-input.csv
  ```

- Round Robin (with quantum):
  ```bash
  ./assignment-4 -r <quantum> assignment-4-input.csv
  ```
  Replace `<quantum>` with a positive integer specifying the time quantum.

## Input File Format

The input file (`assignment-4-input.csv`) should contain lines formatted as:

```
P0,3
P1,8
P2,1
...
```
- Each line specifies the **Process ID** and its corresponding **Burst Time**.
- The arrival time is assumed to match the process ID for simplicity (e.g., P0 arrives at time 0, P1 at time 1, etc.).

## Output

The simulator outputs a step-by-step execution log including:
- Current time step
- Process ID of the running process
- Remaining burst time
- Current waiting time
- Current turnaround time

After simulation completion, it summarizes:
- Individual waiting and turnaround times for each process
- Total average waiting and turnaround times for all processes

## Issues

### Round Robin Wait-Time Calculation Correction
Initially, there was an issue with idle times incorrectly incrementing the waiting time for the currently running process during Round Robin scheduling. This was resolved by updating the wait-time calculation logic as follows:

```c
// Correct wait time logic
if (&processes[i] != current_process && processes[i].arrival_time <= time && processes[i].burst_left > 0) {
    processes[i].wait_time++;
}
```

This ensures only processes in the ready queue increment their wait time, and the currently running process increments only its turnaround time.


## References
- [scanf() and fscanf() in C](https://www.geeksforgeeks.org/scanf-and-fscanf-in-c/)
- [Program for FCFS CPU Scheduling | Set 1](https://github.com/AugustineAykara/CPU-Scheduling-Algorithm-In-C/blob/master/FCFS.c)
- [Shortest Job First Scheduling in C Programming](https://www.edureka.co/blog/sjf-scheduling-in-c/)
- [SJF Scheduling Program in C](https://www.sanfoundry.com/c-program-sjf-scheduling/#google_vignette)
- [Program for Round Robin Scheduling for the Same Arrival Time](https://www.geeksforgeeks.org/program-for-round-robin-scheduling-for-the-same-arrival-time/)
- [Round Robin Program in C](https://www.ccbp.in/blog/articles/round-robin-program-in-c)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_PROCESSES 10
#define QUANTUM 2

typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int start_time;
} Process;

// Function prototypes
void fcfs(Process processes[], int n);
void sjf_non_preemptive(Process processes[], int n);
void priority_non_preemptive(Process processes[], int n);
void round_robin(Process processes[], int n, int quantum);
void display_results(Process processes[], int n, char algorithm[]);
void calculate_average(Process processes[], int n);
void reset_processes(Process original[], Process temp[], int n);
void print_gantt_chart(int timeline[], int process_id[], int time_slots);

// FCFS Scheduling
void fcfs(Process processes[], int n) {
    int current_time = 0;
    
    // Sort by arrival time
    Process temp[MAX_PROCESSES];
    for(int i = 0; i < n; i++) temp[i] = processes[i];
    
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            if(temp[i].arrival_time > temp[j].arrival_time) {
                Process t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }
    
    for(int i = 0; i < n; i++) {
        if(current_time < temp[i].arrival_time) {
            current_time = temp[i].arrival_time;
        }
        temp[i].start_time = current_time;
        temp[i].completion_time = current_time + temp[i].burst_time;
        temp[i].turnaround_time = temp[i].completion_time - temp[i].arrival_time;
        temp[i].waiting_time = temp[i].turnaround_time - temp[i].burst_time;
        temp[i].response_time = temp[i].start_time - temp[i].arrival_time;
        current_time += temp[i].burst_time;
    }
    
    // Copy back to original array
    for(int i = 0; i < n; i++) processes[i] = temp[i];
}

// Non-preemptive SJF
void sjf_non_preemptive(Process processes[], int n) {
    Process temp[MAX_PROCESSES];
    for(int i = 0; i < n; i++) temp[i] = processes[i];
    
    int completed = 0, current_time = 0;
    int is_completed[MAX_PROCESSES] = {0};
    
    while(completed < n) {
        int idx = -1;
        int min_burst = INT_MAX;
        
        // Find the process with smallest burst time among arrived processes
        for(int i = 0; i < n; i++) {
            if(temp[i].arrival_time <= current_time && !is_completed[i]) {
                if(temp[i].burst_time < min_burst) {
                    min_burst = temp[i].burst_time;
                    idx = i;
                }
            }
        }
        
        if(idx != -1) {
            temp[idx].start_time = current_time;
            temp[idx].completion_time = current_time + temp[idx].burst_time;
            temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
            temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
            temp[idx].response_time = temp[idx].start_time - temp[idx].arrival_time;
            current_time += temp[idx].burst_time;
            is_completed[idx] = 1;
            completed++;
        } else {
            current_time++;
        }
    }
    
    for(int i = 0; i < n; i++) processes[i] = temp[i];
}

// Non-preemptive Priority Scheduling
void priority_non_preemptive(Process processes[], int n) {
    Process temp[MAX_PROCESSES];
    for(int i = 0; i < n; i++) temp[i] = processes[i];
    
    int completed = 0, current_time = 0;
    int is_completed[MAX_PROCESSES] = {0};
    
    while(completed < n) {
        int idx = -1;
        int highest_priority = INT_MAX;
        
        // Find process with highest priority (lower number = higher priority)
        for(int i = 0; i < n; i++) {
            if(temp[i].arrival_time <= current_time && !is_completed[i]) {
                if(temp[i].priority < highest_priority) {
                    highest_priority = temp[i].priority;
                    idx = i;
                }
            }
        }
        
        if(idx != -1) {
            temp[idx].start_time = current_time;
            temp[idx].completion_time = current_time + temp[idx].burst_time;
            temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
            temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
            temp[idx].response_time = temp[idx].start_time - temp[idx].arrival_time;
            current_time += temp[idx].burst_time;
            is_completed[idx] = 1;
            completed++;
        } else {
            current_time++;
        }
    }
    
    for(int i = 0; i < n; i++) processes[i] = temp[i];
}

// Round Robin Scheduling
void round_robin(Process processes[], int n, int quantum) {
    Process temp[MAX_PROCESSES];
    for(int i = 0; i < n; i++) {
        temp[i] = processes[i];
        temp[i].remaining_time = temp[i].burst_time;
    }
    
    int current_time = 0;
    int completed = 0;
    int queue[MAX_PROCESSES * 10];
    int front = 0, rear = 0;
    int is_in_queue[MAX_PROCESSES] = {0};
    int first_response[MAX_PROCESSES];
    
    for(int i = 0; i < n; i++) first_response[i] = -1;
    
    // Add first process
    queue[rear++] = 0;
    is_in_queue[0] = 1;
    
    while(completed < n) {
        if(front == rear) {
            // No process in queue, jump to next arrival
            int next_arrival = INT_MAX;
            for(int i = 0; i < n; i++) {
                if(temp[i].remaining_time > 0 && temp[i].arrival_time > current_time) {
                    if(temp[i].arrival_time < next_arrival) {
                        next_arrival = temp[i].arrival_time;
                    }
                }
            }
            current_time = next_arrival;
            // Add all processes arrived at this time
            for(int i = 0; i < n; i++) {
                if(temp[i].arrival_time <= current_time && temp[i].remaining_time > 0 && !is_in_queue[i]) {
                    queue[rear++] = i;
                    is_in_queue[i] = 1;
                }
            }
            continue;
        }
        
        int idx = queue[front++];
        is_in_queue[idx] = 0;
        
        if(first_response[idx] == -1) {
            first_response[idx] = current_time;
        }
        
        int exec_time = (temp[idx].remaining_time < quantum) ? temp[idx].remaining_time : quantum;
        
        current_time += exec_time;
        temp[idx].remaining_time -= exec_time;
        
        // Add newly arrived processes to queue
        for(int i = 0; i < n; i++) {
            if(i != idx && temp[i].remaining_time > 0 && temp[i].arrival_time <= current_time && !is_in_queue[i]) {
                queue[rear++] = i;
                is_in_queue[i] = 1;
            }
        }
        
        if(temp[idx].remaining_time > 0) {
            queue[rear++] = idx;
            is_in_queue[idx] = 1;
        } else {
            temp[idx].completion_time = current_time;
            temp[idx].turnaround_time = temp[idx].completion_time - temp[idx].arrival_time;
            temp[idx].waiting_time = temp[idx].turnaround_time - temp[idx].burst_time;
            temp[idx].response_time = first_response[idx] - temp[idx].arrival_time;
            completed++;
        }
    }
    
    for(int i = 0; i < n; i++) processes[i] = temp[i];
}

// Display results
void display_results(Process processes[], int n, char algorithm[]) {
    printf("\n%s SCHEDULING RESULTS:\n", algorithm);
    printf("================================================================================\n");
    printf("Process\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\tResponse\n");
    printf("ID\tTime\tTime\t\tTime\t\tTime\t\tTime\tTime\n");
    printf("================================================================================\n");
    
    for(int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t\t%d\t\t%d\t%d\n",
               processes[i].id,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);
    }
    printf("================================================================================\n");
    
    calculate_average(processes, n);
}

// Calculate average metrics
void calculate_average(Process processes[], int n) {
    float total_turnaround = 0, total_waiting = 0, total_response = 0;
    
    for(int i = 0; i < n; i++) {
        total_turnaround += processes[i].turnaround_time;
        total_waiting += processes[i].waiting_time;
        total_response += processes[i].response_time;
    }
    
    printf("\nAverage Turnaround Time: %.2f", total_turnaround / n);
    printf("\nAverage Waiting Time: %.2f", total_waiting / n);
    printf("\nAverage Response Time: %.2f\n", total_response / n);
}

// Reset processes to original values
void reset_processes(Process original[], Process temp[], int n) {
    for(int i = 0; i < n; i++) {
        temp[i] = original[i];
    }
}

// Main function
int main() {
    Process processes[MAX_PROCESSES];
    Process temp[MAX_PROCESSES];
    int n, quantum;
    int choice;
    
    printf("\n========== CPU SCHEDULING ALGORITHM SIMULATOR ==========\n");
    printf("\nEnter number of processes (max %d): ", MAX_PROCESSES);
    scanf("%d", &n);
    
    // Input process details
    for(int i = 0; i < n; i++) {
        printf("\nProcess %d:\n", i + 1);
        processes[i].id = i + 1;
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burst_time);
        printf("Priority (lower number = higher priority): ");
        scanf("%d", &processes[i].priority);
    }
    
    printf("\nEnter time quantum for Round Robin: ");
    scanf("%d", &quantum);
    
    do {
        printf("\n================================================\n");
        printf("CPU SCHEDULING ALGORITHMS MENU\n");
        printf("================================================\n");
        printf("1. First Come First Serve (FCFS)\n");
        printf("2. Shortest Job First (SJF) - Non-preemptive\n");
        printf("3. Priority Scheduling - Non-preemptive\n");
        printf("4. Round Robin Scheduling\n");
        printf("5. Compare All Algorithms\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                reset_processes(processes, temp, n);
                fcfs(temp, n);
                display_results(temp, n, "FCFS");
                break;
                
            case 2:
                reset_processes(processes, temp, n);
                sjf_non_preemptive(temp, n);
                display_results(temp, n, "SJF (Non-preemptive)");
                break;
                
            case 3:
                reset_processes(processes, temp, n);
                priority_non_preemptive(temp, n);
                display_results(temp, n, "Priority (Non-preemptive)");
                break;
                
            case 4:
                reset_processes(processes, temp, n);
                round_robin(temp, n, quantum);
                display_results(temp, n, "Round Robin");
                break;
                
            case 5: {
                Process fcfs_proc[MAX_PROCESSES], sjf_proc[MAX_PROCESSES];
                Process priority_proc[MAX_PROCESSES], rr_proc[MAX_PROCESSES];
                
                printf("\n========== COMPARISON OF ALL ALGORITHMS ==========\n");
                
                reset_processes(processes, fcfs_proc, n);
                fcfs(fcfs_proc, n);
                printf("\n1. FCFS:");
                calculate_average(fcfs_proc, n);
                
                reset_processes(processes, sjf_proc, n);
                sjf_non_preemptive(sjf_proc, n);
                printf("\n2. SJF:");
                calculate_average(sjf_proc, n);
                
                reset_processes(processes, priority_proc, n);
                priority_non_preemptive(priority_proc, n);
                printf("\n3. Priority:");
                calculate_average(priority_proc, n);
                
                reset_processes(processes, rr_proc, n);
                round_robin(rr_proc, n, quantum);
                printf("\n4. Round Robin (Quantum=%d):", quantum);
                calculate_average(rr_proc, n);
                
                printf("\n================================================\n");
                break;
            }
            
            case 0:
                printf("\nExiting program...\n");
                break;
                
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    } while(choice != 0);
    
    return 0;
}
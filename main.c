#include <stdio.h>
#include <stdlib.h>  // Include stdlib.h for malloc and free
#include <memory.h>
#include <string.h>

#define MAX_LINES 50
#define TIME_QUANTUM 4

typedef struct {
    char taskName[4]; // at most T50 ~ 3 characters
    int arrivalTime;
    int burstTime;
    int waitTime;
} Task;

void FCFS(Task tasks[]);
void RR(Task tasks[]);
void NSJF(Task tasks[]);
void PSJF(Task tasks[]);
Task* sortTasksByBurstTime(Task tasks[]);
Task* sortTasksByArrivalTime(Task tasks[]);
Task* sortTasksByTaskName(Task tasks[]);
void print(Task tasks[]);

int count = 0, totalBurstTime = 0;

int main() {
    FILE* file = fopen("TaskSpec.txt", "r");

    Task tasks[MAX_LINES];

    // read tasks from TaskSpec.txt
    while (fscanf(file, "%19[^,],%d,%d\n", tasks[count].taskName,
                  &tasks[count].arrivalTime, &tasks[count].burstTime) == 3) {
        count++;
    }

    for (int i = 0; i < count; i++) {
        totalBurstTime += tasks[i].burstTime; // calculate totalBurstTime
    }

    fclose(file);

    freopen("Output.txt", "w", stdout);

    Task tasksForFCFS[MAX_LINES]; // make copies of tasks array for editing in methods
    memcpy(tasksForFCFS, tasks, sizeof(Task) * count);
    FCFS(tasksForFCFS);

    Task tasksForRR[MAX_LINES];
    memcpy(tasksForRR, tasks, sizeof(Task) * count);
    RR(tasksForRR);

    Task tasksForNSJF[MAX_LINES];
    memcpy(tasksForNSJF, tasks, sizeof(Task) * count);
    NSJF(tasksForNSJF);

    Task tasksForPSJF[MAX_LINES];
    memcpy(tasksForPSJF, tasks, sizeof(Task) * count);
    PSJF(tasksForPSJF);

    fclose(stdout);

    return 0;
}

void FCFS(Task tasks[]) {
    printf("FCFS: \n");
    int burstTimeCount = 0;

    for (int i = 0; i < count; i++) {
        tasks[i].waitTime = burstTimeCount - tasks[i].arrivalTime;
        burstTimeCount += tasks[i].burstTime;
        printf("%-4s   %-4d   %-4d\n", tasks[i].taskName, burstTimeCount - tasks[i].burstTime,
               burstTimeCount);
    }

    print(tasks);
}

void RR(Task tasks[]) {
    printf("RR: \n");
    int burstTimeCount = 0;

    while (totalBurstTime > burstTimeCount) {
        tasks = sortTasksByArrivalTime(tasks);
        for (int i = 0; i < count; i++) {
            if (tasks[i].burstTime != 0) {
                if (tasks[i].arrivalTime <= burstTimeCount) {
                    tasks[i].waitTime = burstTimeCount - tasks[i].arrivalTime;
                    if (tasks[i].burstTime > TIME_QUANTUM) {
                        burstTimeCount += TIME_QUANTUM;
                        printf("%-4s   %-4d   %-4d\n", tasks[i].taskName,
                               burstTimeCount - TIME_QUANTUM, burstTimeCount);
                        tasks[i].burstTime -= TIME_QUANTUM;
                        tasks[i].arrivalTime += TIME_QUANTUM; // editing arrival time
                        break;
                    } else {
                        burstTimeCount += tasks[i].burstTime;
                        printf("%-4s   %-4d   %-4d\n", tasks[i].taskName,
                               burstTimeCount - tasks[i].burstTime, burstTimeCount);
                        tasks[i].burstTime = 0;
                    }
                }
            }
        }
    }

    tasks = sortTasksByTaskName(tasks);

    print(tasks);
}

void NSJF(Task tasks[]) {
    printf("NSJF: \n");
    int burstTimeCount = 0;

    tasks = sortTasksByBurstTime(tasks);

    while (totalBurstTime > burstTimeCount) {
        for (int i = 0; i < count; i++) {
            if (tasks[i].burstTime != 0) {
                if (tasks[i].arrivalTime <= burstTimeCount) {
                    tasks[i].waitTime = burstTimeCount - tasks[i].arrivalTime;
                    burstTimeCount += tasks[i].burstTime;
                    printf("%-4s   %-4d   %-4d\n", tasks[i].taskName, burstTimeCount - tasks[i].burstTime,
                           burstTimeCount);
                    tasks[i].burstTime = 0;
                    i = count + 1;
                }
            }
        }
    }

    tasks = sortTasksByArrivalTime(tasks);

    print(tasks);
}

void PSJF(Task tasks[]) {
    printf("PSJF: \n");
    int burstTimeCount = 0;

    tasks = sortTasksByBurstTime(tasks);
    int trackBurstTime[count];

    for (int i = 0; i < count; i++) {
        trackBurstTime[i] = 0; // used for calculating total wait time for each process
        tasks[i].waitTime = 0;
    }

    char* prevTaskName = NULL;
    int startTime = 0;

    while (burstTimeCount < totalBurstTime) {
        tasks = sortTasksByBurstTime(tasks);

        for (int i = 0; i < count; i++) {
            if (tasks[i].burstTime != 0) {
                if (tasks[i].arrivalTime <= burstTimeCount) {

                    if (prevTaskName == NULL || strcmp(prevTaskName, tasks[i].taskName) != 0) {
                        if (prevTaskName != NULL) {
                            printf("%-4s   %-4d   %-4d\n", prevTaskName, startTime,
                                   burstTimeCount);
                        }
                        tasks[i].waitTime += burstTimeCount - tasks[i].arrivalTime - trackBurstTime[i];
                        startTime = burstTimeCount;
                    }
                    burstTimeCount++;
                    trackBurstTime[i]++;
                    tasks[i].burstTime--;
                    prevTaskName = malloc(strlen(tasks[i].taskName) + 1);
                    strcpy(prevTaskName, tasks[i].taskName);

                    break;
                }
            }
        }
    }

    printf("%-4s   %-4d   %-4d\n", prevTaskName, startTime,
           burstTimeCount);

    free(prevTaskName);  // Free allocated memory for prevTaskName

    tasks = sortTasksByArrivalTime(tasks);
    print(tasks);
}

Task* sortTasksByBurstTime(Task tasks[]) {
    Task* sortedTasks = malloc(count * sizeof(Task));
    memcpy(sortedTasks, tasks, count * sizeof(Task));

    Task temp;
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (sortedTasks[i].burstTime > sortedTasks[j].burstTime) {
                temp = sortedTasks[i];
                sortedTasks[i] = sortedTasks[j];
                sortedTasks[j] = temp;
            }
        }
    }

    return sortedTasks;
}

Task* sortTasksByArrivalTime(Task tasks[]) {
    Task* sortedTasks = malloc(count * sizeof(Task));
    memcpy(sortedTasks, tasks, count * sizeof(Task));

    Task temp;
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (sortedTasks[i].arrivalTime > sortedTasks[j].arrivalTime) {
                temp = sortedTasks[i];
                sortedTasks[i] = sortedTasks[j];
                sortedTasks[j] = temp;
            }
        }
    }

    return sortedTasks;
}

Task* sortTasksByTaskName(Task tasks[]) {
    Task* sortedTasks = malloc(count * sizeof(Task));
    memcpy(sortedTasks, tasks, count * sizeof(Task));

    Task temp;
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(sortedTasks[i].taskName, sortedTasks[j].taskName) > 0) {
                temp = sortedTasks[i];
                sortedTasks[i] = sortedTasks[j];
                sortedTasks[j] = temp;
            }
        }
    }

    return sortedTasks;
}

void print(Task tasks[]) {
    double totalWaitTime = 0;

    for (int i = 0; i < count; i++) {
        printf("Waiting Time %s: %d\n", tasks[i].taskName,
               tasks[i].waitTime);
        totalWaitTime += tasks[i].waitTime;
    }

    double avgWaitTime = totalWaitTime / count;
    printf("Average Wait Time: %.2f\n\n", avgWaitTime);

    // Free allocated memory for sortedTasks arrays
    free(sortTasksByBurstTime(tasks));
    free(sortTasksByArrivalTime(tasks));
    free(sortTasksByTaskName(tasks));
}

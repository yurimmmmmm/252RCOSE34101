#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/resource.h>

struct sched_attr {
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

static int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    // kernel syscall return
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

void perform_calculation(int matrix_size) {
    long long **matrixA = malloc(matrix_size * sizeof(long long*));
    long long **matrixB = malloc(matrix_size * sizeof(long long*));
    long long **matrixC = malloc(matrix_size * sizeof(long long*));

    for (int i = 0; i < matrix_size; i++) {
        matrixA[i] = malloc(matrix_size * sizeof(long long));
        matrixB[i] = malloc(matrix_size * sizeof(long long));
        matrixC[i] = malloc(matrix_size * sizeof(long long));
    }

    for (int iter = 0; iter < 50; iter++) {
        for (int i = 0; i < matrix_size; i++){
            for (int j = 0; j < matrix_size; j++){
                for (int k = 0; k < matrix_size; k++){
                    matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
                }
            }
        }
    }

    for (int i = 0; i < matrix_size; i++) {
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixC[i]);
    }
    free(matrixA);
    free(matrixB);
    free(matrixC);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <num_processes> <matrix_size> <nice_value_for_child>\n", argv[0]);
        return 1;
    }

    int num_processes = atoi(argv[1]);
    int matrix_size = atoi(argv[2]);
    int child_nice_value = atoi(argv[3]);

    pid_t pids[num_processes];

    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork failed");
            return 1;
        }

        /*
         * ******** YOUR CODE HERE ********
         * WHAT TO IMPLEMENT: Set scheduling poilcy and nice value for chile processes, turnaround time of each child process
         * CAUTION: sched_attr structure should be initialized before using it in seched_setattr() function.
         *
         * WHAT TO INCLUDE IN REPORT ABOUT THIS FUNCTION:
         * 1) How to set scheduling policy and nice value for child processes
         * 2) How to measure turnaround time of each child process
         *
         * TIP
         * 1) Use sched_setattr() to set scheduling policy and nice value
         * 2) What parent process should do while child process is running?
         * ********************************
        */
        
        if (pids[i] == 0) {
            // CHILD PROCESS
            // sched_attr 초기화
            struct sched_attr attr;
            memset(&attr, 0, sizeof(attr));  // 모든 필드 0으로 초기화
            attr.size = sizeof(struct sched_attr); // struct 크기 저장
            
            attr.sched_policy = SCHED_NORMAL;  // EEVDF 적용 scheduling policy
            attr.sched_nice = child_nice_value;  // nice 값
            
            // 스케줄링 속성 적용
            if (sched_setattr(0, &attr, 0) == -1) { // 커널에 nice value, scheduling policy 전달 -> 성공 시 0, 실패 시 -1
                perror("sched_setattr failed");
                exit(1);
            }
            
            // 시작 시간
            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time); // 시작 시간 기록
            
            // 실제 연산 실행 (Matrix Multiplication)
            printf("[Child] PID: %d, Nice: %d matrix multiplication\n",
                   getpid(), child_nice_value);
            perform_calculation(matrix_size);
            
            // 종료 시간
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            
            // Turnaround time 계산
            double turnaround_time = (end_time.tv_sec - start_time.tv_sec) + 
                                    (end_time.tv_nsec - start_time.tv_nsec) / 1e9; // 나노 초 -> 초 단위 변환
            
            // 결과 출력
            printf("[Child] PID: %d, Parent PID: %d,  Nice: %d Turnaround time: %.6f seconds\n",
                   getpid(), getppid(), child_nice_value, turnaround_time);
            
            // child 프로세스 종료
            exit(0);
        }
    }

    // PARENT PROCESS
    pid_t finished_pid = wait(NULL);
    printf("[Parent] Child (PID: %d) has finished\n", finished_pid);
   

    return 0;
}

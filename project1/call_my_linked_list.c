#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define sys_os2025_add     335
#define sys_os2025_insert  336
#define sys_os2025_remove  337

int main() {
    int ret;
    int random_val;
    int added_values[10];
    int count = 0;
    
    srand(time(NULL));
    
    // Task 1: ADD
    printf("Calling 'add' system call (add to front)\n");
    
    for (int i = 0; i < 3; i++) {
        random_val = rand() % 100 + 1; // 1 ~ 100까지의 숫자 중 랜덤으로 선정
        ret = syscall(sys_os2025_add, random_val); //sys_os2025_add에 랜덤값 전달
        
        if (ret == 0) { // add syscall 호출에 성공 시
            added_values[count++] = random_val; //user application 리스트에 값 추가
            printf("Added: %d\n", random_val);
        }
    }
    
    // Task 2: INSERT
    printf("Calling 'insert' system call (insert after a node)\n");
    
    for (int i = 0; i < 3; i++) {
        int target_value = added_values[i];  // add한 값들만 사용
        int new_value = rand() % 100 + 1; // 1 ~ 100까지의 숫자 중 랜덤으로 선정
        
        ret = syscall(sys_os2025_insert, target_value, new_value); //sys_os2025_insert에 랜덤값 및 타겟값 전달
        
        if (ret == 0) { // insert syscall 호출에 성공 시
            printf("Inserted : %d after %d\n", new_value, target_value);
            added_values[count++] = new_value;  // user application 리스트에 값 추가
        }
    }
    
    // Task 3: REMOVE
    printf("Calling 'remove' system call (remove a node)\n");
    

    for (int i = 0; i < count; i++) { // 리스트 크기 만큼만 반복
        int remove_value = added_values[i]; //삭제할 타겟값 지정
            
        ret = syscall(sys_os2025_remove, remove_value); //sys_os2025_remove에 타겟값 전달
            
        if (ret == 0) { // delete syscall 호출에 성공 시
            printf("Removed : %d\n", remove_value);
            }
        }

        return 0;
}



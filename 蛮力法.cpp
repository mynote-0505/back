#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 定义物品结构体
typedef struct {
    int weight;
    double value;
    int index;  // 原始索引
} Item;

// 生成随机物品数据
void generate_items(Item *items, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = rand() % 100 + 1;           // 1-100之间的随机重量
        items[i].value = (double)(rand() % 901 + 100); // 100-1000之间的随机价值
        items[i].value /= 100.0;                      // 保留两位小数
        items[i].index = i;                           // 记录原始索引
    }
}

// 蛮力法解决0-1背包问题
void brute_force(Item *items, int n, int capacity, const char *filename) {
    printf("处理背包容量 %d...\n", capacity);
    clock_t start = clock();
    
    double max_value = 0;
    int *best_combination = (int *)calloc(n, sizeof(int));
    
    // 遍历所有可能的物品组合（2^n种）
    for (unsigned long long mask = 0; mask < (1ULL << n); mask++) {
        int total_weight = 0;
        double total_value = 0;
        int *current_combination = (int *)calloc(n, sizeof(int));
        
        // 计算当前组合的总重量和总价值
        for (int i = 0; i < n; i++) {
            if (mask & (1ULL << i)) {
                total_weight += items[i].weight;
                total_value += items[i].value;
                current_combination[i] = 1;
            }
        }
        
        // 如果当前组合重量不超过容量且价值更高，则更新最优解
        if (total_weight <= capacity && total_value > max_value) {
            max_value = total_value;
            free(best_combination);
            best_combination = current_combination;
        } else {
            free(current_combination);
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 输出结果到文件
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    
    fprintf(fp, "蛮力法解决0-1背包问题 (n=%d, 容量=%d)\n", n, capacity);
    fprintf(fp, "最大价值: %.2f\n", max_value);
    fprintf(fp, "运行时间: %.4f秒\n", time_taken);
    
    fprintf(fp, "选择的物品索引: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (best_combination[i]) {
            if (count < 10) {  // 只输出前10个，避免文件过大
                fprintf(fp, "%d ", i + 1);
            }
            count++;
        }
    }
    if (count > 10) {
        fprintf(fp, "...等%d个物品", count);
    }
    fprintf(fp, "\n\n详细物品列表:\n");
    fprintf(fp, "物品索引\t重量\t价值\t是否选择\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%s\n", 
                i + 1, items[i].weight, items[i].value, 
                best_combination[i] ? "是" : "否");
    }
    
    fclose(fp);
    printf("结果已保存至 %s\n", filename);
    
    free(best_combination);
}

int main() {
    int n = 20;  // 物品数量（注意：对于n=1000，蛮力法会运行极长时间，建议使用较小值）
    int capacities[3] = {10000, 100000, 1000000};  // 背包容量
    
    printf("0-1背包问题蛮力法解法 (n=%d)\n", n);
    
    // 分配内存并生成物品
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // 针对每个容量运行蛮力算法
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "brute_force_results_n%d_c%d.txt", n, capacities[i]);
        brute_force(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

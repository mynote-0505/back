#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 定义物品结构体
typedef struct {
    int weight;
    double value;
    double ratio;  // 价值/重量比
    int index;     // 原始索引
} Item;

// 比较函数：按价值/重量比降序排序
int compare(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    return (itemA->ratio < itemB->ratio) - (itemA->ratio > itemB->ratio);
}

// 生成随机物品数据
void generate_items(Item *items, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = rand() % 100 + 1;           // 1-100之间的随机重量
        items[i].value = (double)(rand() % 901 + 100); // 100-1000之间的随机价值
        items[i].value /= 100.0;                      // 保留两位小数
        items[i].index = i;                           // 记录原始索引
        items[i].ratio = items[i].value / items[i].weight; // 计算价值/重量比
    }
}

// 计算剩余物品的最大可能价值（用于剪枝）
double bound(Item *items, int level, int n, int capacity, int weight, double value) {
    double bound_value = value;
    int remaining_weight = capacity - weight;
    
    for (int i = level; i < n && remaining_weight > 0; i++) {
        if (items[i].weight <= remaining_weight) {
            bound_value += items[i].value;
            remaining_weight -= items[i].weight;
        } else {
            bound_value += (items[i].value / items[i].weight) * remaining_weight;
            remaining_weight = 0;
        }
    }
    
    return bound_value;
}

// 回溯法解决0-1背包问题的辅助函数
void backtrack(Item *items, int level, int n, int capacity, int weight, double value, 
               double *best_value, int *current_selection, int *best_selection) {
    // 如果已经处理完所有物品
    if (level == n) {
        if (value > *best_value) {
            *best_value = value;
            for (int i = 0; i < n; i++) {
                best_selection[i] = current_selection[i];
            }
        }
        return;
    }
    
    // 选择当前物品（如果重量不超过容量）
    if (weight + items[level].weight <= capacity) {
        current_selection[items[level].index] = 1;
        backtrack(items, level + 1, n, capacity, weight + items[level].weight, 
                 value + items[level].value, best_value, current_selection, best_selection);
        current_selection[items[level].index] = 0;
    }
    
    // 计算不选择当前物品的情况下，剩余物品的最大可能价值
    double b = bound(items, level + 1, n, capacity, weight, value);
    
    // 如果剩余物品的最大可能价值大于当前最优解，则继续搜索
    if (b > *best_value) {
        current_selection[items[level].index] = 0;
        backtrack(items, level + 1, n, capacity, weight, value, 
                 best_value, current_selection, best_selection);
    }
}

// 回溯法解决0-1背包问题
void backtracking(Item *items, int n, int capacity, const char *filename) {
    printf("处理背包容量 %d...\n", capacity);
    clock_t start = clock();
    
    // 按价值/重量比排序，提高剪枝效率
    qsort(items, n, sizeof(Item), compare);
    
    double best_value = 0;
    int *current_selection = (int *)calloc(n, sizeof(int));
    int *best_selection = (int *)calloc(n, sizeof(int));
    
    // 执行回溯搜索
    backtrack(items, 0, n, capacity, 0, 0, &best_value, current_selection, best_selection);
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 输出结果到文件
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    
    fprintf(fp, "回溯法解决0-1背包问题 (n=%d, 容量=%d)\n", n, capacity);
    fprintf(fp, "最大价值: %.2f\n", best_value);
    fprintf(fp, "运行时间: %.4f秒\n", time_taken);
    
    fprintf(fp, "选择的物品索引: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (best_selection[i]) {
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
    fprintf(fp, "物品索引\t重量\t价值\t价值/重量比\t是否选择\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%.4f\t\t%s\n", 
                i + 1, items[i].weight, items[i].value, items[i].ratio, 
                best_selection[i] ? "是" : "否");
    }
    
    fclose(fp);
    printf("结果已保存至 %s\n", filename);
    
    free(current_selection);
    free(best_selection);
}

int main() {
    int n = 50;  // 物品数量（注意：对于n=1000，回溯法仍会运行较长时间，建议使用较小值）
    int capacities[3] = {10000, 100000, 1000000};  // 背包容量
    
    printf("0-1背包问题回溯法解法 (n=%d)\n", n);
    
    // 分配内存并生成物品
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // 针对每个容量运行回溯算法
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "backtracking_results_n%d_c%d.txt", n, capacities[i]);
        backtracking(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

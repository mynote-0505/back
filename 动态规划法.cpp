#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

// 动态规划法解决0-1背包问题
void dynamic_programming(Item *items, int n, int capacity, const char *filename) {
    printf("处理背包容量 %d...\n", capacity);
    clock_t start = clock();
    
    // 创建一维数组dp[w]表示容量为w时的最大价值
    double *dp = (double *)malloc((capacity + 1) * sizeof(double));
    int *selected = (int *)malloc(n * sizeof(int));
    memset(dp, 0, (capacity + 1) * sizeof(double));
    memset(selected, 0, n * sizeof(int));
    
    // 记录每个状态是从哪个状态转移来的
    int **prev = (int **)malloc((capacity + 1) * sizeof(int *));
    for (int w = 0; w <= capacity; w++) {
        prev[w] = (int *)malloc(n * sizeof(int));
        memset(prev[w], -1, n * sizeof(int));
    }
    
    // 填充dp表
    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            if (dp[w] < dp[w - items[i].weight] + items[i].value) {
                dp[w] = dp[w - items[i].weight] + items[i].value;
                // 记录是由哪个物品转移过来的
                memcpy(prev[w], prev[w - items[i].weight], n * sizeof(int));
                prev[w][i] = 1;
            }
        }
    }
    
    double max_value = dp[capacity];
    
    // 回溯找出选择的物品
    for (int i = 0; i < n; i++) {
        selected[i] = prev[capacity][i];
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 输出结果到文件
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    
    fprintf(fp, "动态规划法解决0-1背包问题 (n=%d, 容量=%d)\n", n, capacity);
    fprintf(fp, "最大价值: %.2f\n", max_value);
    fprintf(fp, "运行时间: %.4f秒\n", time_taken);
    fprintf(fp, "选择的物品数量: %d\n", selected[0]);
    
    fprintf(fp, "选择的物品索引: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
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
                selected[i] ? "是" : "否");
    }
    
    fclose(fp);
    printf("结果已保存至 %s\n", filename);
    
    // 释放内存
    free(dp);
    free(selected);
    for (int w = 0; w <= capacity; w++) {
        free(prev[w]);
    }
    free(prev);
}

int main() {
    int n = 1000;  // 物品数量
    int capacities[3] = {10000, 100000, 1000000};  // 背包容量
    
    printf("0-1背包问题动态规划解法 (n=%d)\n", n);
    
    // 分配内存并生成物品
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // 针对每个容量运行动态规划算法
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "dp_results_capacity_%d.txt", capacities[i]);
        dynamic_programming(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

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

// 贪心法解决0-1背包问题
void greedy(Item *items, int n, int capacity, const char *filename) {
    printf("处理背包容量 %d...\n", capacity);
    clock_t start = clock();
    
    // 按价值/重量比排序
    qsort(items, n, sizeof(Item), compare);
    
    double total_value = 0;
    int total_weight = 0;
    int *selected = (int *)calloc(n, sizeof(int));
    
    // 按比率降序选择物品
    for (int i = 0; i < n; i++) {
        if (total_weight + items[i].weight <= capacity) {
            total_weight += items[i].weight;
            total_value += items[i].value;
            selected[items[i].index] = 1; // 使用原始索引标记选择的物品
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
    
    fprintf(fp, "贪心法解决0-1背包问题 (n=%d, 容量=%d)\n", n, capacity);
    fprintf(fp, "最大价值: %.2f\n", total_value);
    fprintf(fp, "运行时间: %.4f秒\n", time_taken);
    fprintf(fp, "选择的物品数量: %d\n", total_weight);
    
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
    fprintf(fp, "物品索引\t重量\t价值\t价值/重量比\t是否选择\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%.4f\t\t%s\n", 
                i + 1, items[i].weight, items[i].value, items[i].ratio, 
                selected[i] ? "是" : "否");
    }
    
    fclose(fp);
    printf("结果已保存至 %s\n", filename);
    
    free(selected);
}

int main() {
    int n = 1000;  // 物品数量
    int capacities[3] = {10000, 100000, 1000000};  // 背包容量
    
    printf("0-1背包问题贪心法解法 (n=%d)\n", n);
    
    // 分配内存并生成物品
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // 针对每个容量运行贪心算法
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "greedy_results_capacity_%d.txt", capacities[i]);
        greedy(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

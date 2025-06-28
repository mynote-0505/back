#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// ������Ʒ�ṹ��
typedef struct {
    int weight;
    double value;
    int index;  // ԭʼ����
} Item;

// ���������Ʒ����
void generate_items(Item *items, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = rand() % 100 + 1;           // 1-100֮����������
        items[i].value = (double)(rand() % 901 + 100); // 100-1000֮��������ֵ
        items[i].value /= 100.0;                      // ������λС��
        items[i].index = i;                           // ��¼ԭʼ����
    }
}

// ��̬�滮�����0-1��������
void dynamic_programming(Item *items, int n, int capacity, const char *filename) {
    printf("���������� %d...\n", capacity);
    clock_t start = clock();
    
    // ����һά����dp[w]��ʾ����Ϊwʱ������ֵ
    double *dp = (double *)malloc((capacity + 1) * sizeof(double));
    int *selected = (int *)malloc(n * sizeof(int));
    memset(dp, 0, (capacity + 1) * sizeof(double));
    memset(selected, 0, n * sizeof(int));
    
    // ��¼ÿ��״̬�Ǵ��ĸ�״̬ת������
    int **prev = (int **)malloc((capacity + 1) * sizeof(int *));
    for (int w = 0; w <= capacity; w++) {
        prev[w] = (int *)malloc(n * sizeof(int));
        memset(prev[w], -1, n * sizeof(int));
    }
    
    // ���dp��
    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            if (dp[w] < dp[w - items[i].weight] + items[i].value) {
                dp[w] = dp[w - items[i].weight] + items[i].value;
                // ��¼�����ĸ���Ʒת�ƹ�����
                memcpy(prev[w], prev[w - items[i].weight], n * sizeof(int));
                prev[w][i] = 1;
            }
        }
    }
    
    double max_value = dp[capacity];
    
    // �����ҳ�ѡ�����Ʒ
    for (int i = 0; i < n; i++) {
        selected[i] = prev[capacity][i];
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // ���������ļ�
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }
    
    fprintf(fp, "��̬�滮�����0-1�������� (n=%d, ����=%d)\n", n, capacity);
    fprintf(fp, "����ֵ: %.2f\n", max_value);
    fprintf(fp, "����ʱ��: %.4f��\n", time_taken);
    fprintf(fp, "ѡ�����Ʒ����: %d\n", selected[0]);
    
    fprintf(fp, "ѡ�����Ʒ����: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            if (count < 10) {  // ֻ���ǰ10���������ļ�����
                fprintf(fp, "%d ", i + 1);
            }
            count++;
        }
    }
    if (count > 10) {
        fprintf(fp, "...��%d����Ʒ", count);
    }
    fprintf(fp, "\n\n��ϸ��Ʒ�б�:\n");
    fprintf(fp, "��Ʒ����\t����\t��ֵ\t�Ƿ�ѡ��\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%s\n", 
                i + 1, items[i].weight, items[i].value, 
                selected[i] ? "��" : "��");
    }
    
    fclose(fp);
    printf("����ѱ����� %s\n", filename);
    
    // �ͷ��ڴ�
    free(dp);
    free(selected);
    for (int w = 0; w <= capacity; w++) {
        free(prev[w]);
    }
    free(prev);
}

int main() {
    int n = 1000;  // ��Ʒ����
    int capacities[3] = {10000, 100000, 1000000};  // ��������
    
    printf("0-1�������⶯̬�滮�ⷨ (n=%d)\n", n);
    
    // �����ڴ沢������Ʒ
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // ���ÿ���������ж�̬�滮�㷨
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "dp_results_capacity_%d.txt", capacities[i]);
        dynamic_programming(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

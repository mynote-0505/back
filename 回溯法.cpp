#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ������Ʒ�ṹ��
typedef struct {
    int weight;
    double value;
    double ratio;  // ��ֵ/������
    int index;     // ԭʼ����
} Item;

// �ȽϺ���������ֵ/�����Ƚ�������
int compare(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    return (itemA->ratio < itemB->ratio) - (itemA->ratio > itemB->ratio);
}

// ���������Ʒ����
void generate_items(Item *items, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = rand() % 100 + 1;           // 1-100֮����������
        items[i].value = (double)(rand() % 901 + 100); // 100-1000֮��������ֵ
        items[i].value /= 100.0;                      // ������λС��
        items[i].index = i;                           // ��¼ԭʼ����
        items[i].ratio = items[i].value / items[i].weight; // �����ֵ/������
    }
}

// ����ʣ����Ʒ�������ܼ�ֵ�����ڼ�֦��
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

// ���ݷ����0-1��������ĸ�������
void backtrack(Item *items, int level, int n, int capacity, int weight, double value, 
               double *best_value, int *current_selection, int *best_selection) {
    // ����Ѿ�������������Ʒ
    if (level == n) {
        if (value > *best_value) {
            *best_value = value;
            for (int i = 0; i < n; i++) {
                best_selection[i] = current_selection[i];
            }
        }
        return;
    }
    
    // ѡ��ǰ��Ʒ���������������������
    if (weight + items[level].weight <= capacity) {
        current_selection[items[level].index] = 1;
        backtrack(items, level + 1, n, capacity, weight + items[level].weight, 
                 value + items[level].value, best_value, current_selection, best_selection);
        current_selection[items[level].index] = 0;
    }
    
    // ���㲻ѡ��ǰ��Ʒ������£�ʣ����Ʒ�������ܼ�ֵ
    double b = bound(items, level + 1, n, capacity, weight, value);
    
    // ���ʣ����Ʒ�������ܼ�ֵ���ڵ�ǰ���Ž⣬���������
    if (b > *best_value) {
        current_selection[items[level].index] = 0;
        backtrack(items, level + 1, n, capacity, weight, value, 
                 best_value, current_selection, best_selection);
    }
}

// ���ݷ����0-1��������
void backtracking(Item *items, int n, int capacity, const char *filename) {
    printf("���������� %d...\n", capacity);
    clock_t start = clock();
    
    // ����ֵ/������������߼�֦Ч��
    qsort(items, n, sizeof(Item), compare);
    
    double best_value = 0;
    int *current_selection = (int *)calloc(n, sizeof(int));
    int *best_selection = (int *)calloc(n, sizeof(int));
    
    // ִ�л�������
    backtrack(items, 0, n, capacity, 0, 0, &best_value, current_selection, best_selection);
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // ���������ļ�
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }
    
    fprintf(fp, "���ݷ����0-1�������� (n=%d, ����=%d)\n", n, capacity);
    fprintf(fp, "����ֵ: %.2f\n", best_value);
    fprintf(fp, "����ʱ��: %.4f��\n", time_taken);
    
    fprintf(fp, "ѡ�����Ʒ����: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (best_selection[i]) {
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
    fprintf(fp, "��Ʒ����\t����\t��ֵ\t��ֵ/������\t�Ƿ�ѡ��\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%.4f\t\t%s\n", 
                i + 1, items[i].weight, items[i].value, items[i].ratio, 
                best_selection[i] ? "��" : "��");
    }
    
    fclose(fp);
    printf("����ѱ����� %s\n", filename);
    
    free(current_selection);
    free(best_selection);
}

int main() {
    int n = 50;  // ��Ʒ������ע�⣺����n=1000�����ݷ��Ի����нϳ�ʱ�䣬����ʹ�ý�Сֵ��
    int capacities[3] = {10000, 100000, 1000000};  // ��������
    
    printf("0-1����������ݷ��ⷨ (n=%d)\n", n);
    
    // �����ڴ沢������Ʒ
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // ���ÿ���������л����㷨
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "backtracking_results_n%d_c%d.txt", n, capacities[i]);
        backtracking(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

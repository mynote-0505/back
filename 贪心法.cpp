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

// ̰�ķ����0-1��������
void greedy(Item *items, int n, int capacity, const char *filename) {
    printf("���������� %d...\n", capacity);
    clock_t start = clock();
    
    // ����ֵ/����������
    qsort(items, n, sizeof(Item), compare);
    
    double total_value = 0;
    int total_weight = 0;
    int *selected = (int *)calloc(n, sizeof(int));
    
    // �����ʽ���ѡ����Ʒ
    for (int i = 0; i < n; i++) {
        if (total_weight + items[i].weight <= capacity) {
            total_weight += items[i].weight;
            total_value += items[i].value;
            selected[items[i].index] = 1; // ʹ��ԭʼ�������ѡ�����Ʒ
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // ���������ļ�
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }
    
    fprintf(fp, "̰�ķ����0-1�������� (n=%d, ����=%d)\n", n, capacity);
    fprintf(fp, "����ֵ: %.2f\n", total_value);
    fprintf(fp, "����ʱ��: %.4f��\n", time_taken);
    fprintf(fp, "ѡ�����Ʒ����: %d\n", total_weight);
    
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
    fprintf(fp, "��Ʒ����\t����\t��ֵ\t��ֵ/������\t�Ƿ�ѡ��\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t\t%d\t%.2f\t%.4f\t\t%s\n", 
                i + 1, items[i].weight, items[i].value, items[i].ratio, 
                selected[i] ? "��" : "��");
    }
    
    fclose(fp);
    printf("����ѱ����� %s\n", filename);
    
    free(selected);
}

int main() {
    int n = 1000;  // ��Ʒ����
    int capacities[3] = {10000, 100000, 1000000};  // ��������
    
    printf("0-1��������̰�ķ��ⷨ (n=%d)\n", n);
    
    // �����ڴ沢������Ʒ
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // ���ÿ����������̰���㷨
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "greedy_results_capacity_%d.txt", capacities[i]);
        greedy(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

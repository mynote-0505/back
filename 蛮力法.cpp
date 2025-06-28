#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// ���������0-1��������
void brute_force(Item *items, int n, int capacity, const char *filename) {
    printf("���������� %d...\n", capacity);
    clock_t start = clock();
    
    double max_value = 0;
    int *best_combination = (int *)calloc(n, sizeof(int));
    
    // �������п��ܵ���Ʒ��ϣ�2^n�֣�
    for (unsigned long long mask = 0; mask < (1ULL << n); mask++) {
        int total_weight = 0;
        double total_value = 0;
        int *current_combination = (int *)calloc(n, sizeof(int));
        
        // ���㵱ǰ��ϵ����������ܼ�ֵ
        for (int i = 0; i < n; i++) {
            if (mask & (1ULL << i)) {
                total_weight += items[i].weight;
                total_value += items[i].value;
                current_combination[i] = 1;
            }
        }
        
        // �����ǰ������������������Ҽ�ֵ���ߣ���������Ž�
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
    
    // ���������ļ�
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }
    
    fprintf(fp, "���������0-1�������� (n=%d, ����=%d)\n", n, capacity);
    fprintf(fp, "����ֵ: %.2f\n", max_value);
    fprintf(fp, "����ʱ��: %.4f��\n", time_taken);
    
    fprintf(fp, "ѡ�����Ʒ����: ");
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (best_combination[i]) {
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
                best_combination[i] ? "��" : "��");
    }
    
    fclose(fp);
    printf("����ѱ����� %s\n", filename);
    
    free(best_combination);
}

int main() {
    int n = 20;  // ��Ʒ������ע�⣺����n=1000�������������м���ʱ�䣬����ʹ�ý�Сֵ��
    int capacities[3] = {10000, 100000, 1000000};  // ��������
    
    printf("0-1���������������ⷨ (n=%d)\n", n);
    
    // �����ڴ沢������Ʒ
    Item *items = (Item *)malloc(n * sizeof(Item));
    generate_items(items, n);
    
    // ���ÿ���������������㷨
    for (int i = 0; i < 3; i++) {
        char filename[50];
        sprintf(filename, "brute_force_results_n%d_c%d.txt", n, capacities[i]);
        brute_force(items, n, capacities[i], filename);
    }
    
    free(items);
    return 0;
}

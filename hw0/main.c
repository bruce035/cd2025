#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 256  

// 定義 hash table 的節點結構
typedef struct HashNode {
    char key;
    int count;
    struct HashNode *next;
} HashNode;

// hash function：以字元的 ASCII 值當key
unsigned int hash(char key) {
    return ((unsigned char)key) % TABLE_SIZE;
}

// 在 hash table 中插入字元
void insert(HashNode* table[], char key) {
    unsigned int index = hash(key);
    HashNode *node = table[index];
    
    // 搜尋該 bucket 看是否存在key
    while (node != NULL) {
        if (node->key == key) {
            node->count++;  // 找到則更新頻率
            return;
        }
        node = node->next;
    }
    
    // 未找到，新增一個節點，插入開頭
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->count = 1;
    newNode->next = table[index];
    table[index] = newNode;
}

// 釋放 hash table 的記憶體
void freeHashTable(HashNode* table[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = table[i];
        while (node) {
            HashNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
}

int main() {
    // 初始化 hash table 的所有 bucket 為 NULL
    HashNode *hashTable[TABLE_SIZE];
    memset(hashTable, 0, sizeof(hashTable));
    
    // 讀取自己的程式檔案
    const char *filename = __FILE__;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return 1;
    }
    
    int ch;
    // 讀取檔案，每讀取一個字元就更新 hash table
    while ((ch = fgetc(fp)) != EOF) {
        insert(hashTable, (char)ch);
    }
    fclose(fp);
    
    // 輸出 hash table 中所有非空的 bucket 內容
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = hashTable[i];
        while (node) {
            // 特殊字元處理：例如換行字元用 "\n" 表示
            if (node->key == '\n')
                printf("\\n : %d\n", node->count);
            else if (node->key == ' ')
                printf("' ' : %d\n", node->count);
            else
                printf("%c : %d\n", node->key, node->count);
            node = node->next;
        }
    }
    
    // 釋放 hash table 記憶體
    freeHashTable(hashTable);
    
    return 0;
}

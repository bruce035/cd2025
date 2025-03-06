#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char data;
    struct Node *next;
} Node;

Node* createNode(char data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

Node* readFileToLinkedList(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    Node *head = NULL, *tail = NULL;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        Node *newNode = createNode((char)ch);
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    fclose(fp);
    return head;
}

void countFrequency(Node* head, int freq[256]) {
    memset(freq, 0, sizeof(int) * 256);
    Node* current = head;
    while (current != NULL) {
        freq[(unsigned char)current->data]++;
        current = current->next;
    }
}

void printFrequency(Node* head, int freq[256]) {
    int printed[256] = {0};  
    Node* current = head;
    while (current != NULL) {
        unsigned char c = current->data;
        if (!printed[c]) {
            if (c == '\n')
                printf("\\n : %d\n", freq[c]);
            else if (c == ' ')
                printf("' ' : %d\n", freq[c]);
            else
                printf("%c : %d\n", c, freq[c]);
            printed[c] = 1;
        }
        current = current->next;
    }
}

void freeLinkedList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

int main() {
    const char *filename = __FILE__;
    
    Node* head = readFileToLinkedList(filename);
    
    int freq[256];
    countFrequency(head, freq);
    
    printFrequency(head, freq);
    
    freeLinkedList(head);
    
    return 0;
}

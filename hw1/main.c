#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 256
#define MAX_TOKEN_LENGTH 100

/* -------- HW#0 保留的 Hash Table + linking list 模組 ------- */
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

/* ------------------ Lexer/Scanner 模組 ------------------ */

/* 輔助字元判斷函式 */
int is_letter(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_alnum(char c) {
    return is_letter(c) || is_digit(c);
}

int is_whitespace(char c) {
    return (c == ' ' || c == '\n' || c == '\t');
}

void lexical_error(char c) {
    printf("lexical error: %c\n", c);
    exit(EXIT_FAILURE);
}

/* 定義 Token 類型 */
typedef enum {
    TOKEN_TYPE,         // 關鍵字 int
    TOKEN_MAIN,         // 關鍵字 main
    TOKEN_IF,           // 關鍵字 if
    TOKEN_ELSE,         // 關鍵字 else
    TOKEN_WHILE,        // 關鍵字 while
    TOKEN_ID,           // 識別字
    TOKEN_LITERAL,      // 整數常數
    TOKEN_EQUAL,        // ==
    TOKEN_GREATEREQUAL, // >=
    TOKEN_LESSEQUAL,    // <=
    TOKEN_GREATER,      // >
    TOKEN_LESS,         // <
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_LEFTPAREN,    // (
    TOKEN_REFTPAREN,    // )
    TOKEN_LEFTBRACE,    // {
    TOKEN_REFTBRACE,    // }
    TOKEN_SEMICOLON,    // ;
    TOKEN_UNKNOWN       // 未知（備用）
} TokenType;

/* 將 TokenType 轉換成字串，方便印出 */
const char *tokenTypeToStr(TokenType type) {
    switch (type) {
        case TOKEN_TYPE: return "TYPE_TOKEN";
        case TOKEN_MAIN: return "MAIN_TOKEN";
        case TOKEN_IF: return "IF_TOKEN";
        case TOKEN_ELSE: return "ELSE_TOKEN";
        case TOKEN_WHILE: return "WHILE_TOKEN";
        case TOKEN_ID: return "ID_TOKEN";
        case TOKEN_LITERAL: return "LITERAL_TOKEN";
        case TOKEN_EQUAL: return "EQUAL_TOKEN";
        case TOKEN_GREATEREQUAL: return "GREATEREQUAL_TOKEN";
        case TOKEN_LESSEQUAL: return "LESSEQUAL_TOKEN";
        case TOKEN_GREATER: return "GREATER_TOKEN";
        case TOKEN_LESS: return "LESS_TOKEN";
        case TOKEN_ASSIGN: return "ASSIGN_TOKEN";
        case TOKEN_PLUS: return "PLUS_TOKEN";
        case TOKEN_MINUS: return "MINUS_TOKEN";
        case TOKEN_LEFTPAREN: return "LEFTPAREN_TOKEN";
        case TOKEN_REFTPAREN: return "REFTPAREN_TOKEN";
        case TOKEN_LEFTBRACE: return "LEFTBRACE_TOKEN";
        case TOKEN_REFTBRACE: return "REFTBRACE_TOKEN";
        case TOKEN_SEMICOLON: return "SEMICOLON_TOKEN";
        default: return "UNKNOWN_TOKEN";
    }
}

/* 從檔案中讀取字元並辨識 token */
void scanTokens(FILE *fp) {
    int ch, c;
    char token[MAX_TOKEN_LENGTH];
    int index;
    
    while ((ch = fgetc(fp)) != EOF) {
        if (is_whitespace(ch))
            continue;  // 忽略空白字元
        
        /* 辨識token或關鍵字：必須以字母開始，後面可接英文字母、數字或底線 */
        if (is_letter(ch)) {
            index = 0;
            token[index++] = (char)ch;
            while ((c = fgetc(fp)) != EOF && (is_alnum(c) || c == '_')) {
                if (index < MAX_TOKEN_LENGTH - 1)
                    token[index++] = (char)c;
            }
            token[index] = '\0';
            if (c != EOF)
                ungetc(c, fp);
            
            TokenType ttype;
            if (strcmp(token, "int") == 0)
                ttype = TOKEN_TYPE;
            else if (strcmp(token, "main") == 0)
                ttype = TOKEN_MAIN;
            else if (strcmp(token, "if") == 0)
                ttype = TOKEN_IF;
            else if (strcmp(token, "else") == 0)
                ttype = TOKEN_ELSE;
            else if (strcmp(token, "while") == 0)
                ttype = TOKEN_WHILE;
            else
                ttype = TOKEN_ID;
            
            printf("%s: %s\n", token, tokenTypeToStr(ttype));
        }
        /* 辨識整數常數 */
        else if (is_digit(ch)) {
            index = 0;
            token[index++] = (char)ch;
            while ((c = fgetc(fp)) != EOF && is_digit(c)) {
                if (index < MAX_TOKEN_LENGTH - 1)
                    token[index++] = (char)c;
            }
            token[index] = '\0';
            if (c != EOF)
                ungetc(c, fp);
            printf("%s: %s\n", token, tokenTypeToStr(TOKEN_LITERAL));
        }
        /* 辨識運算子及符號 */
        else {
            if (ch == '=') {
                c = fgetc(fp);
                if (c == '=') {
                    printf("==: %s\n", tokenTypeToStr(TOKEN_EQUAL));
                } else {
                    if (c != EOF)
                        ungetc(c, fp);
                    printf("=: %s\n", tokenTypeToStr(TOKEN_ASSIGN));
                }
            } else if (ch == '>') {
                c = fgetc(fp);
                if (c == '=') {
                    printf(">=: %s\n", tokenTypeToStr(TOKEN_GREATEREQUAL));
                } else {
                    if (c != EOF)
                        ungetc(c, fp);
                    printf(">: %s\n", tokenTypeToStr(TOKEN_GREATER));
                }
            } else if (ch == '<') {
                c = fgetc(fp);
                if (c == '=') {
                    printf("<=: %s\n", tokenTypeToStr(TOKEN_LESSEQUAL));
                } else {
                    if (c != EOF)
                        ungetc(c, fp);
                    printf("<: %s\n", tokenTypeToStr(TOKEN_LESS));
                }
            } else if (ch == '+') {
                printf("+: %s\n", tokenTypeToStr(TOKEN_PLUS));
            } else if (ch == '-') {
                printf("-: %s\n", tokenTypeToStr(TOKEN_MINUS));
            } else if (ch == '(') {
                printf("(: %s\n", tokenTypeToStr(TOKEN_LEFTPAREN));
            } else if (ch == ')') {
                printf("): %s\n", tokenTypeToStr(TOKEN_REFTPAREN));
            } else if (ch == '{') {
                printf("{: %s\n", tokenTypeToStr(TOKEN_LEFTBRACE));
            } else if (ch == '}') {
                printf("}: %s\n", tokenTypeToStr(TOKEN_REFTBRACE));
            } else if (ch == ';') {
                printf(";: %s\n", tokenTypeToStr(TOKEN_SEMICOLON));
            } else {
                lexical_error((char)ch);
            }
        }
    }
}

/* ------------------ 主程式 ------------------ */
int main() {
    HashNode *hashTable[TABLE_SIZE];
    memset(hashTable, 0, sizeof(hashTable));
    
    /* 開啟 input.c 檔案，該檔案包含要辨識的 token */
    const char *filename = "input.c";
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Unable to open input file");
        return 1;
    }
    
    /* call掃描函式 */
    scanTokens(fp);
    
    fclose(fp);
    
    // 釋放 hashTable 記憶體
    freeHashTable(hashTable);
    
    return 0;
}

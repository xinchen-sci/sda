#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1024
#define MAX_STRING_LENGTH 256
#define INITIAL_CAPACITY 10

typedef struct {
    int id;
    char class_name[MAX_STRING_LENGTH];
    double duration;
    char instructor[MAX_STRING_LENGTH];
} Record;

typedef struct {
    Record *records;
    size_t size;
    size_t capacity;
} RecordTable;

void init_record_table(RecordTable *table) {
    table->size = 0;
    table->capacity = INITIAL_CAPACITY;
    table->records = (Record *)malloc(table->capacity * sizeof(Record));
    if (table->records == NULL) {
        perror("Unable to allocate memory for records");
        exit(EXIT_FAILURE);
    }
}

void free_record_table(RecordTable *table) {
    free(table->records);
    table->records = NULL;
    table->size = 0;
    table->capacity = 0;
}

void resize_record_table(RecordTable *table, size_t new_capacity) {
    Record *new_records = (Record *)realloc(table->records, new_capacity * sizeof(Record));
    if (new_records == NULL) {
        perror("Unable to reallocate memory for records");
        exit(EXIT_FAILURE);
    }
    table->records = new_records;
    table->capacity = new_capacity;
    printf("Resize: new capacity %d\n", new_capacity);
}

void add_record(RecordTable *table, Record *record) {
    if (table->size >= table->capacity) {
        resize_record_table(table, table->capacity * 2);
    }
    table->records[table->size++] = *record;
}

void parse_csv_line(char *line, Record *record) {
    char *token;
    int field_count = 0;
    int in_quotes = 0;
    char *p = line, *start;

    start = p;

    while (*p) {
        if (*p == '"') {
            in_quotes = !in_quotes;
        } else if (*p == ',' && !in_quotes) {
            *p = '\0';
            token = start;
            switch (field_count) {
                case 0: record->id = atoi(token); break;
                case 1: strncpy(record->class_name, token, MAX_STRING_LENGTH); break;
                case 2: record->duration = atof(token); break;
            }
            field_count++;
            start = p + 1;
        }
        p++;
    }

    token = start;
    strncpy(record->instructor, token, MAX_STRING_LENGTH);
}

void read_csv(const char *filename, RecordTable *table) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    Record record;

    // 打开CSV文件
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    // 跳过第一行标题
    fgets(line, sizeof(line), file);

    // 读取文件中的每一行
    while (fgets(line, sizeof(line), file)) {
        // 移除换行符
        line[strcspn(line, "\r\n")] = 0;

        // 解析行内容
        parse_csv_line(line, &record);

        // 添加记录到动态表
        add_record(table, &record);
    }

    // 关闭文件
    fclose(file);
}

Record* find_record_by_id(RecordTable *table, int id) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->records[i].id == id) {
            return &table->records[i];
        }
    }
    return NULL;
}

bool remove_record_by_id(RecordTable *table, int id) {
    for (size_t i = 0; i < table->size; i++) {
        if (table->records[i].id == id) {
            for (size_t j = i; j < table->size - 1; j++) {
                table->records[j] = table->records[j + 1];
            }
            table->size--;
            if (table->size < table->capacity / 4) {
                resize_record_table(table, table->capacity / 2);
            }
            return true;
        }
    }
    return false;
}

int main() {
    RecordTable table;
    init_record_table(&table);

    //读取并添加
    read_csv("courses.csv", &table);
    //输出全部课程
    for (size_t i = 0; i < table.size; i++) {
        printf("Record %zu: id=%d, class_name=%s, duration=%.2f, instructor=%s\n",
               i, table.records[i].id, table.records[i].class_name, table.records[i].duration, table.records[i].instructor);
    }

    //查找
    int search_id = 2;
    Record *record = find_record_by_id(&table, search_id);
    if (record != NULL) {
        printf("Found record: id=%d, class_name=%s, duration=%.2f, instructor=%s\n",
               record->id, record->class_name, record->duration, record->instructor);
    } else {
        printf("Record with id=%d not found.\n", search_id);
    }

    //删除
    int delete_id = 9;
    if (remove_record_by_id(&table, delete_id)) {
        printf("Record with id=%d deleted successfully.\n", delete_id);
    } else {
        printf("Record with id=%d not found.\n", delete_id);
    }

    free_record_table(&table);
    return 0;
}

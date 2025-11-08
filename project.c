#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50
#define MAX_ROLL_LEN 15
#define DATA_FILE "student_records.dat"

struct Student {    
    int rollNumber;
    char name[MAX_NAME_LEN];
    int marksC;
    int marksMaths;
    int marksPhysics;
    int marksElectronics;
    float totalMarks;
    float percentage;
    char grade[3];
};

struct Student students[MAX_STUDENTS]; 
int studentCount = 0; 
int nextRollNumber = 1001; 

void initializeSystem();
void loadRecordsFromFile();
void saveRecordsToFile();
void updateNextRollNumber();

void displayMainMenu();
void executeMenuChoice(int choice);
void pressAnyKeyToContinue();

void handleAddRecord();
void handleCalculate(struct Student *s);
void handleViewAllRecords();
void handleSearchRecord();
void handleSortRecords();
void handleDeleteRecord();
void generatePerformanceReport();

void safeInput(char *buffer, int max_len);
void assignGrade(float percentage, char *gradeBuffer);
int readValidatedMark(const char *prompt);

int main() {
    initializeSystem();

    int choice;
    do {
        displayMainMenu();

        if (scanf("%d", &choice) != 1) {
            printf("\n\t[!] Invalid input. Please enter a number (1-8).\n");
            while (getchar() != '\n');
            choice = 0; 
        } else {
            executeMenuChoice(choice);
        }

    } while (choice != 8); 

    printf("\n[SYSTEM] Saving all student records before exit...\n");
    saveRecordsToFile();
    printf("[SYSTEM] Exiting Student Result Management System. Goodbye!\n");

    return 0;
}

void initializeSystem() {
    printf("--- SRMS Initializing ---\n");
    loadRecordsFromFile();
    updateNextRollNumber();
    printf("Initialization Complete. Loaded %s\n\n", DATA_FILE);
}

void updateNextRollNumber() {
    int maxRoll = 1000;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber > maxRoll) {
            maxRoll = students[i].rollNumber;
        }
    }
    nextRollNumber = maxRoll + 1;
    printf("[SYSTEM] Next unique Roll Number set to: %d\n", nextRollNumber);
}

void loadRecordsFromFile() {
    FILE *file = fopen(DATA_FILE, "rb");

    if (file == NULL) {
        printf("[SYSTEM] Data file '%s' not found. Starting with an empty list.\n", DATA_FILE);
        return;
    }
    
    if (fread(&studentCount, sizeof(int), 1, file) != 1) {
        studentCount = 0;
        fclose(file);
        return;
    }

    if (studentCount > 0) {
        size_t records_read = fread(students, sizeof(struct Student), studentCount, file);
        if (records_read != (size_t)studentCount) {
             printf("[WARNING] Partial data loaded. Expected %d records, read %zu.\n", studentCount, records_read);
             studentCount = (int)records_read;
        }
    }

    printf("[SYSTEM] Successfully loaded %d student records.\n", studentCount);
    fclose(file);
}

void saveRecordsToFile() {
    FILE *file = fopen(DATA_FILE, "wb"); 

    if (file == NULL) {
        perror("Error: Could not open data file for saving");
        return;
    }

    if (fwrite(&studentCount, sizeof(int), 1, file) != 1) {
        perror("Error writing student count");
        fclose(file); 
        return;
    }
    
    if (studentCount > 0) {
        if (fwrite(students, sizeof(struct Student), studentCount, file) != (size_t)studentCount) {
            perror("Error writing student array block");
            fclose(file); 
            return;
        }
    }

    printf("[SYSTEM] Successfully saved %d records to '%s'.\n", studentCount, DATA_FILE);
    fclose(file);
}

void displayMainMenu() {
    printf("\n\n--------------------------------------------\n");
    printf("|  STUDENT RESULT MANAGEMENT SYSTEM (SRMS) |\n");
    printf("--------------------------------------------\n");
    printf("| 1. Add New Student Record                |\n");
    printf("| 2. View All Records                      |\n");
    printf("| 3. Search Record by Roll Number          |\n");
    printf("| 4. Sort Records by Percentage            |\n");
    printf("| 5. Generate Performance Report           |\n");
    printf("| 6. Delete Record                         |\n");
    printf("| 7. System Status (Max %d)              |\n", MAX_STUDENTS);
    printf("| 8. Save & Exit                           |\n");
    printf("--------------------------------------------\n");
    printf("Enter your choice (1-8): ");
}

void executeMenuChoice(int choice) {
    switch (choice) {
        case 1: handleAddRecord(); break;
        case 2: handleViewAllRecords(); break;
        case 3: handleSearchRecord(); break;
        case 4: handleSortRecords(); break;
        case 5: generatePerformanceReport(); break;
        case 6: handleDeleteRecord(); break;
        case 7: 
            printf("\n[STATUS] Total Capacity: %d, Records Used: %d, Records Free: %d\n", 
                   MAX_STUDENTS, studentCount, MAX_STUDENTS - studentCount);
            pressAnyKeyToContinue();
            break;
        case 8: 
            break; 
        default: 
            printf("\n\t[!] Invalid choice: %d. Please select a number from 1 to 8.\n", choice);
            pressAnyKeyToContinue();
    }
}

void pressAnyKeyToContinue() {
    printf("\n\t[INFO] Press ENTER to continue...");
    while (getchar() != '\n'); 
    getchar(); 
}

void handleAddRecord() {
    if (studentCount >= MAX_STUDENTS) {
        printf("\n[WARNING] Cannot add record. System limit of %d students reached.\n", MAX_STUDENTS);
        pressAnyKeyToContinue();
        return;
    }
    
    printf("\n--- ADD NEW STUDENT RECORD ---\n");
    struct Student *s = &students[studentCount];
    
    s->rollNumber = nextRollNumber++;
    printf("Assigned Roll Number: %d\n", s->rollNumber);

    printf("Enter Student Name (Max %d chars): ", MAX_NAME_LEN - 1);
    while (getchar() != '\n');
    safeInput(s->name, MAX_NAME_LEN);

    s->marksC = readValidatedMark("Enter Marks for C Programming (0-100): ");
    s->marksMaths = readValidatedMark("Enter Marks for Mathematics (0-100): ");
    s->marksPhysics = readValidatedMark("Enter Marks for Physics (0-100): ");
    s->marksElectronics = readValidatedMark("Enter Marks for Electronics (0-100): ");

    handleCalculate(s);
    studentCount++;

    printf("\n[SUCCESS] Record for %s (Roll %d) added.\n", s->name, s->rollNumber);
    saveRecordsToFile(); 
    pressAnyKeyToContinue();
}

void handleCalculate(struct Student *s) {
    int max_total_marks = 400;
    
    s->totalMarks = s->marksC + s->marksMaths + s->marksPhysics + s->marksElectronics;
    s->percentage = (s->totalMarks / max_total_marks) * 100.0f;

    assignGrade(s->percentage, s->grade);
}

void handleViewAllRecords() {
    printf("\n--- ALL STUDENT RESULTS (%d RECORDS) ---\n", studentCount);
    if (studentCount == 0) {
        printf("[INFO] No records found.\n");
        pressAnyKeyToContinue();
        return;
    }

    printf("-----------------------------------------------------------------------------------------------------------------\n");
    printf("| Roll No | Name                 | C   | Maths | Phy | Elec | Total | Percent | Grade |\n");
    printf("-----------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < studentCount; i++) {
        struct Student s = students[i];
        printf("| %-7d | %-20s | %-3d | %-5d | %-3d | %-4d | %-5.0f | %-7.2f | %-5s |\n",
               s.rollNumber, s.name, s.marksC, s.marksMaths, s.marksPhysics, s.marksElectronics,
               s.totalMarks, s.percentage, s.grade);
    }
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    pressAnyKeyToContinue();
}

void handleSearchRecord() {
    printf("\n--- SEARCH RECORD ---\n");
    int search_roll;
    printf("Enter Roll Number to search: ");
    if (scanf("%d", &search_roll) != 1) {
        printf("[!] Invalid Roll Number input.\n");
        while (getchar() != '\n');
        pressAnyKeyToContinue();
        return;
    }
    while (getchar() != '\n');

    int found = 0;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == search_roll) {
            printf("\n[FOUND] Record Details:\n");
            handleViewAllRecords();
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("[INFO] Roll Number %d not found in records.\n", search_roll);
    }
    pressAnyKeyToContinue();
}

void handleSortRecords() {
    printf("\n--- SORTING RECORDS (by Percentage: High to Low) ---\n");
    if (studentCount <= 1) {
        printf("[INFO] Not enough records to sort.\n");
        pressAnyKeyToContinue();
        return;
    }

    for (int i = 0; i < studentCount - 1; i++) {
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (students[j].percentage < students[j+1].percentage) {
                struct Student temp = students[j];
                students[j] = students[j+1];
                students[j+1] = temp;
            }
        }
    }

    printf("[SUCCESS] Records sorted. Displaying results:\n");
    handleViewAllRecords();
    saveRecordsToFile();
}

void handleDeleteRecord() {
    printf("\n--- DELETE RECORD ---\n");
    int delete_roll;
    printf("Enter Roll Number to delete: ");
    if (scanf("%d", &delete_roll) != 1) {
        printf("[!] Invalid Roll Number input.\n");
        while (getchar() != '\n');
        pressAnyKeyToContinue();
        return;
    }
    while (getchar() != '\n');

    int found_index = -1;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == delete_roll) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        for (int i = found_index; i < studentCount - 1; i++) {
            students[i] = students[i+1];
        }
        studentCount--;
        printf("[SUCCESS] Record for Roll Number %d deleted.\n", delete_roll);
        saveRecordsToFile();
    } else {
        printf("[WARNING] Roll Number %d not found.\n", delete_roll);
    }
    pressAnyKeyToContinue();
}

void generatePerformanceReport() {
    printf("\n--- CLASS PERFORMANCE REPORT ---\n");
    if (studentCount == 0) {
        printf("[INFO] No records to report on.\n");
        pressAnyKeyToContinue();
        return;
    }
    
    int passed = 0;
    int failed = 0;
    int distinctions = 0;
    float total_percentage = 0.0f;

    for (int i = 0; i < studentCount; i++) {
        total_percentage += students[i].percentage;
        
        if (students[i].percentage >= 40.0f) {
            passed++;
        } else {
            failed++;
        }

        if (students[i].percentage >= 75.0f) {
            distinctions++;
        }
    }
    
    float average_percentage = total_percentage / studentCount;

    printf("\n[SUMMARY]\n");
    printf("Total Students Analyzed: %d\n", studentCount);
    printf("Students Passed (>= 40%%): %d\n", passed);
    printf("Students Failed (< 40%%): %d\n", failed);
    printf("Students with Distinction (>= 75%%): %d\n", distinctions);
    printf("Class Average Percentage: %.2f%%\n", average_percentage);
    printf("----------------------------------------\n");
    
    pressAnyKeyToContinue();
}


void safeInput(char *buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

void assignGrade(float percentage, char *gradeBuffer) {
    if (percentage >= 90.0f) {
        strcpy(gradeBuffer, "A+");
    } else if (percentage >= 80.0f) {
        strcpy(gradeBuffer, "A");
    } else if (percentage >= 70.0f) {
        strcpy(gradeBuffer, "B+");
    } else if (percentage >= 60.0f) {
        strcpy(gradeBuffer, "B");
    } else if (percentage >= 50.0f) {
        strcpy(gradeBuffer, "C");
    } else if (percentage >= 40.0f) {
        strcpy(gradeBuffer, "D");
    } else {
        strcpy(gradeBuffer, "F");
    }

}
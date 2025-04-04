/*
Hospital Management System - Phase 2
Authors: Brownie Tran
         Raymond Yang
Date: 2025-04-01
Description: Enhanced C program to manage patient records and doctor schedules.
             This version includes:
             - Dynamic memory allocation
             - File handling for data persistence
             - Linked lists for efficient data management
             - Reporting and analytics
             - Advanced error handling
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL_CAPACITY 10
#define MAX_DAYS_IN_WEEK 7
#define MAX_SHIFTS_IN_DAY 3
#define MAX_FILENAME_LENGTH 100

// Structure for patient records using linked list
typedef struct Patient {
    int patientID;
    char patientName[50];
    int patientAge;
    char patientDiagnosis[250];
    int patientRoomNum;
    char admissionDate[20];
    char dischargeDate[20];
    int isActive;
    struct Patient *next;
} Patient;

// Structure for doctor records using linked list
typedef struct Doctor {
    int doctorID;
    char doctorName[50];
    int totalShifts;
    struct Doctor *next;
} Doctor;

// Global variables
Patient *patientHead = NULL;
Doctor *doctorHead = NULL;
int totalPatients = 0;
int totalDoctors = 0;
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // 1-n = doctor index, 0 = not assigned

// Function prototypes
// Memory management functions
void initializeSystem();

void cleanupSystem();

Patient *createPatient(int id, const char *name, int age, const char *diagnosis, int roomNum);

Doctor *createDoctor(int id, const char *name);

// File handling functions
int saveData();

int loadData();

int backupData();

int restoreData();

char *selectBackup();

void getCurrentDateTime(char *dateTime, int bufferSize);

int safeLoadData();

// Patient management functions
void addPatient();

void viewPatients();

void searchPatient();

void dischargePatient();

Patient *findPatientByID(int id);

int isRoomAvailable(int roomNum);

// Doctor management functions
void addDoctor();

void viewDoctors();

void manageDoctorSchedule();

void assignShift(int doctorID, int dayInWeek, int shiftInDay);

void viewSchedule();

Doctor *findDoctorByID(int id);

// Reporting functions
void generateReports();

void patientAdmissionReport();

void doctorUtilizationReport();

void roomUtilizationReport();

// UI and utility functions
void menu();

void clearInputBuffer();

void returnToMenu();

int scanInt();

void printHeader(const char *title);

// Main function
int main() {
    initializeSystem();
    loadData();
    menu();
    saveData();
    cleanupSystem();
    return 0;
}

// Initialize the system
void initializeSystem() {
    // Initialize doctor schedule array
    for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
        for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
            doctorSchedule[i][j] = 0;
        }
    }
}

// Clean up allocated memory
void cleanupSystem() {
    // Free patient linked list
    Patient *currentPatient = patientHead;
    Patient *nextPatient;

    while (currentPatient != NULL) {
        nextPatient = currentPatient->next;
        free(currentPatient);
        currentPatient = nextPatient;
    }

    // Free doctor linked list
    Doctor *currentDoctor = doctorHead;
    Doctor *nextDoctor;

    while (currentDoctor != NULL) {
        nextDoctor = currentDoctor->next;
        free(currentDoctor);
        currentDoctor = nextDoctor;
    }
}

// Create a new patient node
Patient *createPatient(int id, const char *name, int age, const char *diagnosis, int roomNum) {
    Patient *newPatient = (Patient *) malloc(sizeof(Patient));
    if (newPatient == NULL) {
        printf("Error: Memory allocation failed for patient record.\n");
        return NULL;
    }

    newPatient->patientID = id;
    strncpy(newPatient->patientName, name, sizeof(newPatient->patientName) - 1);
    newPatient->patientName[sizeof(newPatient->patientName) - 1] = '\0';
    newPatient->patientAge = age;
    strncpy(newPatient->patientDiagnosis, diagnosis, sizeof(newPatient->patientDiagnosis) - 1);
    newPatient->patientDiagnosis[sizeof(newPatient->patientDiagnosis) - 1] = '\0';
    newPatient->patientRoomNum = roomNum;

    // Get current date for admission
    getCurrentDateTime(newPatient->admissionDate, sizeof(newPatient->admissionDate));

    // Initialize new fields
    newPatient->dischargeDate[0] = '\0'; // Empty string
    newPatient->isActive = 1; // Active by default

    newPatient->next = NULL;
    return newPatient;
}

// Create a new doctor node
Doctor *createDoctor(int id, const char *name) {
    Doctor *newDoctor = (Doctor *) malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        printf("Error: Memory allocation failed for doctor record.\n");
        return NULL;
    }

    newDoctor->doctorID = id;
    strncpy(newDoctor->doctorName, name, sizeof(newDoctor->doctorName) - 1);
    newDoctor->doctorName[sizeof(newDoctor->doctorName) - 1] = '\0';
    newDoctor->totalShifts = 0;
    newDoctor->next = NULL;
    return newDoctor;
}

// Save data to files
int saveData() {
    FILE *patientFile = fopen("../data/patients.dat", "wb");
    if (patientFile == NULL) {
        printf("Error: Unable to open patients.dat for writing.\n");
        return 0;
    }

    // Write total patients first
    fwrite(&totalPatients, sizeof(int), 1, patientFile);

    // Write each patient
    Patient *current = patientHead;
    while (current != NULL) {
        fwrite(current, sizeof(Patient) - sizeof(Patient *), 1, patientFile);
        current = current->next;
    }
    fclose(patientFile);

    // Save doctors
    FILE *doctorFile = fopen("../data/doctors.dat", "wb");
    if (doctorFile == NULL) {
        printf("Error: Unable to open doctors.dat for writing.\n");
        return 0;
    }

    // Write total doctors first
    fwrite(&totalDoctors, sizeof(int), 1, doctorFile);

    // Write each doctor
    Doctor *currentDoc = doctorHead;
    while (currentDoc != NULL) {
        fwrite(currentDoc, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);
        currentDoc = currentDoc->next;
    }
    fclose(doctorFile);

    // Save schedule
    FILE *scheduleFile = fopen("../data/schedule.dat", "wb");
    if (scheduleFile == NULL) {
        printf("Error: Unable to open schedule.dat for writing.\n");
        return 0;
    }

    fwrite(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data saved successfully.\n");
    backupData();
    return 1;
}

// Load data from files
int loadData() {
    // Load patients
    FILE *patientFile = fopen("../data/patients.dat", "rb");
    if (patientFile == NULL) {
        printf("No existing patient data found. Starting with empty records.\n");
        return 0;
    }

    // Read total patients first
    fread(&totalPatients, sizeof(int), 1, patientFile);

    // Read each patient
    Patient tempPatient;
    for (int i = 0; i < totalPatients; i++) {
        fread(&tempPatient, sizeof(Patient) - sizeof(Patient *), 1, patientFile);

        // Create a new patient node and add to list
        Patient *newPatient = createPatient(
            tempPatient.patientID,
            tempPatient.patientName,
            tempPatient.patientAge,
            tempPatient.patientDiagnosis,
            tempPatient.patientRoomNum
        );

        // Copy admission date
        strncpy(newPatient->admissionDate, tempPatient.admissionDate, sizeof(newPatient->admissionDate));

        // Add to linked list
        if (patientHead == NULL) {
            patientHead = newPatient;
        } else {
            Patient *current = patientHead;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newPatient;
        }
    }
    fclose(patientFile);

    // Load doctors
    FILE *doctorFile = fopen("../data/doctors.dat", "rb");
    if (doctorFile == NULL) {
        printf("No existing doctor data found. Starting with empty records.\n");
        return 0;
    }

    // Read total doctors first
    fread(&totalDoctors, sizeof(int), 1, doctorFile);

    // Read each doctor
    Doctor tempDoctor;
    for (int i = 0; i < totalDoctors; i++) {
        fread(&tempDoctor, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);

        // Create a new doctor node and add to list
        Doctor *newDoctor = createDoctor(tempDoctor.doctorID, tempDoctor.doctorName);
        newDoctor->totalShifts = tempDoctor.totalShifts;

        // Add to linked list
        if (doctorHead == NULL) {
            doctorHead = newDoctor;
        } else {
            Doctor *current = doctorHead;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newDoctor;
        }
    }
    fclose(doctorFile);

    // Load schedule
    FILE *scheduleFile = fopen("../data/schedule.dat", "rb");
    if (scheduleFile == NULL) {
        printf("No existing schedule data found. Starting with empty schedule.\n");
        return 0;
    }

    fread(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data loaded successfully.\n");
    return 1;
}

// Backup current data with timestamp
int backupData() {
    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));
    // Replace spaces and colons with underscores for a valid filename
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }
    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/patients_%s.dat", timestamp);

    FILE *patientFile = fopen(reportFileName, "wb");
    if (patientFile == NULL) {
        printf("Error: Unable to open patients backup file for writing.\n");
        return 0;
    }

    // Write total patients first
    fwrite(&totalPatients, sizeof(int), 1, patientFile);

    // Write each patient
    Patient *current = patientHead;
    while (current != NULL) {
        fwrite(current, sizeof(Patient) - sizeof(Patient *), 1, patientFile);
        current = current->next;
    }
    fclose(patientFile);

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/doctors_%s.dat", timestamp);

    // Save doctors
    FILE *doctorFile = fopen(reportFileName, "wb");
    if (doctorFile == NULL) {
        printf("Error: Unable to open doctors backup file for writing.\n");
        return 0;
    }

    // Write total doctors first
    fwrite(&totalDoctors, sizeof(int), 1, doctorFile);

    // Write each doctor
    Doctor *currentDoc = doctorHead;
    while (currentDoc != NULL) {
        fwrite(currentDoc, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);
        currentDoc = currentDoc->next;
    }
    fclose(doctorFile);

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/schedule_%s.dat", timestamp);
    // Save schedule
    FILE *scheduleFile = fopen(reportFileName, "wb");
    if (scheduleFile == NULL) {
        printf("Error: Unable to open schedule backup file for writing.\n");
        return 0;
    }

    fwrite(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data back up successfully.\n");
    return 1;
}

// Restore data from a backup timestamp
int restoreData(const char* timestamp) {
    char backupFileName[MAX_FILENAME_LENGTH];
    char dataFileName[MAX_FILENAME_LENGTH];
    FILE *backupFile, *dataFile;
    unsigned char buffer[4096];
    size_t bytesRead;
    int success = 1;

    printf("Starting data restoration from timestamp: %s\n", timestamp);

    // First, ensure the data directory exists
    #ifdef _WIN32
    system("mkdir ..\\data 2>NUL");
    #else
    system("mkdir -p ../data");
    #endif

    // Step 1: Restore patients data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/patients_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/patients.dat");

    printf("Restoring patients data from: %s\n", backupFileName);

    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Error: Cannot open backup file %s\n", backupFileName);
        return 0;
    }

    dataFile = fopen(dataFileName, "wb");
    if (dataFile == NULL) {
        printf("Error: Unable to create data file %s\n", dataFileName);
        fclose(backupFile);
        return 0;
    }

    // Use buffer-based copying for better performance and reliability
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), backupFile)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dataFile) != bytesRead) {
            printf("Error writing to patients data file\n");
            success = 0;
            break;
        }
    }

    fclose(backupFile);
    fclose(dataFile);

    if (!success) {
        printf("Failed to restore patients data\n");
        return 0;
    }

    printf("Patients data restored successfully\n");

    // Step 2: Restore doctors data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/doctors_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/doctors.dat");

    printf("Restoring doctors data from: %s\n", backupFileName);

    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Warning: Cannot open doctors backup file %s\n", backupFileName);
        // Continue restoration process even if this file is missing
    } else {
        dataFile = fopen(dataFileName, "wb");
        if (dataFile == NULL) {
            printf("Error: Unable to create doctors data file\n");
            fclose(backupFile);
            success = 0;
        } else {
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), backupFile)) > 0) {
                if (fwrite(buffer, 1, bytesRead, dataFile) != bytesRead) {
                    printf("Error writing to doctors data file\n");
                    success = 0;
                    break;
                }
            }
            fclose(dataFile);
        }
        fclose(backupFile);
    }

    // Step 3: Restore schedule data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/schedule_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/schedule.dat");

    printf("Restoring schedule data from: %s\n", backupFileName);

    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Warning: Cannot open schedule backup file %s\n", backupFileName);
        // Continue restoration process even if this file is missing
    } else {
        dataFile = fopen(dataFileName, "wb");
        if (dataFile == NULL) {
            printf("Error: Unable to create schedule data file\n");
            fclose(backupFile);
            success = 0;
        } else {
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), backupFile)) > 0) {
                if (fwrite(buffer, 1, bytesRead, dataFile) != bytesRead) {
                    printf("Error writing to schedule data file\n");
                    success = 0;
                    break;
                }
            }
            fclose(dataFile);
        }
        fclose(backupFile);
    }

    printf("All backup files processed. Reloading data...\n");

    // If the restoration failed, don't try to load the data
    if (!success) {
        printf("Warning: Errors occurred during restoration. Data may be incomplete.\n");
        return 0;
    }

    // Step 4: Reload the restored data
    cleanupSystem();  // Clean up current data
    printf("System cleaned up\n");

    initializeSystem(); // Reinitialize
    printf("System reinitialized\n");

    // Try loading the data with better error handling
    int loadResult = safeLoadData();  // Use our new safer load function
    printf("Data load result: %s\n", loadResult ? "Success" : "Failed");

    if (loadResult) {
        printf("Data restored successfully from backup: %s\n", timestamp);
        return 1;
    } else {
        printf("Warning: Restored data files, but had issues loading them.\n");
        return 0;
    }
}

int safeLoadData() {
    printf("Starting safe data loading...\n");

    // Reset global counters
    totalPatients = 0;
    totalDoctors = 0;

    // Make sure our global pointers are NULL
    patientHead = NULL;
    doctorHead = NULL;

    // Load patients
    FILE *patientFile = fopen("../data/patients.dat", "rb");
    if (patientFile == NULL) {
        printf("No existing patient data found. Starting with empty records.\n");
        return 0;
    }

    // Read total patients first
    int readPatients = 0;
    if (fread(&readPatients, sizeof(int), 1, patientFile) != 1) {
        printf("Error reading patient count from file.\n");
        fclose(patientFile);
        return 0;
    }

    printf("Found %d patients in data file.\n", readPatients);

    if (readPatients <= 0 || readPatients > 1000) {  // Sanity check
        printf("Invalid patient count: %d\n", readPatients);
        fclose(patientFile);
        return 0;
    }

    // Read each patient
    Patient tempPatient;
    for (int i = 0; i < readPatients; i++) {
        if (fread(&tempPatient, sizeof(Patient) - sizeof(Patient *), 1, patientFile) != 1) {
            printf("Error reading patient %d data from file.\n", i+1);
            fclose(patientFile);
            return 0;
        }

        // Basic sanity checks on data
        if (tempPatient.patientID <= 0 || tempPatient.patientAge < 0 || tempPatient.patientAge > 150) {
            printf("Invalid patient data for ID: %d, Age: %d\n",
                   tempPatient.patientID, tempPatient.patientAge);
            continue;  // Skip this invalid record
        }

        // Create a new patient node and add to list
        Patient *newPatient = createPatient(
            tempPatient.patientID,
            tempPatient.patientName,
            tempPatient.patientAge,
            tempPatient.patientDiagnosis,
            tempPatient.patientRoomNum
        );

        if (newPatient == NULL) {
            printf("Failed to create patient record for ID: %d\n", tempPatient.patientID);
            continue;  // Skip if memory allocation failed
        }

        // Copy admission date and other fields
        strncpy(newPatient->admissionDate, tempPatient.admissionDate, sizeof(newPatient->admissionDate));
        strncpy(newPatient->dischargeDate, tempPatient.dischargeDate, sizeof(newPatient->dischargeDate));
        newPatient->isActive = tempPatient.isActive;

        // Add to linked list safely
        if (patientHead == NULL) {
            patientHead = newPatient;
        } else {
            Patient *current = patientHead;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newPatient;
        }

        totalPatients++;  // Increment only when successfully added
        printf("Loaded patient ID: %d\n", newPatient->patientID);
    }
    fclose(patientFile);

    printf("Successfully loaded %d patients.\n", totalPatients);

    // Load doctors with similar safety checks
    FILE *doctorFile = fopen("../data/doctors.dat", "rb");
    if (doctorFile == NULL) {
        printf("No existing doctor data found. Starting with empty records.\n");
        // We can continue even without doctor data
    } else {
        // Similar careful loading for doctors...
        int readDoctors = 0;
        if (fread(&readDoctors, sizeof(int), 1, doctorFile) != 1) {
            printf("Error reading doctor count from file.\n");
            fclose(doctorFile);
            // Continue with what we have
        } else if (readDoctors > 0 && readDoctors <= 1000) {  // Sanity check
            printf("Found %d doctors in data file.\n", readDoctors);

            // Read each doctor
            Doctor tempDoctor;
            for (int i = 0; i < readDoctors; i++) {
                if (fread(&tempDoctor, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile) != 1) {
                    printf("Error reading doctor %d data from file.\n", i+1);
                    break;  // Stop but use what we've loaded so far
                }

                // Basic sanity checks
                if (tempDoctor.doctorID <= 0) {
                    printf("Invalid doctor data for ID: %d\n", tempDoctor.doctorID);
                    continue;  // Skip this invalid record
                }

                // Create a new doctor node and add to list
                Doctor *newDoctor = createDoctor(tempDoctor.doctorID, tempDoctor.doctorName);
                if (newDoctor == NULL) {
                    printf("Failed to create doctor record for ID: %d\n", tempDoctor.doctorID);
                    continue;  // Skip if memory allocation failed
                }

                newDoctor->totalShifts = tempDoctor.totalShifts;

                // Add to linked list safely
                if (doctorHead == NULL) {
                    doctorHead = newDoctor;
                } else {
                    Doctor *current = doctorHead;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newDoctor;
                }

                totalDoctors++;  // Increment only when successfully added
                printf("Loaded doctor ID: %d\n", newDoctor->doctorID);
            }
            printf("Successfully loaded %d doctors.\n", totalDoctors);
        }
        fclose(doctorFile);
    }

    // Load schedule
    FILE *scheduleFile = fopen("../data/schedule.dat", "rb");
    if (scheduleFile == NULL) {
        printf("No existing schedule data found. Starting with empty schedule.\n");
        // Initialize schedule to all zeros
        for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
            for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
                doctorSchedule[i][j] = 0;
            }
        }
    } else {
        // Read schedule data
        size_t readResult = fread(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
        if (readResult != 1) {
            printf("Warning: Could not read complete schedule data.\n");
            // Initialize schedule to all zeros as a fallback
            for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
                for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
                    doctorSchedule[i][j] = 0;
                }
            }
        } else {
            printf("Successfully loaded schedule data.\n");
        }
        fclose(scheduleFile);
    }

    return (totalPatients > 0 || totalDoctors > 0);
}

// Simple and robust backup selection function
char* selectBackup() {
    static char selectedTimestamp[30] = {0};
    int backupCount = 0;
    char backupFiles[20][30]; // Store up to 20 backup timestamps

    printf("\nChecking for available backups...\n");

    // First, ensure the backups directory exists
    #ifdef _WIN32
    system("mkdir ..\\backups 2>NUL");
    #else
    system("mkdir -p ../backups");
    #endif

    // List all backup files directly (simpler approach)
    printf("\nAvailable backups:\n");
    printf("----------------\n");

    // Create a temporary file to store the list of backups
    #ifdef _WIN32
    system("dir /b ..\\backups\\patients_*.dat > temp_backups.txt 2>NUL");
    #else
    system("ls -1 ../backups/patients_*.dat > temp_backups.txt 2>/dev/null");
    #endif

    FILE* fileList = fopen("temp_backups.txt", "r");
    if (fileList == NULL) {
        printf("No backups found or cannot access backup directory.\n");
        return NULL;
    }

    // Read each line from the file list
    char line[MAX_FILENAME_LENGTH];
    while (fgets(line, sizeof(line), fileList) && backupCount < 20) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        // Extract just the filename (in case of full path)
        char* filename = line;
        char* lastSlash = strrchr(line, '/');
        if (lastSlash != NULL) {
            filename = lastSlash + 1;
        } else {
            lastSlash = strrchr(line, '\\');
            if (lastSlash != NULL) {
                filename = lastSlash + 1;
            }
        }

        // Extract timestamp from filename
        if (strncmp(filename, "patients_", 9) == 0) {
            char* timestamp = filename + 9;
            char* extension = strstr(timestamp, ".dat");
            if (extension != NULL) {
                *extension = '\0'; // Remove extension

                // Store the timestamp
                strncpy(backupFiles[backupCount], timestamp, sizeof(backupFiles[0]) - 1);
                backupFiles[backupCount][sizeof(backupFiles[0]) - 1] = '\0'; // Ensure null termination

                // Display with index
                printf("%d. %s\n", backupCount + 1, timestamp);
                backupCount++;
            }
        }
    }

    fclose(fileList);
    remove("temp_backups.txt");

    if (backupCount == 0) {
        printf("No valid backup files found.\n");
        return NULL;
    }

    // Let user select a backup
    int choice = 0;
    do {
        printf("\nEnter the number of the backup to restore (0 to cancel): ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 0) {
            printf("Operation cancelled.\n");
            return NULL;
        }

        if (choice < 1 || choice > backupCount) {
            printf("Invalid selection. Please try again.\n");
        }
    } while (choice < 1 || choice > backupCount);

    // Return the selected timestamp
    strcpy(selectedTimestamp, backupFiles[choice - 1]);
    printf("Selected backup from: %s\n", selectedTimestamp);

    return selectedTimestamp;
}

// Get current date and time as string
void getCurrentDateTime(char *dateTime, int bufferSize) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(dateTime, bufferSize, "%Y-%m-%d %H:%M:%S", t);
}

// Add a patient to the system
void addPatient() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Add Patient");

    int patientID;
    char patientName[50];
    int patientAge;
    char patientDiag[250];
    int patientRoomNum;

    printf("\nEnter the patient ID (positive number): ");
    patientID = scanInt();

    if (patientID <= 0) {
        printf("The patient ID must be a positive number!\n");
        returnToMenu();
        return;
    }

    // Check if ID exists
    if (findPatientByID(patientID) != NULL) {
        printf("The patient ID already exists!\n");
        returnToMenu();
        return;
    }

    printf("Enter the patient name: ");
    fgets(patientName, sizeof(patientName), stdin);
    patientName[strcspn(patientName, "\n")] = 0; // Remove newline character

    printf("Enter the patient age: ");
    patientAge = scanInt();

    if (patientAge < 0 || patientAge > 130) {
        printf("The patient age is invalid!\n");
        returnToMenu();
        return;
    }

    printf("Enter the patient diagnosis: ");
    fgets(patientDiag, sizeof(patientDiag), stdin);
    patientDiag[strcspn(patientDiag, "\n")] = 0; // Remove newline character

    printf("Enter the patient room number to assign (positive number): ");
    patientRoomNum = scanInt();

    if (patientRoomNum <= 0 || !isRoomAvailable(patientRoomNum)) {
        printf("Room number invalid or room is full!\n");
        returnToMenu();
        return;
    }

    // Create new patient
    Patient *newPatient = createPatient(patientID, patientName, patientAge, patientDiag, patientRoomNum);
    if (newPatient == NULL) {
        printf("Failed to create patient record!\n");
        returnToMenu();
        return;
    }

    // Add to linked list
    if (patientHead == NULL) {
        patientHead = newPatient;
    } else {
        Patient *current = patientHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPatient;
    }

    totalPatients++;
    printf("Patient record added successfully!\n");

    // Auto-save after adding patient
    saveData();

    returnToMenu();
}

// View all patients
void viewPatients() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("View All Patients");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        returnToMenu();
        return;
    }

    printf("%-10s%-25s%-10s%-30s%-15s%-30s%-10s\n",
           "ID", "Name", "Age", "Diagnosis", "Room Number", "Admission Date", "Status");
    printf(
        "-------------------------------------------------------------------------------------------------------------------------------\n");

    Patient *current = patientHead;
    while (current != NULL) {

        if (current -> isActive) {
            printf("%-10d%-25s%-10d%-30s%-15d%-30s%-10s\n",
               current->patientID,
               current->patientName,
               current->patientAge,
               current->patientDiagnosis,
               current->patientRoomNum,
               current->admissionDate,
               "Active");
        }
        current = current->next;
    }

    returnToMenu();
}

// Search for a patient by ID
void searchPatient() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Search Patient");

    int patientID;
    printf("Enter the patient ID: ");
    patientID = scanInt();

    Patient *patient = findPatientByID(patientID);

    if (patient == NULL) {
        printf("The patient is not found!\n");
    } else {
        printf("\nPatient Details:\n");
        printf("%-10s%-25s%-10s%-30s%-15s%-20s\n", "ID", "Name", "Age", "Diagnosis", "Room Number", "Admission Date");
        printf(
            "--------------------------------------------------------------------------------------------------------\n");
        printf("%-10d%-25s%-10d%-30s%-15d%-20s\n",
               patient->patientID,
               patient->patientName,
               patient->patientAge,
               patient->patientDiagnosis,
               patient->patientRoomNum,
               patient->admissionDate);
    }

    returnToMenu();
}

// Discharge a patient
void dischargePatient() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Discharge Patient");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        returnToMenu();
        return;
    }

    int patientID;
    printf("Enter the patient ID to discharge: ");
    patientID = scanInt();

    Patient *patient = findPatientByID(patientID);

    if (patient == NULL) {
        printf("The patient is not found!\n");
        returnToMenu();
        return;
    }

    if (patient->isActive == 0) {
        printf("This patient has already been discharged!\n");
        returnToMenu();
        return;
    }

    // Record discharge date
    getCurrentDateTime(patient->dischargeDate, sizeof(patient->dischargeDate));
    patient->isActive = 0;

    // Update room availability by setting room to 0
    patient->patientRoomNum = 0;

    printf("Patient discharged successfully!\n");
    saveData(); // Auto-save after discharge
    returnToMenu();
}

// Find a patient by ID
Patient *findPatientByID(int id) {
    Patient *current = patientHead;
    while (current != NULL) {
        if (current->patientID == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Check if a room is available (max 2 patients per room)
int isRoomAvailable(int roomNum) {
    int count = 0;
    Patient *current = patientHead;

    while (current != NULL) {
        if (current->patientRoomNum == roomNum) {
            count++;
            if (count >= 2) {
                return 0; // Room is full
            }
        }
        current = current->next;
    }

    return 1; // Room is available
}

// Add a doctor
void addDoctor() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Add Doctor");

    int doctorID;
    char doctorName[50];

    printf("\nEnter Doctor ID (positive number): ");
    doctorID = scanInt();

    if (doctorID <= 0) {
        printf("The doctor ID must be a positive number!\n");
        returnToMenu();
        return;
    }

    // Check if ID exists
    if (findDoctorByID(doctorID) != NULL) {
        printf("The doctor ID already exists!\n");
        returnToMenu();
        return;
    }

    printf("Enter the doctor name: ");
    fgets(doctorName, sizeof(doctorName), stdin);
    doctorName[strcspn(doctorName, "\n")] = 0; // Remove newline character

    // Create new doctor
    Doctor *newDoctor = createDoctor(doctorID, doctorName);
    if (newDoctor == NULL) {
        printf("Failed to create doctor record!\n");
        returnToMenu();
        return;
    }

    // Add to linked list
    if (doctorHead == NULL) {
        doctorHead = newDoctor;
    } else {
        Doctor *current = doctorHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newDoctor;
    }

    totalDoctors++;
    printf("Doctor record added successfully!\n");

    // Auto-save after adding doctor
    saveData();

    returnToMenu();
}

// View all doctors
void viewDoctors() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("View All Doctors");

    if (totalDoctors == 0) {
        printf("No doctors in the system.\n");
        returnToMenu();
        return;
    }

    printf("%-10s%-25s%-15s\n", "ID", "Name", "Total Shifts");
    printf("---------------------------------------------------\n");

    Doctor *current = doctorHead;
    while (current != NULL) {
        printf("%-10d%-25s%-15d\n",
               current->doctorID,
               current->doctorName,
               current->totalShifts);
        current = current->next;
    }

    returnToMenu();
}

// Manage doctor schedule
void manageDoctorSchedule() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Manage Doctor Schedule");

    if (totalDoctors == 0) {
        printf("No doctors in the system. Please add doctors first.\n");
        returnToMenu();
        return;
    }

    int doctorID, dayInWeek, shiftInDay;

    printf("\nEnter Doctor ID to assign shifts: ");
    doctorID = scanInt();

    Doctor *doctor = findDoctorByID(doctorID);

    if (doctor == NULL) {
        printf("The doctor ID is invalid or doesn't exist!\n");
        returnToMenu();
        return;
    }

    if (doctor->totalShifts >= 7) {
        printf("This doctor has reached the maximum number of shifts this week!\n");
        returnToMenu();
        return;
    }

    printf("Enter day to assign shift (1-7): ");
    dayInWeek = scanInt();

    if (dayInWeek < 1 || dayInWeek > 7) {
        printf("The day must be between 1 and 7!\n");
        returnToMenu();
        return;
    }

    printf("Enter shift to assign (1-morning, 2-afternoon, 3-evening): ");
    shiftInDay = scanInt();

    if (shiftInDay < 1 || shiftInDay > 3) {
        printf("Invalid shift! Must be between 1 and 3.\n");
        returnToMenu();
        return;
    }

    // Check if the shift is already assigned
    if (doctorSchedule[dayInWeek - 1][shiftInDay - 1] != 0) {
        printf("This shift is already assigned to another doctor!\n");
        returnToMenu();
        return;
    }

    // Find the doctor's position in the linked list (1-based index)
    int doctorIndex = 1;
    Doctor *current = doctorHead;
    while (current != NULL && current->doctorID != doctorID) {
        doctorIndex++;
        current = current->next;
    }

    doctorSchedule[dayInWeek - 1][shiftInDay - 1] = doctorIndex;
    doctor->totalShifts++;

    printf("Shift assigned successfully!\n");
    saveData(); // Auto-save after schedule update

    returnToMenu();
}

// View doctor schedule
void viewSchedule() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Doctor Schedule");

    printf("\nWeekly Schedule:\n");
    printf("-------------------------------------------------------------------------\n");
    printf("Day\t\t| Morning\t| Afternoon\t| Evening\n");
    printf("-------------------------------------------------------------------------\n");

    char *dayNames[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
        printf("%-10s\t|", dayNames[i]);

        for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
            int doctorIndex = doctorSchedule[i][j];
            if (doctorIndex == 0) {
                printf(" Not Assigned\t|");
            } else {
                // Find doctor by index
                Doctor *current = doctorHead;
                int currentIndex = 1;

                while (current != NULL && currentIndex < doctorIndex) {
                    current = current->next;
                    currentIndex++;
                }

                if (current != NULL) {
                    printf(" Dr. %s\t|", current->doctorName);
                } else {
                    printf(" Unknown\t|");
                }
            }
        }
        printf("\n");
    }

    returnToMenu();
}

// Find a doctor by ID
Doctor *findDoctorByID(int id) {
    Doctor *current = doctorHead;
    while (current != NULL) {
        if (current->doctorID == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Generate reports menu
void generateReports() {
    int choice;

    do {
        printf("\e[1;1H\e[2J"); // Clear screen
        printHeader("Generate Reports");

        printf("1. Patient Admission Report\n");
        printf("2. Doctor Utilization Report\n");
        printf("3. Room Utilization Report\n");
        printf("4. Return to Main Menu\n");
        printf("Enter your choice: ");

        choice = scanInt();

        switch (choice) {
            case 1: patientAdmissionReport();
                break;
            case 2: doctorUtilizationReport();
                break;
            case 3: roomUtilizationReport();
                break;
            case 4: break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 4);
}

// Generate patient admission report
void patientAdmissionReport() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Patient Admission Report");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for a valid filename
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "patient_admission_report_%s.txt", timestamp);

    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    fprintf(reportFile, "PATIENT ADMISSION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Patients: %d\n\n", totalPatients);
    fprintf(reportFile, "%-10s%-25s%-10s%-30s%-15s%-25s%-10s\n",
            "ID", "Name", "Age", "Diagnosis", "Room Number", "Admission Date", "Status");
    fprintf(reportFile,
            "-------------------------------------------------------------------------------------------------------------------------\n");

    Patient *current = patientHead;
    while (current != NULL) {
        fprintf(reportFile, "%-10d%-25s%-10d%-30s%-15d%-25s%-10s\n",
                current->patientID,
                current->patientName,
                current->patientAge,
                current->patientDiagnosis,
                current->patientRoomNum,
                current->admissionDate,
                current->isActive ? "Active" : "Discharged");
        current = current->next;
    }

    fclose(reportFile);

    printf("Report generated successfully: %s\n", reportFileName);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

// Generate doctor utilization report
void doctorUtilizationReport() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Doctor Utilization Report");

    if (totalDoctors == 0) {
        printf("No doctors in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for a valid filename
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "doctor_utilization_report_%s.txt", timestamp);

    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    fprintf(reportFile, "DOCTOR UTILIZATION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Doctors: %d\n\n", totalDoctors);
    fprintf(reportFile, "%-10s%-25s%-15s%-15s\n",
            "ID", "Name", "Total Shifts", "Utilization %");
    fprintf(reportFile, "--------------------------------------------------------------------\n");

    Doctor *current = doctorHead;
    while (current != NULL) {
        float utilization = (float) current->totalShifts / (MAX_DAYS_IN_WEEK * MAX_SHIFTS_IN_DAY) * 100;
        fprintf(reportFile, "%-10d%-25s%-15d%-15.2f\n",
                current->doctorID,
                current->doctorName,
                current->totalShifts,
                utilization);
        current = current->next;
    }

    fclose(reportFile);

    printf("Report generated successfully: %s\n", reportFileName);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

// Generate room utilization report
void roomUtilizationReport() {
    printf("\e[1;1H\e[2J"); // Clear screen
    printHeader("Room Utilization Report");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Count patients per room
    int roomCounts[100] = {0}; // Assuming max room number is 100
    int maxRoom = 0;

    Patient *current = patientHead;
    while (current != NULL) {
        roomCounts[current->patientRoomNum]++;
        if (current->patientRoomNum > maxRoom) {
            maxRoom = current->patientRoomNum;
        }
        current = current->next;
    }

    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for a valid filename
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "room_utilization_report_%s.txt", timestamp);

    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    fprintf(reportFile, "ROOM UTILIZATION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Patients: %d\n\n", totalPatients);
    fprintf(reportFile, "%-15s%-15s%-15s\n",
            "Room Number", "Patients", "Occupancy %");
    fprintf(reportFile, "------------------------------------------\n");

    for (int i = 1; i <= maxRoom; i++) {
        if (roomCounts[i] > 0) {
            float occupancy = (float) roomCounts[i] / 2 * 100; // 2 patients max per room
            fprintf(reportFile, "%-15d%-15d%-15.2f\n",
                    i,
                    roomCounts[i],
                    occupancy);
        }
    }

    fclose(reportFile);

    printf("Report generated successfully: %s\n", reportFileName);
    printf("Press Enter to continue...");
    clearInputBuffer();
}

// Main menu function
void menu() {
    int choice;

    do {
        printf("\e[1;1H\e[2J"); // Clear screen
        printHeader("Hospital Management System");

        printf("1. Add Patient Record\n");
        printf("2. View All Patients\n");
        printf("3. Search Patient\n");
        printf("4. Discharge Patient\n");
        printf("5. Manage Doctor Schedule\n");
        printf("6. View Doctor Schedule\n");
        printf("7. Add Doctor Record\n");
        printf("8. View All Doctors\n");
        printf("9. Generate Reports\n");
        printf("10. Restore Data\n");
        printf("11. Exit\n");
        printf("Enter your choice: ");

        choice = scanInt();

        switch (choice) {
            case 1: addPatient();
                break;
            case 2: viewPatients();
                break;
            case 3: searchPatient();
                break;
            case 4: dischargePatient();
                break;
            case 5: manageDoctorSchedule();
                break;
            case 6: viewSchedule();
                break;
            case 7: addDoctor();
                break;
            case 8: viewDoctors();
                break;
            case 9: generateReports();
                break;
            case 10: {
                printf("\e[1;1H\e[2J");  // Clear screen
                printHeader("Restore Data from Backup");

                char* timestamp = selectBackup();
                if (timestamp != NULL) {
                    printf("Starting restoration from backup: %s\n", timestamp);
                    if (restoreData(timestamp)) {
                        printf("Restoration completed successfully.\n");
                    } else {
                        printf("Restoration failed.\n");
                    }
                } else {
                    printf("Restore operation cancelled or no backups available.\n");
                }

                returnToMenu();
                break;
            }
            case 11: printf("Saving data and exiting...\n");
                break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 11);
}

// Clear the input buffer
void clearInputBuffer() {
    while (getchar() != '\n') {
    }
}

// Return to menu function
void returnToMenu() {
    printf("\nPress Enter to return to the menu...");
    clearInputBuffer();
}

// Safely scan an integer
int scanInt() {
    int num;
    char term;

    if (scanf("%d%c", &num, &term) != 2 || term != '\n') {
        clearInputBuffer();
        return -1;
    }

    return num;
}

// Print a header with title
void printHeader(const char *title) {
    int len = strlen(title);
    printf("\n");
    for (int i = 0; i < len + 4; i++) printf("=");
    printf("\n= %s =\n", title);
    for (int i = 0; i < len + 4; i++) printf("=");
    printf("\n\n");
}

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

/* Constants for the system */
#define INITIAL_CAPACITY 10     // Initial capacity for data structures
#define MAX_DAYS_IN_WEEK 7      // Number of days in a week
#define MAX_SHIFTS_IN_DAY 3     // Number of shifts per day (morning, afternoon, evening)
#define MAX_FILENAME_LENGTH 100 // Maximum length for filenames

/* Patient structure to store patient information */
typedef struct Patient {
    int patientID;                  // Unique ID for each patient
    char patientName[50];           // Patient's full name
    int patientAge;                 // Patient's age
    char patientDiagnosis[250];     // Medical diagnosis details
    int patientRoomNum;             // Room number assigned to patient
    char admissionDate[20];         // Date and time of admission
    char dischargeDate[20];         // Date and time of discharge (if applicable)
    int isActive;                   // Flag to indicate if patient is currently admitted (1) or discharged (0)
    struct Patient *next;           // Pointer to next patient in linked list
} Patient;

/* Doctor structure to store doctor information */
typedef struct Doctor {
    int doctorID;                   // Unique ID for each doctor
    char doctorName[50];            // Doctor's full name
    int totalShifts;                // Count of total shifts assigned to the doctor
    struct Doctor *next;            // Pointer to next doctor in linked list
} Doctor;

/* Global variables */
Patient *patientHead = NULL;                                // Head of patient linked list
Doctor *doctorHead = NULL;                                  // Head of doctor linked list
int totalPatients = 0;                                      // Total number of patients in the system
int totalDoctors = 0;                                       // Total number of doctors in the system
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY];    // 2D array to store weekly doctor schedule

/* Function prototypes */
void initializeSystem();
void cleanupSystem();
Patient *createPatient(int id, const char *name, int age, const char *diagnosis, int roomNum);
Doctor *createDoctor(int id, const char *name);
int saveData();
int loadData();
int backupData();
int restoreData();
char *selectBackup();
void getCurrentDateTime(char *dateTime, int bufferSize);
int safeLoadData();
void addPatient();
void viewPatients();
void searchPatient();
void dischargePatient();
Patient *findPatientByID(int id);
int isRoomAvailable(int roomNum);
void addDoctor();
void viewDoctors();
void manageDoctorSchedule();
void assignShift(int doctorID, int dayInWeek, int shiftInDay);
void viewSchedule();
Doctor *findDoctorByID(int id);
void generateReports();
void patientAdmissionReport();
void doctorUtilizationReport();
void roomUtilizationReport();
void menu();
void clearInputBuffer();
void returnToMenu();
int scanInt();
void printHeader(const char *title);

int main() {
    initializeSystem();    // Initialize system variables and data structures
    loadData();            // Load existing data from files
    menu();                // Display and handle the main menu
    saveData();            // Save data before exiting
    cleanupSystem();       // Free allocated memory
    return 0;
}

//Initialize the system. Currently initializes the doctor schedule array to zeros
void initializeSystem() {
    // Initialize all slots in doctor schedule to 0 (unassigned)
    for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
        for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
            doctorSchedule[i][j] = 0;
        }
    }
}

//Clean up the system. Frees all dynamically allocated memory for patients and doctors
void cleanupSystem() {
    // Free memory allocated for patients
    Patient *currentPatient = patientHead;
    Patient *nextPatient;

    while (currentPatient != NULL) {
        nextPatient = currentPatient->next;
        free(currentPatient);
        currentPatient = nextPatient;
    }

    // Free memory allocated for doctors
    Doctor *currentDoctor = doctorHead;
    Doctor *nextDoctor;

    while (currentDoctor != NULL) {
        nextDoctor = currentDoctor->next;
        free(currentDoctor);
        currentDoctor = nextDoctor;
    }
}

//Create a new patient record
Patient *createPatient(int id, const char *name, int age, const char *diagnosis, int roomNum) {
    // Allocate memory for a new patient
    Patient *newPatient = (Patient *) malloc(sizeof(Patient));
    if (newPatient == NULL) {
        printf("Error: Memory allocation failed for patient record.\n");
        return NULL;
    }

    // Initialize the patient fields
    newPatient->patientID = id;

    // Copy name with safety checks to prevent buffer overflow
    strncpy(newPatient->patientName, name, sizeof(newPatient->patientName) - 1);
    newPatient->patientName[sizeof(newPatient->patientName) - 1] = '\0';

    newPatient->patientAge = age;

    // Copy diagnosis with safety checks to prevent buffer overflow
    strncpy(newPatient->patientDiagnosis, diagnosis, sizeof(newPatient->patientDiagnosis) - 1);
    newPatient->patientDiagnosis[sizeof(newPatient->patientDiagnosis) - 1] = '\0';

    newPatient->patientRoomNum = roomNum;

    // Get and set the current date/time as admission date
    getCurrentDateTime(newPatient->admissionDate, sizeof(newPatient->admissionDate));

    // Initialize discharge date to empty string and set status as active
    newPatient->dischargeDate[0] = '\0';
    newPatient->isActive = 1;

    // Initialize next pointer to NULL since this is a new patient
    newPatient->next = NULL;
    return newPatient;
}

//Create a new docotor record
Doctor *createDoctor(int id, const char *name) {
    // Allocate memory for a new doctor
    Doctor *newDoctor = (Doctor *) malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        printf("Error: Memory allocation failed for doctor record.\n");
        return NULL;
    }

    // Initialize the doctor fields
    newDoctor->doctorID = id;

    // Copy name with safety checks to prevent buffer overflow
    strncpy(newDoctor->doctorName, name, sizeof(newDoctor->doctorName) - 1);
    newDoctor->doctorName[sizeof(newDoctor->doctorName) - 1] = '\0';

    newDoctor->totalShifts = 0;  // Initialize total shifts to 0
    newDoctor->next = NULL;      // Initialize next pointer to NULL
    return newDoctor;
}

//Save all data to files. Saves patients, doctors, and schedule data to their respective files
int saveData() {
    // Save patient data
    FILE *patientFile = fopen("../data/patients.dat", "wb");
    if (patientFile == NULL) {
        printf("Error: Unable to open patients.dat for writing.\n");
        return 0;
    }

    // Write total number of patients
    fwrite(&totalPatients, sizeof(int), 1, patientFile);

    // Write each patient's data (excluding the next pointer)
    Patient *current = patientHead;
    while (current != NULL) {
        fwrite(current, sizeof(Patient) - sizeof(Patient *), 1, patientFile);
        current = current->next;
    }
    fclose(patientFile);

    // Save doctor data
    FILE *doctorFile = fopen("../data/doctors.dat", "wb");
    if (doctorFile == NULL) {
        printf("Error: Unable to open doctors.dat for writing.\n");
        return 0;
    }

    // Write total number of doctors
    fwrite(&totalDoctors, sizeof(int), 1, doctorFile);

    // Write each doctor's data (excluding the next pointer)
    Doctor *currentDoc = doctorHead;
    while (currentDoc != NULL) {
        fwrite(currentDoc, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);
        currentDoc = currentDoc->next;
    }
    fclose(doctorFile);

    // Save schedule data
    FILE *scheduleFile = fopen("../data/schedule.dat", "wb");
    if (scheduleFile == NULL) {
        printf("Error: Unable to open schedule.dat for writing.\n");
        return 0;
    }

    // Write the entire schedule array
    fwrite(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data saved successfully.\n");

    // Create a backup of the current data
    backupData();
    return 1;
}

//Load data from files. Loads patients, doctors, and schedule data from their respective files

int loadData() {
    // Load patient data
    FILE *patientFile = fopen("../data/patients.dat", "rb");
    if (patientFile == NULL) {
        printf("No existing patient data found. Starting with empty records.\n");
        return 0;
    }

    // Read total number of patients
    fread(&totalPatients, sizeof(int), 1, patientFile);

    // Read and recreate each patient record
    Patient tempPatient;
    for (int i = 0; i < totalPatients; i++) {
        fread(&tempPatient, sizeof(Patient) - sizeof(Patient *), 1, patientFile);

        // Create a new patient with the basic information
        Patient *newPatient = createPatient(
            tempPatient.patientID,
            tempPatient.patientName,
            tempPatient.patientAge,
            tempPatient.patientDiagnosis,
            tempPatient.patientRoomNum
        );

        // Copy the admission date from the loaded data
        strncpy(newPatient->admissionDate, tempPatient.admissionDate, sizeof(newPatient->admissionDate));

        // Copy the discharge date and active status
        strncpy(newPatient->dischargeDate, tempPatient.dischargeDate, sizeof(newPatient->dischargeDate));
        newPatient->isActive = tempPatient.isActive;

        // Add the patient to the linked list
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

    // Load doctor data
    FILE *doctorFile = fopen("../data/doctors.dat", "rb");
    if (doctorFile == NULL) {
        printf("No existing doctor data found. Starting with empty records.\n");
        return 0;
    }

    // Read total number of doctors
    fread(&totalDoctors, sizeof(int), 1, doctorFile);

    // Read and recreate each doctor record
    Doctor tempDoctor;
    for (int i = 0; i < totalDoctors; i++) {
        fread(&tempDoctor, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);

        // Create a new doctor with the basic information
        Doctor *newDoctor = createDoctor(tempDoctor.doctorID, tempDoctor.doctorName);
        newDoctor->totalShifts = tempDoctor.totalShifts;

        // Add the doctor to the linked list
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

    // Load schedule data
    FILE *scheduleFile = fopen("../data/schedule.dat", "rb");
    if (scheduleFile == NULL) {
        printf("No existing schedule data found. Starting with empty schedule.\n");
        return 0;
    }

    // Read the entire schedule array
    fread(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data loaded successfully.\n");
    return 1;
}

//Back up all data to timestamped files. Creates backup files for patients, doctors, and schedule data
int backupData() {
    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];

    // Get current timestamp for backup filenames
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for filename compatibility
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    // Back up patient data
    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/patients_%s.dat", timestamp);

    FILE *patientFile = fopen(reportFileName, "wb");
    if (patientFile == NULL) {
        printf("Error: Unable to open patients backup file for writing.\n");
        return 0;
    }

    // Write total number of patients
    fwrite(&totalPatients, sizeof(int), 1, patientFile);

    // Write each patient's data (excluding the next pointer)
    Patient *current = patientHead;
    while (current != NULL) {
        fwrite(current, sizeof(Patient) - sizeof(Patient *), 1, patientFile);
        current = current->next;
    }
    fclose(patientFile);

    // Back up doctor data
    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/doctors_%s.dat", timestamp);

    FILE *doctorFile = fopen(reportFileName, "wb");
    if (doctorFile == NULL) {
        printf("Error: Unable to open doctors backup file for writing.\n");
        return 0;
    }

    // Write total number of doctors
    fwrite(&totalDoctors, sizeof(int), 1, doctorFile);

    // Write each doctor's data (excluding the next pointer)
    Doctor *currentDoc = doctorHead;
    while (currentDoc != NULL) {
        fwrite(currentDoc, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile);
        currentDoc = currentDoc->next;
    }
    fclose(doctorFile);

    // Back up schedule data
    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../backups/schedule_%s.dat", timestamp);
    FILE *scheduleFile = fopen(reportFileName, "wb");
    if (scheduleFile == NULL) {
        printf("Error: Unable to open schedule backup file for writing.\n");
        return 0;
    }

    // Write the entire schedule array
    fwrite(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
    fclose(scheduleFile);

    printf("Data back up successfully.\n");
    return 1;
}

//Restore data from a backup. Copies backup files to the data directory and reloads the data
int restoreData(const char* timestamp) {
    char backupFileName[MAX_FILENAME_LENGTH];
    char dataFileName[MAX_FILENAME_LENGTH];
    FILE *backupFile, *dataFile;
    unsigned char buffer[4096];    // Buffer for file copying
    size_t bytesRead;
    int success = 1;

    printf("Starting data restoration from timestamp: %s\n", timestamp);

    // Create data directory if it doesn't exist
    #ifdef _WIN32
    system("mkdir ..\\data 2>NUL");
    #else
    system("mkdir -p ../data");
    #endif

    // Restore patients data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/patients_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/patients.dat");

    printf("Restoring patients data from: %s\n", backupFileName);

    // Open backup file for reading
    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Error: Cannot open backup file %s\n", backupFileName);
        return 0;
    }

    // Open data file for writing
    dataFile = fopen(dataFileName, "wb");
    if (dataFile == NULL) {
        printf("Error: Unable to create data file %s\n", dataFileName);
        fclose(backupFile);
        return 0;
    }

    // Copy data from backup to data file
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

    // Restore doctors data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/doctors_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/doctors.dat");

    printf("Restoring doctors data from: %s\n", backupFileName);

    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Warning: Cannot open doctors backup file %s\n", backupFileName);
    } else {
        dataFile = fopen(dataFileName, "wb");
        if (dataFile == NULL) {
            printf("Error: Unable to create doctors data file\n");
            fclose(backupFile);
            success = 0;
        } else {
            // Copy data from backup to data file
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

    // Restore schedule data
    snprintf(backupFileName, MAX_FILENAME_LENGTH, "../backups/schedule_%s.dat", timestamp);
    snprintf(dataFileName, MAX_FILENAME_LENGTH, "../data/schedule.dat");

    printf("Restoring schedule data from: %s\n", backupFileName);

    backupFile = fopen(backupFileName, "rb");
    if (backupFile == NULL) {
        printf("Warning: Cannot open schedule backup file %s\n", backupFileName);
    } else {
        dataFile = fopen(dataFileName, "wb");
        if (dataFile == NULL) {
            printf("Error: Unable to create schedule data file\n");
            fclose(backupFile);
            success = 0;
        } else {
            // Copy data from backup to data file
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

    if (!success) {
        printf("Warning: Errors occurred during restoration. Data may be incomplete.\n");
        return 0;
    }

    // Clean up and reinitialize the system with the restored data
    cleanupSystem();
    printf("System cleaned up\n");

    initializeSystem();
    printf("System reinitialized\n");

    int loadResult = safeLoadData();
    printf("Data load result: %s\n", loadResult ? "Success" : "Failed");

    if (loadResult) {
        printf("Data restored successfully from backup: %s\n", timestamp);
        return 1;
    } else {
        printf("Warning: Restored data files, but had issues loading them.\n");
        return 0;
    }
}

//Safely load data with validation. Similar to loadData() but with additional validation checks
int safeLoadData() {
    printf("Starting safe data loading...\n");

    // Reset counters and list heads
    totalPatients = 0;
    totalDoctors = 0;

    patientHead = NULL;
    doctorHead = NULL;

    // Load patient data
    FILE *patientFile = fopen("../data/patients.dat", "rb");
    if (patientFile == NULL) {
        printf("No existing patient data found. Starting with empty records.\n");
        return 0;
    }

    // Read total number of patients with validation
    int readPatients = 0;
    if (fread(&readPatients, sizeof(int), 1, patientFile) != 1) {
        printf("Error reading patient count from file.\n");
        fclose(patientFile);
        return 0;
    }

    printf("Found %d patients in data file.\n", readPatients);

    // Validate patient count is reasonable
    if (readPatients <= 0 || readPatients > 1000) {
        printf("Invalid patient count: %d\n", readPatients);
        fclose(patientFile);
        return 0;
    }

    // Read and recreate each patient record with validation
    Patient tempPatient;
    for (int i = 0; i < readPatients; i++) {
        if (fread(&tempPatient, sizeof(Patient) - sizeof(Patient *), 1, patientFile) != 1) {
            printf("Error reading patient %d data from file.\n", i+1);
            fclose(patientFile);
            return 0;
        }

        // Validate patient data
        if (tempPatient.patientID <= 0 || tempPatient.patientAge < 0 || tempPatient.patientAge > 150) {
            printf("Invalid patient data for ID: %d, Age: %d\n",
                   tempPatient.patientID, tempPatient.patientAge);
            continue;  // Skip this invalid patient
        }

        // Create a new patient with the validated information
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

        // Copy the admission date, discharge date, and active status
        strncpy(newPatient->admissionDate, tempPatient.admissionDate, sizeof(newPatient->admissionDate));
        strncpy(newPatient->dischargeDate, tempPatient.dischargeDate, sizeof(newPatient->dischargeDate));
        newPatient->isActive = tempPatient.isActive;

        // Add the patient to the linked list
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
        printf("Loaded patient ID: %d\n", newPatient->patientID);
    }
    fclose(patientFile);

    printf("Successfully loaded %d patients.\n", totalPatients);

    // Load doctor data
    FILE *doctorFile = fopen("../data/doctors.dat", "rb");
    if (doctorFile == NULL) {
        printf("No existing doctor data found. Starting with empty records.\n");
    } else {
        // Read total number of doctors with validation
        int readDoctors = 0;
        if (fread(&readDoctors, sizeof(int), 1, doctorFile) != 1) {
            printf("Error reading doctor count from file.\n");
            fclose(doctorFile);
        } else if (readDoctors > 0 && readDoctors <= 1000) {
            printf("Found %d doctors in data file.\n", readDoctors);

            // Read and recreate each doctor record with validation
            Doctor tempDoctor;
            for (int i = 0; i < readDoctors; i++) {
                if (fread(&tempDoctor, sizeof(Doctor) - sizeof(Doctor *), 1, doctorFile) != 1) {
                    printf("Error reading doctor %d data from file.\n", i+1);
                    break;
                }

                // Validate doctor data
                if (tempDoctor.doctorID <= 0) {
                    printf("Invalid doctor data for ID: %d\n", tempDoctor.doctorID);
                    continue;  // Skip this invalid doctor
                }

                // Create a new doctor with the validated information
                Doctor *newDoctor = createDoctor(tempDoctor.doctorID, tempDoctor.doctorName);
                if (newDoctor == NULL) {
                    printf("Failed to create doctor record for ID: %d\n", tempDoctor.doctorID);
                    continue;  // Skip if memory allocation failed
                }

                newDoctor->totalShifts = tempDoctor.totalShifts;

                // Add the doctor to the linked list
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
                printf("Loaded doctor ID: %d\n", newDoctor->doctorID);
            }
            printf("Successfully loaded %d doctors.\n", totalDoctors);
        }
        fclose(doctorFile);
    }

    // Load schedule data
    FILE *scheduleFile = fopen("../data/schedule.dat", "rb");
    if (scheduleFile == NULL) {
        printf("No existing schedule data found. Starting with empty schedule.\n");
        // Initialize schedule to zeros
        for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
            for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
                doctorSchedule[i][j] = 0;
            }
        }
    } else {
        size_t readResult = fread(doctorSchedule, sizeof(doctorSchedule), 1, scheduleFile);
        if (readResult != 1) {
            printf("Warning: Could not read complete schedule data.\n");
            // Initialize schedule to zeros if read failed
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

    // Return success if any data was loaded
    return (totalPatients > 0 || totalDoctors > 0);
}

//Select a backup to restore. Lists available backups and prompts the user to select one

char* selectBackup() {
    static char selectedTimestamp[30] = {0};  // Static to persist after function returns
    int backupCount = 0;
    char backupFiles[20][30];  // Store up to 20 backup timestamps

    printf("\nChecking for available backups...\n");

    // Create backups directory if it doesn't exist
    #ifdef _WIN32
    system("mkdir ..\\backups 2>NUL");
    #else
    system("mkdir -p ../backups");
    #endif

    printf("\nAvailable backups:\n");
    printf("----------------\n");

    // List available backup files
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

    // Read each backup filename and extract timestamp
    char line[MAX_FILENAME_LENGTH];
    while (fgets(line, sizeof(line), fileList) && backupCount < 20) {
        line[strcspn(line, "\n")] = 0;  // Remove newline

        if (strlen(line) == 0) {
            continue;
        }

        // Extract filename from path
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
                *extension = '\0';  // Remove .dat extension

                // Store the timestamp and display it
                strncpy(backupFiles[backupCount], timestamp, sizeof(backupFiles[0]) - 1);
                backupFiles[backupCount][sizeof(backupFiles[0]) - 1] = '\0';

                printf("%d. %s\n", backupCount + 1, timestamp);
                backupCount++;
            }
        }
    }

    fclose(fileList);
    remove("temp_backups.txt");  // Clean up the temporary file

    if (backupCount == 0) {
        printf("No valid backup files found.\n");
        return NULL;
    }

    // Ask the user to select a backup
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

    // Store the selected timestamp for return
    strcpy(selectedTimestamp, backupFiles[choice - 1]);
    printf("Selected backup from: %s\n", selectedTimestamp);

    return selectedTimestamp;
}

//Get current date and time as a formatted string
void getCurrentDateTime(char *dateTime, int bufferSize) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(dateTime, bufferSize, "%Y-%m-%d %H:%M:%S", t);
}

//Add a new patient to the system. Collects patient information and creates a new patient record
void addPatient() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Add Patient");

    int patientID;
    char patientName[50];
    int patientAge;
    char patientDiag[250];
    int patientRoomNum;

    // Get patient ID with validation
    printf("\nEnter the patient ID (positive number): ");
    patientID = scanInt();

    if (patientID <= 0) {
        printf("The patient ID must be a positive number!\n");
        returnToMenu();
        return;
    }

    // Check if patient ID already exists
    if (findPatientByID(patientID) != NULL) {
        printf("The patient ID already exists!\n");
        returnToMenu();
        return;
    }

    // Get patient name
    printf("Enter the patient name: ");
    fgets(patientName, sizeof(patientName), stdin);
    patientName[strcspn(patientName, "\n")] = 0;  // Remove newline

    // Get patient age with validation
    printf("Enter the patient age: ");
    patientAge = scanInt();

    if (patientAge < 0 || patientAge > 130) {
        printf("The patient age is invalid!\n");
        returnToMenu();
        return;
    }

    // Get patient diagnosis
    printf("Enter the patient diagnosis: ");
    fgets(patientDiag, sizeof(patientDiag), stdin);
    patientDiag[strcspn(patientDiag, "\n")] = 0;  // Remove newline

    // Get room number with validation
    printf("Enter the patient room number to assign (positive number): ");
    patientRoomNum = scanInt();

    if (patientRoomNum <= 0 || !isRoomAvailable(patientRoomNum)) {
        printf("Room number invalid or room is full!\n");
        returnToMenu();
        return;
    }

    // Create the new patient record
    Patient *newPatient = createPatient(patientID, patientName, patientAge, patientDiag, patientRoomNum);
    if (newPatient == NULL) {
        printf("Failed to create patient record!\n");
        returnToMenu();
        return;
    }

    // Add the new patient to the linked list
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

    // Save the updated data
    saveData();

    returnToMenu();
}


 //Display all active patients in the system
void viewPatients() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("View All Patients");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        returnToMenu();
        return;
    }

    // Print table header
    printf("%-10s%-25s%-10s%-30s%-15s%-30s%-10s\n",
           "ID", "Name", "Age", "Diagnosis", "Room Number", "Admission Date", "Status");
    printf(
        "-------------------------------------------------------------------------------------------------------------------------------\n");

    // Print each active patient's details
    Patient *current = patientHead;
    while (current != NULL) {
        if (current->isActive) {
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

//Search for a patient by ID. Displays the patient's details if found
void searchPatient() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Search Patient");

    int patientID;
    printf("Enter the patient ID: ");
    patientID = scanInt();

    // Find and display patient
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

//Discharge a patient. Sets the patient's status to inactive and records discharge date
void dischargePatient() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Discharge Patient");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        returnToMenu();
        return;
    }

    int patientID;
    printf("Enter the patient ID to discharge: ");
    patientID = scanInt();

    // Find the patient
    Patient *patient = findPatientByID(patientID);

    if (patient == NULL) {
        printf("The patient is not found!\n");
        returnToMenu();
        return;
    }

    // Check if already discharged
    if (patient->isActive == 0) {
        printf("This patient has already been discharged!\n");
        returnToMenu();
        return;
    }

    // Set discharge date and mark as inactive
    getCurrentDateTime(patient->dischargeDate, sizeof(patient->dischargeDate));
    patient->isActive = 0;

    // Free up the room
    patient->patientRoomNum = 0;

    printf("Patient discharged successfully!\n");
    saveData();
    returnToMenu();
}

//Find a patient by ID
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

//Check if a room is available. A room is considered available if it has less than 2 patients
int isRoomAvailable(int roomNum) {
    int count = 0;
    Patient *current = patientHead;

    // Count how many active patients are in this room
    while (current != NULL) {
        if (current->patientRoomNum == roomNum) {
            count++;
            if (count >= 2) {  // Maximum 2 patients per room
                return 0;
            }
        }
        current = current->next;
    }

    return 1;
}

//Add a new doctor to the system. Collects doctor information and creates a new doctor record

void addDoctor() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Add Doctor");

    int doctorID;
    char doctorName[50];

    // Get doctor ID with validation
    printf("\nEnter Doctor ID (positive number): ");
    doctorID = scanInt();

    if (doctorID <= 0) {
        printf("The doctor ID must be a positive number!\n");
        returnToMenu();
        return;
    }

    // Check if doctor ID already exists
    if (findDoctorByID(doctorID) != NULL) {
        printf("The doctor ID already exists!\n");
        returnToMenu();
        return;
    }

    // Get doctor name
    printf("Enter the doctor name: ");
    fgets(doctorName, sizeof(doctorName), stdin);
    doctorName[strcspn(doctorName, "\n")] = 0;  // Remove newline

    // Create the new doctor record
    Doctor *newDoctor = createDoctor(doctorID, doctorName);
    if (newDoctor == NULL) {
        printf("Failed to create doctor record!\n");
        returnToMenu();
        return;
    }

    // Add the new doctor to the linked list
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

    // Save the updated data
    saveData();

    returnToMenu();
}

//Display all doctors in the system
void viewDoctors() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("View All Doctors");

    if (totalDoctors == 0) {
        printf("No doctors in the system.\n");
        returnToMenu();
        return;
    }

    // Print table header
    printf("%-10s%-25s%-15s\n", "ID", "Name", "Total Shifts");
    printf("---------------------------------------------------\n");

    // Print each doctor's details
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

//Manage doctor schedules. Assigns a doctor to a specific day and shift
void manageDoctorSchedule() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Manage Doctor Schedule");

    if (totalDoctors == 0) {
        printf("No doctors in the system. Please add doctors first.\n");
        returnToMenu();
        return;
    }

    int doctorID, dayInWeek, shiftInDay;

    // Get doctor ID
    printf("\nEnter Doctor ID to assign shifts: ");
    doctorID = scanInt();

    // Find the doctor
    Doctor *doctor = findDoctorByID(doctorID);

    if (doctor == NULL) {
        printf("The doctor ID is invalid or doesn't exist!\n");
        returnToMenu();
        return;
    }

    // Check if doctor has reached maximum shifts
    if (doctor->totalShifts >= 7) {  // Maximum 7 shifts per week
        printf("This doctor has reached the maximum number of shifts this week!\n");
        returnToMenu();
        return;
    }

    // Get day to assign
    printf("Enter day to assign shift (1-7): ");
    dayInWeek = scanInt();

    if (dayInWeek < 1 || dayInWeek > 7) {
        printf("The day must be between 1 and 7!\n");
        returnToMenu();
        return;
    }

    // Get shift to assign
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

    // Find doctor's index in the list
    int doctorIndex = 1;
    Doctor *current = doctorHead;
    while (current != NULL && current->doctorID != doctorID) {
        doctorIndex++;
        current = current->next;
    }

    // Assign the shift
    doctorSchedule[dayInWeek - 1][shiftInDay - 1] = doctorIndex;
    doctor->totalShifts++;

    printf("Shift assigned successfully!\n");
    saveData();

    returnToMenu();
}

 //Display the weekly doctor schedule
void viewSchedule() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Doctor Schedule");

    printf("\nWeekly Schedule:\n");
    printf("-------------------------------------------------------------------------\n");
    printf("Day\t\t| Morning\t| Afternoon\t| Evening\n");
    printf("-------------------------------------------------------------------------\n");

    char *dayNames[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    // Print each day's schedule
    for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
        printf("%-10s\t|", dayNames[i]);

        // Print each shift for this day
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

//Find a doctor by ID
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

//Generate reports menu. Provides options for different types of reports
void generateReports() {
    int choice;

    do {
        printf("\e[1;1H\e[2J");  // Clear the screen
        printHeader("Generate Reports");

        // Display report options
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

//Generate a patient admission report. Creates a report file with details of all patients
void patientAdmissionReport() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Patient Admission Report");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Create filename with timestamp
    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for filename compatibility
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../reports/patient_admission_report_%s.txt", timestamp);

    // Create the report file
    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Write report header
    fprintf(reportFile, "PATIENT ADMISSION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Patients: %d\n\n", totalPatients);
    fprintf(reportFile, "%-10s%-25s%-10s%-30s%-15s%-25s%-10s\n",
            "ID", "Name", "Age", "Diagnosis", "Room Number", "Admission Date", "Status");
    fprintf(reportFile,
            "-------------------------------------------------------------------------------------------------------------------------\n");

    // Write patient data
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


//Generate a doctor utilization report. Creates a report file with details of all doctors and their shift utilization

void doctorUtilizationReport() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Doctor Utilization Report");

    if (totalDoctors == 0) {
        printf("No doctors in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Create filename with timestamp
    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for filename compatibility
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../reports/doctor_utilization_report_%s.txt", timestamp);

    // Create the report file
    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Write report header
    fprintf(reportFile, "DOCTOR UTILIZATION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Doctors: %d\n\n", totalDoctors);
    fprintf(reportFile, "%-10s%-25s%-15s%-15s\n",
            "ID", "Name", "Total Shifts", "Utilization %");
    fprintf(reportFile, "--------------------------------------------------------------------\n");

    // Write doctor data
    Doctor *current = doctorHead;
    while (current != NULL) {
        // Calculate utilization percentage (shifts / total possible shifts * 100)
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

//Generate a room utilization report. Creates a report file with details of room occupancy

void roomUtilizationReport() {
    printf("\e[1;1H\e[2J");  // Clear the screen
    printHeader("Room Utilization Report");

    if (totalPatients == 0) {
        printf("No patients in the system.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Count patients in each room
    int roomCounts[100] = {0};  // Assume maximum 100 rooms
    int maxRoom = 0;

    Patient *current = patientHead;
    while (current != NULL) {
        roomCounts[current->patientRoomNum]++;
        if (current->patientRoomNum > maxRoom) {
            maxRoom = current->patientRoomNum;
        }
        current = current->next;
    }

    // Create filename with timestamp
    char reportFileName[MAX_FILENAME_LENGTH];
    char timestamp[20];
    getCurrentDateTime(timestamp, sizeof(timestamp));

    // Replace spaces and colons with underscores for filename compatibility
    for (int i = 0; timestamp[i] != '\0'; i++) {
        if (timestamp[i] == ' ' || timestamp[i] == ':') {
            timestamp[i] = '_';
        }
    }

    snprintf(reportFileName, MAX_FILENAME_LENGTH, "../reports/room_utilization_report_%s.txt", timestamp);

    // Create the report file
    FILE *reportFile = fopen(reportFileName, "w");
    if (reportFile == NULL) {
        printf("Error: Unable to create report file.\n");
        printf("Press Enter to continue...");
        clearInputBuffer();
        return;
    }

    // Write report header
    fprintf(reportFile, "ROOM UTILIZATION REPORT\n");
    fprintf(reportFile, "Generated on: %s\n\n", timestamp);
    fprintf(reportFile, "Total Patients: %d\n\n", totalPatients);
    fprintf(reportFile, "%-15s%-15s%-15s\n",
            "Room Number", "Patients", "Occupancy %");
    fprintf(reportFile, "------------------------------------------\n");

    // Write room occupancy data
    for (int i = 1; i <= maxRoom; i++) {
        if (roomCounts[i] > 0) {
            // Calculate occupancy percentage (patients / max capacity * 100)
            float occupancy = (float) roomCounts[i] / 2 * 100;  // 2 is max capacity per room
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

//Main menu function. Displays the main menu and handles user choices

void menu() {
    int choice;

    do {
        printf("\e[1;1H\e[2J");  // Clear the screen
        printHeader("Hospital Management System");

        // Display menu options
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

        // Process user choice
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
                printf("\e[1;1H\e[2J");  // Clear the screen
                printHeader("Restore Data from Backup");

                // Select and restore backup
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
            case 11:
                saveData();
                printf("Exiting...");
                break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 11);
}

//Clear the input buffer. Used after scanf to clear any remaining input

void clearInputBuffer() {
    while (getchar() != '\n') {
        // Keep reading and discarding characters until a newline is found
    }
}


// Function to return to the main menu. Prompts the user to press Enter to continue
void returnToMenu() {
    printf("\nPress Enter to return to the menu...");
    clearInputBuffer();
}

// Safely scan an integer input and handles input validation
int scanInt() {
    int num;
    char term;

    // Read an integer followed by a character
    if (scanf("%d%c", &num, &term) != 2 || term != '\n') {
        clearInputBuffer();
        return -1;  // Return -1 for invalid input
    }

    return num;
}

 //Print a formatted header
void printHeader(const char *title) {
    int len = strlen(title);
    printf("\n");

    // Print top border
    for (int i = 0; i < len + 4; i++) printf("=");

    // Print title with borders
    printf("\n= %s =\n", title);

    // Print bottom border
    for (int i = 0; i < len + 4; i++) printf("=");

    printf("\n\n");
}
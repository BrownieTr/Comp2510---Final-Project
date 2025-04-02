/*
Hospital Management System
Author: Brownie Tran
Author: Raymond Yang
Date: 2025-02-11
Description: A simple C program to manage patient records and doctor schedules.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 50
#define MAX_DOCTORS 3
#define MAX_DAYS_IN_WEEK 7
#define MAX_SHIFTS_IN_DAY 3
#define MAX_CHAR_IN_DIAGNOSIS 255
#define MAX_CHAR_IN_NAME 50

// Function prototypes
void menu();

void initializeLists();

void addPatient();

void viewPatients();

void searchPatient();

void dischargePatient();

void manageDoctorSchedule();

void assignShift(int index, int dayInWeek);

void viewSchedule();

void addDoctor();

void viewDoctors();

void searchDoctor();

void dischargeDoctor();

void clearInputBuffer();

void returnToMenu();

int scanInt();

int idExists(int id, int type);

int roomNumExists(int roomNum);

void deleteNode(int id, int type);

void freeList(int type);

void validateType(int type);

void saveData();

void loadData();

// Set up self-referential structure for Patient
struct Patient {
    int iD;
    char patientName[MAX_CHAR_IN_NAME];
    int age;
    char diagnosis[MAX_CHAR_IN_DIAGNOSIS];
    int roomNum;
    struct Patient *next;
};

// Set up self-referential structure for Doctor
struct Doctor {
    int iD;
    char doctorName[MAX_CHAR_IN_NAME];
    int totalShift;
    struct Doctor *next;
};

// Generic Node structure
struct Node {
    int iD;
    struct Node *next;
};

// Declare global pointers to hold the lists of patients and doctors
struct Patient *firstPatient = NULL;
struct Patient *currentPatient = NULL;
struct Doctor *firstDoctor = NULL;
struct Doctor *currentDoctor = NULL;

// Lists to manage doctor schedules
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // Has an id number = shift assigned, 0 = not assigned

// Variable to warn if LinkedList is out of memory
int lastPatient = 0;
int lastDoctor = 0;

// Main function.
int main() {
    // Initialize patient and doctor lists
    initializeLists();
    loadData();

    // Call the menu function
    menu();
    return 0;
}

// Function to initialize patient and doctor lists
void initializeLists() {
    // Dynamically allocate memory for the first patient and doctor
    firstPatient = (struct Patient *) malloc(sizeof(struct Patient));
    if (firstPatient == NULL) {
        printf("Memory allocation failed for firstPatient\n");
        exit(-1);
    }
    currentPatient = firstPatient;

    firstDoctor = (struct Doctor *) malloc(sizeof(struct Doctor));
    if (firstDoctor == NULL) {
        printf("Memory allocation failed for firstDoctor\n");
        exit(-1);
    }
    currentDoctor = firstDoctor;
}

// Main menu function
void menu() {
    int choice;
    printf("\e[1;1H\e[2J"); // Clear screen
    do {
        choice = 0;
        printf("1. Add Patient record\n");
        printf("2. View All Patients\n");
        printf("3. Search Patient\n");
        printf("4. Discharge Patient\n");
        printf("5. Manage Doctor Schedule\n");
        printf("6. View Doctor Schedule\n");
        printf("7. Add Doctor Record\n");
        printf("8. View All Doctors\n");
        printf("9. Search Doctor\n");
        printf("10. Delete Doctor\n");
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
            case 9: searchDoctor();
                break;
            case 10: dischargeDoctor();
                break;
            case 11: printf("Exiting...\n");
                saveData();
                freeList(1);
                freeList(2);
                break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 11);
}

// Function to add a patient record
void addPatient() {
    printf("\e[1;1H\e[2J");

    int patientID;
    char patientName[50];
    int patientAge;
    char patientDiag[250];
    int patientRoomNum;

    printf("Enter the patient ID (positive number): ");
    patientID = scanInt();

    if (patientID <= 0 || idExists(patientID, 1) != -1) {
        printf("The patient ID is invalid or already exists!\n");
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

    if (patientRoomNum < 0 || roomNumExists(patientRoomNum) != -1) {
        printf("Room number invalid or room is full!\n");
        returnToMenu();
        return;
    }
    struct Patient *newPatient = (struct Patient *) malloc(sizeof(struct Patient)); // Allocate more memory for Patient

    newPatient->iD = 0;
    newPatient->age = 0;
    newPatient->roomNum = 0;
    newPatient->patientName[0] = '\0';
    newPatient->diagnosis[0] = '\0';
    newPatient->next = NULL;

    // Store patient details in LinkedList
    currentPatient->iD = patientID;
    strcpy(currentPatient->patientName, patientName);
    currentPatient->age = patientAge;
    strcpy(currentPatient->diagnosis, patientDiag);
    currentPatient->roomNum = patientRoomNum;
    if (currentPatient->next != NULL) {
        currentPatient = currentPatient->next;
    } else {
        currentPatient->next = newPatient;;
        currentPatient = currentPatient->next;
        currentPatient->next = NULL;
    }


    printf("Patient record added successfully!\n");
    returnToMenu();
}

// Function to view all patient records
void viewPatients() {
    struct Patient *temp = firstPatient;
    printf("\e[1;1H\e[2J");
    printf("%-10s%-15s%-10s%-15s%-10s\n", "ID", "Name", "Age", "Diagnosis", "Room Number");
    while (temp != NULL) {
        if (temp->iD > 0) {
            printf("%-10d%-15s%-10d%-15s%-10d\n",
               temp->iD,
               temp->patientName,
               temp->age,
               temp->diagnosis,
               temp->roomNum);
        }

        temp = temp->next;
    }
    returnToMenu();
}

// Function to search and display a patient record
void searchPatient() {
    int patientID;
    struct Patient *temp = firstPatient;

    printf("Enter the patient ID: \n");
    patientID = scanInt();

    if (patientID <= 0 || idExists(patientID, 1) == -1) {
        printf("The patient ID is invalid or couldn't found!\n");
        returnToMenu();
        return;
    }

    printf("\e[1;1H\e[2J"); // Clears the screen
    while (temp != NULL) {
        if (temp->iD == patientID) {
            printf("%-10s%-15s%-10s%-15s%-10s\n", "ID", "Name", "Age", "Diagnosis", "Room Number");
            printf("%-10d%-15s%-10d%-15s%-10d\n",
                   temp->iD,
                   temp->patientName,
                   temp->age,
                   temp->diagnosis,
                   temp->roomNum);
            break;
        }
    }
    returnToMenu();
}

// Function to remove patient record
void dischargePatient() {
    printf("\e[1;1H\e[2J"); // Clears the screen

    int patientID;
    printf("Enter the patient ID: ");
    patientID = scanInt();

    deleteNode(patientID, 1);
    returnToMenu();
}

// Function to manage doctore schedule
void manageDoctorSchedule() {
    // Assigns a shift to a doctor based on availability.
    int doctorID, dayInWeek;
    struct Doctor *temp = firstDoctor;

    printf("\e[1;1H\e[2J"); // Clears the screen
    printf("Enter Doctor ID to assign shifts: ");
    doctorID = scanInt();

    int id = idExists(doctorID, 2);
    while (temp != NULL) {
        if (temp->iD == id) {
            break;
        }
        temp = temp->next;
    }

    if (doctorID < 0 || id == -1) {
        printf("The doctor ID is invalid or doesn't exist!\n");
        returnToMenu();
        return;
    } else if (temp->totalShift >= 7) {
        printf("This doctor has reached the maximum number of shifts this week!\n");
        returnToMenu();
        return;
    }

    do {
        printf("Enter day to assign shift (8 to exit session): ");
        dayInWeek = scanInt();

        if (dayInWeek < 1 || dayInWeek > 8) {
            printf("The day must be between 1 and 7! Try again.\n");
        } else if (dayInWeek == 8) {
            printf("Session ending...\n");
            returnToMenu();
            return;
        } else {
            // Check if there is an available shift
            for (int i = 0; i < MAX_SHIFTS_IN_DAY; i++) {
                if (doctorSchedule[dayInWeek - 1][i] == 0) {
                    assignShift(id, dayInWeek);
                    temp->totalShift++;
                    dayInWeek = 8; // Exit loop after assigning a shift
                    break;
                }
            }

            if (dayInWeek != 8) {
                printf("All shifts for this day are taken! Choose a different day.\n");
            }
        }
    } while (dayInWeek != 8);

    returnToMenu();
}

// Function to assign shift to doctors
void assignShift(int id, int dayInWeek) {
    // Assigns a specific shift (morning, afternoon, evening) to a doctor on a given day.
    int shiftInDay;
    printf("\e[1;1H\e[2J");

    do {
        printf("Enter shift to assign (1-morning, 2-afternoon, 3-evening): ");
        shiftInDay = scanInt();

        if (shiftInDay < 1 || shiftInDay > 3) {
            printf("Invalid shift! Try again.\n");
        } else {
            if (doctorSchedule[dayInWeek - 1][shiftInDay - 1] == 0) {
                doctorSchedule[dayInWeek - 1][shiftInDay - 1] = id;
                printf("Shift assigned successfully!\n");
            } else {
                printf("This shift is already taken! Try again.\n");
                shiftInDay = 0;
            }
        }
    } while (shiftInDay < 1 || shiftInDay > 3);

    returnToMenu();
}

// Function to view schedule
void viewSchedule() {
    struct Doctor *temp = firstDoctor;

    // Displays the schedule of all doctors for the week.
    printf("\e[1;1H\e[2J");
    printf("Doctor Schedule\n");

    for (int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
        printf("Day %d:\t", i + 1);

        for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
            if (doctorSchedule[i][j] == 0) {
                printf("Shift %d: Not Assigned Yet\t", j + 1);
            } else {
                while (temp != NULL) {
                    if (temp->iD == doctorSchedule[i][j]) {
                        break;
                    }
                    temp = temp->next;
                }
                printf("Shift %d: Doctor %s\t\t", j + 1, temp->doctorName);
            }
        }
        printf("\n");
    }
    returnToMenu();
}

// Function to add doctor
void addDoctor() {
    printf("\e[1;1H\e[2J");

    int doctorID;
    char doctorName[50];

    printf("Enter the doctor ID (positive number): ");
    doctorID = scanInt();

    if (doctorID <= 0 || idExists(doctorID, 2) != -1) {
        printf("The patient ID is invalid or already exists!\n");
        returnToMenu();
        return;
    }

    printf("Enter the doctor name: ");
    fgets(doctorName, sizeof(doctorName), stdin);
    doctorName[strcspn(doctorName, "\n")] = 0; // Remove newline character
    struct Doctor *newDoctor = (struct Doctor *) malloc(sizeof(struct Doctor)); // Allocate more memory for Doctor

    // Store doctor details in LinkedList
    currentDoctor->iD = doctorID;
    strcpy(currentDoctor->doctorName, doctorName);
    currentDoctor->totalShift = 0;
    if (currentDoctor->next != NULL) {
        currentDoctor = currentDoctor->next;
    } else {
        currentDoctor->next = newDoctor;;
        currentDoctor = currentDoctor->next;
        currentDoctor->next = NULL;
    }

    printf("Patient record added successfully!\n");
    returnToMenu();
}


// Function to view all doctors
void viewDoctors() {
    struct Doctor *temp = firstDoctor;
    printf("\e[1;1H\e[2J");
    printf("%-10s%-15s%-10s\n", "ID", "Name", "TotalShift this week");
    while (temp != NULL && temp != currentDoctor) {
        printf("%-10d%-15s%-10d\n",
               temp->iD,
               temp->doctorName,
               temp->totalShift);
        temp = temp->next;
    }
    returnToMenu();
}

void searchDoctor() {
    int doctorID;
    struct Doctor *temp = firstDoctor;

    printf("Enter the Doctor ID: \n");
    doctorID = scanInt();

    if (doctorID <= 0 || idExists(doctorID, 1) != -1) {
        printf("The patient ID is invalid or already exists!\n");
        returnToMenu();
        return;
    }

    printf("\e[1;1H\e[2J"); // Clears the screen
    while (temp != NULL) {
        if (temp->iD == doctorID) {
            printf("%-10s%-15s%-10s%\n", "ID", "Name", "Total Shift this week");
            printf("%-10d%-15s%-10d%\n",
                   temp->iD,
                   temp->doctorName,
                   temp->totalShift);
            break;
        }
    }
    printf("Doctor record not found!\n");
    returnToMenu();
}

void dischargeDoctor() {
    printf("\e[1;1H\e[2J"); // Clears the screen

    int doctorID;
    printf("Enter the doctor ID: ");
    doctorID = scanInt();

    deleteNode(doctorID, 2);
    returnToMenu();
}

// Function to check if an ID exists in the LinkedList
int idExists(const int id, const int type) {
    validateType(type);

    if (id == 0) {
        return -1;
    }

    if (type == 2) {
        // Checking Doctor list
        struct Doctor *temp = firstDoctor;
        while (temp != NULL) {
            if (temp->iD == id) {
                return id;
            }
            temp = temp->next;
        }
    } else {
        // Checking Patient list
        struct Patient *temp = firstPatient;
        while (temp != NULL) {
            if (temp->iD == id) {
                return id;
            }
            temp = temp->next;
        }
    }
    return -1; // Not found
}

// Function to check if a room number is already occupied by two patients
int roomNumExists(const int roomNum) {
    const struct Patient *temp = firstPatient;
    while (temp != NULL) {
        if (temp->roomNum == roomNum) {
            return 1;
        }
        temp = temp->next;
    }
    return -1; // Return -1 if the room is available
}

void deleteNode(const int id, const int type) {
    validateType(type);

    if (type == 2) {
        struct Doctor *temp = firstDoctor;
        struct Doctor *prevPtr = NULL;

        while (temp != NULL) {
            if (temp->iD == id) {
                if (prevPtr == NULL) {
                    firstDoctor = temp->next;
                } else {
                    prevPtr->next = temp->next;
                }
                free(temp);
                printf("Doctor record deleted successfully.\n");
                return;
            }
            prevPtr = temp;
            temp = temp->next;
        }
    } else {
        struct Patient *temp = currentPatient;
        struct Patient *prevPtr = NULL;

        while (temp != NULL) {
            if (temp->iD == id) {
                if (prevPtr == NULL) {
                    currentPatient = temp->next;
                } else {
                    prevPtr->next = temp->next;
                }
                free(temp);
                printf("Patient record deleted successfully.\n");
                return;
            }
            prevPtr = temp;
            temp = temp->next;
        }
    }

    printf("Record with ID %d not found.\n", id);
}

void freeList(const int type) {
    validateType(type);
    struct Node *temp = (type == 2) ? (struct Node *) firstDoctor : (struct Node *) currentPatient;

    while (temp != NULL) {
        struct Node *next = temp->next;
        free(temp);
        temp = next;
    }
}

void validateType(int type) {
    if (type < 1 || type > 2) {
        printf("Invalid type!\n");
        returnToMenu();
    }
}

// Function to clear the input buffer to handle invalid input cases.
void clearInputBuffer() {
    // Clears the input buffer to handle invalid input cases.
    while (getchar() != '\n') {
    }
}

// Function to safely scans an integer input and handles invalid inputs.
int scanInt() {
    int num;
    char term;

    if (scanf("%d%c", &num, &term) != 2 || term != '\n') {
        clearInputBuffer();
        return -1;
    }

    return num;
}

// Function to prompt the user to press Enter to return to the main menu.
void returnToMenu() {
    // Prompts the user to press Enter to return to the main menu.
    printf("Press enter to return to the menu\n");
    clearInputBuffer();
    printf("\e[1;1H\e[2J");
}

void saveData() {
    // Save patient records
    FILE *patientFile = fopen("patient.txt", "w");
    if (patientFile == NULL) {
        printf("Error opening patient file for writing!\n");
        return;
    }

    struct Patient *tempPatient = firstPatient;
    while (tempPatient != NULL && tempPatient != currentPatient) {
        fprintf(patientFile, "%d,%s,%d,%s,%d\n",
                tempPatient->iD,
                tempPatient->patientName,
                tempPatient->age,
                tempPatient->diagnosis,
                tempPatient->roomNum);
        tempPatient = tempPatient->next;
    }
    fclose(patientFile);
}

void loadData() {
    char line[512];
    char *token;
    const char delim[] = ",";
    int lineCount = 0;

    printf("Loading data from files...\n");

    // Load patient data
    FILE *patientFile = fopen("patient.txt", "r");
    if (patientFile != NULL) {
        // Skip header line
        lineCount = 0;

        // Read remaining data lines
        while (fgets(line, sizeof(line), patientFile)) {
            // Remove newline character if present
            line[strcspn(line, "\n")] = 0;

            struct Patient *newPatient = NULL;

            // First line needs special handling as we already have firstPatient allocated
            if (lineCount == 0) {
                newPatient = firstPatient;
            } else {
                newPatient = (struct Patient *) malloc(sizeof(struct Patient));
                if (newPatient == NULL) {
                    printf("Memory allocation failed while loading patient data\n");
                    fclose(patientFile);
                    return;
                }
                currentPatient->next = newPatient;
            }

            // Parse CSV line
            token = strtok(line, delim);
            if (token != NULL) {
                newPatient->iD = atoi(token);

                token = strtok(NULL, delim);
                if (token != NULL) {
                    strncpy(newPatient->patientName, token, MAX_CHAR_IN_NAME - 1);
                    newPatient->patientName[MAX_CHAR_IN_NAME - 1] = '\0';

                    token = strtok(NULL, delim);
                    if (token != NULL) {
                        newPatient->age = atoi(token);

                        token = strtok(NULL, delim);
                        if (token != NULL) {
                            strncpy(newPatient->diagnosis, token, MAX_CHAR_IN_DIAGNOSIS - 1);
                            newPatient->diagnosis[MAX_CHAR_IN_DIAGNOSIS - 1] = '\0';

                            token = strtok(NULL, delim);
                            if (token != NULL) {
                                newPatient->roomNum = atoi(token);
                            }
                        }
                    }
                }
            }

            // Move to next patient
            currentPatient = newPatient;
            currentPatient->next = NULL;
            lineCount++;
        }
        printf("Loaded %d patient records\n", lineCount);

        fclose(patientFile);
    } else {
        printf("No patient data file found or unable to open\n");
    }
}

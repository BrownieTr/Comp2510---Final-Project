/*
Hospital Management System
Author: tkngu
Author: Raymond Yang
Date: 2025-02-11
Description: A simple C program to manage patient records and doctor schedules.
*/

#include <stdio.h>
#include <string.h>

#define MAX_PATIENTS 50
#define MAX_DOCTORS 3
#define MAX_DAYS_IN_WEEK 7
#define MAX_SHIFTS_IN_DAY 3

// Arrays to store patient records
int patientIDs[MAX_PATIENTS];
char patientNames[MAX_PATIENTS][50];
int patientAges[MAX_PATIENTS];
char patientDiagnosis[MAX_PATIENTS][250];
int patientRoomNums[MAX_PATIENTS];
int totalPatients = 0;

// Arrays to manage doctor schedules
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // 1-3 = shift assigned, 0 = not assigned
int doctorIDs[MAX_DOCTORS];
char doctorNames[MAX_DOCTORS][50];
int totalDoctorShifts[MAX_DOCTORS];
int totalDoctors = 0;

// Function prototypes
void addPatientRecord(); // Adds a new patient record to the system
void viewPatientRecords(); // Displays all patient records
void searchPatientRecord(); // Searches for a patient record by ID
void dischargePatientRecord(); // Removes a patient record from the system
void manageDoctorSchedule(); // Manages doctor shift schedules
void assignShift(int index, int dayInWeek); // Assigns a shift to a doctor
void viewSchedule(); // Displays the doctor schedule
void addDoctor(); // Adds a new doctor record to the system
void viewDoctors(); // Displays all doctor records
void clearInputBuffer(); // Clears the input buffer to handle incorrect inputs
void returnToMenu(); // Function to make user press enter to return to the menu
int scanInt(); // Scans integers only
int idExists(int arr[], int size, int id); // Checks if an ID exists in a given array
int roomNumExists(int arr[], int size, int roomNum); // Checks if a room number is occupied by two patients

// Main menu function
void menu() {
  int choice;
  printf("\e[1;1H\e[2J");// Clear screen
  do {
    choice = 0;
    printf("\n1. Add Patient record\n");
    printf("2. View All Patients\n");
    printf("3. Search Patient\n");
    printf("4. Discharge Patient\n");
    printf("5. Manage Doctor Schedule\n");
    printf("6. View Doctor Schedule\n");
    printf("7. Add Doctor Record\n");
    printf("8. View All Doctors\n");
    printf("9. Exit\n");
    printf("Enter your choice: ");
    choice = scanInt();

    switch (choice) {
      case 1: addPatientRecord(); break;
      case 2: viewPatientRecords(); break;
      case 3: searchPatientRecord(); break;
      case 4: dischargePatientRecord(); break;
      case 5: manageDoctorSchedule(); break;
      case 6: viewSchedule(); break;
      case 7: addDoctor(); break;
      case 8: viewDoctors(); break;
      case 9: printf("Exiting...\n"); break;
      default: printf("Invalid choice! Try again.\n");
    }
  } while (choice != 9);
}

// Function to check if an ID exists in an array
int idExists(int arr[], int size, int id) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == id) {
      return i; // Return the index where the ID exists
    }
  }
  return -1; // Return -1 if not found
}

// Function to check if a room number is already occupied by two patients
int roomNumExists(int arr[], int size, int roomNum) {
  int totalRoomNum = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] == roomNum) {
      totalRoomNum++; // Count how many times the room is assigned
    }
    if (totalRoomNum >= 2) {
      return i; // Return the index if the room is full
    }
  }
  return -1; // Return -1 if the room is available
}

// Function to add a patient record
void addPatientRecord() {
  printf("\e[1;1H\e[2J");
  if (totalPatients >= MAX_PATIENTS) {
    printf("Too many patients at the moment. Discharge one first!\n");
    return;
  }

  int patientID;
  char patientName[50];
  int patientAge;
  char patientDiag[250];
  int patientRoomNum;

  printf("\nEnter the patient ID (positive number): ");
  patientID = scanInt();

  if (patientID <= 0 || idExists(patientIDs, totalPatients, patientID) != -1) {
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

  if (patientRoomNum < 0 || roomNumExists(patientRoomNums, totalPatients, patientRoomNum) != -1) {
    printf("Room number invalid or room is full!\n");
    returnToMenu();
    return;
  }

  // Store patient details in arrays
  patientIDs[totalPatients] = patientID;
  strcpy(patientNames[totalPatients], patientName);
  patientAges[totalPatients] = patientAge;
  strcpy(patientDiagnosis[totalPatients], patientDiag);
  patientRoomNums[totalPatients] = patientRoomNum;
  totalPatients++;

  printf("Patient record added successfully!\n");
  returnToMenu();
  return;
}

// Scans integers only
int scanInt() {
  int num;
  char term;
  if (scanf("%d%c", &num, &term) != 2 || term != '\n') {
    clearInputBuffer();
    return -1;
  } else {
    return num;
  }
}

// Function to clear input buffer
void clearInputBuffer() {
  while (getchar() != '\n') {
  }
  return;
}

// Function to make user press enter to return to the menu
void returnToMenu() {
  printf("Press enter to return to the menu\n");
  clearInputBuffer();
  return;
}

int main() {
  printf("%d", totalPatients); // Print the total number of patients (debugging purpose)
  menu(); // Call the menu function
  return 0;
}
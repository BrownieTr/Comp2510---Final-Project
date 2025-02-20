/*
 * Hospital Management System
 * Author: Brownie Tran, Raymond Yang
 * Version: 1.0
 * Created on: 2025-02-11
 */

#include <stdio.h>
#include <string.h>

// Constants for system limits
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
int totalPatients = 0; // Counter for total patients

// Arrays to store doctor schedules
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // 0 = unassigned, 1-3 = doctor assigned
int doctorIDs[MAX_DOCTORS];
char doctorNames[MAX_DOCTORS][50];
int totalDoctorShifts[MAX_DOCTORS]; // Track shifts assigned per doctor
int totalDoctors = 0; // Counter for total doctors

// Function prototypes
void addPatientRecord();
void viewPatientRecords();
void searchPatientRecord();
void dischargePatientRecord();
void manageDoctorSchedule();
void assignShift(int index, int dayInWeek);
void viewSchedule();
void addDoctor();
void viewDoctors();
void clearInputBuffer();
int scanInt();
int idExists(int arr[], int size, int id);
int roomNumExists(int arr[], int size, int roomNum);

// Function to display menu and handle user choices
void menu() {
  int choice;
  do {
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
      return i;
    }
  }
  return -1;
}

// Function to check if a room number is already occupied by two patients
int roomNumExists(int arr[], int size, int roomNum) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] == roomNum) {
      count++;
    }
    if (count >= 2) {
      return i;
    }
  }
  return -1;
}

// Function to add a new patient record
void addPatientRecord() {
  if (totalPatients >= MAX_PATIENTS) {
    printf("Too many patients at the moment. Discharge one first!\n");
    return;
  }

  int patientID;
  char patientName[50];
  int patientAge;
  char patientDiag[250];
  int patientRoomNum;

  // Get patient ID
  printf("\nEnter the patient ID (positive number): ");
  patientID = scanInt();
  if (patientID <= 0 || idExists(patientIDs, totalPatients, patientID) != -1) {
    printf("The patient ID is invalid or already exists!\n");
    return;
  }

  // Get patient name
  printf("Enter the patient name: ");
  fgets(patientName, sizeof(patientName), stdin);
  patientName[strcspn(patientName, "\n")] = 0; // Remove newline

  // Get patient age
  printf("Enter the patient age: ");
  patientAge = scanInt();
  if (patientAge < 0 || patientAge > 130) {
    printf("Invalid age!\n");
    return;
  }

  // Get patient diagnosis
  printf("Enter the patient diagnosis: ");
  fgets(patientDiag, sizeof(patientDiag), stdin);
  patientDiag[strcspn(patientDiag, "\n")] = 0;

  // Get patient room number
  printf("Enter the patient room number to assign (positive number): ");
  patientRoomNum = scanInt();
  if (patientRoomNum < 0 || roomNumExists(patientRoomNums, totalPatients, patientRoomNum) != -1) {
    printf("Room number invalid or room is full!\n");
    return;
  }

  // Store patient details
  patientIDs[totalPatients] = patientID;
  strcpy(patientNames[totalPatients], patientName);
  patientAges[totalPatients] = patientAge;
  strcpy(patientDiagnosis[totalPatients], patientDiag);
  patientRoomNums[totalPatients] = patientRoomNum;
  totalPatients++;

  printf("Patient record added successfully!\n");
}

// Function to view all patient records
void viewPatientRecords() {
  printf("\nPatient ID\tName\t\t\tAge\tDiagnosis\tRoom Number\n");
  for(int i = 0; i < totalPatients; i++) {
    printf("%d\t\t%s\t\t%d\t%s\t\t%d\n", patientIDs[i], patientNames[i], patientAges[i], patientDiagnosis[i], patientRoomNums[i]);
  }
}

// Function to clear the input buffer
void clearInputBuffer() {
  while (getchar() != '\n');
}

// Function to safely scan an integer input
int scanInt() {
  int num;
  char term;
  if (scanf("%d%c", &num, &term) != 2 || term != '\n') {
    clearInputBuffer();
    return 0; // Return invalid input
  }
  return num;
}

// Main function to run the program
int main() {
  menu();
  return 0;
}

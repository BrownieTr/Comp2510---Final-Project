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
void returnToMenu();
int scanInt();
int idExists(int id, int type);
int roomNumExists(int roomNum);
void deleteNode(int id, int type);
void freeList(int type);
void validateType(int type);

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
struct  Doctor {
  int iD;
  char doctorName[MAX_CHAR_IN_NAME];
  int totalShift;
  struct Doctor* next;
};

// List to store patient records
struct Patient *firstPatient = malloc(sizeof(struct Patient));
struct Patient *currentPatient = firstPatient;

// Lists to manage doctor schedules
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // 1-3 = shift assigned, 0 = not assigned
struct Doctor *firstDoctor = malloc(sizeof(struct Doctor));
struct Doctor *currentDoctor = firstDoctor;

// Main menu function
void menu() {
  int choice;
  printf("\e[1;1H\e[2J");// Clear screen
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
      case 9: printf("Exiting...\n"); freeList(1); freeList(2); break;
      default: printf("Invalid choice! Try again.\n");
    }
  } while (choice != 9);
}

// Function to check if an ID exists in the LinkedList
int idExists(const int id, const int type) {
  validateType(type);
  if (type == 2) {
    const struct Doctor* temp = firstDoctor;
  }
  const struct Patient *temp = firstPatient;

  while (temp != NULL) {
    if (temp->iD == id) {
      return 1;
    }
    temp = temp->next;
  }
  return -1; // Return -1 if not found
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

// Function to add a patient record
void addPatientRecord () {
  printf("\e[1;1H\e[2J");

  int patientID;
  char patientName[50];
  int patientAge;
  char patientDiag[250];
  int patientRoomNum;

  printf("\nEnter the patient ID (positive number): ");
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

  // Store patient details in LinkedList
  currentPatient->iD =  patientID;
  strcpy(currentPatient->patientName, patientName);
  currentPatient->age = patientAge;
  strcpy(currentPatient->diagnosis, patientDiag);
  currentPatient->roomNum = patientRoomNum;
  currentPatient = currentPatient->next;

  printf("Patient record added successfully!\n");
  returnToMenu();
}

// Function to view all patient records
void viewPatientRecords () {
  struct Patient *temp = firstPatient;
  printf("\e[1;1H\e[2J");
  printf("%-10s%-15s%-10s%-15s%-10s\n","ID","Name","Age","Diagnosis","Room Number");
  while (temp != NULL) {
    printf("%-10d%-15s%-10d%-15s%-10d\n",
      temp->iD,
      temp->patientName,
      temp->age,
      temp->diagnosis,
      temp->roomNum);
    temp = temp->next;
  }
  returnToMenu();
}

// Function to search and display a patient record
void searchPatientRecord () {
  int patientID;
  struct Patient *temp = firstPatient;

  printf("Enter the patient ID: \n");
  patientID = scanInt();

  if (patientID <= 0 || idExists(patientID, 1) != -1) {
    printf("The patient ID is invalid or already exists!\n");
    returnToMenu();
    return;
  }

  printf("\e[1;1H\e[2J");  // Clears the screen
  while (temp != NULL) {
    if (temp->iD == patientID) {
      printf("%-10s%-15s%-10s%-15s%-10s\n","ID","Name","Age","Diagnosis","Room Number");
      printf("%-10d%-15s%-10d%-15s%-10d\n",
        temp->iD,
        temp->patientName,
        temp->age,
        temp->diagnosis,
        temp->roomNum);
      break;
    }
  }
  printf("Patient record not found!\n");
  returnToMenu();
}

// Function to remove patient record
void dischargePatientRecord () {
  // Removes a patient record from the system using the patient ID.
  int foundPatientID = 0;
  int patientIndex;
  printf("\e[1;1H\e[2J");  // Clears the screen

  int patientID;
  printf("Enter the patient ID: ");
  patientID = scanInt();

  // Locate the patient record
  for(int i = 0; i < totalPatients; i++) {
    if (patientIDs[i] == patientID) {
      foundPatientID = 1;
      patientIndex = i;
      break;
    }
  }

  if (foundPatientID == 0) {
    printf("The patient is not found!\n");
    return;
  } else {
    totalPatients--;  // Reduce patient count
    // Shift all records after the removed patient forward in the array
    for(int j = patientIndex; j < totalPatients; j++) {
      patientIDs[j] = patientIDs[j+1];
      strcpy(patientNames[j], patientNames[j+1]);
      patientAges[j] = patientAges[j+1];
      strcpy(patientDiagnosis[j], patientDiagnosis[j+1]);
      patientRoomNums[j] = patientRoomNums[j+1];
    }
  }

  printf("Patient removed!");
  returnToMenu();
  return;
}

// Function to manage doctore schedule
void manageDoctorSchedule () {
  // Assigns a shift to a doctor based on availability.
  int doctorID, dayInWeek;

  printf("\e[1;1H\e[2J");  // Clears the screen
  printf("\nEnter Doctor ID to assign shifts: ");
  doctorID = scanInt();

  int index = idExists(doctorIDs, totalDoctors, doctorID);

  if (doctorID < 0 || index == -1) {
    printf("The doctor ID is invalid or doesn't exist!\n");
    returnToMenu();
    return;
  } else if (totalDoctorShifts[index] >= 7) {
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
          assignShift(index, dayInWeek);
          dayInWeek = 8;  // Exit loop after assigning a shift
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
void assignShift(int index, int dayInWeek) {
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
        doctorSchedule[dayInWeek - 1][shiftInDay - 1] = index + 1;
        totalDoctorShifts[index]++;
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
void viewSchedule () {
  // Displays the schedule of all doctors for the week.
  printf("\e[1;1H\e[2J");
  printf("\nDoctor Schedule\n");

  for(int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
    printf("Day %d:\t", i + 1);

    for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
      if (doctorSchedule[i][j] == 0) {
        printf("Shift %d: Not Assigned Yet\t", j + 1);
      } else {
        printf("Shift %d: Doctor %s\t\t", j + 1, doctorNames[doctorSchedule[i][j] - 1]);
      }
    }
    printf("\n");
  }
  returnToMenu();
}

// Function to add doctor
void addDoctor () {
  // Adds a new doctor record to the system.
  printf("\e[1;1H\e[2J");

  if (totalDoctors >= MAX_DOCTORS) {
    printf("Maximum number of doctors reached!\n");
    returnToMenu();
    return;
  }

  int doctorID;
  char doctorName[50];

  printf("Enter Doctor ID (positive number): ");
  doctorID = scanInt();

  if (doctorID < 0 || idExists(doctorIDs, totalDoctors, doctorID) != -1) {
    printf("The doctor ID is invalid or already exists!\n");
    returnToMenu();
    return;
  }

  printf("Enter the doctor name: ");
  fgets(doctorName, sizeof(doctorName), stdin);
  doctorName[strcspn(doctorName, "\n")] = 0;

  doctorIDs[totalDoctors] = doctorID;
  strcpy(doctorNames[totalDoctors], doctorName);
  totalDoctors++;

  printf("Doctor record added successfully!\n");
  returnToMenu();
}


// Function to view all doctors
void viewDoctors () {
  // Displays a list of all registered doctors.
  printf("\e[1;1H\e[2J");
  printf("\nDoctor ID\tName\n");

  for(int i = 0; i < totalDoctors; i++) {
    printf("%d\t\t%s\n", doctorIDs[i], doctorNames[i]);
  }

  returnToMenu();
}

void deleteRecord(const int id, const int type) {
  validateType(type);
  if (type == 2) {
    const struct Doctor* temp = firstDoctor;
  }
  const struct Patient *temp = currentPatient;
  int found = 1;

  while (temp != NULL) {
    if (temp->iD == id) {
      free(temp);
      found = 0;
      break;
    }
  }

  if (found) {
    printf("Record not found!\n");
  } else {
    printf("Record deleted successfully!\n");
  }
  returnToMenu();
}

void freeList(const int type) {
  validateType(type);
  if (type == 2) {
    const struct Doctor* temp = firstDoctor;
  }
  const struct Patient *temp = currentPatient;

  while (temp != NULL) {
    temp = temp->next;
    free(temp);
  }
}

void validateType(int type) {
  if (type < 1 || type > 2) {
    printf("Invalid type!\n");
    returnToMenu();
  }
}

// Function to clear the input buffer to handle invalid input cases.
void clearInputBuffer () {
  // Clears the input buffer to handle invalid input cases.
  while (getchar() != '\n') {}
  return;
}

// Function to safely scans an integer input and handles invalid inputs.
int scanInt() {
  int num;
  char term;

  if(scanf("%d%c", &num, &term) != 2 || term != '\n') {
    clearInputBuffer();
    return -1;
  }

  return num;
}

// Function to prompt the user to press Enter to return to the main menu.
void returnToMenu () {
  // Prompts the user to press Enter to return to the main menu.
  printf("Press enter to return to the menu\n");
  clearInputBuffer();
  return;
}

// Main function.
int main() {
  menu(); // Call the menu function
  return 0;
}
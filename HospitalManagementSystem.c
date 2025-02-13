//
// Created by tkngu on 2025-02-11.
//

#include <stdio.h>
#include <string.h>

#define MAX_PATIENTS 50
#define MAX_DOCTORS 3
#define MAX_DAYS_IN_WEEK 7
#define MAX_SHIFTS_IN_DAY 3

// Arrays to store patient's record details
int patientIDs[MAX_PATIENTS];
char patientNames[MAX_PATIENTS][50];
int patientAges[MAX_PATIENTS];
char patientDiagnosis[MAX_PATIENTS][250];
int patientRoomNums[MAX_PATIENTS];
int totalPatients = 0;

// Array to manage doctor schedule
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY]; // 1-3 = shift assigned, 0 = shift not assigned
int doctorIDs[MAX_DOCTORS];
char doctorNames[MAX_DOCTORS][50];
int totalDoctorShifts[MAX_DOCTORS];
int totalDoctors = 0;

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
int idExists(int arr[], int size,int id);
int roomNumExists(int arr[], int size,int roomNum);

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
    scanf("%d", &choice);
    getchar();

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

int idExists (int arr[], int size, int id) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == id) {
      return i;
    }
  }
  return -1;
}

int roomNumExists (int arr[], int size, int roomNum) {
  int totalRoomNum = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] == roomNum) {
      totalRoomNum++;
    }
    if (totalRoomNum >= 2) {
      return i;
    }
  }
  return -1;
}

void addPatientRecord () {
  if (totalPatients >= MAX_PATIENTS) {
    printf("Too many patients in the moment. Discharge one first!\n");
    return;
  }

  int patientID;
  char patientName[50];
  int patientAge;
  char patientDiag[250];
  int patientRoomNum;

  printf("\nEnter the patient ID (positive number): ");
  scanf("%d", &patientID);
  getchar();

  if (patientID <= 0 || idExists(patientIDs, totalPatients, patientID) != -1) {
    printf("The patient ID is invalid or already exist!\n");
    return;
  }

  printf("Enter the patient name: ");
  fgets(patientName, sizeof(patientName), stdin);
  patientName[strcspn(patientName, "\n")] = 0;

  printf("Enter the patient age: ");
  scanf("%d", &patientAge);
  getchar();

  if (patientAge < 0 || patientAge > 130) {
    printf("The patient age is invalid!\n");
    return;
  }

  printf("Enter the patient diagnosis: ");
  fgets(patientDiag, sizeof(patientDiag), stdin);
  patientDiag[strcspn(patientDiag, "\n")] = 0;

  printf("Enter the patient room number to assign (positive number): ");
  scanf("%d", &patientRoomNum);
  getchar();

  if (patientRoomNum < 0 || roomNumExists(patientRoomNums, totalPatients, patientRoomNum) != -1) {
    printf("Room number invalid or room is full!\n");
    return;
  }

  patientIDs[totalPatients] = patientID;
  strcpy(patientNames[totalPatients], patientName);
  patientAges[totalPatients] = patientAge;
  strcpy(patientDiagnosis[totalPatients], patientDiag);
  patientRoomNums[totalPatients] = patientRoomNum;
  totalPatients++;

  printf("Patient record added successfully!\n");
}

void viewPatientRecords () {
  printf("\nPatient ID\tName\tAge\tDiagnosis\tRoom Number\n");
  for(int i = 0; i < totalPatients; i++) {
    printf("%d\t%s\t%d\t%s\t%d\n", patientIDs[i], patientNames[i], patientAges[i], patientDiagnosis[i],patientRoomNums[i]);
  }
}

void searchPatientRecord () {}

void dischargePatientRecord () {}

void manageDoctorSchedule () {
  int doctorID;
  int dayInWeek;
  int shiftInDay;

  printf("\nEnter Doctor ID to assign shifts: ");
  scanf("%d", &doctorID);
  getchar();
  int index = idExists(doctorIDs, totalDoctors, doctorID);

  if (doctorID < 0 || index == -1) {
    printf("The doctor ID is invalid or didn't exist!\n");
    return;
  } else if (totalDoctorShifts[index] >= 7) {
    printf("This doctor took enough shifts this week!\n");
    return;
  }

  do {
    printf("Enter day to assign shift (8 to exit session): ");
    scanf("%d", &dayInWeek);
    getchar();
    if (dayInWeek < 1 || dayInWeek > 8) {
      printf("The days in week should be between 1 and 7! Try again.\n");

    } else if (dayInWeek == 8) {
      printf("Session ending...\n");
      return;
    } else {
      for (int i = 0; i < MAX_SHIFTS_IN_DAY; i++) {
        if (doctorSchedule[dayInWeek-1][i] == 0) {
          assignShift(index, dayInWeek);
          dayInWeek = 8;
          break;
        }
      }
      if (dayInWeek == 8) {
        break;
      }
      printf("This day's shifts have all been taken! Choose a different day.\n");
    }
  } while (dayInWeek != 8);
}

void assignShift(int index, int dayInWeek) {
  int shiftInDay;
  do {
    printf("Enter shift to assign (1-morning, 2-afternoon, 3-evening)\n");
    scanf("%d", &shiftInDay);
    getchar();
    if (shiftInDay < 1 || shiftInDay > 3) {
      printf("This shift is invalid! Try again\n");
    } else {
        if (doctorSchedule[dayInWeek - 1][shiftInDay - 1] == 0) {
          doctorSchedule[dayInWeek - 1][shiftInDay - 1] = index + 1;
          totalDoctorShifts[index]++ ;
          printf("The shift is successfully assigned!\n");
        } else {
          printf("This shift is taken! Try again\n");
          shiftInDay = 0;
        }
    }
  } while (shiftInDay < 1 || shiftInDay > 3);
}

void viewSchedule () {
  printf("\nDoctor Schedule\n");
  for(int i = 0; i < MAX_DAYS_IN_WEEK; i++) {
    printf("Day %d:\t", i + 1);
    for (int j = 0; j < MAX_SHIFTS_IN_DAY; j++) {
      if (doctorSchedule[i][j] == 0) {
        printf("Shift %d: Not Assigned Yet\t", doctorSchedule[i][j]);
      } else {
        printf("Shift %d: Doctor %s\t\t", doctorSchedule[i][j], doctorNames[doctorSchedule[i][j] - 1]);
      }
    }
    printf("\n");
  }
}

void addDoctor () {
  if (totalDoctors >= MAX_DOCTORS) {
    printf("Too many doctors!\n");
    return;
  }

  int doctorID;
  char doctorName[50];

  printf("\nEnter Doctor ID (positive number): ");
  scanf("%d", &doctorID);
  getchar();

  if (doctorID < 0 || idExists(doctorIDs, totalDoctors, doctorID) != -1) {
    printf("The doctor ID is invalid or already exist!\n");
    return;
  }

  printf("Enter the doctor name: ");
  fgets(doctorName, sizeof(doctorName), stdin);
  doctorName[strcspn(doctorName, "\n")] = 0;

  doctorIDs[totalDoctors] = doctorID;
  strcpy(doctorNames[totalDoctors], doctorName);
  totalDoctors++;

  printf("Doctor record added successfully!\n");
}

void viewDoctors () {
  printf("\nDoctor ID\tName\n");
  for(int i = 0; i < totalDoctors; i++) {
    printf("%d\t\t%s\n", doctorIDs[i], doctorNames[i]);
  }
}

int main() {
  menu();
  return 0;
}

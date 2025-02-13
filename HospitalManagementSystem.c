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
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY];
int doctorIDs[MAX_DOCTORS];
char doctorNames[MAX_DOCTORS][50];
int totalDoctors = 0;

// Function prototypes
void addPatientRecord();
void viewPatientRecords();
void searchPatientRecord();
void dischargePatientRecord();
void manageDoctorSchedule();
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
    printf("6. Add Doctor Record\n");
    printf("7. View All Doctors\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1: addPatientRecord(); break;
      case 2: viewPatientRecords(); break;
      case 3: searchPatientRecord(); break;
      case 4: dischargePatientRecord(); break;
      case 5: manageDoctorSchedule(); break;
      case 6: addDoctor(); break;
      case 7: viewDoctors(); break;
      case 8: printf("Exiting...\n"); break;
      default: printf("Invalid choice! Try again.\n");
    }
  } while (choice != 8);
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
  int doctorID;

  printf("Enter Doctor ID to verify: ");
  scanf("%d", &doctorID);
  getchar();

  if (doctorID < 0  || idExists(doctorIDs, totalDoctors, doctorID) == -1) {
    printf("Invalid ID or non-existent ID!\n");
    return;
  }

  printf("Enter the patient ID (positive number): ");
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
  printf("Patient ID\tName\t\t\tAge\tDiagnosis\tRoom Number\n");
  for(int i = 0; i < totalPatients; i++) {
    printf("%d\t\t%s\t\t%d\t%s\t\t%d\n", patientIDs[i], patientNames[i], patientAges[i], patientDiagnosis[i],patientRoomNums[i]);
  }
}

void searchPatientRecord () {
  int inputMethod;
  int foundPatientID = 0;
  printf("Do you want to search by patient's ID or name?\n1.By ID\n2.By name");
  scanf("%d", &inputMethod);
  getchar();
  switch (inputMethod) {
    case 1:
      int patientID;
      printf("Enter the patient ID: \n");
      scanf("%d", &patientID);
      getchar();
      for(int i = 0; i < totalPatients; i++) {
        if (patientIDs[i] == patientID) {
          foundPatientID = 1;
          printf("Patient ID\tName\t\t\tAge\tDiagnosis\tRoom Number\n");
          printf("%d\t\t%s\t\t%d\t%s\t\t%d\n", patientIDs[i], patientNames[i], patientAges[i], patientDiagnosis[i],patientRoomNums[i]);
          break;
        }
      }
      break;
    case 2:
      char patientName[50];
      printf("Enter the patient name: ");
      fgets(patientName, sizeof(patientName), stdin);
      getchar();
      for(int i = 0; i < totalPatients; i++) {
        if (patientNames[i] == patientName) {
          foundPatientID = 1;
          printf("Patient ID\tName\t\t\tAge\tDiagnosis\tRoom Number\n");
          printf("%d\t\t%s\t\t%d\t%s\t\t%d\n", patientIDs[i], patientNames[i], patientAges[i], patientDiagnosis[i],patientRoomNums[i]);
          break;
        }
      }
      break;
      default:
        printf("Invalid Input!\n");
        return;
  }
  if (foundPatientID == 0) {
    printf("The patient is not found!\n");
  }
  return;
}

void dischargePatientRecord () {
  int inputMethod;
  int foundPatientID = 0;
  int patientIndex;
  printf("Do you want to search by patient's ID or name?\n1.By ID\n2.By name");
  scanf("%d", &inputMethod);
  getchar();
  switch (inputMethod) {
    case 1:
      int patientID;
    printf("Enter the patient ID: ");
    scanf("%d", &patientID);
    getchar();
    for(int i = 0; i < totalPatients; i++) {
      if (patientIDs[i] == patientID) {
        foundPatientID = 1;
        patientIndex = i;
        break;
      }
    }
    break;
    case 2:
      char patientName[50];
    printf("Enter the patient name: ");
    fgets(patientName, sizeof(patientName), stdin);
    getchar();
    for(int i = 0; i < totalPatients; i++) {
      if (patientNames[i] == patientName) {
        foundPatientID = 1;
        patientIndex = i;
        break;
      }
    }
    break;
    default:
      printf("Invalid Input!\n");
    return;
  }
  if (foundPatientID == 0) {
    printf("The patient is not found!\n");
    return;
  }else{
    totalPatients--;
    for(int j = patientIndex; j < totalPatients-1; j++) {
      patientIDs[j] = patientIDs[j+1];
      strcpy(patientNames[j+1], patientNames[j]);
      patientAges[j] = patientAges[j+1];
      strcpy(patientDiagnosis[j+1],patientDiagnosis[j]);
      patientRoomNums[j] = patientRoomNums[j+1];
    }
  }
  return;
}


void manageDoctorSchedule () {

}

void addDoctor () {
  if (totalDoctors >= MAX_DOCTORS) {
    printf("Too many doctors!\n");
    return;
  }

  int doctorID;
  char doctorName[50];

  printf("Enter Doctor ID (positive number): ");
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
  printf("Doctor ID\tName\n");
  for(int i = 0; i < totalDoctors; i++) {
    printf("%d\t\t%s\n", doctorIDs[i], doctorNames[i]);
  }
}

int main() {
  printf("%d", totalPatients);
  menu();
  return 0;
}

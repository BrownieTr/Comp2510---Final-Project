//
// Created by tkngu on 2025-02-11.
//

#include <stdio.h>
#include <string.h>

#define MAX_PATIENTS 50
#define MAX_DAYS_IN_WEEK 7
#define MAX_SHIFTS_IN_DAY 3

// Arrays to store patient's record details
int patientIDs[MAX_PATIENTS];
char patientNames[MAX_PATIENTS][50];
int patientAges[MAX_PATIENTS];
char patientDiagnosis[MAX_PATIENTS][250];
int patientRoomNums[MAX_PATIENTS];

// Array to manage doctor schedule
int doctorSchedule[MAX_DAYS_IN_WEEK][MAX_SHIFTS_IN_DAY];

// Function prototypes
void addPatientRecord();
void viewPatientRecords();
void searchPatientRecord();
void dischargePatientRecord();
void manageDoctorSchedule();

void menu() {
  int choice;
  do {
    printf("1. Add Patient record\n");
    printf("2. View All Patients\n");
    printf("3. Search Patient\n");
    printf("4. Discharge Patient\n");
    printf("5. Manage Doctor Schedule\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1: addPatientRecord(); break;
      case 2: viewPatientRecords(); break;
      case 3: searchPatientRecord(); break;
      case 4: dischargePatientRecord(); break;
      case 5: manageDoctorSchedule(); break;
      case 6: printf("Exiting...\n"); break;
      default: printf("Invalid choice! Try again.\n");
    }
  } while (choice != 6);
}


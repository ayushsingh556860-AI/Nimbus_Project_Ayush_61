#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_MAX_LEN   50
#define VACCINE_MAX_LEN 20
#define STATUS_MAX_LEN  24
#define DATE_STR_LEN    11   /* dd/mm/yyyy + '\0' */
#define LINE_BUF_LEN    256

typedef struct {
    int  citizenId;
    char fullName[NAME_MAX_LEN];
    int  age;
    char vaccineName[VACCINE_MAX_LEN];
    int  dosesTaken;                 /* 0, 1 or 2 */
    char status[STATUS_MAX_LEN];     /* Not / Partially / Fully vaccinated */
    char lastDoseDate[DATE_STR_LEN];
    char nextDoseDate[DATE_STR_LEN];
} CitizenRecord;

typedef struct {
    char vaccineName[VACCINE_MAX_LEN];
    int  availableDoses;
} VaccineStock;

/* Global dynamic array for citizens */
static CitizenRecord *gCitizens = NULL;
static size_t gCitizenCount = 0u;
static size_t gCitizenCapacity = 0u;

/* File where we store all records in text form */
static const char *CITIZEN_FILE = "citizens.txt";

static void printBanner(void) {
    printf("-----------------------------------------\n");
    printf("COVID Vaccination System\n");
    printf("Developed by: Ayush Singh\n");
    printf("-----------------------------------------\n\n");
}

static void pressEnterToContinue(void) {
    int ch;
    printf("\nPress Enter to continue...");
    /* Clear buffer until newline */
    do {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}

/* Ensure we have room for one more citizen in dynamic array */
static int ensureCapacity(void) {
    if (gCitizenCount < gCitizenCapacity) {
        return 1;
    }

    size_t newCapacity = (gCitizenCapacity == 0u) ? 4u : gCitizenCapacity * 2u;
    CitizenRecord *tmp = (CitizenRecord *)realloc(gCitizens,
                                                  newCapacity * sizeof(*gCitizens));
    if (tmp == NULL) {
        printf("Memory allocation failed. Cannot add more records.\n");
        return 0;
    }

    gCitizens = tmp;
    gCitizenCapacity = newCapacity;
    return 1;
}

/* Trim newline from fgets result */
static void trimNewline(char *s) {
    size_t len = strlen(s);
    if (len > 0u && s[len - 1u] == '\n') {
        s[len - 1u] = '\0';
    }
}

/*
   File format (one citizen per line, '|' separated):

   id|name|age|vaccine|doses|status|lastDate|nextDate\n
*/
static void loadFromFile(void) {
    FILE *fp = fopen(CITIZEN_FILE, "r");
    if (fp == NULL) {
        /* First run: no file yet. That's fine. */
        return;
    }

    char buffer[LINE_BUF_LEN];
    while (fgets(buffer, (int)sizeof(buffer), fp) != NULL) {
        trimNewline(buffer);
        CitizenRecord temp;
        char last[DATE_STR_LEN];
        char next[DATE_STR_LEN];
        char nameBuf[NAME_MAX_LEN];
        char vaccineBuf[VACCINE_MAX_LEN];
        char statusBuf[STATUS_MAX_LEN];

        int scanned = sscanf(buffer,
                             "%d|%49[^|]|%d|%19[^|]|%d|%23[^|]|%10[^|]|%10[^|\n]",
                             &temp.citizenId,
                             nameBuf,
                             &temp.age,
                             vaccineBuf,
                             &temp.dosesTaken,
                             statusBuf,
                             last,
                             next);

        if (scanned == 8) {
            if (!ensureCapacity()) {
                break;
            }
            (void)strncpy(temp.fullName, nameBuf, NAME_MAX_LEN - 1);
            temp.fullName[NAME_MAX_LEN - 1] = '\0';

            (void)strncpy(temp.vaccineName, vaccineBuf, VACCINE_MAX_LEN - 1);
            temp.vaccineName[VACCINE_MAX_LEN - 1] = '\0';

            (void)strncpy(temp.status, statusBuf, STATUS_MAX_LEN - 1);
            temp.status[STATUS_MAX_LEN - 1] = '\0';

            (void)strncpy(temp.lastDoseDate, last, DATE_STR_LEN - 1);
            temp.lastDoseDate[DATE_STR_LEN - 1] = '\0';

            (void)strncpy(temp.nextDoseDate, next, DATE_STR_LEN - 1);
            temp.nextDoseDate[DATE_STR_LEN - 1] = '\0';

            gCitizens[gCitizenCount] = temp;
            gCitizenCount++;
        }
    }

    (void)fclose(fp);
}

static void saveToFile(void) {
    FILE *fp = fopen(CITIZEN_FILE, "w");
    if (fp == NULL) {
        printf("Unable to open '%s' for writing.\n", CITIZEN_FILE);
        return;
    }

    for (size_t i = 0u; i < gCitizenCount; ++i) {
        const CitizenRecord *c = &gCitizens[i];
        /* Use '|' as separator to keep parsing easy */
        (void)fprintf(fp,
                      "%d|%s|%d|%s|%d|%s|%s|%s\n",
                      c->citizenId,
                      c->fullName,
                      c->age,
                      c->vaccineName,
                      c->dosesTaken,
                      c->status,
                      c->lastDoseDate,
                      c->nextDoseDate);
    }

    (void)fclose(fp);
}

/* Returns index in array or -1 if not found */
static int findCitizenIndexById(int id) {
    for (size_t i = 0u; i < gCitizenCount; ++i) {
        if (gCitizens[i].citizenId == id) {
            return (int)i;
        }
    }
    return -1;
}

static int findVaccineIndex(const VaccineStock stock[], size_t stockCount,
                            const char *name) {
    for (size_t i = 0u; i < stockCount; ++i) {
        if (strcmp(stock[i].vaccineName, name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static void addNewCitizen(void) {
    CitizenRecord rec;
    int id;
    int age;
    int flushChar;

    printf("\n--- Register New Citizen ---\n");
    printf("Enter ID (number): ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    if (findCitizenIndexById(id) != -1) {
        printf("Citizen with this ID already exists.\n");
        return;
    }
    rec.citizenId = id;

    /* clear newline from buffer after scanf */
    do {
        flushChar = getchar();
    } while (flushChar != '\n' && flushChar != EOF);

    printf("Enter Full Name: ");
    if (fgets(rec.fullName, NAME_MAX_LEN, stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    trimNewline(rec.fullName);

    printf("Enter Age: ");
    if (scanf("%d", &age) != 1) {
        printf("Invalid age.\n");
        return;
    }
    rec.age = age;

    do {
        flushChar = getchar();
    } while (flushChar != '\n' && flushChar != EOF);

    printf("Enter Vaccine Name (e.g., COVAXIN/COVISHIELD/SPUTNIK): ");
    if (fgets(rec.vaccineName, VACCINE_MAX_LEN, stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    trimNewline(rec.vaccineName);

    rec.dosesTaken = 0;
    (void)strncpy(rec.status, "Not Vaccinated", STATUS_MAX_LEN - 1);
    rec.status[STATUS_MAX_LEN - 1] = '\0';
    (void)strncpy(rec.lastDoseDate, "N/A", DATE_STR_LEN - 1);
    rec.lastDoseDate[DATE_STR_LEN - 1] = '\0';
    (void)strncpy(rec.nextDoseDate, "N/A", DATE_STR_LEN - 1);
    rec.nextDoseDate[DATE_STR_LEN - 1] = '\0';

    if (!ensureCapacity()) {
        return;
    }

    gCitizens[gCitizenCount] = rec;
    gCitizenCount++;

    saveToFile();
    printf("Citizen registered successfully.\n");
}

static void showAllCitizens(void) {
    printf("\n--- All Registered Citizens ---\n");
    if (gCitizenCount == 0u) {
        printf("No records found.\n");
        return;
    }

    for (size_t i = 0u; i < gCitizenCount; ++i) {
        const CitizenRecord *c = &gCitizens[i];
        printf("\nID           : %d\n", c->citizenId);
        printf("Name         : %s\n", c->fullName);
        printf("Age          : %d\n", c->age);
        printf("Vaccine      : %s\n", c->vaccineName);
        printf("Doses Taken  : %d\n", c->dosesTaken);
        printf("Status       : %s\n", c->status);
        printf("Last Dose    : %s\n", c->lastDoseDate);
        printf("Next Dose    : %s\n", c->nextDoseDate);
    }
}

static void updateVaccination(VaccineStock stock[], size_t stockCount) {
    int id;
    char today[DATE_STR_LEN];
    int flushChar;

    printf("\n--- Vaccination Update ---\n");
    printf("Enter Citizen ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    int index = findCitizenIndexById(id);
    if (index < 0) {
        printf("Citizen not found. Please register first.\n");
        return;
    }

    CitizenRecord *c = &gCitizens[(size_t)index];

    if (c->age < 18) {
        printf("Citizen is under 18 and not eligible.\n");
        return;
    }

    if (c->dosesTaken >= 2) {
        printf("Citizen is already fully vaccinated.\n");
        return;
    }

    int vIndex = findVaccineIndex(stock, stockCount, c->vaccineName);
    if (vIndex < 0) {
        printf("Vaccine '%s' not present in stock list.\n", c->vaccineName);
        return;
    }

    VaccineStock *v = &stock[(size_t)vIndex];
    if (v->availableDoses <= 0) {
        printf("No stock available for %s.\n", v->vaccineName);
        return;
    }

    do {
        flushChar = getchar();
    } while (flushChar != '\n' && flushChar != EOF);

    printf("Enter today's date (dd/mm/yyyy): ");
    if (fgets(today, DATE_STR_LEN, stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    trimNewline(today);

    v->availableDoses--;
    c->dosesTaken++;
    (void)strncpy(c->lastDoseDate, today, DATE_STR_LEN - 1);
    c->lastDoseDate[DATE_STR_LEN - 1] = '\0';

    if (c->dosesTaken == 1) {
        (void)strncpy(c->status, "Partially Vaccinated", STATUS_MAX_LEN - 1);
        c->status[STATUS_MAX_LEN - 1] = '\0';

        printf("Enter scheduled date for 2nd dose (dd/mm/yyyy): ");
        if (fgets(c->nextDoseDate, DATE_STR_LEN, stdin) == NULL) {
            printf("Input error.\n");
            return;
        }
        trimNewline(c->nextDoseDate);
    } else {
        (void)strncpy(c->status, "Fully Vaccinated", STATUS_MAX_LEN - 1);
        c->status[STATUS_MAX_LEN - 1] = '\0';
        (void)strncpy(c->nextDoseDate, "N/A", DATE_STR_LEN - 1);
        c->nextDoseDate[DATE_STR_LEN - 1] = '\0';
    }

    saveToFile();
    printf("Vaccination details updated successfully.\n");
}

static void manageStock(VaccineStock stock[], size_t stockCount) {
    int choice = -1;
    while (choice != 0) {
        printf("\n--- Vaccine Stock Management ---\n");
        printf("1. View Stock\n");
        printf("2. Update Stock Quantity\n");
        printf("0. Back to Main Menu\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");
            /* clear buffer */
            int ch;
            do {
                ch = getchar();
            } while (ch != '\n' && ch != EOF);
            choice = -1;
            continue;
        }

        if (choice == 1) {
            printf("\nCurrent Stock:\n");
            for (size_t i = 0u; i < stockCount; ++i) {
                printf("%s : %d doses\n",
                       stock[i].vaccineName,
                       stock[i].availableDoses);
            }
        } else if (choice == 2) {
            char name[VACCINE_MAX_LEN];
            int qty;
            int flushChar;

            do {
                flushChar = getchar();
            } while (flushChar != '\n' && flushChar != EOF);

            printf("Enter vaccine name to update: ");
            if (fgets(name, VACCINE_MAX_LEN, stdin) == NULL) {
                printf("Input error.\n");
                continue;
            }
            trimNewline(name);

            int idx = findVaccineIndex(stock, stockCount, name);
            if (idx < 0) {
                printf("Vaccine not found in list.\n");
            } else {
                printf("Enter new quantity: ");
                if (scanf("%d", &qty) != 1) {
                    printf("Invalid quantity.\n");
                } else {
                    stock[(size_t)idx].availableDoses = qty;
                    printf("Stock updated for %s.\n", stock[(size_t)idx].vaccineName);
                }
            }
        } else if (choice == 0) {
            /* Back to main menu */
        } else {
            printf("Invalid option. Try again.\n");
        }
    }
}

static void dailyReport(void) {
    char date[DATE_STR_LEN];
    int flushChar;

    if (gCitizenCount == 0u) {
        printf("\nNo citizens registered. Report not available.\n");
        return;
    }

    do {
        flushChar = getchar();
    } while (flushChar != '\n' && flushChar != EOF);

    printf("\n--- Daily Vaccination Report ---\n");
    printf("Enter date (dd/mm/yyyy): ");
    if (fgets(date, DATE_STR_LEN, stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    trimNewline(date);

    size_t totalToday = 0u;
    size_t dose1Today = 0u;
    size_t dose2Today = 0u;
    size_t notVac = 0u;
    size_t partial = 0u;
    size_t full = 0u;

    for (size_t i = 0u; i < gCitizenCount; ++i) {
        const CitizenRecord *c = &gCitizens[i];

        if (strcmp(c->lastDoseDate, date) == 0) {
            totalToday++;
            if (c->dosesTaken == 1) {
                dose1Today++;
            } else if (c->dosesTaken == 2) {
                dose2Today++;
            }
        }

        if (strcmp(c->status, "Not Vaccinated") == 0) {
            notVac++;
        } else if (strcmp(c->status, "Partially Vaccinated") == 0) {
            partial++;
        } else if (strcmp(c->status, "Fully Vaccinated") == 0) {
            full++;
        }
    }

    printf("\nReport for %s\n", date);
    printf("Total vaccinations today : %zu\n", totalToday);
    printf("Dose 1 given today       : %zu\n", dose1Today);
    printf("Dose 2 given today       : %zu\n", dose2Today);

    printf("\nOverall Summary (All Days):\n");
    printf("Total citizens registered : %zu\n", gCitizenCount);
    printf("Not Vaccinated            : %zu\n", notVac);
    printf("Partially Vaccinated      : %zu\n", partial);
    printf("Fully Vaccinated          : %zu\n", full);
}

static void pendingSecondDoseList(void) {
    printf("\n--- Citizens Pending Second Dose ---\n");
    size_t count = 0u;

    for (size_t i = 0u; i < gCitizenCount; ++i) {
        const CitizenRecord *c = &gCitizens[i];
        if (c->dosesTaken == 1 &&
            strcmp(c->status, "Partially Vaccinated") == 0) {
            printf("\nID        : %d\n", c->citizenId);
            printf("Name      : %s\n", c->fullName);
            printf("Vaccine   : %s\n", c->vaccineName);
            printf("Last Dose : %s\n", c->lastDoseDate);
            printf("Next Due  : %s\n", c->nextDoseDate);
            count++;
        }
    }

    if (count == 0u) {
        printf("No citizens are pending their second dose.\n");
    }
}

int main(void) {
    VaccineStock stockList[] = {
        {"COVAXIN",    100},
        {"COVISHIELD", 100},
        {"SPUTNIK",    100}
    };
    size_t stockCount = sizeof(stockList) / sizeof(stockList[0]);

    int choice = -1;

    printBanner();
    loadFromFile();

    while (choice != 0) {
        printf("\n=========== MAIN MENU ===========\n");
        printf("1. Register New Citizen\n");
        printf("2. Give Dose / Update Vaccination\n");
        printf("3. View All Citizens\n");
        printf("4. Daily Vaccination Report\n");
        printf("5. Pending Second Dose Alerts\n");
        printf("6. Manage Vaccine Stock\n");
        printf("0. Exit Program\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            /* Clear bad input */
            int ch;
            do {
                ch = getchar();
            } while (ch != '\n' && ch != EOF);
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                addNewCitizen();
                pressEnterToContinue();
                break;
            case 2:
                updateVaccination(stockList, stockCount);
                pressEnterToContinue();
                break;
            case 3:
                showAllCitizens();
                pressEnterToContinue();
                break;
            case 4:
                dailyReport();
                pressEnterToContinue();
                break;
            case 5:
                pendingSecondDoseList();
                pressEnterToContinue();
                break;
            case 6:
                manageStock(stockList, stockCount);
                pressEnterToContinue();
                break;
            case 0:
                printf("\nSaving data and exiting...\n");
                saveToFile();
                printf("\n-----------------------------------------\n");
                printf("Thank you for using the system.\n");
                printf("Developed by: Ayush Singh\n");
                printf("-----------------------------------------\n");
                break;
            default:
                printf("Unknown option. Please try again.\n");
                pressEnterToContinue();
                break;
        }
    }

    free(gCitizens);
    return 0;
}
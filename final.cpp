#include <iostream>
#include <sqlite3.h>

using namespace std;

void createTable(sqlite3* db) {
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS Students ("
                                "StudentID INTEGER PRIMARY KEY,"
                                "FirstName TEXT NOT NULL,"
                                "LastName TEXT NOT NULL,"
                                "Major TEXT NOT NULL,"
                                "GraduationYear INTEGER NOT NULL);";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "Failed to create table: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Table created successfully\n";
    }
}


void displayRecords(sqlite3* db) {
    const char* query = "SELECT * FROM Students";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "Student ID\tFirst Name\tLast Name\tMajor\tGraduation Year\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cout << sqlite3_column_int(stmt, 0) << "\t\t" << sqlite3_column_text(stmt, 1) << "\t\t"
             << sqlite3_column_text(stmt, 2) << "\t\t" << sqlite3_column_text(stmt, 3) << "\t\t"
             << sqlite3_column_int(stmt, 4) << endl;
    }

    sqlite3_finalize(stmt);
}

void deleteStudent(sqlite3* db, int studentId) {
    const char* query = "DELETE FROM Students WHERE StudentID = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, studentId); // bind studentId to the first placeholder

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Student with ID " << studentId << " deleted successfully.\n";
    } else {
        cout << "Student with ID " << studentId << " not found.\n";
    }

    sqlite3_finalize(stmt);
}

void updateGraduationYear(sqlite3* db, int studentId, int newGradYear) {
    const char* query = "UPDATE Students SET GraduationYear = ? WHERE StudentID = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, newGradYear);
    sqlite3_bind_int(stmt, 2, studentId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Graduation year updated successfully.\n";
    } else {
        cout << "Student with ID " << studentId << " not found.\n";
    }

    sqlite3_finalize(stmt);
}

void insertStudent(sqlite3* db, const char* firstName, const char* lastName, const char* major, int gradYear) {
    const char* query = "INSERT INTO Students (FirstName, LastName, Major, GraduationYear) VALUES (?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, firstName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, lastName, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, major, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, gradYear);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "Student inserted successfully.\n";
    } else {
        cout << "Failed to insert student.\n";
    }

    sqlite3_finalize(stmt);
}

int main() {
    sqlite3* db;
    int rc = sqlite3_open("Student.accdb", &db);

    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    cout << "Opened database successfully\n";

    createTable(db);

    char choice;

    do {
        cout << "\nPress (S) to display all students records\n"
             << "Press (D) to delete a student by ID#\n"
             << "Press (U) to update a students graduation year\n"
             << "Press (I) to insert a new student\n"
             << "Press (Q) to quit\n";

        cin >> choice;

        switch (choice) {

            case 'S':
            case 's':
                displayRecords(db);
                break;

            case 'D':
            case 'd': {
                int studentId;
                cout << "Enter student ID to delete: ";
                cin >> studentId;
                deleteStudent(db, studentId);
                break;
            }

            case 'U':
            case 'u': {
                int studentId, newGradYear;
                cout << "Enter student ID to update: ";
                cin >> studentId;
                cout << "Enter new graduation year: ";
                cin >> newGradYear;
                updateGraduationYear(db, studentId, newGradYear);
                break;
            }

            case 'I':
            case 'i': {
                char firstName[50], lastName[50], major[50];
                int gradYear;
                cout << "Enter first name: ";
                cin >> firstName;
                cout << "Enter last name: ";
                cin.getline(lastName, 50);
                cin >> lastName;
                cout << "Enter major: ";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.getline(major, 50);
                cout << "Enter graduation year: ";
                cin >> gradYear;
                insertStudent(db, firstName, lastName, major, gradYear);
                break;
            }

            case 'Q':
            case 'q':
                cout << "Quitting the program.\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 'Q' && choice != 'q');

    sqlite3_close(db);
    return 0;
}

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class PhoneRecord;

class PhoneBook;

ostream &operator<<(ostream &out, const PhoneRecord *phoneRecord);

ifstream &operator>>(ifstream &in, const PhoneRecord *phoneRecord);

class PhoneRecord {
private:
    string firstName;
    string lastName;
    string phoneNumber;
    PhoneRecord *left;
    PhoneRecord *right;

public:
    // constructor to initialize left and right to NULL
    PhoneRecord() {
        this->left = nullptr;
        this->right = nullptr;
    }

    // parameterized constructor to create Phone Record from First Name, Last Name
    // and Phone Number
    PhoneRecord(string firstName, string lastName, string phoneNumber) {
        this->firstName = firstName;
        this->lastName = lastName;
        this->phoneNumber = phoneNumber;
        this->left = nullptr;
        this->right = nullptr;
    }

    void setFirstName(string firstName) { this->firstName = firstName; }

    void setLastName(string lastName) { this->lastName = lastName; }

    void setPhoneNumber(string phoneNumber) { this->phoneNumber = phoneNumber; }

    // Insert a Phone Record
    void insert(PhoneRecord *phoneRecord) {
        if (phoneRecord->lastName < this->lastName) {
            if (this->left == nullptr) {
                this->left = phoneRecord;
            }
            this->left->insert(phoneRecord);
        } else if (phoneRecord->lastName > this->lastName) {
            if (this->right == nullptr) {
                this->right = phoneRecord;
            }
            this->right->insert(phoneRecord);
        }

        return;
    }

    // Inorder traversal to display in sorted order
    void inorder() {
        if (this->left != nullptr) {
            this->left->inorder();
        }

        cout << "* " << this->lastName << ", " << this->firstName << " ["
             << this->phoneNumber << "]" << endl;

        if (this->right != nullptr) {
            this->right->inorder();
        }
    }

    // Search for a Phone Record with Last Name
    PhoneRecord *search(string lastName) {
        if (this == nullptr) {
            return nullptr;
        } else if (lastName == this->lastName) {
            return this;
        } else if (lastName < this->lastName) {
            return this->left->search(lastName);
        } else if (lastName > this->lastName) {
            return this->right->search(lastName);
        }
    }

    // Delete a phone record
    void destroy(PhoneRecord *&phoneRecord) {
        // Save a reference of the PhoneRecord to be deleted in a temporary variable
        PhoneRecord *temp = phoneRecord;
        if (phoneRecord != nullptr) {

            if (phoneRecord->left == nullptr && phoneRecord->right == nullptr) {
                phoneRecord = nullptr;
            } else if (phoneRecord->right == nullptr) {
                // PhoneRecord has no right child, we attach the left child to its
                // parent.
                phoneRecord = phoneRecord->left;
            } else if (phoneRecord->left == nullptr) {
                // PhoneRecord has no left child, we attach the right child to its
                // parent.
                phoneRecord = phoneRecord->right;
            } else {
                // move left
                temp = phoneRecord->left;

                // then find the right most child
                while (temp->right != nullptr) {
                    temp = temp->right;
                }

                // The phoneRecord's left child's rightmost child is now the parent of
                // the phoneRecord's right child
                temp->right = phoneRecord->right;

                // save phoneRecord to temporary variable again
                temp = phoneRecord;

                // phoneRecord is now replaced by phoneRecord's left child
                phoneRecord = phoneRecord->left;
            }

            // temp holds the reference of the node to be deleted
            delete temp;
        }
    }

    // Convert the BST to a List to write to file.
    vector<PhoneRecord *> toList(vector<PhoneRecord *> &list) {
        if (this->left != nullptr) {
            this->left->toList(list);
        }
        list.push_back(this);
        if (this->right != NULL) {
            this->right->toList(list);
        }
        return list;
    }

    friend ostream &operator<<(ostream &out, const PhoneRecord *phoneRecord);

    friend ifstream &operator>>(ifstream &in, PhoneRecord *phoneRecord);
};

// operator overloaded to display on console and write to file.
ostream &operator<<(ostream &out, const PhoneRecord *phoneRecord) {
    if (&out == &cout) {
        // if displaying on console
        return out << phoneRecord->lastName << ", " << phoneRecord->firstName
                   << " [" << phoneRecord->phoneNumber << "] ";
    } else {
        // if writing to file
        return out << phoneRecord->lastName << "," << phoneRecord->firstName << ","
                   << phoneRecord->phoneNumber;
    }
}

// operator overloaded to read from file
ifstream &operator>>(ifstream &in, PhoneRecord *phoneRecord) {
    string row;
    string cell;

    getline(in, row);
    stringstream ss(row);

    getline(ss, cell, ',');
    phoneRecord->setFirstName(cell);

    getline(ss, cell, ',');
    phoneRecord->setLastName(cell);

    getline(ss, cell, ',');
    phoneRecord->setPhoneNumber(cell);

    return in;
}

class PhoneBook {
private:
    PhoneRecord *_phoneBook = nullptr;
    vector<PhoneRecord *> _phoneDirectory;

public:
    void add(string firstName, string lastName, string phoneNumber) {
        PhoneRecord *phoneRecord =
                new PhoneRecord(firstName, lastName, phoneNumber);
        cout << "Adding Phone Record " << phoneRecord << endl;

        if (_phoneBook == nullptr) {
            _phoneBook = phoneRecord;
        } else {
            _phoneBook->insert(phoneRecord);
        }
    }

    void add(PhoneRecord *phoneRecord) {
        if (_phoneBook == nullptr) {
            _phoneBook = phoneRecord;
        } else {
            _phoneBook->insert(phoneRecord);
        }
    }

    void search(string lastName) {
        PhoneRecord *phoneRecord = _phoneBook->search(lastName);
        if (phoneRecord != nullptr) {
            cout << "Phone Record found : " << phoneRecord << endl;
        } else {
            cout << "Phone Record Not found for " << lastName << endl;
        }
    }

    void destroy(string lastName) {
        PhoneRecord *phoneRecord = _phoneBook->search(lastName);
        if (phoneRecord == nullptr) {
            cout << "Phone Record Not found for " << lastName << endl;
        } else {
            cout << "Phone Record " << phoneRecord << " deleted" << endl;
            _phoneBook->destroy(phoneRecord);
        }
    }

    void load(char *filePath = "PhoneBook.txt") {
        cout << "Loading Phone Records from " << filePath << endl;
        ifstream phoneDirectory(filePath);
        int records = 0;

        while (phoneDirectory.is_open() && !phoneDirectory.eof()) {
            PhoneRecord *phoneRecord = new PhoneRecord();
            phoneDirectory >> phoneRecord;
            this->add(phoneRecord);
            cout << "Imported Phone Record: " << phoneRecord << endl;
            records++;
        }

        cout << "Imported " << records << " records successfully" << endl;
        phoneDirectory.close();
    }

    void save(char *filePath = "PhoneBook.txt") {
        fstream phoneDirectory(filePath);
        _phoneBook->toList(_phoneDirectory);

        for (PhoneRecord *phoneRecord: _phoneDirectory) {
            phoneDirectory << phoneRecord << endl;
        }

        cout << "Phone Records saved to " << filePath << endl;
        phoneDirectory.close();
    }

    void show() {
        if (_phoneBook == nullptr) {
            cout << "Phone Book is empty" << endl;
        } else {
            _phoneBook->inorder();
        }
    }
};

int main() {
    cout << "__________.__                           __________               __    " << endl;
    cout << "\\______   \\  |__   ____   ____   ____   \\______   \\ ____   ____ |  | __" << endl;
    cout << " |     ___/  |  \\ /  _ \\ /    \\_/ __ \\   |    |  _//  _ \\ /  _ \\|  |/ /" << endl;
    cout << " |    |   |   Y  (  <_> )   |  \\  ___/   |    |   (  <_> |  <_> )    < " << endl;
    cout << " |____|   |___|  /\\____/|___|  /\\___  >  |______  /\\____/ \\____/|__|_ \\" << endl;
    cout << "               \\/            \\/     \\/          \\/                   \\/" << endl;

    int option;
    PhoneBook *phoneBook = new PhoneBook();
    string firstName, lastName, phoneNumber;

    while (option != 0) {
        cout << endl << endl << endl;
        cout << "Welcome to PhoneBook" << endl;
        cout << "[1] Add a New Phone Record" << endl;
        cout << "[2] Search for a Phone Record" << endl;
        cout << "[3] Delete a Phone Record" << endl;
        cout << "[4] Load Phone Records from a saved file" << endl;
        cout << "[5] Save Phone Records to a file" << endl;
        cout << "[6] Display Phone Records in sorted order" << endl;
        cout << "[0] Exit" << endl;
        cout << "Select from the above options: ";
        cin >> option;

        switch (option) {
            case 1:
                cout << "Enter the details to add a Phone Record" << endl;
                cout << "Enter the First Name: ";
                cin >> firstName;
                cout << "Enter the Last Name: ";
                cin >> lastName;
                cout << "Enter the Phone Number: ";
                cin >> phoneNumber;
                phoneBook->add(firstName, lastName, phoneNumber);
                break;
            case 2:
                cout << "Enter the last name to search: ";
                cin >> lastName;
                phoneBook->search(lastName);
                break;
            case 3:
                cout << "Enter the last name to delete: ";
                cin >> lastName;
                phoneBook->destroy(lastName);
                break;
            case 4:
                phoneBook->load();
                break;
            case 5:
                phoneBook->save();
                break;
            case 6:
                cout << "The Phone Records are:" << endl;
                phoneBook->show();
                break;
            case 0:
                cout << "Exiting the application" << endl;
                exit(0);
            default:
                cout << "Invalid option. Please try again" << endl;
                break;
        }
    }

    return 0;
}

// compile
// $ g++ main.cpp -std=c++11 --no-warnings -o main

// run
// $ ./main
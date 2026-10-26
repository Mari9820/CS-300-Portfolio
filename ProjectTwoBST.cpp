#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

string uppercase(string text) {
    for (size_t i = 0; i < text.length(); ++i)
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    return text;
}

struct Node {
    Course course;
    Node* left;
    Node* right;
    explicit Node(const Course& value) : course(value), left(NULL), right(NULL) {}
};

class BinarySearchTree {
private:
    Node* root;
    size_t count;

    static void deleteNodes(Node* node) {
        if (node == NULL) return;
        deleteNodes(node->left);
        deleteNodes(node->right);
        delete node;
    }

    static void printInOrder(Node* node) {
        if (node == NULL) return;
        printInOrder(node->left);
        cout << node->course.number << ", " << node->course.title << endl;
        printInOrder(node->right);
    }

    BinarySearchTree(const BinarySearchTree&);
    BinarySearchTree& operator=(const BinarySearchTree&);

public:
    BinarySearchTree() : root(NULL), count(0) {}
    ~BinarySearchTree() { clear(); }

    void clear() {
        deleteNodes(root);
        root = NULL;
        count = 0;
    }

    bool empty() const { return root == NULL; }
    size_t size() const { return count; }

    void insert(const Course& course) {
        Node** current = &root;
        while (*current != NULL) {
            if (course.number < (*current)->course.number)
                current = &((*current)->left);
            else if (course.number > (*current)->course.number)
                current = &((*current)->right);
            else {
                (*current)->course = course;
                return;
            }
        }
        *current = new Node(course);
        ++count;
    }

    const Course* search(const string& number) const {
        Node* current = root;
        while (current != NULL) {
            if (number == current->course.number) return &(current->course);
            if (number < current->course.number) current = current->left;
            else current = current->right;
        }
        return NULL;
    }

    void printCourses() const { printInOrder(root); }

    void swap(BinarySearchTree& other) {
        Node* tempRoot = root;
        root = other.root;
        other.root = tempRoot;
        size_t tempCount = count;
        count = other.count;
        other.count = tempCount;
    }
};

bool loadCourses(const string& filename, BinarySearchTree& courses) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Unable to open file: " << filename << endl;
        return false;
    }

    BinarySearchTree loadedCourses;
    string line;
    while (getline(file, line)) {
        if (trim(line).empty()) continue;
        stringstream stream(line);
        Course course;
        getline(stream, course.number, ',');
        getline(stream, course.title, ',');
        course.number = uppercase(trim(course.number));
        course.title = trim(course.title);
        if (course.number.empty() || course.title.empty()) {
            cout << "Skipping an incomplete course record." << endl;
            continue;
        }
        string prerequisite;
        while (getline(stream, prerequisite, ',')) {
            prerequisite = uppercase(trim(prerequisite));
            if (!prerequisite.empty()) course.prerequisites.push_back(prerequisite);
        }
        loadedCourses.insert(course);
    }

    if (loadedCourses.empty()) {
        cout << "No valid courses were found." << endl;
        return false;
    }
    courses.swap(loadedCourses);
    cout << courses.size() << " courses loaded successfully." << endl;
    return true;
}

void printCourseList(const BinarySearchTree& courses) {
    if (courses.empty()) {
        cout << "Please load the course data first." << endl;
        return;
    }
    cout << "\nHere is a sample schedule:\n" << endl;
    courses.printCourses();
}

void printCourse(const BinarySearchTree& courses) {
    if (courses.empty()) {
        cout << "Please load the course data first." << endl;
        return;
    }
    string courseNumber;
    cout << "What course do you want to know about? ";
    getline(cin, courseNumber);
    courseNumber = uppercase(trim(courseNumber));
    const Course* course = courses.search(courseNumber);
    if (course == NULL) {
        cout << "Course not found. Please try again." << endl;
        return;
    }
    cout << course->number << ", " << course->title << endl;
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << course->prerequisites[i];
        }
        cout << endl;
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            const Course* prerequisite = courses.search(course->prerequisites[i]);
            if (prerequisite != NULL)
                cout << "  " << prerequisite->number << ": " << prerequisite->title << endl;
        }
    }
}

void displayMenu() {
    cout << "\n1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "\nWhat would you like to do? ";
}

int main() {
    BinarySearchTree courses;
    string choice;
    cout << "Welcome to the course planner." << endl;
    while (true) {
        displayMenu();
        getline(cin, choice);
        choice = trim(choice);
        if (choice == "1") {
            string filename;
            cout << "Enter the course data filename: ";
            getline(cin, filename);
            loadCourses(trim(filename), courses);
        } else if (choice == "2") {
            printCourseList(courses);
        } else if (choice == "3") {
            printCourse(courses);
        } else if (choice == "9") {
            cout << "Thank you for using the course planner!" << endl;
            break;
        } else {
            cout << choice << " is not a valid option." << endl;
        }
    }
    return 0;
}

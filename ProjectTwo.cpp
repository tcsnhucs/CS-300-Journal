// Thomas Comer
// Project 2 
// CS-300 8/15/2025

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <sstream>
using namespace std;

// Represents a course with its number, name, and prerequisites
class Course {
public:
    string courseNumber; // Course identifier
    string courseName;   // Course title
    vector<string> prerequisites; // List of prerequisite course numbers
    Course(const string& number, const string& name, const vector<string>& prereqs)
        : courseNumber(number), courseName(name), prerequisites(prereqs) {
    }
};

// Node for the binary search tree, holding a Course
class TreeNode {
public:
    Course data;         // Course data
    TreeNode* left;      // Left child
    TreeNode* right;     // Right child
    TreeNode(const Course& course) : data(course), left(nullptr), right(nullptr) {}
};

// Binary Search Tree for storing courses
class CourseBST {
private:
    TreeNode* root; // Root node of the BST

    // Recursively insert a new course into the BST
    TreeNode* insertRecursive(TreeNode* node, const Course& newCourse) {
        if (node == nullptr) {
            return new TreeNode(newCourse); // Insert new node here
        }
        if (newCourse.courseNumber < node->data.courseNumber) {
            node->left = insertRecursive(node->left, newCourse); // Go left
        }
        else {
            node->right = insertRecursive(node->right, newCourse); // Go right
        }
        return node;
    }

    // Recursively find a course by its number
    TreeNode* findRecursive(TreeNode* node, const string& courseNumber) {
        if (node == nullptr) {
            return nullptr; // Not found
        }
        if (courseNumber == node->data.courseNumber) {
            return node; // Found
        }
        if (courseNumber < node->data.courseNumber) {
            return findRecursive(node->left, courseNumber); // Search left
        }
        else {
            return findRecursive(node->right, courseNumber); // Search right
        }
    }

    // In-order traversal to print courses in sorted order
    void inOrderTraversal(TreeNode* node) {
        if (node != nullptr) {
            inOrderTraversal(node->left);
            cout << node->data.courseNumber << ": " << node->data.courseName << endl;
            inOrderTraversal(node->right);
        }
    }
public:
    CourseBST() : root(nullptr) {}
    // Insert a new course into the BST
    void insert(const Course& newCourse) {
        root = insertRecursive(root, newCourse);
    }
    // Find a course by its number
    TreeNode* find(const string& courseNumber) {
        return findRecursive(root, courseNumber);
    }
    // Print all courses in order
    void inOrderTraversal() {
        inOrderTraversal(root);
    }
    // Get the root node
    TreeNode* getRoot() { return root; }
};

// Load course data from a file and populate the BST
void loadData(const string& filename, CourseBST& courseTree) {
    ifstream file;
    file.open(filename); // Try opening file directly
    if (!file.is_open()) {
        file.open("./" + filename); // Try with relative path
    }
    if (!file.is_open()) {
        char buffer[MAX_PATH];
        GetFullPathNameA(filename.c_str(), MAX_PATH, buffer, nullptr); // Try full path
        file.open(buffer);
    }
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'." << endl;
        return;
    }
    string line;
    vector<Course> allCoursesRaw; // Store all loaded courses for validation
    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines
        stringstream ss(line);
        string token;
        vector<string> tokens;
        // Split line by commas
        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t\r\n")); // Trim leading whitespace
            token.erase(token.find_last_not_of(" \t\r\n") + 1); // Trim trailing whitespace
            tokens.push_back(token);
        }
        if (tokens.size() < 2) {
            cout << "Error: Line format incorrect. Must contain at least course number and name." << endl;
            continue;
        }
        string courseNumber = tokens[0];
        string courseName = tokens[1];
        vector<string> prerequisites;
        if (tokens.size() > 2) {
            prerequisites.assign(tokens.begin() + 2, tokens.end()); // Get prerequisites
        }
        Course newCourse(courseNumber, courseName, prerequisites);
        courseTree.insert(newCourse); // Insert into BST
        allCoursesRaw.push_back(newCourse); // Store for validation
    }
    file.close();
    // Validate that all prerequisites exist in the course list
    for (const auto& course : allCoursesRaw) {
        for (const auto& prereq : course.prerequisites) {
            if (prereq.empty()) continue; // skip empty prerequisite entries
            if (courseTree.find(prereq) == nullptr) {
                cout << "Error: Prerequisite " << prereq << " for course " << course.courseNumber << " not found in course list." << endl;
            }
        }
    }
}

// Print detailed information for a specific course
void printCourseInfo(const string& courseNumber, CourseBST& courseTree) {
    TreeNode* courseNode = courseTree.find(courseNumber);
    if (courseNode != nullptr) {
        cout << "Course Number: " << courseNode->data.courseNumber << endl;
        cout << "Course Name: " << courseNode->data.courseName << endl;
        if (!courseNode->data.prerequisites.empty()) {
            cout << "Prerequisites: ";
            for (const auto& prereq : courseNode->data.prerequisites) {
                cout << prereq << " ";
            }
            cout << endl;
        }
        else {
            cout << "Prerequisites: None" << endl;
        }
    }
    else {
        cout << "Error: Course " << courseNumber << " not found." << endl;
    }
}

// Print all courses using in-order traversal
void printAllCourses(CourseBST& courseTree) {
    courseTree.inOrderTraversal();
}

// Print all courses in alphanumeric order (sorted by course number)
void printAlphanumericCSC(CourseBST& courses) {
    vector<Course> courseList;
    // Traverse tree and collect all courses
    function<void(TreeNode*)> traverseTree = [&](TreeNode* node) {
        if (node != nullptr) {
            traverseTree(node->left);
            courseList.push_back(node->data);
            traverseTree(node->right);
        }
        };
    traverseTree(courses.getRoot());
    // Sort courses by course number
    sort(courseList.begin(), courseList.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
        });
    // Print sorted courses
    for (const auto& course : courseList) {
        cout << course.courseNumber << ": " << course.courseName << endl;
    }
}

// Main menu loop for user interaction
void mainMenu() {
    bool running = true;
    CourseBST courses; // BST to store courses
    while (running) {
        cout << "1. Load Data" << endl;
        cout << "2. Print Course List (Alphanumeric Order)" << endl;
        cout << "3. Print Course Information" << endl;
        cout << "9. Exit" << endl;
        int choice;
        cin >> choice;
        switch (choice) {
        case 1: {
            string filename;
            cout << "Enter filename: ";
            cin >> filename;
            loadData(filename, courses); // Load courses from file
            break;
        }
        case 2:
            printAlphanumericCSC(courses); // Print all courses sorted
            break;
        case 3: {
            string courseNumber;
            cout << "Enter course number: ";
            cin >> courseNumber;
            printCourseInfo(courseNumber, courses); // Print info for specific course
            break;
        }
        case 9:
            running = false;
            cout << "Exiting program. Goodbye!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Program entry point
int main() {
    mainMenu();
    return 0;
}
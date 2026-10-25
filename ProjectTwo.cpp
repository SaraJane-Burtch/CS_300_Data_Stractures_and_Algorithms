
/*
 * Author: Sara Jane Burtch
 * Course: CS 300 - Data Structures and Algorithms
 * Professor: Jayantha Muthukudage
 * Assignment: Project Two - Advising Assistance Program
 * School: Southern New Hampshire University
 * Description: This program loads course information from a
 * CSV file into a binary search tree. It allows users to
 * display an alphanumerically sorted course list and search
 * for individual courses and their prerequisites.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

// Stores each course's number, title, and prerequisite course numbers.
// Course objects will be stored in the binary search tree.
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

/*
 * Represents a node in the binary search tree. 
 * Each node stores one course and pointers to its left and
 * right children. Course numbers determine node placement.
 */
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Initializes a node with course data and no children.
    Node(Course newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};

/*
 * Manages the binary search tree used to store course data.
 * Courses are organized by course number, allowing the tree
 * to support searching and alphanumerically ordered output.
 */
class BinarySearchTree {
private:
    Node* root;

    // Inserts a course into its correct position in the tree.
    Node* insertNode(Node* node, const Course& course) {
        if (node == nullptr) {
            return new Node(course);
        }

        if (course.courseNumber < node->course.courseNumber) {
            node->left = insertNode(node->left, course);
        }
        else if (course.courseNumber > node->course.courseNumber) {
            node->right = insertNode(node->right, course);
        }

        return node;
    }

    // Traverses the tree in order to print courses alphanumerically.
    void inOrderTraversal(Node* node) const {
        if (node == nullptr) {
            return;
        }

        inOrderTraversal(node->left);
        cout << node->course.courseNumber << ", "
             << node->course.courseName << endl;
        inOrderTraversal(node->right);
    }

    // Releases all nodes in the tree to prevent memory leaks.
    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    // Initializes an empty binary search tree.
    BinarySearchTree() {
        root = nullptr;
    }

    // Cleans up dynamically allocated memory when the tree is destroyed.
    ~BinarySearchTree() {
        destroyTree(root);
    }

    // Prevents accidental copying of a tree that owns dynamic memory.
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    // Adds a course to the tree using its course number.
    void insert(const Course& course) {
        root = insertNode(root, course);
    }

    // Searches the tree for a course using its course number.
    // Returns the matching course or nullptr if it is not found.
    const Course* search(const string& courseNumber) const {
        Node* currentNode = root;

        while (currentNode != nullptr) {
            if (courseNumber == currentNode->course.courseNumber) {
                return &currentNode->course;
            }
            else if (courseNumber < currentNode->course.courseNumber) {
                currentNode = currentNode->left;
            }
            else {
                currentNode = currentNode->right;
            }
        }

        return nullptr;
    }

    // Prints all courses in alphanumeric order.
    void printCourseList() const {
        cout << "Here is a sample schedule:" << endl << endl;
        inOrderTraversal(root);
    }

    // Removes existing course data so a new file can be loaded.
    void clear() {
        destroyTree(root);
        root = nullptr;
    }

    // Checks whether the tree contains any courses.
    bool isEmpty() const {
        return root == nullptr;
    }
};

// Verifies that every prerequisite exists in the course data.
bool validatePrerequisites(const vector<Course>& courseList) {
    
    // Check for duplicate course numbers before validating prerequisites.
    for (size_t i = 0; i < courseList.size(); ++i) {
        for (size_t j = i + 1; j < courseList.size(); ++j) {
            if (courseList[i].courseNumber == courseList[j].courseNumber) {
                cout << "Error: Duplicate course number: "
                     << courseList[i].courseNumber << endl;
                return false;
            }
        }
    }

    for (const Course& course : courseList) {
        for (const string& prerequisite : course.prerequisites) {
            bool prerequisiteFound = false;

            for (const Course& possibleCourse : courseList) {
                if (possibleCourse.courseNumber == prerequisite) {
                    prerequisiteFound = true;
                    break;
                }
            }

            if (!prerequisiteFound) {
                cout << "Error: Prerequisite course does not exist: "
                     << prerequisite << endl;
                return false;
            }
        }
    }

    return true;
}

/*
 * Reads course information from a CSV file, validates the data,
 * and loads each course into the binary search tree.
 * Existing course data is replaced only after validation succeeds.
 */
bool loadCourses(const string& fileName, BinarySearchTree& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Unable to open file." << endl;
        return false;
    }

    vector<Course> courseList;
    string line;

    // Read each line and separate the comma-delimited fields.
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Every course must have a course number and title.
        if (tokens.size() < 2 || tokens[0].empty() ||
            tokens[1].empty()) {
            cout << "Error: Invalid file format." << endl;
            return false;
        }

        Course course;
        course.courseNumber = tokens[0];
        course.courseName = tokens[1];

        // Store any remaining fields as prerequisite course numbers.
        
        // Store nonempty prerequisite fields; ignore blank CSV entries.
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(tokens[i]);
            }
        }

        courseList.push_back(course);
    }

    file.close();

    if (courseList.empty()) {
        cout << "Error: No course data found." << endl;
        return false;
    }

    // Validate prerequisites before modifying the existing tree.
    if (!validatePrerequisites(courseList)) {
        return false;
    }

    courses.clear();

    for (const Course& course : courseList) {
        courses.insert(course);
    }

    cout << "Course data loaded successfully!" << endl;
    return true;
}

/*
 * Displays the course planner menu and processes user selections.
 * Course data must be loaded before viewing course information.
 */
int main() {
    BinarySearchTree courses;
    string input;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        cout << endl;
        cout << "    1. Load Data Structure." << endl;
        cout << "    2. Print Course List." << endl;
        cout << "    3. Print Course." << endl;
        cout << "    9. Exit" << endl << endl;
        cout << "What would you like to do? ";

        getline(cin, input);

        if (input == "1") {
            string fileName;

            cout << "Enter the course data file name: ";
            getline(cin, fileName);

            loadCourses(fileName, courses);
        }
        else if (input == "2") {
            if (courses.isEmpty()) {
                cout << "Please load course data first." << endl;
            }
            else {
                courses.printCourseList();
            }
        }
        else if (input == "3") {
            if (courses.isEmpty()) {
                cout << "Please load course data first." << endl;
            }
            else {
                string courseNumber;

                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);

                // Convert input to uppercase for case-insensitive searching.
                transform(courseNumber.begin(), courseNumber.end(),
                          courseNumber.begin(),
                          [](unsigned char c) { return toupper(c); });

                const Course* course = courses.search(courseNumber);

                if (course == nullptr) {
                    cout << "Course not found." << endl;
                }
                else {
                    cout << course->courseNumber << ", "
                         << course->courseName << endl;

                    if (course->prerequisites.empty()) {
                        cout << "Prerequisites: None" << endl;
                    }
                    else {
                        cout << "Prerequisites: ";
                
                        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
                            if (i > 0) {
                                cout << ", ";
                            }

                            const string& prerequisiteNumber = course->prerequisites[i];
                            const Course* prerequisite = courses.search(prerequisiteNumber);

                            cout << prerequisiteNumber;

                            if (prerequisite != nullptr) {
                                cout << " (" << prerequisite->courseName << ")";
                            }
                        }

                        cout << endl;
                    }
                }
            }
        }
        else if (input == "9") {
            cout << "Thank you for using the course planner!" << endl;
            break;
        }
        else {
            cout << input << " is not a valid option." << endl;
        }
    }

    return 0;
}

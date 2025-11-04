#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Data Structures ---

// Structure for Employee Data
typedef struct Employee {
    int employee_id;
    char name[100];
    double performance_score;
} Employee;

// Structure for AVL Tree Node
typedef struct Node {
    Employee data;
    struct Node *left;
    struct Node *right;
    int height; // Height of the node
} Node;

// --- Utility Functions ---

// Get height of a node (NULL node has height 0)
int height(Node *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// Get max of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Update a node's height based on its children
void updateHeight(Node *N) {
    if (N != NULL) {
        N->height = 1 + max(height(N->left), height(N->right));
    }
}

// Create a new Employee
Employee createEmployee(int id, const char* name, double score) {
    Employee emp;
    emp.employee_id = id;
    strncpy(emp.name, name, 99);
    emp.name[99] = '\0'; // Ensure null-termination
    emp.performance_score = score;
    return emp;
}

// Create a new AVL Node
Node* newNode(Employee emp) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    node->data = emp;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially at height 1
    return node;
}

// Get Balance Factor of a node
int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// --- AVL Tree Rotations (The "Automatic Rebalancing") ---
Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    // Return new root
    return x;
}

Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    // Return new root
    return y;
}

// --- Core Operations ---
Node* insert(Node* node, Employee emp) {
    // 1. Perform standard BST insertion
    if (node == NULL)
        return(newNode(emp));

    if (emp.employee_id < node->data.employee_id)
        node->left  = insert(node->left, emp);
    else if (emp.employee_id > node->data.employee_id)
        node->right = insert(node->right, emp);
    else {
        // Duplicate employee_id is not allowed
        printf("  ERROR: Employee ID %d already exists. Skipping insertion.\n", emp.employee_id);
        return node;
    }

    // 2. Update height of this ancestor node
    updateHeight(node);

    // 3. Get the balance factor of this ancestor node
    int balance = getBalance(node);

    // 4. If the node becomes unbalanced, there are 4 cases
    //    (This is the "Automatic Rebalancing")

    // Left Left (LL) Case
    if (balance > 1 && emp.employee_id < node->left->data.employee_id)
        return rightRotate(node);

    // Right Right (RR) Case
    if (balance < -1 && emp.employee_id > node->right->data.employee_id)
        return leftRotate(node);

    // Left Right (LR) Case
    if (balance > 1 && emp.employee_id > node->left->data.employee_id) {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left (RL) Case
    if (balance < -1 && emp.employee_id < node->right->data.employee_id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // Return the (possibly unchanged) node pointer
    return node;
}

Node* search(Node* root, int employee_id) {
    // Base Cases: root is null or key is present at root
    if (root == NULL || root->data.employee_id == employee_id)
       return root;

    // Key is greater than root's key
    if (root->data.employee_id < employee_id)
       return search(root->right, employee_id);

    // Key is smaller than root's key
    return search(root->left, employee_id);
}

// --- Display Functions ---

// In-order traversal to print all employees sorted by ID
void inOrder(Node *root) {
    if (root != NULL) {
        inOrder(root->left);
        printf("  [ID: %-4d | Name: %-15s | Score: %.2f]\n", 
               root->data.employee_id, 
               root->data.name, 
               root->data.performance_score);
        inOrder(root->right);
    }
}

// Helper to free the tree memory
void freeTree(Node* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}


// --- Main Function ---
int main() {
    Node *root = NULL;
    int choice;
    int emp_id;
    char name[100];
    double score;

    while (1) {
        printf("\n--- Employee Performance Tracker Menu ---\n");
        printf("1. Add Employee\n");
        printf("2. Search Employee\n");
        printf("3. Display All Employees (Sorted by ID)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        // Check if input is a valid integer
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Clear the invalid input from the buffer
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                printf("\n--- Add New Employee ---\n");
                printf("Enter Employee ID: ");
                scanf("%d", &emp_id);

                printf("Enter Employee Name: ");
                scanf(" %[^\n]", name);

                printf("Enter Performance Score: ");
                scanf("%lf", &score);

                root = insert(root, createEmployee(emp_id, name, score));
                printf("Employee added successfully.\n");
                break;

            case 2:
                printf("\n--- Search for Employee ---\n");
                printf("Enter Employee ID to search: ");
                scanf("%d", &emp_id);

                Node* found = search(root, emp_id);
                if (found) {
                    printf("  Found: [ID: %d, Name: %s, Score: %.2f]\n", 
                           found->data.employee_id, 
                           found->data.name, 
                           found->data.performance_score);
                } else {
                    printf("  Employee with ID %d not found.\n", emp_id);
                }
                break;

            case 3:
                printf("\n--- All Employees (Sorted by ID) ---\n");
                if (root == NULL) {
                    printf("  No employees in the tracker.\n");
                } else {
                    inOrder(root);
                }
                printf("-----------------------------------------\n");
                break;

            case 4:
                printf("Exiting... Cleaning up memory.\n");
                freeTree(root);
                return 0;

            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
        }
    }
}
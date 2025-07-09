//
// Created by Ahmet Eren Kutsal on 8.07.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
struct Node {
    int data;
    struct Node* next;
};

struct Node* createNode (int data) {
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    if (!newNode) {
        printf("Memory allocation failed!");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void append (struct Node** head, int data) {
    if (*head == NULL) {
        *head = createNode(data);
        return;
    }
    struct Node* newNode = createNode(data);
    struct Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void freeList (struct Node* head) {
    struct Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void clearScreen() {
    printf("\033[2J");        // Clear screen
    printf("\033[H");         // Move cursor to top-left
}

void initTerminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Reset terminal to normal mode
void resetTerminal() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void setNonBlocking(int enable) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (enable)
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}


#define cols 48
#define rows 12

void drawNode(struct Node* node) {
    int row = node->data / cols + 1;
    int col = node->data % cols;
    if (col == 0) {
        col = 48;
    }
    printf("\033[%d;%dH", row, col);
    printf("S");
    printf("\033[%d;%dH", rows, cols);
    printf("\nrow: %d\ncol: %d\n", row, col);
}

void drawNodes(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        drawNode(temp);
        temp = temp->next;
    }
}

void addNodeToStart(struct Node** head, int data) {
    struct Node* newNode = createNode(data);
    newNode->next = *head;
    *head = newNode;
}

void moveNodes(struct Node* head, int diff) {
    int oldData = head->data;
    head->data += diff;
    int currentData;
    struct Node* temp = head->next;
    while (temp != NULL) {
        currentData = temp->data;
        temp->data = oldData;
        oldData = currentData;
        temp = temp->next;
    }
}

void drawApple(int row, int col) {
    printf("\033[%d;%dH", row, col);
    printf("D");
}

void end(struct Node* head) {
    freeList(head);
    exit(0);
}
int main ()
{
    initTerminal();
    setNonBlocking(1);

    int randomNumber = 40;

    struct Node* head = createNode(50);
    char c;
    while (1) {

        clearScreen();

        if (read(STDIN_FILENO, &c, 1) > 0) {
            int diff = 0;
            switch (c) {
                case 'w':
                    diff = -cols;
                    break;
                case 'a':
                    diff = -1;
                    break;
                case 's':
                    diff = cols;
                    break;
                case 'd':
                    diff = 1;
                    break;
                default:
                    break;
            }

            if (head->data + diff == randomNumber) {
                printf("\033[%d;%dH", rows + 5, 0);
                printf("hit!!");
                addNodeToStart(&head, randomNumber);
                randomNumber = ((randomNumber * 97) % cols * rows) + 1;
            } else {
                moveNodes(head, diff);
            }
            if (c == 'q') break;
        }
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                printf(".");
            }
            printf("\n");
        }

        drawApple((randomNumber / cols) + 1, randomNumber % cols);
        drawNodes(head);

        fflush(stdout);   // Ensure output is printed immediately
        usleep(100000);   // Sleep for 100 ms (100000 microseconds)
    }
    resetTerminal();
    setNonBlocking(0);
    return 1;
}
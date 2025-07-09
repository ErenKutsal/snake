//
// Created by Ahmet Eren Kutsal on 8.07.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

struct Node {
    int row;
    int col;
    struct Node* next;
};

struct Node* createNode (int row, int col) {
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    if (!newNode) {
        printf("Memory allocation failed!");
        exit(1);
    }
    newNode->row = row;
    newNode->col = col;
    newNode->next = NULL;
    return newNode;
}

void append (struct Node** head, int row, int col) {
    if (*head == NULL) {
        *head = createNode(row, col);
        return;
    }
    struct Node* newNode = createNode(row, col);
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
    int row = node->row;
    int col = node->col;
    printf("\033[%d;%dH", row, col);
    printf("S");
}

void drawNodes(struct Node* head) {
    struct Node* temp = head;
    //printf("\033[%d;%dH", rows, cols);
    //printf("\nrow: %d\ncol: %d\n", head->row, head->col);
    while (temp != NULL) {
        drawNode(temp);
        temp = temp->next;
    }
}

void addNodeToStart(struct Node** head, int row, int col) {
    struct Node* newNode = createNode(row, col);
    newNode->next = *head;
    *head = newNode;
}

void moveNodes(struct Node* head, int row, int col) {
    int oldRow = head->row;
    int oldCol = head->col;
    head->row = row;
    head->col = col;
    if (head->col > cols) {
        head->col = cols;
        return;
    } else if (head->col < 1) {
        head->col = 1;
        return;
    }
    if (head->row > rows) {
        head->row = rows;
        return;
    } else if (head->row < 1) {
        head->row = 1;
        return;
    }

    int currentRow;
    int currentCol;
    struct Node* temp = head->next;
    while (temp != NULL) {
        currentRow = temp->row;
        currentCol = temp->col;
        temp->row = oldRow;
        temp->col = oldCol;
        oldRow = currentRow;
        oldCol = currentCol;
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
    srand(time(NULL));

    initTerminal();
    setNonBlocking(1);

    int randomRow = rand() % (rows + 1);
    int randomCol = rand() % (cols + 1);

    struct Node* head = createNode(2, 2);
    char c;
    while (1) {

        clearScreen();

        if (read(STDIN_FILENO, &c, 1) > 0) {
            int predictedRow = head->row;
            int predictedCol = head->col;
            switch (c) {
                case 'w':
                    predictedRow += -1;
                    break;
                case 'a':
                    predictedCol += -1;
                    break;
                case 's':
                    predictedRow += 1;
                    break;
                case 'd':
                    predictedCol += 1;
                    break;
                default:
                    break;
            }
            if (predictedCol == randomCol && predictedRow == randomRow) {
                addNodeToStart(&head, randomRow, randomCol);
                randomRow = rand() % (rows + 1) + 1;
                randomCol = rand() % (cols + 1) + 1;
            } else {
                moveNodes(head, predictedRow, predictedCol);
            }
            if (c == 'q') break;
        }
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                printf(".");
            }
            printf("\n");
        }

        drawApple(randomRow, randomCol);
        drawNodes(head);

        fflush(stdout);   // Ensure output is printed immediately
        usleep(100000);   // Sleep for 100 ms (100000 microseconds)
    }
    resetTerminal();
    setNonBlocking(0);
    return 1;
}
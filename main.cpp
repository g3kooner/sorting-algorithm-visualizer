#include "include/raylib.h"
#include "vector"
#include <cmath>
#include <iostream>

// Screen Definitions:
#define minWidth 500
#define minHeight 800

#define CREAM (Color) {247, 236, 222, 255}

// Array States:
#define DEFAULT 0
#define SORTED 1
#define SELECTED 2
#define LEFT 3
#define RIGHT 4
#define PIVOT 5

// Screens:
bool shouldMenuScreen = true;
void showMenuScreen(Font font);
bool shouldConfigScreen = false;
void showConfigScreen(Font font);

// Config:
int currentSpeed = 60;
bool incSpeed = false;
bool decSpeed = false;
bool incSize = false;
bool decSize = false;
bool defaultSpeed = false;
bool defaultSize = false;
void changeSpeed(char operation, int &currentSpeed);
void changeSize(char operation, int &currentSize);

// Algorithms:
void bubbleSort(std::vector<std::pair<int, int>> &array);
void insertionSort(std::vector<std::pair<int, int>> &array);
void mergeSort(std::vector<std::pair<int, int>> &array, int left, int right);
void merge(std::vector<std::pair<int, int>> &array, int leftIndex, int rightIndex, int midpont);
int partition(std::vector<std::pair<int,int>> &array, int left, int right);
void quickSort(std::vector<std::pair<int,int>> &array, int left, int right);

// Buttons:
void bubbleButton(float length, char bubble_sort_text[], Font font);
void insertionButton(float length, char insertion_sort_text[], Font font);
void mergeButton(float length, char merge_sort_text[], Font font);
void quickButton(float length, char quick_sort_text[], Font font);
void sortButton(float length, float fontSize, char text[], Font font);
void createButton(float xpos, float ypos, char text[], Color color, bool &isPressed, Font font);
bool soundOff = false;
bool bubblePressed = false;
bool insertionPressed = false;
bool mergePressed = false;
bool quickPressed = false;
bool sortingPressed = false;
bool generatePressed = true;
bool titlePressed = false;

// Array Components:
int numberOfElements = 60;
std::vector<std::pair<int, int>> array(numberOfElements);

void sort();
void drawArray(std::vector<std::pair<int, int>> &array);
void randomizeArray(std::vector<std::pair<int, int>> &array);
Color retrieveColor(int currentState);

int main()
{
    // Intialize Window Components:
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1200, 900, "Sorting Algorithm Visualizer");
    Font textFont = LoadFont("resources/fonts/romulus.png");
    SetWindowMinSize(minWidth, minHeight);

    Image soundimg = LoadImage("resources/images/soundimg.png");
    Image mutesoundimg = LoadImage("resources/images/mutesoundimg.png");
    ImageResize(&soundimg, 20, 20);
    ImageResize(&mutesoundimg, 25, 25);
    Sound route101 = LoadSound("resources/sound/pokemon_verdanturftown.mp3");
    Texture2D soundOn = LoadTextureFromImage(soundimg);
    Texture2D soundMute = LoadTextureFromImage(mutesoundimg);

    // Set FPS:
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Rectangle rec = {(float)1 * GetScreenWidth() / 100, (float)1 * GetScreenHeight() / 100, 20, 20};

        if (CheckCollisionPointRec(GetMousePosition(), rec)) {
            if (IsMouseButtonPressed(0))
                soundOff = !soundOff;
        }

        if (soundOff) {
            DrawTexture(soundMute, 1 * GetScreenWidth() / 100, 1 * GetScreenWidth() / 100, BLACK);
            PauseSound(route101);
        }
        else {
            DrawTexture(soundOn, 1 * GetScreenWidth() / 100, 1 * GetScreenWidth() / 100, BLACK);
            ResumeSound(route101);
        }

        if (!IsSoundPlaying(route101) && !soundOff)
            PlaySound(route101);

        if (shouldMenuScreen)
            showMenuScreen(textFont);

        if (bubblePressed || insertionPressed || mergePressed || quickPressed)
            shouldConfigScreen = true;
        else
            shouldConfigScreen = false;

        if (shouldConfigScreen)
            showConfigScreen(textFont);

        if (generatePressed)
            randomizeArray(array);

        if (sortingPressed)
            sort();

        BeginDrawing();
        ClearBackground(CREAM);
        EndDrawing();
    }

    UnloadImage(soundimg);
    UnloadImage(mutesoundimg);
    UnloadTexture(soundMute);
    UnloadTexture(soundOn);
    UnloadSound(route101);

    CloseAudioDevice();
    CloseWindow(); 
    return 0;
}

void drawArray(std::vector<std::pair<int, int>> &array) {
    float barWidth = GetScreenWidth() /  numberOfElements;
    for (int i = 0; i < numberOfElements; i++) {
        Color currentColor = retrieveColor(array[i].second);
        DrawRectangle(i * barWidth, GetScreenHeight() - array[i].first, barWidth, array[i].first, currentColor);
    }
}

void randomizeArray(std::vector<std::pair<int, int>> &array) {
    for (int i = 0; i < numberOfElements; i++) {
        array[i] = {GetRandomValue(40, minWidth - 60), DEFAULT};
    }
    generatePressed = false;
}

void showMenuScreen(Font font) {
    float fontSize = (2.5 * GetScreenWidth() / 100);
    char bubble_sort_text[]{"Bubble Sort"};
    float length = (8 * GetScreenWidth()) / 100;
    bubbleButton(length, bubble_sort_text, font);

    char insertion_sort_text[]{"Insertion Sort"};
    length += MeasureText(insertion_sort_text, fontSize) + fontSize * 2;
    insertionButton(length, insertion_sort_text, font);

    char merge_sort_text[]{"Merge Sort"};
    length += MeasureText(merge_sort_text, fontSize) + fontSize * 4;
    mergeButton(length, merge_sort_text, font);

    char quick_sort_text[]{"Quick Sort"};
    length += MeasureText(quick_sort_text, fontSize) + fontSize * 3;
    quickButton(length, quick_sort_text, font);

    char title_text[]{"Sorting Algorithm Visualizer"};
    createButton(31.5 * GetScreenWidth() / 100, GetScreenHeight() / 90, title_text, BLACK, titlePressed, font);

    drawArray(array);
}

void showConfigScreen(Font font) {
    incSpeed = false;
    incSize = false;
    decSize = false;
    decSpeed = false;
    defaultSize = false;
    defaultSpeed = false;

    float fontSize = (2.5 * GetScreenWidth() / 100);
    char beginText[] = "Begin Sort!";
    float temp = (29 * GetScreenWidth()) / 100;

    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, beginText, DARKGRAY, sortingPressed, font);

    temp += MeasureText(beginText, fontSize) + 80;

    char generateText[] = "Generate Array!";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, generateText, DARKGRAY, generatePressed, font);

    temp = (82 * GetScreenWidth()) / 100;
    char speedText[] = "Speed";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, speedText, BLACK, defaultSpeed, font);

    if (defaultSpeed)
        changeSpeed('/', currentSpeed);

    temp += MeasureText(speedText, fontSize) + 20;
    char addSpeedText[] = "+";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, addSpeedText, DARKGRAY, incSpeed, font);

    if (incSpeed)
        changeSpeed('+', currentSpeed);

    temp += MeasureText(addSpeedText, fontSize) + 20;
    char subSpeedText[] = "-";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, subSpeedText, DARKGRAY, decSpeed, font);

    if (decSpeed)
        changeSpeed('-', currentSpeed);

    temp = (8 * GetScreenWidth()) / 100;
    char sizeText[] = "Size";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, sizeText, BLACK, defaultSize, font);

    if (defaultSize)
        changeSize('/', numberOfElements);

    temp += MeasureText(sizeText, fontSize) + 20;
    char addSizeText[] = "+";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, addSizeText, DARKGRAY, incSize, font);

    if (incSize)
        changeSize('+', numberOfElements);

    temp += MeasureText(addSizeText, fontSize) + 20;
    char subSizeText[] = "-";
    createButton(temp, GetScreenHeight() / 20 + fontSize * 2, subSizeText, DARKGRAY, decSize, font);

    if (decSize)
        changeSize('-', numberOfElements);
}

void changeSpeed(char operation, int &currentSpeed) {
    switch (operation)
    {
    case '+':
        currentSpeed += 10;
        break;
    case '-':
        currentSpeed = (currentSpeed - 10 > 0) ? currentSpeed - 10 : currentSpeed;
        break;
    default:
        currentSpeed = 50;
        break;
    }

    SetTargetFPS(currentSpeed);
}

void changeSize(char operation, int &currentSize) {
    switch (operation) {
    case '+':
        currentSize += 5;
        for (int i = 0; i < 5; i++) array.push_back({GetRandomValue(40, minWidth - 60), DEFAULT});
        break;
    case '-':
        if (currentSize > 10) {
            currentSize--;
            array.pop_back();
        }
        break;
    default:
        while (currentSize > 50) {
            array.pop_back();
            currentSize--;
        }
        while (currentSize < 50) {
            array.push_back({GetRandomValue(40, minWidth - 60), DEFAULT});
            currentSize++;
        }
        break;
    }

    for (int i = 0; i < currentSize; i++) {
        array[i].second = DEFAULT;
    }
}

void bubbleButton(float length, char bubble_sort_text[], Font font) {
    Color buttonColor;
    if (bubblePressed)
        buttonColor = RED;
    else
        buttonColor = BLACK;

    createButton(length, GetScreenHeight() / 17, bubble_sort_text, buttonColor, bubblePressed, font);
}

void insertionButton(float length, char insertion_sort_text[], Font font) {
    Color buttonColor;
    if (insertionPressed)
        buttonColor = RED;
    else
        buttonColor = BLACK;

    createButton(length, GetScreenHeight() / 17, insertion_sort_text, buttonColor, insertionPressed, font);
}

void mergeButton(float length, char merge_sort_text[], Font font) {
    Color buttonColor;
    if (mergePressed)
        buttonColor = RED;
    else
        buttonColor = BLACK;

    createButton(length, GetScreenHeight() / 17, merge_sort_text, buttonColor, mergePressed, font);
}

void quickButton(float length, char quick_sort_text[], Font font) {
    Color buttonColor;
    if (quickPressed)
        buttonColor = RED;
    else
        buttonColor = BLACK;

    createButton(length, GetScreenHeight() / 17, quick_sort_text, buttonColor, quickPressed, font);
}

Color retrieveColor(int currentState) {
    switch (currentState)
    {
    case SELECTED:
        return LIGHTGRAY;
        break;

    case SORTED:
        return DARKGRAY;
        break;

    case LEFT:
        return RED;
        break;

    case RIGHT:
        return GREEN;
        break;

    case PIVOT:
        return BLUE;

    default:
        return BLACK;
        break;
    }
}

void createButton(float xpos, float ypos, char *text, Color color, bool &isPressed, Font font) {
    float fontSize = (2.5 * GetScreenWidth() / 100);
    Rectangle rec = {xpos, ypos, (float)MeasureText(text, fontSize), fontSize};

    if (CheckCollisionPointRec(GetMousePosition(), rec)) {
        DrawTextEx(font, text, Vector2{xpos, ypos}, fontSize, 3, GRAY);
        // DrawText(text, xpos, ypos, fontSize, GRAY);

        if (IsMouseButtonPressed(0)) {
            if (isPressed == true)
                isPressed = false;
            else
                isPressed = true;
        }
    }
    else DrawTextEx(font, text, Vector2{xpos, ypos}, fontSize, 3, color);
}

void sort() {
    if (bubblePressed)
        bubbleSort(array);
    else if (insertionPressed)
        insertionSort(array);
    else if (mergePressed)
        mergeSort(array, 0, numberOfElements - 1);
    else if (quickPressed)
        quickSort(array, 0, numberOfElements - 1);

    sortingPressed = false;
    drawArray(array);
}

void bubbleSort(std::vector<std::pair<int, int>> &array) {
    int remaining = 0;
    bool isSorted = false;

    for (int k = numberOfElements; k > 0; k--) {
        isSorted = true;
        for (int i = 0; i < k - 1; i++) {
            array[i].second = SELECTED;
            if (array[i] > array[i + 1]) {
                std::swap(array[i], array[i + 1]);
                isSorted = false;
            }

            BeginDrawing();
            ClearBackground(CREAM);
            for (int j = numberOfElements - 1; j >= k; j--)
                array[j].second = SORTED;
            drawArray(array);
            for (int j = i; j >= 0; j--)
                array[j].second = DEFAULT;
            EndDrawing();
        }
        if (isSorted == true)
            break;
        remaining = k;
    }

    for (int i = 0; i < remaining; i++)
        array[i].second = SORTED;
}

void insertionSort(std::vector<std::pair<int, int>> &array) {
    array[0].second = SORTED;
    int value = 0;
    int hole = 0;
    for (int i = 1; i < numberOfElements; i++)
    {
        BeginDrawing();
        ClearBackground(CREAM);
        array[i].second = SELECTED;
        value = array[i].first;
        hole = i;
        while (hole > 0 && array[hole - 1].first > value)
        {
            array[hole - 1].second = SELECTED;
            BeginDrawing();
            ClearBackground(CREAM);
            drawArray(array);
            array[hole].second = SORTED;
            std::swap(array[hole - 1].first, array[hole].first);
            EndDrawing();
            hole--;
        }

        for (int k = i - 1; k >= 0; k--)
            array[k].second = SORTED;
        array[hole].first = value;
        array[hole].second = SORTED;

        EndDrawing();
    }

    for (int i = 0; i < numberOfElements; i++)
        array[i].second = SORTED;
}

void mergeSort(std::vector<std::pair<int, int>> &array, int leftIndex, int rightIndex) {
    if (leftIndex >= rightIndex)
        return;

    int midpoint = (rightIndex + leftIndex) / 2;

    mergeSort(array, leftIndex, midpoint);
    mergeSort(array, midpoint + 1, rightIndex);
    merge(array, leftIndex, rightIndex, midpoint);
}

void merge(std::vector<std::pair<int, int>> &array, int leftIndex, int rightIndex, int midpoint) {
    int leftCap = midpoint - leftIndex + 1;
    int rightCap = rightIndex - midpoint;
    int left[leftCap];
    int right[rightCap];

    for (int i = 0; i < leftCap; i++) {
        left[i] = array[leftIndex + i].first;
        array[leftIndex + i].second = LEFT;
    }
    for (int i = 0; i < rightCap; i++) {
        right[i] = array[midpoint + 1 + i].first;
        array[midpoint + 1 + i].second = RIGHT;
    }

    BeginDrawing();
    ClearBackground(CREAM);
    drawArray(array);
    EndDrawing();

    int a = 0, b = 0, c = leftIndex;
    while (a < leftCap && b < rightCap) {
        if (left[a] <= right[b]) {
            array[c].first = left[a];
            a++;
        }
        else {
            array[c].first = right[b];
            b++;
        }
        array[c].second = SORTED;
        c++;
    }

    while (a < leftCap) {
        array[c].first = left[a];
        a++;
        array[c].second = SORTED;
        c++;
    }

    while (b < rightCap) {
        array[c].first = right[b];
        b++;
        array[c].second = SORTED;
        c++;
    }
}

int partition(std::vector<std::pair<int,int>> &array, int left, int right){
    int pivotPosition = left;
    int pivot = array[left].first;

    for (int i = left + 1; i <= right; i++) {
        if (array[i].first <= pivot) pivotPosition++;
    }

    std::swap(array[pivotPosition], array[left]);
    array[left].second = LEFT;
    array[right].second = RIGHT;
    array[pivotPosition].second = PIVOT;

    while (left < pivotPosition && right > pivotPosition) {
        BeginDrawing();

        while (array[left].first <= pivot) {
            left++;
            array[left].second = LEFT;
            array[left - 1].second = DEFAULT;
            ClearBackground(CREAM);
            drawArray(array);
        }
 
        while (array[right].first > pivot) {
            right--;
            array[right].second = RIGHT;
            array[right + 1].second = DEFAULT;
            ClearBackground(CREAM);
            drawArray(array);
        }

        if (left < pivotPosition && right > pivotPosition) {
            array[left].second = DEFAULT;
            array[right].second = DEFAULT;
            std::swap(array[left++], array[right--]);
        }

        EndDrawing();
    }
    array[pivotPosition].second = SORTED;

    return pivotPosition;
}

void quickSort(std::vector<std::pair<int, int>> &array, int left, int right) {
    if (left < right) {
        int breakPt = partition(array, left, right);
        quickSort(array, left, breakPt - 1);
        quickSort(array, breakPt + 1, right);
    } else array[left].second = SORTED; array[right].second = SORTED;
}

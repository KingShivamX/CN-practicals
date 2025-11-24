#include <bits/stdc++.h>

using namespace std;

// --- 2D Parity Check ---

void printMatrix(const vector<vector<int>>& data) {
    for (const auto& row : data) {
        for (int bit : row) {
            cout << bit << " ";
        }
        cout << endl;
    }
}

// Function to find and correct 2D Parity Error
void performParityCheck() {
    int rows, cols;
    cout << "Enter number of rows: ";
    cin >> rows;
    cout << "Enter number of columns: ";
    cin >> cols;

    vector<vector<int>> data(rows, vector<int>(cols));
    vector<int> rowParity(rows);
    vector<int> colParity(cols);

    cout << "Enter data bits (row by row):\n";
    for (int i = 0; i < rows; i++) {
        int rowOnes = 0;
        for (int j = 0; j < cols; j++) {
            cin >> data[i][j];
            if (data[i][j] == 1) {
                rowOnes++;
            }
        }
        rowParity[i] = (rowOnes % 2); // Even parity
    }

    for (int j = 0; j < cols; j++) {
        int colOnes = 0;
        for (int i = 0; i < rows; i++) {
            if (data[i][j] == 1) {
                colOnes++;
            }
        }
        colParity[j] = (colOnes % 2); // Even parity
    }

    cout << "\nOriginal Data:\n";
    printMatrix(data);
    cout << "Row Parity: ";
    for (int p : rowParity) cout << p << " ";
    cout << "\nCol Parity: ";
    for (int p : colParity) cout << p << " ";
    cout << "\n\n";

    // Simulate Error
    int errorRow, errorCol;
    cout << "Enter error position (row col) to simulate [0-indexed]: ";
    cin >> errorRow >> errorCol;

    if (errorRow < rows && errorCol < cols) {
        data[errorRow][errorCol] = 1 - data[errorRow][errorCol]; // Flip bit
        cout << "Data with error:\n";
        printMatrix(data);
    } else {
        cout << "Invalid error position.\n";
        return;
    }

    // Receiver Side: Detect & Correct
    vector<int> recvRowParity(rows);
    vector<int> recvColParity(cols);
    int finalErrorRow = -1;
    int finalErrorCol = -1;

    for (int i = 0; i < rows; i++) {
        int rowOnes = 0;
        for (int j = 0; j < cols; j++) {
            if (data[i][j] == 1) rowOnes++;
        }
        recvRowParity[i] = (rowOnes % 2);
        if (recvRowParity[i] != rowParity[i]) {
            finalErrorRow = i;
        }
    }

    for (int j = 0; j < cols; j++) {
        int colOnes = 0;
        for (int i = 0; i < rows; i++) {
            if (data[i][j] == 1) colOnes++;
        }
        recvColParity[j] = (colOnes % 2);
        if (recvColParity[j] != colParity[j]) {
            finalErrorCol = j;
        }
    }

    if (finalErrorRow != -1 && finalErrorCol != -1) {
        cout << "\nError detected at: (row " << finalErrorRow << ", col " << finalErrorCol << ")\n";
        data[finalErrorRow][finalErrorCol] = 1 - data[finalErrorRow][finalErrorCol];
        cout << "Corrected Data:\n";
        printMatrix(data);
    } else {
        cout << "\nNo single-bit error detected.\n";
    }
}

// --- Hamming Code ---

void performHammingCode() {
    vector<int> data(4);
    cout << "Enter 4 data bits: ";
    cin >> data[0] >> data[1] >> data[2] >> data[3];

    // (7,4) Hamming Code
    vector<int> codeWord(7);
    
    // d7 d6 d5 r4 d3 r2 r1
    // pos: 7  6  5  4  3  2  1
    codeWord[2] = data[0]; // pos 3
    codeWord[4] = data[1]; // pos 5
    codeWord[5] = data[2]; // pos 6
    codeWord[6] = data[3]; // pos 7

    // Calculate parity bits (even parity)
    codeWord[0] = codeWord[2] ^ codeWord[4] ^ codeWord[6]; // r1 = d3 ^ d5 ^ d7
    codeWord[1] = codeWord[2] ^ codeWord[5] ^ codeWord[6]; // r2 = d3 ^ d6 ^ d7
    codeWord[3] = codeWord[4] ^ codeWord[5] ^ codeWord[6]; // r4 = d5 ^ d6 ^ d7

    cout << "Sent Codeword (d7 d6 d5 r4 d3 r2 r1): ";
    for (int i = 6; i >= 0; i--) {
        cout << codeWord[i] << " ";
    }
    cout << endl;

    int errorPos;
    cout << "Enter error position to simulate (1-7): ";
    cin >> errorPos;

    if (errorPos >= 1 && errorPos <= 7) {
        codeWord[errorPos - 1] = 1 - codeWord[errorPos - 1]; // Flip bit
        cout << "Received (erroneous) Codeword: ";
        for (int i = 6; i >= 0; i--) {
            cout << codeWord[i] << " ";
        }
        cout << endl;
    } else {
        cout << "Invalid error position.\n";
        return;
    }

    // Receiver Side: Detect & Correct
    int c1 = codeWord[0] ^ codeWord[2] ^ codeWord[4] ^ codeWord[6];
    int c2 = codeWord[1] ^ codeWord[2] ^ codeWord[5] ^ codeWord[6];
    int c4 = codeWord[3] ^ codeWord[4] ^ codeWord[5] ^ codeWord[6];

    int syndrome = c4 * 4 + c2 * 2 + c1;

    if (syndrome == 0) {
        cout << "No error detected.\n";
    } else {
        cout << "Error detected at position: " << syndrome << endl;
        codeWord[syndrome - 1] = 1 - codeWord[syndrome - 1]; // Correct the error
        cout << "Corrected Codeword: ";
        for (int i = 6; i >= 0; i--) {
            cout << codeWord[i] << " ";
        }
        cout << endl;
    }
}

// --- CRC ---

// Helper for XOR division
string xorDivision(string data, string generator) {
    int genLen = generator.length();
    int dataLen = data.length();

    for (int i = 0; i <= dataLen - genLen; ) {
        if (data[i] == '1') {
            for (int j = 0; j < genLen; j++) {
                data[i + j] = (data[i + j] == generator[j]) ? '0' : '1';
            }
        }
        // Find next '1' to start division from
        int nextOne = -1;
        for(int k = i + 1; k < dataLen; k++) {
            if(data[k] == '1') {
                nextOne = k;
                break;
            }
        }
        if(nextOne == -1) break; // No more '1's
        
        // If next '1' is too far, we are done
        if(nextOne > dataLen - genLen) break;

        i = nextOne;
    }
    return data.substr(dataLen - genLen + 1);
}

void performCrc() {
    string data, generator;
    cout << "Enter data bit string: ";
    cin >> data;
    cout << "Enter generator polynomial string: ";
    cin >> generator;

    int r = generator.length() - 1;
    string augmentedData = data;
    for (int i = 0; i < r; i++) {
        augmentedData += '0';
    }

    // Sender side
    string remainder = xorDivision(augmentedData, generator);
    string codeWord = data + remainder;

    cout << "Remainder (FCS): " << remainder << endl;
    cout << "Transmitted Codeword: " << codeWord << endl;

    // Receiver side
    cout << "\n--- Receiver Side ---\n";
    string receivedWord;
    cout << "Enter received codeword (or copy transmitted one): ";
    cin >> receivedWord;

    string recvRemainder = xorDivision(receivedWord, generator);

    cout << "Receiver Remainder: " << recvRemainder << endl;
    
    bool error = false;
    for (char c : recvRemainder) {
        if (c == '1') {
            error = true;
            break;
        }
    }

    if (error) {
        cout << "Result: Error detected in data.\n";
    } else {
        cout << "Result: No error detected.\n";
    }
}

// --- Main Menu ---

int main() {
    int choice;
    cout << "--- Error Detection & Correction ---\n";
    cout << "Name: Shivam Hippalgave\n";
    cout << "PRN: 202301040046\n";
    cout << "------------------------------------\n";
    cout << "Select Method:\n";
    cout << "1. 2D Parity Check (Detect & Correct)\n";
    cout << "2. Hamming Code (Detect & Correct)\n";
    cout << "3. CRC (Detect Only)\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            performParityCheck();
        break;
        case 2:
            performHammingCode();
            break;
        case 3:
            performCrc();
            break;
        default:
            cout << "Invalid choice.\n";
            break;
    }

    return 0;
}
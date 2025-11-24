# Practical 3: Code Walkthrough - Line by Line

---

## 🔍 1D PARITY CHECK (Conceptual - Not in code, but foundation)

### Why Understand This First?
2D Parity is built on 1D Parity concepts!

### Algorithm:
```
1. Count the number of 1s in the data
2. If using EVEN parity:
   - If count is even → parity bit = 0
   - If count is odd → parity bit = 1
3. Append parity bit to data
4. Transmit: Data + Parity bit
```

### Example Walkthrough:
```
Data: 1 0 1 1 0
Step 1: Count 1s = 3 (odd)
Step 2: For even parity, add 1 → Total = 4 (even)
Step 3: Parity bit = 1
Step 4: Codeword = 1 0 1 1 0 | 1
```

---

## 📊 2D PARITY CHECK - Code Explanation

### Function: `performParityCheck()`

Let's break it down step by step:

#### **Step 1: Input Data Matrix**
```cpp
int rows, cols;
cout << "Enter number of rows: ";
cin >> rows;
cout << "Enter number of columns: ";
cin >> cols;

vector<vector<int>> data(rows, vector<int>(cols));
```
- Takes dimensions: **rows × columns**
- Creates a 2D vector to store data bits

#### **Step 2: Calculate Row Parity Bits**
```cpp
vector<int> rowParity(rows);

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
```

**What's happening?**
- For each **row**, count how many **1s** are present
- `rowOnes % 2` gives:
  - `0` if count is **even** (even number of 1s)
  - `1` if count is **odd** (odd number of 1s)
- This is **even parity** (even count = 0, odd count = 1)

**Example:**
```
Row: 1 0 1 1
Count of 1s = 3 (odd)
rowParity = 3 % 2 = 1
```

#### **Step 3: Calculate Column Parity Bits**
```cpp
vector<int> colParity(cols);

for (int j = 0; j < cols; j++) {
    int colOnes = 0;
    for (int i = 0; i < rows; i++) {
        if (data[i][j] == 1) {
            colOnes++;
        }
    }
    colParity[j] = (colOnes % 2); // Even parity
}
```

**What's happening?**
- Same logic, but for each **column**
- Count 1s **vertically** down each column
- Store column parity bits

**Example:**
```
Column: [1, 0, 1]ᵀ
Count of 1s = 2 (even)
colParity = 2 % 2 = 0
```

#### **Step 4: Simulate Error**
```cpp
int errorRow, errorCol;
cout << "Enter error position (row col) to simulate [0-indexed]: ";
cin >> errorRow >> errorCol;

if (errorRow < rows && errorCol < cols) {
    data[errorRow][errorCol] = 1 - data[errorRow][errorCol]; // Flip bit
}
```

**What's happening?**
- User enters position where error should occur
- `1 - data[errorRow][errorCol]` flips the bit:
  - If bit was `0` → becomes `1`
  - If bit was `1` → becomes `0`

#### **Step 5: Receiver Side - Detect Error**
```cpp
vector<int> recvRowParity(rows);
vector<int> recvColParity(cols);
int finalErrorRow = -1;
int finalErrorCol = -1;

// Recalculate row parity on received data
for (int i = 0; i < rows; i++) {
    int rowOnes = 0;
    for (int j = 0; j < cols; j++) {
        if (data[i][j] == 1) rowOnes++;
    }
    recvRowParity[i] = (rowOnes % 2);
    if (recvRowParity[i] != rowParity[i]) {  // Parity mismatch!
        finalErrorRow = i;  // Error in this row
    }
}
```

**What's happening?**
- Recalculate row parity from **received** data
- Compare with **original** row parity
- If they don't match → error in that row!

**Example:**
```
Original row parity: [1, 0, 1]
Received row parity: [1, 1, 1]  (row 2 changed!)
→ finalErrorRow = 2
```

#### **Step 6: Detect Column Error**
```cpp
for (int j = 0; j < cols; j++) {
    int colOnes = 0;
    for (int i = 0; i < rows; i++) {
        if (data[i][j] == 1) colOnes++;
    }
    recvColParity[j] = (colOnes % 2);
    if (recvColParity[j] != colParity[j]) {
        finalErrorCol = j;  // Error in this column
    }
}
```

- Same logic for columns
- Find which column has error

#### **Step 7: Correct Error**
```cpp
if (finalErrorRow != -1 && finalErrorCol != -1) {
    cout << "\nError detected at: (row " << finalErrorRow << ", col " << finalErrorCol << ")\n";
    data[finalErrorRow][finalErrorCol] = 1 - data[finalErrorRow][finalErrorCol]; // Flip bit back!
    cout << "Corrected Data:\n";
    printMatrix(data);
}
```

**Magic! ✨**
- Intersection of **error row** + **error column** = exact error position
- Flip that bit back → **Error corrected!**

**Why This Works:**
```
If error at (row 2, col 3):
- Row 2 parity fails (error in this row)
- Col 3 parity fails (error in this column)
- Intersection = (2, 3) → That's where error is!
```

---

## 🔧 HAMMING CODE - Code Explanation

### Function: `performHammingCode()`

Implements **(7,4) Hamming Code** = 4 data bits + 3 parity bits

#### **Step 1: Input Data**
```cpp
vector<int> data(4);
cout << "Enter 4 data bits: ";
cin >> data[0] >> data[1] >> data[2] >> data[3];
```

#### **Step 2: Setup Codeword Structure**
```cpp
vector<int> codeWord(7);

// Position mapping:
// d7 d6 d5 r4 d3 r2 r1
// pos: 7  6  5  4  3  2  1 (1-indexed)
// idx: 6  5  4  3  2  1  0 (0-indexed in array)

codeWord[2] = data[0]; // d3 at position 3
codeWord[4] = data[1]; // d5 at position 5
codeWord[5] = data[2]; // d6 at position 6
codeWord[6] = data[3]; // d7 at position 7
```

**Key Concept:**
- Parity bits go at positions: **1, 2, 4** (powers of 2)
- Data bits fill positions: **3, 5, 6, 7**

#### **Step 3: Calculate Parity Bits**

```cpp
// r1 at position 1: Checks positions 1, 3, 5, 7
codeWord[0] = codeWord[2] ^ codeWord[4] ^ codeWord[6]; 
// r1 = d3 ^ d5 ^ d7

// r2 at position 2: Checks positions 2, 3, 6, 7
codeWord[1] = codeWord[2] ^ codeWord[5] ^ codeWord[6]; 
// r2 = d3 ^ d6 ^ d7

// r4 at position 4: Checks positions 4, 5, 6, 7
codeWord[3] = codeWord[4] ^ codeWord[5] ^ codeWord[6]; 
// r4 = d5 ^ d6 ^ d7
```

**How Parity Bits Work:**
- Each parity bit checks **specific positions**
- Uses **XOR (^)** operation = binary addition without carry
- `0^0=0, 0^1=1, 1^0=1, 1^1=0`

**Parity Bit Position Strategy:**
- Position **1** (r1): Checks all positions where bit 1 (LSB) is set: 1,3,5,7
- Position **2** (r2): Checks all positions where bit 2 is set: 2,3,6,7
- Position **4** (r4): Checks all positions where bit 3 is set: 4,5,6,7

**Example:**
```
Data: d3=1, d5=0, d6=1, d7=1

r1 = d3 ^ d5 ^ d7 = 1 ^ 0 ^ 1 = 0
r2 = d3 ^ d6 ^ d7 = 1 ^ 1 ^ 1 = 1
r4 = d5 ^ d6 ^ d7 = 0 ^ 1 ^ 1 = 0

Codeword: d7 d6 d5 r4 d3 r2 r1
          1  1  0  0  1  1  0
```

#### **Step 4: Display Codeword**
```cpp
cout << "Sent Codeword (d7 d6 d5 r4 d3 r2 r1): ";
for (int i = 6; i >= 0; i--) {
    cout << codeWord[i] << " ";
}
```
- Prints from index 6 → 0 (position 7 → 1)

#### **Step 5: Simulate Error**
```cpp
int errorPos;
cout << "Enter error position to simulate (1-7): ";
cin >> errorPos;

if (errorPos >= 1 && errorPos <= 7) {
    codeWord[errorPos - 1] = 1 - codeWord[errorPos - 1]; // Flip bit
}
```
- User inputs position (1-7)
- Array index = position - 1

#### **Step 6: Receiver - Calculate Syndrome**
```cpp
// Recalculate parity checks
int c1 = codeWord[0] ^ codeWord[2] ^ codeWord[4] ^ codeWord[6];
int c2 = codeWord[1] ^ codeWord[2] ^ codeWord[5] ^ codeWord[6];
int c4 = codeWord[3] ^ codeWord[4] ^ codeWord[5] ^ codeWord[6];

// Syndrome = binary number (c4 c2 c1)
int syndrome = c4 * 4 + c2 * 2 + c1;
```

**Syndrome Calculation:**
- If no error → all checks should be 0
- If error → checks will show which parity bits are wrong
- Syndrome gives **exact error position**!

**Example:**
```
If error at position 5:
- Position 5 is checked by r1 and r4
- So c1=1, c2=0, c4=1
- Syndrome = 1*4 + 0*2 + 1 = 5 ✓
```

**Position → Syndrome Mapping:**
```
Error at pos 1 → syndrome = 001 = 1
Error at pos 2 → syndrome = 010 = 2
Error at pos 3 → syndrome = 011 = 3
Error at pos 4 → syndrome = 100 = 4
Error at pos 5 → syndrome = 101 = 5
Error at pos 6 → syndrome = 110 = 6
Error at pos 7 → syndrome = 111 = 7
```

#### **Step 7: Correct Error**
```cpp
if (syndrome == 0) {
    cout << "No error detected.\n";
} else {
    cout << "Error detected at position: " << syndrome << endl;
    codeWord[syndrome - 1] = 1 - codeWord[syndrome - 1]; // Correct
}
```
- If syndrome = 0 → No error
- Otherwise → Flip bit at syndrome position

---

## 🔐 CRC - Code Explanation

### Function: `performCrc()`

#### **Step 1: Input Data and Generator**
```cpp
string data, generator;
cout << "Enter data bit string: ";
cin >> data;
cout << "Enter generator polynomial string: ";
cin >> generator;
```

**Example:**
- Data: `"101101"`
- Generator: `"1101"` (polynomial x³ + x² + 1)

#### **Step 2: Augment Data (Append Zeros)**
```cpp
int r = generator.length() - 1;
string augmentedData = data;
for (int i = 0; i < r; i++) {
    augmentedData += '0';
}
```

**What's happening?**
- Append **r zeros** where r = degree of generator - 1
- If generator has length 4 → degree 3 → append 3 zeros

**Example:**
```
Data: "101101"
Generator length = 4 → r = 3
Augmented: "101101000"
```

#### **Step 3: XOR Division (Key Algorithm)**

```cpp
string xorDivision(string data, string generator) {
    int genLen = generator.length();
    int dataLen = data.length();

    for (int i = 0; i <= dataLen - genLen; ) {
        if (data[i] == '1') {
            // XOR with generator
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
        
        if(nextOne > dataLen - genLen) break;
        i = nextOne;
    }
    
    return data.substr(dataLen - genLen + 1); // Return remainder
}
```

**XOR Division Algorithm:**
1. Start from leftmost bit
2. If current bit is **'1'**:
   - XOR current position + generator length with generator
3. Move to next **'1'** bit
4. Repeat until you can't divide anymore
5. Last (genLen-1) bits = **Remainder**

**Example Walkthrough:**
```
Augmented: 101101000
Generator: 1101

Step 1: Start at pos 0 (bit='1')
  101101000
  1101      (XOR)
  --------
  011001000  → Result after XOR
  ↑ (moved here, next '1' at pos 1)

Step 2: At pos 1 (bit='1')
  011001000
   1101     (XOR)
  --------
  001111000  → Result

Step 3: Continue...
Final remainder = last 3 bits = "001"
```

**XOR Rule:**
- Same bits → '0' (0^0=0, 1^1=0)
- Different bits → '1' (0^1=1, 1^0=1)

#### **Step 4: Create Codeword**
```cpp
string remainder = xorDivision(augmentedData, generator);
string codeWord = data + remainder;
```

- Remainder = **FCS (Frame Check Sequence)**
- Transmit = Original Data + FCS

**Example:**
```
Data: "101101"
Remainder: "001"
Transmitted: "101101001"
```

#### **Step 5: Receiver Side**
```cpp
string receivedWord;
cout << "Enter received codeword: ";
cin >> receivedWord;

string recvRemainder = xorDivision(receivedWord, generator);
```

- Divide received word by same generator
- Get remainder

#### **Step 6: Check for Errors**
```cpp
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
```

**Logic:**
- If remainder = all zeros → **No error** ✅
- If remainder has any '1' → **Error detected** ❌

**Why This Works:**
- If no error → received = transmitted exactly
- Division gives remainder = 0 (all zeros)
- If error → remainder ≠ 0 (has ones)

---


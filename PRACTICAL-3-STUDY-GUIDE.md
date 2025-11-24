# Practical 3: Error Detection & Correction - Study Guide

## 📚 THEORY: Why Do We Need Error Detection & Correction?

### The Problem

When data is transmitted over a network (wired or wireless), it can get **corrupted** due to:

-   **Noise** in communication channels
-   **Interference** from other signals
-   **Signal attenuation** (weak signals)
-   **Timing errors**

### The Solution: Redundancy

We add **extra bits** (called **redundancy bits** or **check bits**) to our original data. This helps us:

-   **Detect** if errors occurred
-   **Correct** errors automatically (in some methods)

---

## 🔍 1. SIMPLE (1D) PARITY CHECK

### Concept:

-   Add **ONE extra bit** to the data
-   This bit makes the total count of 1s either **even** or **odd**

### Types:

-   **Even Parity**: Total number of 1s (including parity bit) = Even
-   **Odd Parity**: Total number of 1s (including parity bit) = Odd

### Example:

```
Original Data: 1 0 1 1 0
Count of 1s = 3 (odd)

For EVEN parity:
- Parity bit = 1 (to make total even: 3+1=4)
- Codeword: 1 0 1 1 0 | 1

For ODD parity:
- Parity bit = 0 (to keep total odd: 3+0=3)
- Codeword: 1 0 1 1 0 | 0
```

### Working:

1. **Sender**: Count 1s → add parity bit → transmit
2. **Receiver**: Count 1s in received data
    - If count matches expected parity → **No error**
    - If count doesn't match → **Error detected** ❌

### Limitations:

-   ✅ Can **detect** single-bit errors
-   ✅ Can **detect** odd number of errors
-   ❌ **Cannot detect** even number of errors (e.g., 2 bits flipped)
-   ❌ **Cannot correct** errors (only detects)

---

## 📊 2. 2D PARITY CHECK (Two-Dimensional Parity)

### Concept:

-   Organize data in a **matrix/table** (rows × columns)
-   Add **row parity bits** for each row
-   Add **column parity bits** for each column
-   This creates a **2D parity structure**

### Why Better Than 1D?

-   Can **detect** AND **correct** single-bit errors! ✨
-   Can **detect** multiple errors (though may not correct all)

### Example:

```
Original Data (3×4):
1 0 1 1   → Row 1 has 3 ones (odd) → Row parity = 1 (even parity)
0 1 1 0   → Row 2 has 2 ones (even) → Row parity = 0
1 1 0 1   → Row 3 has 3 ones (odd) → Row parity = 1

Column parity (bottom row):
2 2 2 2 (count of 1s) → Column parity = 0 0 0 0 (all even)

Final Matrix:
1 0 1 1 | 1
0 1 1 0 | 0
1 1 0 1 | 1
-----------
0 0 0 0 | ? (corner parity)
```

### Error Detection & Correction:

1. **Detect**: Check if row parity OR column parity fails
2. **Locate**: Intersection of failed row + failed column = error position
3. **Correct**: Flip the bit at that position!

---

## 🔧 3. HAMMING CODE

### Concept:

-   Uses **multiple parity bits** placed at specific positions
-   Can **detect** AND **correct** single-bit errors
-   Common: **(7,4) Hamming Code** = 4 data bits + 3 parity bits = 7 bits total

### Position Strategy:

-   Parity bits at positions: **1, 2, 4, 8, 16...** (powers of 2)
-   Data bits fill remaining positions

### (7,4) Hamming Code Structure:

```
Position:  7   6   5   4   3   2   1
Bit:      d7  d6  d5  r4  d3  r2  r1
          │   │   │   │   │   │   │
          Data bits    Parity bits
```

### Parity Bit Calculation:

-   **r1** (pos 1): Checks positions 1, 3, 5, 7
-   **r2** (pos 2): Checks positions 2, 3, 6, 7
-   **r4** (pos 4): Checks positions 4, 5, 6, 7

### Syndrome Calculation:

-   If error occurs, calculate **syndrome** (c4, c2, c1)
-   Syndrome value directly gives **error position**!

---

## 🔐 4. CRC (Cyclic Redundancy Check)

### Concept:

-   Uses **polynomial division** (binary XOR division)
-   More powerful than parity checks
-   Used in Ethernet, WiFi, etc.

### Working:

1. **Sender**:

    - Append **r zero bits** (r = generator degree - 1)
    - Divide by **generator polynomial** (binary)
    - Remainder = **FCS (Frame Check Sequence)**
    - Append FCS to original data → Transmit

2. **Receiver**:
    - Divide received data by same generator
    - If remainder = 0 → **No error** ✅
    - If remainder ≠ 0 → **Error detected** ❌

### Example:

```
Data: 101101
Generator: 1101 (degree 3, so append 3 zeros)

1. Augment: 101101000
2. Divide by 1101 using XOR
3. Remainder = FCS
4. Transmit: Data + FCS
```

### Features:

-   ✅ Very effective for detecting **burst errors**
-   ✅ Can detect up to **r errors** (r = generator degree)
-   ❌ **Cannot correct** errors (only detects)

---

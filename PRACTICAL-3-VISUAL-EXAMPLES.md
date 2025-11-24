# Practical 3: Visual Examples & Quick Reference

---

## 🎯 QUICK COMPARISON TABLE

| Method | Can Detect | Can Correct | Redundancy Bits | Complexity |
|--------|-----------|-------------|-----------------|------------|
| **1D Parity** | Single-bit, Odd errors | ❌ No | 1 bit | Very Simple |
| **2D Parity** | Single-bit, Multiple errors | ✅ Single-bit | n+m+1 bits | Simple |
| **Hamming Code** | Single-bit | ✅ Single-bit | log₂(n+1) bits | Medium |
| **CRC** | Burst errors, Multiple errors | ❌ No | r bits (generator degree) | Complex |

---

## 🔍 1D PARITY - VISUAL EXAMPLE

### Example 1: Even Parity
```
Original Data:  1 0 1 1 0
Count of 1s:    3 (odd)
Parity bit:     1 (to make even: 3+1=4)
Codeword:       1 0 1 1 0 | 1
                └─ Data ─┘ │
                           └─ Parity
```

### Example 2: Error Detection
```
Sent:        1 0 1 1 0 | 1  (4 ones - even ✓)
Received:    1 0 1 0 0 | 1  (2 ones - even ✓)
Wait! Original had 4 ones, received has 2 ones!
→ Error detected! (Should have detected, but shows limitation)

Actually, for even parity:
Sent: Count = 4 (even) → OK
Received: Count = 2 (even) → OK
→ Error NOT detected! (2 bits flipped)
```

**This is why 1D parity has limitations!**

---

## 📊 2D PARITY - VISUAL EXAMPLE

### Example: 3×4 Matrix

#### Original Data + Parity Bits:
```
         Col1  Col2  Col3  Col4  | Row Parity
Row 1:    1     0     1     1    |     1
Row 2:    0     1     1     0    |     0
Row 3:    1     1     0     1    |     1
         ─────────────────────────
Col Par:  0     0     0     0    |     ?
```

**Calculation:**
- Row 1: 1,0,1,1 → 3 ones (odd) → parity = 1
- Row 2: 0,1,1,0 → 2 ones (even) → parity = 0
- Row 3: 1,1,0,1 → 3 ones (odd) → parity = 1
- Col 1: 1,0,1 → 2 ones (even) → parity = 0
- Col 2: 0,1,1 → 2 ones (even) → parity = 0
- Col 3: 1,1,0 → 2 ones (even) → parity = 0
- Col 4: 1,0,1 → 2 ones (even) → parity = 0

#### Error at Position (2, 3):
```
Original:            After Error:
  1 0 1 1 | 1          1 0 1 1 | 1
  0 1 1 0 | 0          0 1 0 0 | 1  ← Row parity fails!
  1 1 0 1 | 1          1 1 0 1 | 1
  ───────────────────────────────────
  0 0 0 0               0 0 1 0  ← Col parity fails!
                      ↑
                   Error here!
```

**Detection:**
- Row 2 parity: Expected 0, Got 1 → **Row 2 has error**
- Col 3 parity: Expected 0, Got 1 → **Col 3 has error**
- Intersection = (2, 3) → **Error position found!**

**Correction:**
- Flip bit at (2, 3): 0 → 1
- **Error corrected!** ✨

---

## 🔧 HAMMING CODE - VISUAL EXAMPLE

### (7,4) Hamming Code Structure

```
Position:  7   6   5   4   3   2   1
           │   │   │   │   │   │   │
Bit Type:  d7  d6  d5  r4  d3  r2  r1
           │   │   │   │   │   │   │
           └───┴───┴───┴───┴───┴───┘
           Data Bits    Parity Bits
```

### Example: Data = 1 0 1 1

#### Step 1: Place Data Bits
```
Position:  7   6   5   4   3   2   1
Bit:      d7  d6  d5  ?   d3  ?   ?
Value:     1   0   1   ?   1   ?   ?
```

#### Step 2: Calculate Parity Bits
```
r1 (pos 1): Checks 1,3,5,7 → d3 ^ d5 ^ d7 = 1 ^ 1 ^ 1 = 1
r2 (pos 2): Checks 2,3,6,7 → d3 ^ d6 ^ d7 = 1 ^ 0 ^ 1 = 0
r4 (pos 4): Checks 4,5,6,7 → d5 ^ d6 ^ d7 = 1 ^ 0 ^ 1 = 0
```

#### Step 3: Complete Codeword
```
Position:  7   6   5   4   3   2   1
Bit:      d7  d6  d5  r4  d3  r2  r1
Value:     1   0   1   0   1   0   1

Codeword: 1 0 1 0 1 0 1
          (d7 d6 d5 r4 d3 r2 r1)
```

#### Error at Position 5:
```
Sent:      1   0   1   0   1   0   1
Error:     1   0   0   0   1   0   1  (bit at pos 5 flipped)

Receiver calculates:
c1 = r1 ^ d3 ^ d5 ^ d7 = 1 ^ 1 ^ 0 ^ 1 = 1
c2 = r2 ^ d3 ^ d6 ^ d7 = 0 ^ 1 ^ 0 ^ 1 = 0
c4 = r4 ^ d5 ^ d6 ^ d7 = 0 ^ 0 ^ 0 ^ 1 = 1

Syndrome = c4×4 + c2×2 + c1 = 1×4 + 0×2 + 1 = 5

Error at position 5! → Flip bit → Corrected! ✓
```

---

## 🔐 CRC - VISUAL EXAMPLE

### Example: Data = 101101, Generator = 1101

#### Step 1: Augment Data
```
Data:       101101
Generator:  1101 (degree 3)
Append:     000 (3 zeros)

Augmented:  101101000
```

#### Step 2: XOR Division Process

```
         101101000  (augmented data)
         1101       (generator)
         ────
         011001000
          1101
          ────
          0111000
           1101
           ────
           000000

Remainder: 001
```

**Step-by-step XOR:**
- XOR rule: Same → 0, Different → 1
- Align generator with first '1' in data
- XOR bit by bit

#### Step 3: Create Codeword
```
Data:      101101
Remainder: 001
───────────────
Transmit:  101101001
```

#### Step 4: Receiver Check
```
Received:  101101001
Divide by: 1101

Remainder: 000  (all zeros)
→ No error! ✓

If error occurred:
Received:  101101011
Divide by: 1101

Remainder: 010  (has ones)
→ Error detected! ❌
```

---

## 🎓 KEY FORMULAS & RULES

### Parity Calculation:
```
Even Parity: parity = (count_of_1s) % 2
Odd Parity:  parity = 1 - (count_of_1s % 2)
```

### Hamming Code Parity:
```
r1 = d3 ⊕ d5 ⊕ d7
r2 = d3 ⊕ d6 ⊕ d7
r4 = d5 ⊕ d6 ⊕ d7

Syndrome = c4×4 + c2×2 + c1
```

### CRC:
```
Augmented Data = Original Data + r zeros
Transmitted = Original Data + Remainder (FCS)
Receiver checks: Remainder == 0? No error : Error
```

---

## 📝 COMMON MISTAKES TO AVOID

### 1. Parity Check:
- ❌ Forgetting that even parity with odd count = 1
- ❌ Confusing row and column parity
- ❌ Not checking intersection for error location

### 2. Hamming Code:
- ❌ Mixing up 0-indexed array vs 1-indexed position
- ❌ Wrong parity bit calculation (XOR positions)
- ❌ Syndrome calculation error (wrong formula)

### 3. CRC:
- ❌ Not appending enough zeros (should be r = degree - 1)
- ❌ XOR division mistakes (same→0, different→1)
- ❌ Taking wrong remainder (should be last r bits)

---


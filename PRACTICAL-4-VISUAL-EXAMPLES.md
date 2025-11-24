# Practical 4: Visual Examples & Scenarios

---

## 🛑 STOP AND WAIT ARQ - Examples

### Example 1: Successful Transmission
```
Time    Sender                  Receiver
─────────────────────────────────────────
T1      Send Frame 1 [Seq=0]
                          ────>
T2                              Receive Frame 1 [Seq=0]
                                Send ACK 0
                          <────
T3      Receive ACK 0
        Send Frame 2 [Seq=1]
                          ────>
T4                              Receive Frame 2 [Seq=1]
                                Send ACK 1
                          <────
T5      Receive ACK 1
        Done!
```

### Example 2: Frame Loss
```
Time    Sender                  Receiver
─────────────────────────────────────────
T1      Send Frame 1 [Seq=0]
                          ────>
                          [LOST!]
T2      (Wait...)
        Timeout!
T3      Resend Frame 1 [Seq=0]
                          ────>
T4                              Receive Frame 1 [Seq=0]
                                Send ACK 0
                          <────
T5      Receive ACK 0
        Send Frame 2 [Seq=1]
```

**Key Point**: Sequence number stays **same** for retransmission!
- Receiver can detect duplicate using sequence number

### Example 3: ACK Loss
```
Time    Sender                  Receiver
─────────────────────────────────────────
T1      Send Frame 1 [Seq=0]
                          ────>
T2                              Receive Frame 1 [Seq=0]
                                Send ACK 0
                          <──── [LOST!]
T3      Timeout!
        Resend Frame 1 [Seq=0]
                          ────>
T4                              Receive Frame 1 [Seq=0]
                                "Duplicate! (Seq=0 already received)"
                                Discard, Resend ACK 0
                          <────
T5      Receive ACK 0
        Send Frame 2 [Seq=1]
```

**Key Point**: Receiver detects duplicate using **same sequence number**!

---

## ⏪ GO-BACK-N ARQ - Examples

### Example 1: Successful Transmission (Window Size = 4)
```
Window: [1, 2, 3, 4]

Sender sends all frames in window:
Frame 1 ────> Received ✓ → ACK 2
Frame 2 ────> Received ✓ → ACK 3
Frame 3 ────> Received ✓ → ACK 4
Frame 4 ────> Received ✓ → ACK 5

All ACKs received → Slide window
New Window: [5, 6, 7, 8]
```

### Example 2: Frame Loss (Go Back!)
```
Window: [1, 2, 3, 4]

Sender sends:
Frame 1 ────> Received ✓ → ACK 2
Frame 2 ────> [LOST!]
Frame 3 ────> Received ✗ → Discard! (out-of-order)
Frame 4 ────> Received ✗ → Discard! (out-of-order)

Receiver:
- Accepts Frame 1 ✓
- Discards Frame 3 & 4 (out-of-order)
- Waiting for Frame 2

Sender: Timeout for ACK 2
Go back to Frame 2!

Retransmit:
Frame 2 ────> Received ✓ → ACK 3
Frame 3 ────> Received ✓ → ACK 4
Frame 4 ────> Received ✓ → ACK 5
```

**Key Point**: **Go back** to lost frame and **retransmit all after it**!

### Visual Window Representation:
```
Initial State:
Window: [1] [2] [3] [4] | 5 6 7 ...
       ───── Window ────
       base=1

After Frame 2 Lost:
Go back!
Window: [1] [2] [3] [4] | 5 6 7 ...
            ── Window ──
            base=2
            nextSeqNum=2 (go back!)
```

---

## 🔁 SELECTIVE REPEAT ARQ - Examples

### Example 1: Frame Loss (Selective Retransmission)
```
Window: [1, 2, 3, 4]

Sender sends:
Frame 1 ────> Received ✓ → Buffered, ACK 1
Frame 2 ────> [LOST!]
Frame 3 ────> Received ✓ → BUFFERED (not discarded!), ACK 3
Frame 4 ────> Received ✓ → BUFFERED (not discarded!), ACK 4

Receiver Buffer: [1, _, 3, 4]
                  ✓     ✓ ✓
                  Waiting for Frame 2

Sender:
- ACK 1 received → acked[1] = true
- ACK 2 missing → Timeout!
- ACK 3 received → acked[3] = true
- ACK 4 received → acked[4] = true

Selective Retransmission:
Frame 2 ────> Received ✓ → Buffered, ACK 2

Receiver Buffer: [1, 2, 3, 4] ✓
All frames in order → Deliver!
```

**Key Differences from Go-Back-N:**
- ✅ **Buffers** Frame 3 and 4 (not discarded!)
- ✅ **Only retransmits** Frame 2 (selective!)
- ❌ Go-Back-N would retransmit Frame 2, 3, 4

### Example 2: Window Sliding (Independent)
```
Initial: base = 1, Window = [1, 2, 3, 4]

Frame 1 ACKed → base = 2, Window = [2, 3, 4, 5]
Frame 3 ACKed → base = 2 (Frame 2 still waiting), Window = [2, 3, 4, 5]
Frame 2 ACKed → base = 4, Window = [4, 5, 6, 7]
Frame 4 ACKed → base = 5, Window = [5, 6, 7, 8]
```

**Key Point**: Window slides **independently** as frames are ACKed!
- Not blocked waiting for all frames in sequence

### Visual Window Representation:
```
Sender Window:          Receiver Window:
[1] [2] [3] [4]        [1] [2] [3] [4]
 ▲           ▲          ▲           ▲
base      Window      base      Window

Both windows can slide independently!

After Frame 1 ACKed:
[2] [3] [4] [5]        [2] [3] [4] [5]
 ▲           ▲          ▲           ▲
```

---

## 📊 COMPARISON: Scenario with Frame 2 Lost

### Stop and Wait:
```
Frame 1: Sent → ACK → Done ✓
Frame 2: Sent → [LOST] → Timeout → Resend Frame 2 → ACK → Done ✓
Frame 3: Sent → ACK → Done ✓
...
Efficiency: 33% (3 transmissions for 3 frames)
```

### Go-Back-N (Window Size = 4):
```
Send: Frame 1, 2, 3, 4
Frame 2 lost!
- Frame 1: Accepted ✓
- Frame 3: Discarded (out-of-order) ✗
- Frame 4: Discarded (out-of-order) ✗

Retransmit: Frame 2, 3, 4 (all after lost frame)

Total transmissions: 7 frames sent
Efficiency: ~57%
```

### Selective Repeat (Window Size = 4):
```
Send: Frame 1, 2, 3, 4
Frame 2 lost!
- Frame 1: Buffered ✓
- Frame 3: Buffered ✓ (not discarded!)
- Frame 4: Buffered ✓ (not discarded!)

Retransmit: Frame 2 only (selective!)

Total transmissions: 5 frames sent
Efficiency: ~80%
```

**Winner**: Selective Repeat! 🏆

---

## 🎯 SEQUENCE NUMBER VISUALIZATION

### Stop and Wait (0 and 1):
```
Frame 1: [Seq=0]
Frame 2: [Seq=1]
Frame 3: [Seq=0] ← Toggle back
Frame 4: [Seq=1]
...
Alternates between 0 and 1
```

### Go-Back-N / Selective Repeat:
```
Frame 1: [Seq=1]
Frame 2: [Seq=2]
Frame 3: [Seq=3]
Frame 4: [Seq=4]
...
Sequential numbers (within window)
```

---

## 🔑 COMMON SCENARIOS

### Scenario 1: Multiple Frame Loss in Window

**Go-Back-N:**
```
Window: [1, 2, 3, 4]
Frame 2 and 3 lost!

Receiver:
- Accepts Frame 1 ✓
- Discards Frame 4 (out-of-order)

Retransmit: Frame 2, 3, 4
```

**Selective Repeat:**
```
Window: [1, 2, 3, 4]
Frame 2 and 3 lost!

Receiver:
- Buffers Frame 1 ✓
- Buffers Frame 4 ✓

Retransmit: Frame 2, 3 only (selective!)
```

### Scenario 2: ACK Loss

**Stop and Wait:**
```
Frame 1 sent → Received ✓
ACK lost → Timeout
Retransmit Frame 1 [Seq=0] (same sequence!)
Receiver detects duplicate → Discard, Resend ACK
```

**Go-Back-N:**
```
Frame 1, 2, 3, 4 sent
ACK 2 lost (Frame 2 ACK)

Sender: Timeout for ACK 2
Go back to Frame 2 → Retransmit Frame 2, 3, 4
```

**Selective Repeat:**
```
Frame 1, 2, 3, 4 sent
ACK 2 lost

Sender: Timeout for ACK 2
Retransmit: Frame 2 only (others already ACKed)
```

---

## 🎓 KEY TAKEAWAYS

### Stop and Wait:
- ✅ **Sequence toggle**: 0 → 1 → 0 → 1
- ✅ **Duplicate detection**: Same sequence number = duplicate
- ❌ **Low efficiency**: One frame at a time

### Go-Back-N:
- ✅ **Cumulative ACK**: ACK N means frames 1 to N received
- ✅ **Window mechanism**: Send multiple frames
- ❌ **Wasteful**: Retransmits correct frames after error

### Selective Repeat:
- ✅ **Individual ACK**: Each frame ACKed separately
- ✅ **Buffering**: Receiver stores out-of-order frames
- ✅ **Selective**: Only retransmit lost frames
- ✅ **Efficient**: Best performance!

---


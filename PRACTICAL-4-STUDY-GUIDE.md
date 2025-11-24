# Practical 4: Sliding Window Protocols & ARQ - Study Guide

## 📚 THEORY: Why Do We Need ARQ Protocols?

### The Problem
When data is transmitted over unreliable networks:
- **Frames can be lost** during transmission
- **ACKs (Acknowledgments) can be lost**
- **Frames can arrive out of order**
- **Noisy channels** cause errors

### The Solution: ARQ (Automatic Repeat Request)
ARQ protocols ensure **reliable data transfer** by:
1. **Detecting** lost or corrupted frames
2. **Retransmitting** lost frames automatically
3. **Maintaining order** of frames

---

## 🔄 KEY CONCEPTS

### 1. Reliable Transmission
- **Sender** sends frames and waits for **ACK (Acknowledgment)**
- **Receiver** receives frames and sends **ACK** back
- If **ACK not received** → **Timeout** → **Retransmit**

### 2. Sequence Numbers
- Each frame has a **sequence number**
- Helps detect **duplicate frames** and **out-of-order frames**
- **Stop and Wait**: Uses 0 and 1 (alternating)
- **Sliding Window**: Uses range of numbers

### 3. Window Size
- **Sender Window**: Number of frames sender can send **without ACK**
- **Receiver Window**: Number of frames receiver can buffer
- Allows **pipelining** (multiple frames in transit)

### 4. Efficiency
- **Stop and Wait**: Sends one frame → waits for ACK (inefficient)
- **Sliding Window**: Sends multiple frames → more efficient! ✨

---

## 🛑 1. STOP AND WAIT ARQ

### Concept:
- Simplest ARQ protocol
- Sender sends **ONE frame** and **waits** for ACK
- **Only after** receiving ACK, sends next frame
- Uses **sequence numbers** 0 and 1 (alternating)

### Working:
1. **Sender**: Send frame with sequence number
2. **Wait** for ACK
3. **Receiver**: Receive frame → send ACK
4. **Sender**: If ACK received → send next frame
5. **If timeout** → retransmit same frame

### Sequence Numbers:
- Frame 1: Seq = 0
- Frame 2: Seq = 1
- Frame 3: Seq = 0 (alternates)
- Prevents **duplicate frame** problem

### Flow:
```
Sender          Receiver
  |                 |
  |--Frame 0------->|
  |                 | Process frame
  |<-----ACK 0------|
  |                 |
  |--Frame 1------->|
  |                 |
  |<-----ACK 1------|
  |                 |
```

### Limitations:
- ❌ **Very inefficient** (one frame at a time)
- ❌ **Low throughput** (waiting for ACK wastes time)
- ✅ **Simple** to implement

---

## ⏪ 2. GO-BACK-N ARQ

### Concept:
- **Sliding window** protocol
- Sender can send **multiple frames** (window size = N)
- **Receiver** only accepts frames **in order**
- If frame N is lost → **discard all frames after N**
- **Go back** and retransmit **from lost frame**

### Working:
1. **Sender**: Sends **window** of frames (frames 1 to N)
2. **Receiver**: Accepts frames **in order only**
3. **If frame lost**: Discard all subsequent frames
4. **Sender**: If ACK missing → **go back** to lost frame
5. **Retransmit** all frames from lost frame onwards

### Window Mechanism:
```
Sender Window (Size = 4):
[1] [2] [3] [4] | 5 6 7 ...
 ▲           ▲
 │           │
Base      Window
         Boundary

Sent frames: 1, 2, 3, 4
Waiting for ACKs...
```

### Example:
```
Sender sends: Frame 1, 2, 3, 4
Frame 2 lost!
Receiver: Accepts Frame 1, Discards 3 & 4 (out of order)
Sender: Timeout for ACK 2
Go back to Frame 2 → Retransmit: 2, 3, 4
```

### Key Features:
- ✅ **More efficient** than Stop and Wait
- ✅ **Pipelining** (multiple frames in transit)
- ❌ **Wasteful**: Retransmits **correct** frames after error
- ❌ Receiver window size = 1 (only accepts in-order)

---

## 🔁 3. SELECTIVE REPEAT ARQ

### Concept:
- **Best** sliding window protocol!
- Sender and receiver both have **windows**
- **Receiver buffers** out-of-order frames
- **Only retransmits** the **lost/damaged frame** (not all!)

### Working:
1. **Sender**: Sends window of frames
2. **Receiver**: **Buffers** frames even if out-of-order
3. **If frame lost**: Receiver buffers subsequent frames
4. **Sender**: Only retransmits **lost frame** (selective!)
5. Receiver can deliver **in-order** from buffer

### Window Mechanism:
```
Sender Window:    [1] [2] [3] [4] | 5 6 7 ...
Receiver Window:  [1] [2] [3] [4] | 5 6 7 ...

Both can slide independently!
```

### Example:
```
Sender sends: Frame 1, 2, 3, 4
Frame 2 lost!
Receiver: Accepts Frame 1, BUFFERS 3 & 4 (not discarded!)
Receiver: Sends ACK 1, NAK 2, ACK 3, ACK 4
Sender: Retransmits ONLY Frame 2
Receiver: Now has 1,2,3,4 in-order → Deliver!
```

### Key Features:
- ✅ **Most efficient** (no wasted retransmissions)
- ✅ **Receiver buffers** out-of-order frames
- ✅ **Selective retransmission** (only lost frame)
- ⚠️ **More complex** implementation

---

## 📊 COMPARISON TABLE

| Protocol | Window Size | Retransmission | Efficiency | Complexity |
|----------|-------------|----------------|------------|------------|
| **Stop and Wait** | 1 (sender only) | Lost frame only | ❌ Low | ✅ Simple |
| **Go-Back-N** | N (sender), 1 (receiver) | Lost frame + all after | ⚠️ Medium | ⚠️ Medium |
| **Selective Repeat** | N (sender), N (receiver) | Only lost frame | ✅ High | ❌ Complex |

---

## 🎯 KEY TERMS

### 1. ACK (Acknowledgment)
- Confirmation that frame was received correctly
- Sequence number indicates "expected next frame"

### 2. NAK (Negative Acknowledgment)
- Indicates frame was received **with error** or **lost**
- Request for retransmission

### 3. Timeout
- If ACK not received within **time limit**
- Sender assumes frame was **lost**
- **Retransmits** frame

### 4. Sequence Number
- Unique identifier for each frame
- Prevents duplicate and out-of-order problems

### 5. Sliding Window
- **Base**: Oldest unacked frame
- **Window**: Frames that can be sent
- **Slides forward** as frames are ACKed

---

## 🔑 IMPORTANT POINTS

### Stop and Wait:
- **Sequence numbers**: 0, 1 (toggle)
- **ACK lost** → Retransmit (duplicate frame)
- **Duplicate detection**: Check sequence number

### Go-Back-N:
- **Receiver discards** out-of-order frames
- **Retransmits all** from lost frame
- **Cumulative ACK**: ACK N means frames 1 to N received

### Selective Repeat:
- **Receiver buffers** out-of-order frames
- **Individual ACK**: Each frame ACKed separately
- **Selective retransmission**: Only lost frame retransmitted
- **Window size constraint**: Must be ≤ 2^(n-1) where n = sequence number bits

---


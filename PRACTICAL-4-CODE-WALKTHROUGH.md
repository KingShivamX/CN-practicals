# Practical 4: Code Walkthrough - Line by Line

---

## 🛑 1. STOP AND WAIT ARQ - Code Explanation

### Function: `performStopAndWait()`

#### **Concept Recap:**
- Simplest ARQ protocol
- Send **ONE frame** → Wait for **ACK** → Send next frame
- Sequence numbers: 0 and 1 (alternate)

#### **Code Walkthrough:**

**Step 1: Initialize Variables**
```cpp
int totalFrames;
cout << "Enter number of frames to send: ";
cin >> totalFrames;

int currentFrame = 1;
int seqNum = 0;
const int lossProb = 30; // 30% chance of loss
```

**What's happening?**
- `currentFrame`: Current frame number (1, 2, 3, ...)
- `seqNum`: Sequence number (0 or 1, toggles)
- `lossProb`: Probability of frame/ACK loss (30%)

**Step 2: Main Loop**
```cpp
while (currentFrame <= totalFrames) {
    cout << "Sender: Sending Frame " << currentFrame << " [Seq=" << seqNum << "]..." << endl;
    sleep(1);
```

**What's happening?**
- Loop until all frames sent
- Send frame with current sequence number
- `sleep(1)`: Simulates transmission delay

**Step 3: Simulate Frame Loss**
```cpp
// Simulate frame loss
if (shouldEventHappen(lossProb)) {
    cout << "  -> Frame " << currentFrame << " lost!" << endl;
    cout << "Sender: Timeout... Resending Frame " << currentFrame << ".\n" << endl;
    continue; // Resend the same frame
}
```

**What's happening?**
- `shouldEventHappen(30)`: 30% chance returns true
- If frame lost → print message → `continue` (retransmit same frame)
- **Key point**: `currentFrame` and `seqNum` **don't change** → retransmit

**Example:**
```
Sending Frame 1 [Seq=0]...
  -> Frame 1 lost!
Timeout... Resending Frame 1 [Seq=0]... (same seq number!)
```

**Step 4: Receiver Receives Frame**
```cpp
cout << "Receiver: Received Frame " << currentFrame << " [Seq=" << seqNum << "]." << endl;
sleep(1);
```

**What's happening?**
- Frame successfully received
- Receiver processes frame

**Step 5: Simulate ACK Loss**
```cpp
// Simulate ACK loss
if (shouldEventHappen(lossProb)) {
    cout << "  -> ACK for Frame " << currentFrame << " lost!" << endl;
    cout << "Sender: Timeout... Resending Frame " << currentFrame << ".\n" << endl;
    continue; // Resend the same frame
}
```

**What's happening?**
- **ACK can also be lost!**
- If ACK lost → sender timeout → retransmit frame
- This creates **duplicate frame** problem
- **Solution**: Sequence numbers help detect duplicates!

**Example:**
```
Frame 1 sent → Received ✓
ACK lost! → Timeout
Retransmit Frame 1 [Seq=0]
Receiver: "I already have Seq=0, this is duplicate!"
Discard duplicate frame, send ACK again
```

**Step 6: Move to Next Frame**
```cpp
cout << "Sender: Received ACK for Frame " << currentFrame << " [Seq=" << seqNum << "].\n" << endl;

// Move to the next frame
currentFrame++;
seqNum = 1 - seqNum; // Toggle sequence number 0 -> 1 or 1 -> 0
```

**What's happening?**
- ACK received successfully!
- `currentFrame++`: Move to next frame
- `seqNum = 1 - seqNum`: **Toggle** sequence number
  - If seqNum = 0 → becomes 1
  - If seqNum = 1 → becomes 0

**Sequence Number Toggle Logic:**
```
Frame 1: seqNum = 0
Frame 2: seqNum = 1
Frame 3: seqNum = 0
Frame 4: seqNum = 1
...
Alternates between 0 and 1
```

**Why Toggle?**
- If duplicate frame arrives → sequence number is **same** → receiver knows it's duplicate!
- If new frame arrives → sequence number is **different** → receiver accepts!

---

## ⏪ 2. GO-BACK-N ARQ - Code Explanation

### Function: `performGoBackN()`

#### **Concept Recap:**
- Sliding window protocol
- Sender sends **window** of frames
- Receiver accepts **in-order only** (discards out-of-order)
- If frame lost → **go back** and retransmit **from lost frame**

#### **Code Walkthrough:**

**Step 1: Initialize Variables**
```cpp
int totalFrames, windowSize;
cout << "Enter total number of frames: ";
cin >> totalFrames;
cout << "Enter window size: ";
cin >> windowSize;

int base = 1; // Base of the sender's window
int nextSeqNum = 1; // Next frame to be sent
const int lossProb = 25; // 25% chance of loss
```

**What's happening?**
- `windowSize`: Number of frames sender can send without ACK
- `base`: Oldest unacked frame (start of window)
- `nextSeqNum`: Next frame number to send

**Window Visualization:**
```
Window Size = 4

Sent:     [1] [2] [3] [4] | 5 6 7 ...
         ────── Window ──────
         ▲                 ▲
      base=1         nextSeqNum=5
```

**Step 2: Main Loop**
```cpp
while (base <= totalFrames) {
    // Send all frames in the current window
    while (nextSeqNum < base + windowSize && nextSeqNum <= totalFrames) {
        cout << "Sender: Sending Frame " << nextSeqNum << "..." << endl;
        sleep(1);
        nextSeqNum++;
    }
```

**What's happening?**
- Outer loop: Continue until `base` exceeds total frames
- Inner loop: Send all frames in **window**
- **Condition**: `nextSeqNum < base + windowSize`
  - Window = [base to base + windowSize - 1]
  - Send frames until window is full

**Example:**
```
base = 1, windowSize = 4
Window = [1, 2, 3, 4]
Send: Frame 1, 2, 3, 4
nextSeqNum becomes 5
```

**Step 3: Simulate Reception**
```cpp
int ackToReceive = base;
bool frameLost = false;

for (int i = base; i < nextSeqNum; i++) {
    if (!frameLost && shouldEventHappen(lossProb)) {
        cout << "  -> Frame " << i << " was lost!" << endl;
        frameLost = true;
    }

    if (!frameLost) {
        cout << "Receiver: Received Frame " << i << ". Sending ACK " << (i + 1) << "." << endl;
        ackToReceive++;
    } else {
        cout << "Receiver: Discarding Frame " << i << " (out of order)." << endl;
    }
}
```

**What's happening?**
- Loop through all frames in window
- `frameLost`: Tracks if any frame was lost
- **If frame lost**:
  - Mark `frameLost = true`
  - **Discard all subsequent frames** (Go-Back-N behavior!)
  - Receiver **only accepts in-order** frames

**Go-Back-N Logic:**
```
Sender sends: Frame 1, 2, 3, 4
Frame 2 lost!

Receiver:
- Accepts Frame 1 ✓
- Frame 2 lost → Discard!
- Discards Frame 3 (out-of-order) ❌
- Discards Frame 4 (out-of-order) ❌
```

**Step 4: Handle Loss/Timeout**
```cpp
if (frameLost) {
    int lostFrame = ackToReceive; // ackToReceive stuck at the lost frame number
    cout << "Sender: Timeout! (Did not receive ACK " << lostFrame << ")." << endl;
    cout << "Sender: Going back to Frame " << lostFrame << ".\n" << endl;
    nextSeqNum = lostFrame; // Go back
    base = lostFrame;
} else {
    cout << "Sender: Received all ACKs for window. Sliding window." << endl;
    cout << "Window base is now " << nextSeqNum << ".\n" << endl;
    base = nextSeqNum;
}
```

**What's happening?**
- **If frame lost**:
  - `ackToReceive` = lost frame number (didn't increment past lost frame)
  - **Go back**: `nextSeqNum = lostFrame`, `base = lostFrame`
  - Next iteration will **retransmit from lost frame**

- **If all frames received**:
  - **Slide window**: `base = nextSeqNum`
  - Window moves forward!

**Go-Back-N Retransmission:**
```
Frame 2 lost → ackToReceive = 2
nextSeqNum = 2 (go back!)
base = 2

Next iteration:
Send: Frame 2, 3, 4 (retransmit all from lost frame!)
```

---

## 🔁 3. SELECTIVE REPEAT ARQ - Code Explanation

### Function: `performSelectiveRepeat()`

#### **Concept Recap:**
- Best sliding window protocol
- **Receiver buffers** out-of-order frames
- **Selective retransmission**: Only retransmit lost frame
- Both sender and receiver have windows

#### **Code Walkthrough:**

**Step 1: Initialize Variables**
```cpp
int totalFrames, windowSize;
cout << "Enter total number of frames: ";
cin >> totalFrames;
cout << "Enter window size: ";
cin >> windowSize;

vector<bool> acked(totalFrames + 1, false);
int base = 1;
int nextSeqNum = 1;
const int lossProb = 30;
```

**What's happening?**
- `acked[i]`: Tracks if frame `i` is ACKed
- `base`: Oldest unacked frame
- `nextSeqNum`: Next frame to send

**Key Difference from Go-Back-N:**
- Uses **array to track ACKs** for each frame individually
- **Individual ACKs** (not cumulative)

**Step 2: Display Current Window**
```cpp
while (base <= totalFrames) {
    cout << "--- Current Window [" << base << " to " << min(base + windowSize - 1, totalFrames) << "] ---" << endl;
```

**What's happening?**
- Shows current window range
- Window = [base to base + windowSize - 1]

**Step 3: Send Frames in Window**
```cpp
// Send all unacked frames in the window
for (int i = nextSeqNum; i < base + windowSize && i <= totalFrames; i++) {
    if (i == nextSeqNum) { // Only advance nextSeqNum for new frames
        cout << "Sender: Sending new Frame " << i << "..." << endl;
        nextSeqNum++;
    } else if (!acked[i]) {
        cout << "Sender: Resending Frame " << i << "..." << endl;
    }
}
```

**What's happening?**
- Loop through window frames
- **If `i == nextSeqNum`**: New frame → send and increment
- **If `!acked[i]`**: Frame not ACKed → resend
- **Selective retransmission**: Only resends frames that are not ACKed!

**Example:**
```
Window: [1, 2, 3, 4]
Frame 2 lost, Frame 1, 3, 4 ACKed

Send:
- Frame 1: Already ACKed → Skip
- Frame 2: Not ACKed → Resend!
- Frame 3: Already ACKed → Skip
- Frame 4: Already ACKed → Skip

Only Frame 2 is retransmitted! (Selective!)
```

**Step 4: Simulate Receiver (Buffering!)**
```cpp
// Simulate receiver side for all frames in flight
for (int i = base; i < nextSeqNum; i++) {
    if (acked[i]) continue; // Already ACKed, don't re-process

    if (shouldEventHappen(lossProb)) {
        cout << "  -> Frame " << i << " lost in transit!" << endl;
    } else {
        cout << "Receiver: Received Frame " << i << ". Buffering. Sending ACK " << i << "." << endl;
        acked[i] = true;
    }
}
```

**What's happening?**
- **Key difference**: Receiver **buffers** frames even if out-of-order!
- If frame received → **ACK it** → mark `acked[i] = true`
- **Buffering**: Receiver stores out-of-order frames in buffer

**Selective Repeat Behavior:**
```
Sender sends: Frame 1, 2, 3, 4
Frame 2 lost!

Receiver:
- Receives Frame 1 → Buffers, ACK 1 ✓
- Frame 2 lost!
- Receives Frame 3 → BUFFERS (not discarded!) ✓
- Receives Frame 4 → BUFFERS (not discarded!) ✓
- Sends ACK 1, 3, 4

Compare with Go-Back-N:
- Would discard Frame 3 and 4!
```

**Step 5: Check ACKs and Timeout**
```cpp
bool allAckedInWindow = true;
for (int i = base; i < nextSeqNum; i++) {
    if (!acked[i]) {
        cout << "Sender: Timeout! Did not receive ACK for Frame " << i << "." << endl;
        allAckedInWindow = false;
    }
}

if (allAckedInWindow && base == nextSeqNum - windowSize) {
     cout << "Sender: All frames in window ACKed." << endl;
}
```

**What's happening?**
- Check if all frames in window are ACKed
- If any frame not ACKed → timeout message
- **Individual ACK check**: Each frame checked separately

**Step 6: Slide Window Base**
```cpp
// Slide window base
int oldBase = base;
while (base <= totalFrames && acked[base]) {
    base++;
}

if (base > oldBase) {
    cout << "Sender: Sliding window base to " << base << ".\n" << endl;
} else {
    cout << "Sender: Window base remains at " << base << ". Retrying.\n" << endl;
}
```

**What's happening?**
- **Slide window**: Move `base` forward for all ACKed frames
- `while (acked[base])`: Keep moving `base` forward while frames are ACKed
- Window slides **independently** of which frames are received

**Window Sliding Example:**
```
Initial: base = 1, Window = [1, 2, 3, 4]
Frame 1 ACKed → base = 2, Window = [2, 3, 4, 5]
Frame 2 ACKed → base = 3, Window = [3, 4, 5, 6]
...

Window slides forward as frames are ACKed!
```

**Key Point:**
- Window slides **continuously** as frames are ACKed
- **Not blocked** waiting for all frames in window

---

## 🎯 KEY DIFFERENCES IN CODE

### Stop and Wait:
- ✅ Simple: One frame at a time
- ✅ Toggle sequence number: `seqNum = 1 - seqNum`
- ❌ Inefficient: Wait for each ACK

### Go-Back-N:
- ✅ Window mechanism: Send multiple frames
- ✅ `base` and `nextSeqNum` for window management
- ❌ Discards out-of-order frames
- ❌ Retransmits all from lost frame

### Selective Repeat:
- ✅ `acked[]` array: Track individual ACKs
- ✅ **Buffers** out-of-order frames
- ✅ **Selective retransmission**: Only lost frames
- ✅ **Independent window sliding**

---


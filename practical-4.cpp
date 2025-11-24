#include <bits/stdc++.h>
#include <unistd.h> 

using namespace std;

// Function to simulate random event (loss)
bool shouldEventHappen(int probabilityPercent) {
    return (rand() % 100) < probabilityPercent;
}

// 1. Stop and Wait ARQ
void performStopAndWait() {
    int totalFrames;
    cout << "Enter number of frames to send: ";
    cin >> totalFrames;
    
    int currentFrame = 1;
    int seqNum = 0;
    const int lossProb = 30; // 30% chance of loss

    while (currentFrame <= totalFrames) {
        cout << "Sender: Sending Frame " << currentFrame << " [Seq=" << seqNum << "]..." << endl;
        sleep(1);

        // Simulate frame loss
        if (shouldEventHappen(lossProb)) {
            cout << "  -> Frame " << currentFrame << " lost!" << endl;
            cout << "Sender: Timeout... Resending Frame " << currentFrame << ".\n" << endl;
            continue; // Resend the same frame
        }

        cout << "Receiver: Received Frame " << currentFrame << " [Seq=" << seqNum << "]." << endl;
        sleep(1);

        // Simulate ACK loss
        if (shouldEventHappen(lossProb)) {
            cout << "  -> ACK for Frame " << currentFrame << " lost!" << endl;
            cout << "Sender: Timeout... Resending Frame " << currentFrame << ".\n" << endl;
            continue; // Resend the same frame
        }

        cout << "Sender: Received ACK for Frame " << currentFrame << " [Seq=" << seqNum << "].\n" << endl;
        
        // Move to the next frame
        currentFrame++;
        seqNum = 1 - seqNum; // Toggle sequence number 0 -> 1 or 1 -> 0
    }

    cout << "All frames transmitted successfully!" << endl;
}

// 2. Go-Back-N ARQ
void performGoBackN() {
    int totalFrames, windowSize;
    cout << "Enter total number of frames: ";
    cin >> totalFrames;
    cout << "Enter window size: ";
    cin >> windowSize;

    int base = 1; // Base of the sender's window
    int nextSeqNum = 1; // Next frame to be sent
    const int lossProb = 25; // 25% chance of loss

    while (base <= totalFrames) {
        // Send all frames in the current window
        while (nextSeqNum < base + windowSize && nextSeqNum <= totalFrames) {
            cout << "Sender: Sending Frame " << nextSeqNum << "..." << endl;
            sleep(1);
            nextSeqNum++;
        }

        // Simulate transmission results
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
            sleep(1);
        }
        cout << endl;

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
    }

    cout << "All frames transmitted successfully!" << endl;
}

// 3. Selective Repeat ARQ
void performSelectiveRepeat() {
    int totalFrames, windowSize;
    cout << "Enter total number of frames: ";
    cin >> totalFrames;
    cout << "Enter window size: ";
    cin >> windowSize;

    vector<bool> acked(totalFrames + 1, false);
    int base = 1;
    int nextSeqNum = 1;
    const int lossProb = 30;

    while (base <= totalFrames) {
        cout << "--- Current Window [" << base << " to " << min(base + windowSize - 1, totalFrames) << "] ---" << endl;
        
        // Send all unacked frames in the window
        for (int i = nextSeqNum; i < base + windowSize && i <= totalFrames; i++) {
            if (i == nextSeqNum) { // Only advance nextSeqNum for new frames
                cout << "Sender: Sending new Frame " << i << "..." << endl;
                nextSeqNum++;
            } else if (!acked[i]) {
                cout << "Sender: Resending Frame " << i << "..." << endl;
            }
        }
        cout << endl;
        sleep(1);

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
        cout << endl;
        sleep(1);

        // Simulate ACKs received (or not) at sender
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
        sleep(1);
    }

    cout << "All frames transmitted successfully!" << endl;
}

// --- Main Menu ---

int main() {
    srand(time(0)); // Seed random number generator
    int choice;

    cout << "--- Sliding Window Protocols ---\n";
    cout << "Name: Shivam Hippalgave\n";
    cout << "PRN: 202301040046\n";
    cout << "--------------------------------\n";
    cout << "Select Protocol:\n";
    cout << "1. Stop and Wait ARQ\n";
    cout << "2. Go-Back-N ARQ\n";
    cout << "3. Selective Repeat ARQ\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            performStopAndWait();
            break;
        case 2:
            performGoBackN();
            break;
        case 3:
            performSelectiveRepeat();
            break;
        default:
            cout << "Invalid choice.\n";
            break;
    }

    return 0;
}
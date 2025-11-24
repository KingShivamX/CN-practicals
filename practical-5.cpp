#include <bits/stdc++.h>
using namespace std;

// Splits a.b.c.d into a vector [a, b, c, d]
vector<int> splitIp(const string& ipStr) {
    vector<int> parts;
    stringstream ss(ipStr);
    string segment;
    while (getline(ss, segment, '.')) {
        parts.push_back(stoi(segment));
    }
    return parts;
}

// Converts a vector [a, b, c, d] to string "a.b.c.d"
string ipToString(const vector<int>& ipParts) {
    return to_string(ipParts[0]) + "." + to_string(ipParts[1]) + "." +
           to_string(ipParts[2]) + "." + to_string(ipParts[3]);
}

// Converts a CIDR prefix (e.g., 24) to a mask vector [255, 255, 255, 0]
vector<int> cidrToMask(int prefix) {
    vector<int> maskParts(4);
    unsigned long mask = (prefix == 0) ? 0 : (0xFFFFFFFF << (32 - prefix));
    
    maskParts[0] = (mask >> 24) & 255;
    maskParts[1] = (mask >> 16) & 255;
    maskParts[2] = (mask >> 8) & 255;
    maskParts[3] = mask & 255;
    
    return maskParts;
}

int main() {
    string ipStr, maskStr;

    cout << "Enter IP address: ";
    cin >> ipStr;
    cout << "Enter Subnet Mask (e.g., 255.255.255.0 or /24): ";
    cin >> maskStr;
    
    vector<int> ipParts = splitIp(ipStr);
    vector<int> maskParts;
    int prefix;

    if (maskStr[0] == '/') {
        prefix = stoi(maskStr.substr(1));
        maskParts = cidrToMask(prefix);
    } else {
        maskParts = splitIp(maskStr);
        // Calculate prefix from mask
        prefix = 0;
        for (int part : maskParts) {
            prefix += bitset<8>(part).count();
        }
    }

    // Calculate Network and Broadcast Addresses
    vector<int> networkId(4);
    vector<int> broadcastId(4);
    for (int i = 0; i < 4; i++) {
        networkId[i] = ipParts[i] & maskParts[i];
        broadcastId[i] = networkId[i] | (~maskParts[i] & 255);
    }

    // Calculate Host Range
    vector<int> firstHost = networkId;
    vector<int> lastHost = broadcastId;
    firstHost[3]++; 
    lastHost[3]--;

    // Handle edge case of /31 and /32
    int hostBits = 32 - prefix;
    long long numHosts = (hostBits > 0) ? (static_cast<long long>(pow(2, hostBits)) - 2) : 0;
    if (prefix == 31) numHosts = 2; // Special case for point-to-point
    if (prefix == 32) numHosts = 1;

    // Display results
    cout << "\n--- Results ---\n";
    cout << "Network Address:   " << ipToString(networkId) << endl;
    cout << "Broadcast Address: " << ipToString(broadcastId) << endl;
    
    if (numHosts > 0) {
        cout << "First Usable IP:   " << ipToString(firstHost) << endl;
        cout << "Last Usable IP:    " << ipToString(lastHost) << endl;
    } else {
        cout << "First Usable IP:   N/A" << endl;
        cout << "Last Usable IP:    N/A" << endl;
    }
    
    cout << "Number of Hosts:   " << numHosts << endl;
    cout << "Subnet Mask:       " << ipToString(maskParts) << endl;
    cout << "CIDR Notation:     /" << prefix << endl;

    return 0;
}
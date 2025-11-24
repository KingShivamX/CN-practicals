## Practical 5: Subnet Calculator - Code Walkthrough

The file `practical-5.cpp` implements an IPv4 subnet calculator. Here’s the logic step by step.

---

### 🔧 Helper 1: `splitIp` – `"a.b.c.d"` → `[a, b, c, d]`

```11:18:practical-5.cpp
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
```

- Input: string like `"192.168.1.10"`.
- Uses `stringstream` to read until `'.'` and converts each segment to `int` via `stoi`.
- Output: `vector<int> {192, 168, 1, 10}`.

Use in program:
- For both IP address and dotted-decimal subnet mask.

---

### 🔧 Helper 2: `ipToString` – `[a, b, c, d]` → `"a.b.c.d"`

```21:25:practical-5.cpp
// Converts a vector [a, b, c, d] to string "a.b.c.d"
string ipToString(const vector<int>& ipParts) {
    return to_string(ipParts[0]) + "." + to_string(ipParts[1]) + "." +
           to_string(ipParts[2]) + "." + to_string(ipParts[3]);
}
```

- Input: `vector<int> {192,168,1,0}`.
- Output: `"192.168.1.0"`.
- Used for printing network, broadcast, first/last host, mask.

---

### 🔧 Helper 3: `cidrToMask` – `/prefix` → mask `[octets]`

```27:37:practical-5.cpp
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
```

Concept:
- Start from 32 bits of `1` (`0xFFFFFFFF`).
- Shift left by \(32 - \text{prefix}\) to leave `prefix` ones, then zeros.
- Extract each 8-bit chunk into 4 octets.

Example: prefix = 24
- `mask = 0xFFFFFFFF << (32 - 24) = 0xFFFFFFFF << 8`
  - Result: `11111111.11111111.11111111.00000000` → `255.255.255.0`

Then:
- `maskParts[0] = (mask >> 24) & 255;` → first 8 bits
- `maskParts[1] = (mask >> 16) & 255;`
- `maskParts[2] = (mask >> 8) & 255;`
- `maskParts[3] = mask & 255;`

Used when user inputs CIDR like `/24`.

---

### 🧭 `main` – Overall Flow

```40:52:practical-5.cpp
int main() {
    string ipStr, maskStr;

    cout << "--- IP Subnet Calculator ---\n";
    cout << "Name: Shivam Hippalgave\n";
    cout << "PRN: 202301040046\n";
    cout << "----------------------------\n";
    
    cout << "Enter IP address: ";
    cin >> ipStr;
    cout << "Enter Subnet Mask (e.g., 255.255.255.0 or /24): ";
    cin >> maskStr;
```

- Reads:
  - `ipStr`: IP address string.
  - `maskStr`: either dotted mask (`255.255.255.0`) or CIDR (`/24`).

---

### 🧩 Step 1: Parse IP and Determine Mask & Prefix

```53:67:practical-5.cpp
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
```

What this does:
- Always converts the IP to `ipParts` using `splitIp`.

If mask is in **CIDR**:
- `maskStr[0] == '/'`:
  - Extract prefix: `stoi(maskStr.substr(1))`.
  - Example: `"/24"` → `"24"` → `24`.
  - Get dotted mask using `cidrToMask(prefix)`.

If mask is in **dotted-decimal**:
- `maskStr` like `"255.255.255.0"`:
  - `maskParts = splitIp(maskStr)` → `[255, 255, 255, 0]`.
  - Compute prefix by counting `1` bits in each octet:
    - `bitset<8>(part).count()` = number of 1s.

Example:
- Mask: `255.255.254.0`
  - `255` → `11111111` → 8 ones
  - `255` → 8 ones
  - `254` → `11111110` → 7 ones
  - `0` → `00000000` → 0 ones
  - Prefix = `8 + 8 + 7 + 0 = 23` → `/23`.

So after this block, we always have:
- `ipParts` → `[a,b,c,d]`
- `maskParts` → `[m1,m2,m3,m4]`
- `prefix` → integer prefix length.

---

### 🌐 Step 2: Network & Broadcast Addresses

```69:75:practical-5.cpp
    // Calculate Network and Broadcast Addresses
    vector<int> networkId(4);
    vector<int> broadcastId(4);
    for (int i = 0; i < 4; i++) {
        networkId[i] = ipParts[i] & maskParts[i];
        broadcastId[i] = networkId[i] | (~maskParts[i] & 255);
    }
```

#### Network ID
- Computed per-octet:
  - `networkId[i] = ipParts[i] & maskParts[i];`
- This is **bitwise AND** between IP and mask.

Example:
- IP: `192.168.1.10` → `[192,168,1,10]`
- Mask: `255.255.255.0` → `[255,255,255,0]`
- For each octet:
  - `192 & 255 = 192`
  - `168 & 255 = 168`
  - `1 & 255 = 1`
  - `10 & 0 = 0`
- Network ID: `[192,168,1,0]` → `"192.168.1.0"`.

#### Broadcast ID
- `broadcastId[i] = networkId[i] | (~maskParts[i] & 255);`
- `~maskParts[i]` flips bits of the mask octet.
- `& 255` keeps just 8 bits.
- Then bitwise OR with network ID sets all host bits to 1.

Example for `/24`:
- Mask last octet: `0` → `00000000`
- `~0 & 255` → `11111111` (255)
- Broadcast last octet = `networkLastOctet | 255` = `0 | 255` = `255`.
- Broadcast = `192.168.1.255`.

This matches theory:
- Network: host bits all 0.
- Broadcast: host bits all 1.

---

### 👥 Step 3: First & Last Usable Host

```77:81:practical-5.cpp
    // Calculate Host Range
    vector<int> firstHost = networkId;
    vector<int> lastHost = broadcastId;
    firstHost[3]++; 
    lastHost[3]--;
```

Logic:
- Start from network and broadcast addresses.
- Increase last octet of network (`+1`) → first host.
- Decrease last octet of broadcast (`-1`) → last host.

Example `/24`:
- Network: `192.168.1.0`
  - First host: `192.168.1.1`
- Broadcast: `192.168.1.255`
  - Last host: `192.168.1.254`

Note:
- This assumes host bits lie within the last octet (true for `/8, /16, /24` etc.).
- For general prefixes, concept is still: first host = network + 1; last host = broadcast - 1 (in full 32-bit integer).

---

### 🔢 Step 4: Number of Hosts & Edge Cases

```83:87:practical-5.cpp
    // Handle edge case of /31 and /32
    int hostBits = 32 - prefix;
    long long numHosts = (hostBits > 0) ? (static_cast<long long>(pow(2, hostBits)) - 2) : 0;
    if (prefix == 31) numHosts = 2; // Special case for point-to-point
    if (prefix == 32) numHosts = 1;
```

Standard formula:
- Host bits = \(32 - \text{prefix}\).
- Total addresses = \(2^{\text{hostBits}}\).
- Usable hosts (usual subnets) = \(2^{\text{hostBits}} - 2\).
  - Subtract 2 for network and broadcast.

Example:
- `/24` → hostBits = 8 → \(2^8 = 256\) total → `256 - 2 = 254` usable.
- `/30` → hostBits = 2 → \(2^2 = 4\) total → `4 - 2 = 2` usable.

Special cases:
- `/31`:
  - hostBits = 1 → \(2^1 = 2\) total.
  - In classical subnetting, 0 usable; but for **point-to-point** links both addresses are used, so 2 hosts.
  - Code sets `numHosts = 2`.
- `/32`:
  - hostBits = 0 → a single IP.
  - Represents one host (no “network/broadcast” distinction).
  - Code sets `numHosts = 1`.

If `hostBits <= 0`:
- Sets `numHosts = 0` (then corrected for `/31` and `/32` above).

---

### 📤 Step 5: Output Results

```89:106:practical-5.cpp
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
```

Printed values:
- **Network Address**: result of IP & mask.
- **Broadcast Address**: result of network | (~mask & 255).
- **First/Last Usable IP**:
  - Only shown when `numHosts > 0`.
  - For `/31` and `/32`, code shows `N/A` for first/last usable (but still prints `numHosts`).
- **Number of Hosts**: usable hosts count.
- **Subnet Mask**: always printed in dotted-decimal.
- **CIDR Notation**: printed as `/prefix`.

This matches standard subnet calculator outputs and is aligned with the theory from the study guide.

---

### 🔁 Summary of Data Flow

1. **Input**:
   - `ipStr`, `maskStr`
2. **Parse**:
   - `ipParts = splitIp(ipStr)`
   - If `/prefix`: `prefix` from string, `maskParts = cidrToMask(prefix)`
   - Else: `maskParts = splitIp(maskStr)`, `prefix` from counting 1 bits
3. **Compute**:
   - `networkId[i] = ipParts[i] & maskParts[i]`
   - `broadcastId[i] = networkId[i] | (~maskParts[i] & 255)`
   - `firstHost` & `lastHost` from network/broadcast
   - `numHosts` from `prefix` (with `/31` & `/32` specials)
4. **Output**:
   - Network, broadcast, usable range, host count, mask, prefix.

If you can trace a single example through these steps, you fully understand both the code and the subnetting logic.



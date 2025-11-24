## Practical 5: IP Subnetting & Calculator - Study Guide

### 📚 1. Basics of IP Addressing (IPv4)

-   **IPv4 address**: 32 bits, usually written as four decimal octets: `a.b.c.d`
    -   Example: `192.168.1.10`
    -   Each octet: 0–255 (8 bits)
-   Internally, the address is a 32-bit binary number, e.g.\
    `192.168.1.10` → `11000000.10101000.00000001.00001010`

-   **Two parts in an IP address**:
    -   **Network part**: identifies the network
    -   **Host part**: identifies a host within that network

The **subnet mask** or **CIDR prefix** tells us how many bits are network vs host.

---

### 🧮 2. Subnet Mask

-   A **subnet mask** is also a 32-bit value, written like an IP:
    -   Example: `255.255.255.0`
-   It has:
    -   **1s** in all **network bits**
    -   **0s** in all **host bits**

Example:

-   Mask `255.255.255.0` in binary:
    -   `11111111.11111111.11111111.00000000`
    -   Network bits: 24 (first 24 ones)
    -   Host bits: 8 (last 8 zeros)

Relationship:

-   **More 1s in mask → larger prefix → fewer hosts per subnet.**

---

### 🔢 3. CIDR Notation

-   **CIDR** = Classless Inter-Domain Routing
-   CIDR notation: `IP/prefix`
    -   Example: `192.168.1.10/24`
-   **Prefix** = number of bits used for the **network**.
    -   `/24` → first 24 bits are network, last 8 bits are host.

Examples:

-   `/24` → mask `255.255.255.0`
-   `/16` → mask `255.255.0.0`
-   `/30` → mask `255.255.255.252`

Conversion:

-   Given prefix `/p`, subnet mask is:
    -   First `p` bits = 1
    -   Remaining `32 - p` bits = 0

The code’s `cidrToMask(int prefix)` does exactly this conversion.

---

### 🌐 4. Network Address

-   **Network Address** = “ID of the subnet”
-   All **host bits = 0**.
-   Computed as:
    -   \(\text{network} = \text{IP} \ \text{bitwise-AND} \ \text{mask}\)

Example:

-   IP: `192.168.1.10` → `11000000.10101000.00000001.00001010`
-   Mask: `255.255.255.0` → `11111111.11111111.11111111.00000000`

Bitwise AND:

```text
IP:    11000000.10101000.00000001.00001010
MASK:  11111111.11111111.11111111.00000000
------------------------------------------
NET:   11000000.10101000.00000001.00000000
     = 192.168.1.0
```

So **network address** = `192.168.1.0`.

In the code:

-   `networkId[i] = ipParts[i] & maskParts[i];`

---

### 📣 5. Broadcast Address

-   **Broadcast Address** = “last” address of the subnet.
-   All **host bits = 1**.
-   It is used to send to **all hosts** in that subnet.

How to compute:

-   Option 1: Take network address, set all host bits to 1.
-   Option 2 (like in code):
    -   \(\text{broadcast} = \text{network} \ | \ (\sim\,\text{mask} \ \&\ 255)\) (per octet)
    -   `~mask` flips bits: 1→0, 0→1
    -   `~mask & 255` keeps only the last 8 bits for each octet.

Example with `/24`:

-   Network: `192.168.1.0` → `11000000.10101000.00000001.00000000`
-   Mask: `255.255.255.0` → `11111111.11111111.11111111.00000000`
-   `~mask` last octet: `00000000` → `11111111` (255)
-   So broadcast: last octet becomes 255:
    -   `192.168.1.255`

In the code:

-   `broadcastId[i] = networkId[i] | (~maskParts[i] & 255);`

---

### 👥 6. Host Range (First & Last Usable IP)

-   **First usable host**:
    -   Network address + 1 in the host part.
    -   Example: `192.168.1.0` → `192.168.1.1`
-   **Last usable host**:
    -   Broadcast address - 1 in the host part.
    -   Example: `192.168.1.255` → `192.168.1.254`

So for `192.168.1.10/24`:

-   Network: `192.168.1.0`
-   Broadcast: `192.168.1.255`
-   First host: `192.168.1.1`
-   Last host: `192.168.1.254`

In the code:

-   `firstHost = networkId; firstHost[3]++;`
-   `lastHost = broadcastId; lastHost[3]--;`

Note: This assumes **host bits are only in the last octet**, which is true for common prefixes like `/8, /16, /24`, but conceptually for other prefixes you adjust in binary.

---

### 🔢 7. Number of Hosts in a Subnet

For a prefix `/p`:

-   **Host bits** = \(32 - p\)
-   **Total IPs in subnet** = \(2^{32-p}\)
-   **Usable hosts** (normal subnets) = \(2^{32-p} - 2\)
    -   1 address is **network**
    -   1 address is **broadcast**

Examples:

-   `/24`: host bits = 8 → \(2^8 = 256\) addresses\
    Usable = \(256 - 2 = 254\)
-   `/30`: host bits = 2 → \(2^2 = 4\) addresses\
    Usable = \(4 - 2 = 2\)

In the code:

-   `hostBits = 32 - prefix;`
-   `numHosts = pow(2, hostBits) - 2;`

#### Special Cases: /31 and /32

-   `/31`:
    -   Host bits = 1 → \(2^1 = 2\) addresses
    -   Normally 0 usable (network + broadcast), **but** for point-to-point links both are used.
    -   Code sets `numHosts = 2`.
-   `/32`:
    -   Host bits = 0 → single IP
    -   Used to represent a **single host**.
    -   Code sets `numHosts = 1`.

---

### 🧠 8. Converting Mask ↔ Prefix

**Mask → Prefix** (done in code when user enters `255.255.255.0` style):

-   Count number of 1 bits in all 4 octets.
-   Example: `255.255.254.0`
    -   `255` → `11111111` → 8 ones
    -   `255` → `11111111` → 8 ones
    -   `254` → `11111110` → 7 ones
    -   `0` → `00000000` → 0 ones
    -   Prefix = \(8 + 8 + 7 + 0 = 23\) → `/23`

In the code:

-   `bitset<8>(part).count()` counts 1 bits in each octet.

**Prefix → Mask** (when user enters `/24`):

-   Place `prefix` ones, then zeros to make 32 bits.
-   Split into 4 octets:
    -   `/24` → `11111111.11111111.11111111.00000000` → `255.255.255.0`
    -   `/20` → `11111111.11111111.11110000.00000000` → `255.255.240.0`

In the code:

-   Uses a 32-bit integer `mask = 0xFFFFFFFF << (32 - prefix);`
-   Then extracts 4 octets by shifting and masking with 255.

---

### 🧾 9. What Practical 5 Program Does (High Level)

Given:

-   An **IP address** (e.g., `192.168.1.10`)
-   A **subnet mask** (`255.255.255.0`) **or** a **CIDR prefix** (`/24`)

The program calculates:

-   **Network Address**
-   **Broadcast Address**
-   **First Usable Host IP**
-   **Last Usable Host IP**
-   **Number of Usable Hosts**
-   **Subnet Mask** (in dotted-decimal)
-   **CIDR Notation** (`/prefix`)

This matches the standard subnetting procedure you’d do by hand in networking exams.

---

### ✅ 10. Quick Cheat Sheet

-   **Network** = IP AND Mask
-   **Broadcast** = Network OR (NOT Mask)
-   **First Host** = Network + 1 (host part)
-   **Last Host** = Broadcast – 1 (host part)
-   **Number of Hosts**:
    -   Normal: \(2^{32-p} - 2\)
    -   `/31`: 2 (point-to-point)
    -   `/32`: 1 (single host)
-   **Prefix from Mask**: Count 1s in all 4 octets

If you understand this sheet, you understand both the **theory** and what your **code** is doing.

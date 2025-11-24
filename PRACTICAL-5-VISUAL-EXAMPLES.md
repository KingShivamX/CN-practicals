## Practical 5: Visual Subnetting Examples & Quick Reference

---

### 🌱 Example 1: Classic /24 Subnet

**Given:**
- IP: `192.168.1.10`
- Mask: `/24` (or `255.255.255.0`)

**Step 1: Write in binary (conceptually)**
- IP:\
  `192.168.1.10` → `11000000.10101000.00000001.00001010`
- Mask `/24`:\
  `255.255.255.0` → `11111111.11111111.11111111.00000000`

**Step 2: Network Address = IP AND Mask**
- Only last octet changes (because mask last octet = 0):
- Last octet: `00001010 AND 00000000 = 00000000`

Result:
- **Network** = `192.168.1.0`

**Step 3: Broadcast Address = Network OR (~Mask)**
- Last octet of mask: `00000000` → `~` → `11111111` (255)
- Last octet: `00000000 OR 11111111 = 11111111`w

Result:
- **Broadcast** = `192.168.1.255`

**Step 4: First & Last Usable Host**
- First host: `192.168.1.0 + 1` → `192.168.1.1`
- Last host: `192.168.1.255 - 1` → `192.168.1.254`

**Step 5: Number of Hosts**
- Prefix `/24` → host bits = \(32 - 24 = 8\)
- Total addresses = \(2^8 = 256\)
- Usable hosts = \(256 - 2 = 254\)

**Final:**
- Network: `192.168.1.0`
- Broadcast: `192.168.1.255`
- First host: `192.168.1.1`
- Last host: `192.168.1.254`
- Hosts: `254`

Matches what the program prints for this input.

---

### 🌱 Example 2: /26 Subnet (Smaller Subnet)

**Given:**
- IP: `10.0.0.70`
- Mask: `/26`

**Step 1: Convert /26 to mask**
- `/26` → 26 network bits, 6 host bits.
- Binary mask:
  - `11111111.00000000.00000000.00000000` → /8
  - `/26` specifically: `255.255.255.192`
    - Last octet: `11000000` = `192`

So:
- Mask: `255.255.255.192`

**Step 2: Network Address**
- Focus on last octet:
  - IP last octet: `70`
  - In binary: `01000110`
  - Mask last octet: `192` → `11000000`

AND:
```text
  01000110
& 11000000
  --------
  01000000  = 64
```

So:
- **Network** = `10.0.0.64`

**Step 3: Broadcast Address**
- Last octet:
  - Inverted mask: `~11000000` → `00111111` (63)
  - Broadcast last octet = `64 OR 63`:

```text
  01000000  (64)
| 00111111  (63)
  --------
  01111111  = 127
```

So:
- **Broadcast** = `10.0.0.127`

**Step 4: First & Last Host**
- First host: `10.0.0.64 + 1` → `10.0.0.65`
- Last host: `10.0.0.127 - 1` → `10.0.0.126`

**Step 5: Number of Hosts**
- `/26` → host bits = \(32 - 26 = 6\)
- Total addresses = \(2^6 = 64\)
- Usable = `64 - 2 = 62`

**Final:**
- Network: `10.0.0.64`
- Broadcast: `10.0.0.127`
- First host: `10.0.0.65`
- Last host: `10.0.0.126`
- Hosts: `62`

---

### 🌱 Example 3: Non-Standard Mask 255.255.254.0 ( /23 )

**Given:**
- IP: `172.16.5.100`
- Mask: `255.255.254.0`

**Step 1: Find prefix from mask**
- `255` → `11111111` → 8 ones
- `255` → 8 ones
- `254` → `11111110` → 7 ones
- `0`   → `00000000` → 0 ones
- Prefix = \(8 + 8 + 7 + 0 = 23\) → `/23`

**Step 2: Network Address**
- Third octet range for `/23`:
  - `/23` means network increments by 2 in the third octet:
    - `...0.0` – `...1.255`
    - `...2.0` – `...3.255`
    - `...4.0` – `...5.255`
    - `...6.0` – `...7.255`
  - Our IP is `172.16.5.100` → third octet = `5` → falls in `4–5` range
- So:
  - **Network** = `172.16.4.0`

(You can also compute via AND in binary; this is the shortcut logic.)

**Step 3: Broadcast Address**
- For `/23`, each block spans 2 in third octet, so:
  - Block: `4.0` to `5.255`
  - So **Broadcast** = `172.16.5.255`

**Step 4: First & Last Host**
- First host: `172.16.4.0 + 1` → `172.16.4.1`
- Last host: `172.16.5.255 - 1` → `172.16.5.254`

**Step 5: Number of Hosts**
- `/23` → host bits = \(32 - 23 = 9\)
- Total = \(2^9 = 512\)
- Usable = `512 - 2 = 510`

**Final:**
- Network: `172.16.4.0`
- Broadcast: `172.16.5.255`
- First host: `172.16.4.1`
- Last host: `172.16.5.254`
- Hosts: `510`

---

### 🌱 Example 4: /30 – Point-to-Point Style Subnet

**Given:**
- IP: `192.168.10.14`
- Mask: `/30` → `255.255.255.252`

**Step 1: Host bits**
- `/30` → host bits = \(32 - 30 = 2\)
- Total addresses = \(2^2 = 4\)

Each /30 block of last octet:
- Ranges of size 4:\
  `...0–3`, `4–7`, `8–11`, `12–15`, `16–19`, ...
- Our IP last octet = 14 → belongs to block `12–15`.

So:
- Network: `192.168.10.12`
- Broadcast: `192.168.10.15`

**Host range:**
- First host: `192.168.10.13`
- Last host: `192.168.10.14`

**Usable hosts:**
- `4 - 2 = 2`

Program output for `192.168.10.14 /30`:
- Network Address:   `192.168.10.12`
- Broadcast Address: `192.168.10.15`
- First Usable IP:   `192.168.10.13`
- Last Usable IP:    `192.168.10.14`
- Number of Hosts:   `2`

---

### 🌱 Example 5: /31 – Special Case

**Given:**
- IP: `10.1.1.4`
- Mask: `/31`

**Theory:**
- `/31` → host bits = 1 → total addresses = 2
- Normally:
  - 1 network + 1 broadcast → 0 usable hosts
- But in modern networking:
  - Used for **point-to-point** links (router–router)
  - Both IPs are usable (no broadcast needed)

Block sizes:
- `/31` → range size 2:
  - `...0–1`, `2–3`, `4–5`, `6–7`, ...
- `10.1.1.4` is in block `4–5`.

So:
- Network (theoretical): `10.1.1.4`
- Broadcast (theoretical): `10.1.1.5`
- Usable addresses (practically): **2**

In the code:
- `hostBits = 1 → pow(2,1) - 2 = 0` initially
- But special case:
  - `if (prefix == 31) numHosts = 2;`
- So it reports `Number of Hosts: 2` and `First/Last Usable IP: N/A` (because host range logic is based on normal subnets).

---

### 🌱 Example 6: /32 – Single Host

**Given:**
- IP: `203.0.113.25`
- Mask: `/32`

**Theory:**
- `/32` → host bits = 0 → only **one IP** in the subnet.
- Used to represent a **single host** (no network/broadcast distinction).

In the code:
- `hostBits = 0 → numHosts = 0` initially (via formula)
- Special case:
  - `if (prefix == 32) numHosts = 1;`

Likely output:
- Network Address:   `203.0.113.25`
- Broadcast Address: `203.0.113.25`
- First Usable IP:   `N/A`
- Last Usable IP:    `N/A`
- Number of Hosts:   `1`

---

### 📊 Quick Table: Common Prefixes

| Prefix | Mask              | Host Bits | Total IPs | Usable Hosts |
|--------|-------------------|-----------|-----------|--------------|
| /24    | 255.255.255.0     | 8         | 256       | 254          |
| /25    | 255.255.255.128   | 7         | 128       | 126          |
| /26    | 255.255.255.192   | 6         | 64        | 62           |
| /27    | 255.255.255.224   | 5         | 32        | 30           |
| /28    | 255.255.255.240   | 4         | 16        | 14           |
| /29    | 255.255.255.248   | 3         | 8         | 6            |
| /30    | 255.255.255.252   | 2         | 4         | 2            |
| /31    | 255.255.255.254   | 1         | 2         | 2 (special)  |
| /32    | 255.255255.255    | 0         | 1         | 1 (host)     |

---

### 📝 How This Matches the Code

For each test case, the program:
- Takes IP + mask or CIDR.
- Normalizes to:
  - `ipParts`: `[a,b,c,d]`
  - `maskParts`: `[m1,m2,m3,m4]`
  - `prefix`: `/p`
- Then:
  - `networkId[i] = ipParts[i] & maskParts[i]`
  - `broadcastId[i] = networkId[i] | (~maskParts[i] & 255)`
  - `firstHost[3]++`, `lastHost[3]--` (common cases)
  - `numHosts = 2^(32-p) - 2`, fixed for `/31` and `/32`

You can plug any IP/prefix from your exam into this pattern and predict the program’s output.

---



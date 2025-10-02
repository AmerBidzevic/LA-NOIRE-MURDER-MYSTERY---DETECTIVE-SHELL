# LA NOIRE MURDER MYSTERY - DETECTIVE SHELL  
**Case File: The Killing of Johnny "Rats" Malone**  

A text-based detective game written in **C**, blending interactive storytelling with operating system concepts.  
You play as a detective solving a murder by gathering evidence, questioning suspects, and navigating different locations through shell-like commands.  

---

## Features  
- **Investigation Gameplay**: Collect evidence, question suspects, and travel between locations.  
- **Custom Shell Commands**: Extended with utilities such as:  
  - `mv`, `du`, `date`, `rev`, `ls`, `ld`, `cat`  
  - A safe `fork()` implementation with process limits  
  - A (dangerous but fun) `forkbomb` command  
- **Dynamic Evidence System**: Generates in-game files (`ledger.txt`, `forensics.txt`, `tox_report.txt`, etc.)  
- **Color-coded Terminal Interface** for immersion  
- **Robust Input Handling** with error checking  

---

## Quick Start  

### Compile  
```bash
gcc -o OS-Noire-Shell OS-Noire-Shell.c
```  

### Run  
```bash
./OS-Noire-Shell
```  

---
## Troubleshooting
If evidence files don't appear:
    mkdir -p case_files
    chmod 755 case_files
Then re-launch the game.

---

## Game Commands  

- `investigate` – Search current location for clues  
- `interview [name]` – Question suspects (`Victoria`, `Tony`, `Louie`, `Mickey`, `Eleanor`, `Sal`)  
- `examine [file]` – View evidence (`ledger`, `ballistics`, `witness`, `forensics`, `hotel_key`, `tox_report`)  
- `move [location]` – Travel to new area (`Police Station`, `Crime Scene`, `Velvet Nightclub`, `Docks`, `Roosevelt Hotel`, `City Morgue`)  
- `whereis [name]` – Find suspect’s location  
- `status` – Check investigation progress  
- `help` – List all commands  
- `exit` – Quit investigation  
- mv [source] [destination] - Move a file from source to destination
- du [file]             - Display the size of a file
- date                  - Display the current system date and time
- rev [file]            - Reverse the contents of a file
- forkbomb              - Create a fork bomb (use cautiously)
- ls [directory]        - List contents of a directory
- ld [file]             - List the dynamic libraries linked to a file
- cat [file]            - Display the contents of a file
  
---

## Evidence Files (Auto-Generated)  

- **ledger.txt** – Victim owed $5000 to "Vixen"  
- **ballistics.txt** – Gun was registered to Victoria  
- **witness.txt** – Places Victoria at crime scene  
- **forensics.txt** – Lipstick match from Victoria  
- **hotel_key.txt** – Links victim to Roosevelt Hotel  
- **tox_report.txt** – Shows victim was drugged  

---

## Technical Details  

- Written in **C**  
- Demonstrates **process management with fork()**  
- File I/O with `fopen()` and dynamic evidence creation  
- Basic **location-based NPC tracking**  
- Built-in **command parsing system**  

---

## Known Issues / Limitations  

- Commands are case-sensitive  
- No game state persistence (cannot save/load progress)  
- `case_files` directory must exist with correct permissions:  
  ```bash
  mkdir -p case_files && chmod 755 case_files
  ```  
- Some evidence generation may fail silently if directory setup is incorrect  

---

## Future Improvements  

- Save/load game system  
- Branching storylines or randomized evidence placement  
- Expanded set of commands and suspect interactions  

---

## Credits  

- Developed by Amer Bidžević and Emel Čoloman 
- Uses system calls & file handling concepts from Unix/Linux environments  
- References:  
  - Linux man pages (`fork`, `fopen`, `fgets`, `strcmp`)  
  - GeeksforGeeks (C file I/O basics)  
  - Stack Overflow discussions on process limits & string parsing  
  - ANSI color codes for terminal output  

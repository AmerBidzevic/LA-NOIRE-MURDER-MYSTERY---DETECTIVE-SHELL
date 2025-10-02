#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

typedef struct {
    int progress;
    char locations[6][32];
    char suspect[6][32];
    char suspect_locations[6][32];
    char weapon[32];
    char motive[128];
    int correct_suspect;
    int evidence_found;
    int interviews_done[6];
} GameState;

GameState game;

void init_game();
void print_prompt();
void parse_input(char *input, char **args);
void execute_command(char **args);
void print_welcome_message();
void handle_investigate();
void handle_interview(char *suspect);
void handle_accuse(char *suspect);
void print_ending(int correct);
void create_case_files();
void handle_examine(char *item);
void handle_move(char *location);
void print_case_status();
void handle_whereis(char *suspect);
void create_case_file(const char *filename, const char *content);
void handle_redirection(char **args);
void execute_with_pipe(char **args);
void demonstrate_exec();
void demonstrate_waitpid();
void demonstrate_execvp();
void controlled_forkbomb();

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    init_game();
    create_case_files();
    print_welcome_message();

    while (1) {
        print_prompt();
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) break;
        parse_input(input, args);
        
        int i;
        for (i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "|") == 0) {
                execute_with_pipe(args);
                break;
            }
        }
        if (args[i] == NULL) {
            execute_command(args);
        }
    }

    return 0;
}

void create_case_file(const char *filename, const char *content) {
    char path[256];
    snprintf(path, sizeof(path), "case_files/%s", filename);
    
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        printf("Error creating %s: %s\n", filename, strerror(errno));
        return;
    }
    fprintf(fp, "%s", content);
    fclose(fp);
}

void init_game() {
    game.progress = 0;
    game.evidence_found = 0;
    
    strcpy(game.locations[0], "Police Station");
    strcpy(game.locations[1], "Crime Scene");
    strcpy(game.locations[2], "Velvet Nightclub");
    strcpy(game.locations[3], "Docks");
    strcpy(game.locations[4], "Roosevelt Hotel");
    strcpy(game.locations[5], "City Morgue");
    
    strcpy(game.suspect[0], "Tony 'Fingers' Moretti");
    strcpy(game.suspect[1], "Victoria 'Vixen' LaRue");
    strcpy(game.suspect[2], "Big Louie Scaletta");
    strcpy(game.suspect[3], "Mickey O'Shea");
    strcpy(game.suspect[4], "Dr. Eleanor Whitmore");
    strcpy(game.suspect[5], "Sal 'The Tailor' Russo");
    
    strcpy(game.suspect_locations[0], "Velvet Nightclub");
    strcpy(game.suspect_locations[1], "Velvet Nightclub");
    strcpy(game.suspect_locations[2], "Docks");
    strcpy(game.suspect_locations[3], "Police Station");
    strcpy(game.suspect_locations[4], "City Morgue");
    strcpy(game.suspect_locations[5], "Roosevelt Hotel");
    
    strcpy(game.weapon, "a .38 snubnose");
    strcpy(game.motive, "The victim knew about Victoria's drug operation and was blackmailing her");
    game.correct_suspect = 1;
    
    for (int i = 0; i < 6; i++) {
        game.interviews_done[i] = 0;
    }
}

void create_case_files() {
    system("mkdir -p case_files");
}

void print_prompt() {
    printf("\033[1;31m[%s] detective@LA-Noire:~$\033[0m ", game.locations[game.progress % 6]);
    fflush(stdout);
}

void parse_input(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \t\n");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        token = strtok(NULL, " \t\n");
        i++;
    }
    args[i] = NULL;
}

void execute_command(char **args) {
    if (!args[0]) return;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 && args[i+1] != NULL) {
            handle_redirection(args);
            return;
        }
    }

    if (strcmp(args[0], "investigate") == 0) {
        handle_investigate();
    } 
    else if (strcmp(args[0], "interview") == 0 && args[1]) {
        handle_interview(args[1]);
    }
    else if (strcmp(args[0], "accuse") == 0 && args[1]) {
        handle_accuse(args[1]);
    }
    else if (strcmp(args[0], "examine") == 0 && args[1]) {
        handle_examine(args[1]);
    }
    else if (strcmp(args[0], "move") == 0 && args[1]) {
        handle_move(args[1]);
    }
    else if (strcmp(args[0], "whereis") == 0 && args[1]) {
        handle_whereis(args[1]);
    }
    else if (strcmp(args[0], "status") == 0) {
        print_case_status();
    }
    else if (strcmp(args[0], "help") == 0) {
        printf("\n\033[1;33mCOMMANDS:\033[0m\n");
        printf("  investigate - Search for clues\n");
        printf("  interview [name] - Question suspects\n");
        printf("  examine [file] - Review evidence\n");
        printf("  move [location] - Go to another location\n");
        printf("  whereis [suspect] - Find a suspect's location\n");
        printf("  accuse [name] - Make your final charge\n");
        printf("  status - Show case progress\n");
        printf("  forkbomb - Controlled fork bomb demonstration (10 max)\n");
        printf("  mv/du/date/rev - Standard Unix commands\n");
        printf("  ls/cd/cat - Examine files\n");
        printf("  help - Show this message\n");
        printf("  exit - Quit the game\n");
        printf("\n\033[1;33mREDIRECTION:\033[0m\n");
        printf("  command > file - Redirect output to file\n");
        printf("  command1 | command2 - Pipe output\n\n");
    }
    else if (strcmp(args[0], "ls") == 0) {
        system("ls case_files");
    }
    else if (strcmp(args[0], "cd") == 0 && args[1]) {
        if (chdir(args[1])) {
            perror("cd failed");
        }
    }
    else if (strcmp(args[0], "cat") == 0 && args[1]) {
        char cmd[MAX_INPUT_SIZE];
        snprintf(cmd, sizeof(cmd), "cat case_files/%s", args[1]);
        system(cmd);
    }
    else if (strcmp(args[0], "forkbomb") == 0) {
        controlled_forkbomb();
    }
    else if (strcmp(args[0], "mv") == 0 && args[1] && args[2]) {
        if (rename(args[1], args[2]) == 0) {
            printf("Moved %s to %s\n", args[1], args[2]);
        } else {
            perror("mv failed");
        }
    }
    else if (strcmp(args[0], "du") == 0) {
        system("du -sh case_files");
    }
    else if (strcmp(args[0], "date") == 0) {
        system("date");
    }
    else if (strcmp(args[0], "rev") == 0 && args[1]) {
        char cmd[MAX_INPUT_SIZE];
        snprintf(cmd, sizeof(cmd), "rev case_files/%s", args[1]);
        system(cmd);
    }
    else if (strcmp(args[0], "execdemo") == 0) {
        demonstrate_exec();
    }
    else if (strcmp(args[0], "waitdemo") == 0) {
        demonstrate_waitpid();
    }
    else if (strcmp(args[0], "execvpdemo") == 0) {
        demonstrate_execvp();
    }
    else if (strcmp(args[0], "exit") == 0) {
        printf("\nCase abandoned. The streets remain unsafe...\n");
        exit(0);
    }
    else {
        printf("Command not recognized. Type 'help' for options.\n");
    }
}

void handle_redirection(char **args) {
    int i;
    for (i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            break;
        }
    }
    
    if (args[i] == NULL || args[i+1] == NULL) {
        printf("Invalid redirection syntax\n");
        return;
    }
    
    args[i] = NULL;
    int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open failed");
        return;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execute_command(args);
        exit(0);
    } else if (pid > 0) {
        close(fd);
        wait(NULL);
    } else {
        perror("fork failed");
    }
}

void execute_with_pipe(char **args) {
    int i;
    for (i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            break;
        }
    }
    
    if (args[i] == NULL || args[i+1] == NULL) {
        printf("Invalid pipe syntax\n");
        return;
    }
    
    char *cmd1[MAX_ARGS];
    char *cmd2[MAX_ARGS];
    
    memcpy(cmd1, args, i * sizeof(char*));
    cmd1[i] = NULL;
    
    memcpy(cmd2, &args[i+1], (MAX_ARGS - i - 1) * sizeof(char*));
    
    int pipefd[2];
    if (pipe(pipefd)) {
        perror("pipe failed");
        return;
    }
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execute_command(cmd1);
        exit(0);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execute_command(cmd2);
        exit(0);
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void demonstrate_exec() {
    printf("\nDemonstrating exec() system call:\n");
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process will now execute 'ls case_files'\n");
        execlp("ls", "ls", "case_files", NULL);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
        printf("Parent process continuing\n\n");
    } else {
        perror("fork failed");
    }
}

void demonstrate_waitpid() {
    printf("\nDemonstrating waitpid() system call:\n");
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process working...\n");
        sleep(2);
        printf("Child process done\n");
        exit(42);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child exited with status: %d\n\n", WEXITSTATUS(status));
        }
    } else {
        perror("fork failed");
    }
}

void demonstrate_execvp() {
    printf("\nDemonstrating execvp() system call:\n");
    char *argv[] = {"ls", "-l", "case_files", NULL};
    
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child will now execute 'ls -l case_files'\n");
        execvp("ls", argv);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
        printf("Parent process continuing\n\n");
    } else {
        perror("fork failed");
    }
}

void controlled_forkbomb() {
    printf("\n\033[1;31mCONTROLLED FORKBOMB DEMONSTRATION\033[0m\n");
    printf("This will create 10 processes then stop automatically\n");
    printf("All processes will terminate after 2 seconds\n");
    
    for (int i = 0; i < 10; i++) {
        if (fork() == 0) {
            printf("Fork bomb child %d created (PID: %d)\n", i+1, getpid());
            sleep(2);
            exit(0);
        }
    }
    
    sleep(3);
    printf("\nFork bomb test complete. System stable.\n\n");
}

void handle_investigate() {
    char *clues[] = {
        "You find %s under the victim's body",
        "The victim's %s shows suspicious entries",
        "A %s contains damning evidence",
        "The %s tells a revealing story",
        "You discover %s in the victim's coat pocket",
        "The %s reveals critical details"
    };

    char *discoveries[] = {
        game.weapon,
        "ledger (type 'examine ledger')",
        "ballistics report (type 'examine ballistics')",
        "witness statement (type 'examine witness')",
        "hotel key (type 'examine hotel_key')",
        "toxicology report (type 'examine tox_report')"
    };

    switch(game.progress) {
        case 0:
            create_case_file("ledger.txt", "Last entry: Owes $5000 to Vixen");
            break;
        case 1:
            create_case_file("ballistics.txt", "Fingerprint match: V. LaRue");
            break;
        case 2:
            create_case_file("witness.txt", "10pm: Heard arguing near dumpsters\n11:30pm: Gunshot heard");
            break;
        case 3:
            create_case_file("forensics.txt", "Victim had traces of lipstick on collar (shade matches Victoria)");
            break;
        case 4:
            create_case_file("hotel_key.txt", "Hotel Key #47 found in victim's pocket (Roosevelt Hotel)");
            break;
        case 5:
            create_case_file("tox_report.txt", "Toxicology: High levels of barbiturates in victim's system");
            break;
    }

    printf("\n\033[1;33m=== Crime Scene Report ===\033[0m\n");
    printf(clues[game.progress], discoveries[game.progress]);
    printf("\n\033[1;33m=========================\033[0m\n\n");

    game.progress++;
    game.evidence_found++;
    if (game.progress > 5) {
        printf("\033[1;32mYou've found all the evidence! Time to accuse a suspect.\033[0m\n");
    }
}

void handle_interview(char *suspect) {
    char *responses[6][3] = {
        {"I was playing poker at the Tropicana!", "Me? Hurt someone? I'm a lover, not a fighter!", "Victoria? She's been acting real jumpy lately..."},
        {"I was home alone all night", "I don't know what you're implying!", "*nervously checks watch*"},
        {"I was unloading cargo, ask my crew", "The victim? Never liked him, but I didn't kill him", "Tony's been desperate for money..."},
        {"I was tending bar all night", "That gun ain't mine, copper!", "Victoria's been dealing under the table..."},
        {"Time of death was approximately 11:15pm", "The wound shows signs of a close-range shot", "Victoria came by earlier asking about sedatives..."},
        {"Johnny owed me rent for three months!", "Room #47 was his usual spot with... certain ladies", "I heard Victoria threatened him last week"}
    };

    int found = -1;
    for (int i = 0; i < 6; i++) {
        if (strstr(game.suspect[i], suspect)) {
            found = i;
            break;
        }
    }

    if (found >= 0) {
        if (strcmp(game.locations[game.progress % 6], game.suspect_locations[found]) != 0) {
            printf("\n%s isn't here. Try 'whereis %s' to find them.\n", game.suspect[found], game.suspect[found]);
            return;
        }
        
        printf("\n\033[1;36m%s's responses:\033[0m\n", game.suspect[found]);
        for (int j = 0; j < 3; j++) {
            printf("- %s\n", responses[found][j]);
        }
        
        if (found == game.correct_suspect) {
            printf("\033[1;31mYou notice her hands shaking...\033[0m\n");
        }
        
        if (!game.interviews_done[found]) {
            game.interviews_done[found] = 1;
            printf("\n\033[1;32m(New information added to your notes)\033[0m\n");
            
            if (found == 4) {
                printf("\033[1;33m(Note: Check tox_report.txt for details on sedatives)\033[0m\n");
            }
            else if (found == 5) {
                printf("\033[1;33m(Note: Examine hotel_key.txt about Room #47)\033[0m\n");
            }
        }
    } else {
        printf("No suspect by that name. Try: Tony, Victoria, Louie, Mickey, Eleanor, Sal\n");
    }
}

void handle_examine(char *item) {
    if (strcmp(item, "ledger") == 0) {
        system("cat case_files/ledger.txt");
        printf("\n\033[1;33mNote: Large payment to 'Vixen' noted\033[0m\n");
    }
    else if (strcmp(item, "ballistics") == 0) {
        system("cat case_files/ballistics.txt");
        printf("\n\033[1;33mNote: Gun registered to Victoria LaRue\033[0m\n");
    }
    else if (strcmp(item, "witness") == 0) {
        system("cat case_files/witness.txt");
        printf("\n\033[1;33mNote: Timeline matches Victoria's alibi gap\033[0m\n");
    }
    else if (strcmp(item, "forensics") == 0) {
        system("cat case_files/forensics.txt");
        printf("\n\033[1;33mNote: Victoria was seen with victim before murder\033[0m\n");
    }
    else if (strcmp(item, "hotel_key") == 0) {
        system("cat case_files/hotel_key.txt");
        printf("\n\033[1;33mNote: Connects victim to Roosevelt Hotel\033[0m\n");
    }
    else if (strcmp(item, "tox_report") == 0) {
        system("cat case_files/tox_report.txt");
        printf("\n\033[1;33mNote: Matches Victoria's access to sedatives\033[0m\n");
    }
    else {
        printf("No evidence by that name. Try: ledger, ballistics, witness, forensics, hotel_key, tox_report\n");
    }
}

void handle_move(char *location) {
    int found = -1;
    for (int i = 0; i < 6; i++) {
        if (strstr(game.locations[i], location)) {
            found = i;
            break;
        }
    }
    
    if (found >= 0) {
        game.progress = found;
        printf("\nMoved to %s\n", game.locations[game.progress]);
        
        printf("\nPeople here:\n");
        int anyone_here = 0;
        for (int i = 0; i < 6; i++) {
            if (strcmp(game.locations[game.progress], game.suspect_locations[i]) == 0) {
                printf("- %s\n", game.suspect[i]);
                anyone_here = 1;
            }
        }
        if (!anyone_here) {
            printf("No suspects present\n");
        }
        
        if (found == 4) {
            printf("\n\033[1;33m(A worn ledger sits at the front desk)\033[0m\n");
        }
        else if (found == 5) {
            printf("\n\033[1;33m(The smell of antiseptic hangs in the air)\033[0m\n");
        }
    } else {
        printf("Invalid location. Choose from: Police Station, Crime Scene, Velvet Nightclub, Docks, Roosevelt Hotel, City Morgue\n");
    }
}

void handle_whereis(char *suspect) {
    int found = -1;
    for (int i = 0; i < 6; i++) {
        if (strstr(game.suspect[i], suspect)) {
            found = i;
            break;
        }
    }
    
    if (found >= 0) {
        printf("\n%s is at the %s\n", game.suspect[found], game.suspect_locations[found]);
    } else {
        printf("No suspect by that name. Try: Tony, Victoria, Louie, Mickey, Eleanor, Sal\n");
    }
}

void print_case_status() {
    printf("\n\033[1;35m=== CASE STATUS ===\033[0m\n");
    printf("Evidence found: %d/6\n", game.evidence_found);
    printf("Suspects interviewed:\n");
    for (int i = 0; i < 6; i++) {
        printf("- %s: %s\n", game.suspect[i], 
               game.interviews_done[i] ? "Interviewed" : "Not interviewed");
    }
    printf("\nUse 'examine' to review evidence files\n");
    printf("\033[1;35m==================\033[0m\n\n");
}

void handle_accuse(char *suspect) {
    int correct = 0;
    for (int i = 0; i < 6; i++) {
        if (strstr(game.suspect[i], suspect)) {
            correct = (i == game.correct_suspect);
            break;
        }
    }
    print_ending(correct);
    exit(0);
}

void print_ending(int correct) {
    if (correct) {
        printf("\n\033[1;32m**** CASE SOLVED ****\n");
        printf("Victoria 'Vixen' LaRue confesses!\n\n");
        printf("Weapon: %s\n", game.weapon);
        printf("Motive: %s\n", game.motive);
        printf("\nThe Chief hands you your gold detective shield.\033[0m\n");
    } else {
        printf("\n\033[1;31m**** CASE CLOSED - UNSOLVED ****\n");
        printf("The real killer walks free...\n");
        printf("Internal Affairs has suspended you.\033[0m\n");
    }
}

void print_welcome_message() {
    printf("\n\033[1;34m========================================\n");
    printf("         LA NOIRE MURDER MYSTERY         \n");
    printf("========================================\n");
    printf("October 1947. A gunshot echoes through\n");
    printf("the foggy streets. Another body in the\n");
    printf("war between the gangs and the vice lords.\n\n");
    printf("\033[1;31mVICTIM: Johnny 'Rats' Malone\033[0m\n");
    printf("\033[1;33mSUSPECTS:\033[0m\n");
    printf("- Tony 'Fingers' Moretti (bookie)\n");
    printf("- Victoria 'Vixen' LaRue (club owner)\n");
    printf("- Big Louie Scaletta (dock worker)\n");
    printf("- Mickey O'Shea (bartender)\n");
    printf("- Dr. Eleanor Whitmore (medical examiner)\n");
    printf("- Sal 'The Tailor' Russo (hotel owner)\n");
    printf("\033[1;34m========================================\033[0m\n\n");
}

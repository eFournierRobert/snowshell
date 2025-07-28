struct history {
    char **hist;
    int length;
};

void get_commands_history(struct history *);

void free_history(char **);

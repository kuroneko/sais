FILE *myopen(const char *fname, const char *flags);
int read_line(FILE *in, char *out1, char *out2);
int read_line1(FILE *in, char *out1);
void ik_start_log();
void ik_print_log(char *ln, ...);

extern FILE *logfile;
extern int last_logdate;
extern char moddir[256];
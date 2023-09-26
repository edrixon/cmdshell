#ifndef __GOT_TYPES

#define __GOT_TYPES

typedef struct
{
    char *name;
    char *helpStr;
    void (*fn)(char *param);
} CLICOMMAND;

#endif

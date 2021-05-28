#include <stdio.h>
#include <termios.h>

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"

#include <stdarg.h>

#include "pongo_doom_vfs.h"
#include "pongo.h"

void (*existing_preboot_hook)();
void m_preboot_hook()
{
    puts("Called pre-boot hook");
    /* Do patches here */
    if (existing_preboot_hook != NULL)
    	existing_preboot_hook();
    return;
}


void __error() {
    puts("Who the fuck is calling this???\n");
}

FILE *__stdoutp = 1;
FILE *__stderrp = 2;

int abs(int j) {
    return j < 0 ? -j : j;
}

int atoi(const char *nptr) {

    int res = 0;

    for (int i = 0; nptr[i] != '\0'; ++i)
        res = res * 10 + nptr[i] - '0';
 
    // return result.
    return res;
}


int main(int argc, const char** argv)
{

    printf("Starting...\n");

    printf("main addr: %p\n", main);

    init_pongo_vfs();
    printf("Init'd VFS\n");

    myargc = argc;
    myargv = argv;

    // struct termios initial_settings, new_settings;
    // tcgetattr(0,&initial_settings);

    // new_settings = initial_settings;
    // new_settings.c_lflag &= ~ICANON;
    // new_settings.c_lflag &= ~ECHO;
    // new_settings.c_lflag &= ~ISIG;
    // new_settings.c_cc[VMIN] = 0;
    // new_settings.c_cc[VTIME] = 0;

    // tcsetattr(0, TCSANOW, &new_settings);

    D_DoomMain();

    // tcsetattr(0, TCSANOW, &initial_settings);

    return 0;
}


void entry() {
    char *arg0 = "doom";
    const char *argv[] = {arg0, 0};
    main(1, argv);
}

void module_entry() {
    existing_preboot_hook = preboot_hook;
    preboot_hook = m_preboot_hook;
    command_register("doom", "doom", entry);
}


char* module_name = "doom";

struct pongo_exports exported_symbols[] = {
    {.name = 0, .value = 0}
};

void __chkstk_darwin() {
}
/*
extern int sniprintf(char *str, size_t size, const char *format, ...);

int sprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result =  sniprintf(str, 4096 * 16, format, args); // Hack
    va_end(args);
    return result;
}*/


int toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - 0x20;
    }
    return c;
}

int strcasecmp(const char *s1, const char *s2)
{
    int offset,ch;
    unsigned char a,b;

    offset = 0;
    ch = 0;
    while( *(s1+offset) != '\0' )
    {
        /* check for end of s2 */
        if( *(s2+offset)=='\0')
            return( *(s1+offset) );

        a = (unsigned)*(s1+offset);
        b = (unsigned)*(s2+offset);
        ch = toupper(a) - toupper(b);
        if( ch<0 || ch>0 )
            return(ch);
        offset++;
    }

    return(ch);
}

int strncasecmp(const char *s1, const char *s2, register size_t n)
{
    register unsigned char u1, u2;
    for (; n != 0; --n) {
        u1 = (unsigned char) *s1++;
        u2 = (unsigned char) *s2++;
        if (toupper(u1) != toupper(u2)) {
            return toupper(u1) - toupper(u2);
        }
        if (u1 == '\0') {
            return 0;
        }
    }
    return 0;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    while (i < n) {
        dest[i++] = '\0';
    }
    return dest;
}

int strncmp( const char * s1, const char * s2, size_t n )
{
    while ( n && *s1 && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( n == 0 )
    {
        return 0;
    }
    else
    {
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
    }
}


void I_StartTic (void)
{
    // event_t event = {0,0,0,0};
    // char key = getchar();
    // if (key != EOF)
    // {
    //          if (key == 'a') key = KEY_LEFTARROW;
    //     else if (key == 'd') key = KEY_RIGHTARROW;
    //     else if (key == 'w') key = KEY_UPARROW;
    //     else if (key == 's') key = KEY_DOWNARROW;

    //     event.type = ev_keydown;
    //     event.data1 = key;
    //     D_PostEvent(&event);

    //     event.type = ev_keyup;
    //     event.data1 = key;
    //     D_PostEvent(&event);
    // }
}

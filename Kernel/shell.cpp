#include <shell.h>
#include <shitio.h>
#include <port.h>
#include <memory.h>
#include <interrupt.h>
#include <paging.h>
#include <alloc.h>
#include <string.h>
#include <scheduler.h>
#include <sound.h>
#include <vesa.h>

/* prototypes */

void version();

void clr();

void startpage();

void help();

void reboot();

void shutdown();

void print(const char *str);

void test();

void test_panic();

void print_uptime();

void add_command(const char base_name[10], int args, int size);

int command_parse(char argument[5][10], int start, const char *input);

/* globals */

using namespace standardout;
using namespace shell;
using namespace MM;

command arg[1];

const char *command_list[] = {  "version", "clr", "shutdown", "reboot",
                                "testMM", "reg-dump", "seg-dump", "uptime",
                                "song", "help"
                             };

const char *arg_command[] = { "print" };

typedef void (*command_functions)();

command_functions comm_func[] = {   version, clear_promnt, shutdown, reboot,
                                    test, reg_flow, seg_flow, print_uptime,
                                    play, help
                                };

void command_handler(const char *input)
{
    static bool set_up = false;

    if(!set_up) {
        add_command("print", 1, 20);
        set_up = true;
    }

    char arguments[5][10];

    bool commandFound = false;

    for(long unsigned int i = 0; i < sizeof command_list/sizeof *command_list; i++) {
        if(strcmp(input, command_list[i]) == 0) {
            t_print("SHELL: receiving request for command \"%s\"", input);
            comm_func[i]();
            commandFound = true;
        }
        if(commandFound)
            break;
    }

    if(!commandFound) {
        char base[10];

        int length = strlen(input);
        int break_point = 0;

        for(int i = 0; i < length; i++) {
            if(input[i] == ' ') {
                break_point = i;
                break;
            }
            else
                base[i] = input[i];
        }

        for(int i = 0; i < 1; i++) { //TODO: make this nicer

            if(strcmp(arg[i].name, base) == 0) {

                commandFound = true;

                int argsize = command_parse(arguments, break_point, input);

                char sendstr[256];
                for(int i = 0; i < 1; i++)
                    for(int j = 0; j < argsize; j++)
                        sendstr[j] = arguments[i][j];

                k_print("%s", sendstr);

                print(sendstr);

                memset(sendstr, 0, 256);

                break;
            }
        }
    }

    if(!commandFound && strlen(input) != 0)
        k_print("\n%s command not found\n", input);

    if(!commandFound && strlen(input) == 0)
        putchar('\n');

    change_text_color(VGA_LIGHT_BLUE);
    k_print("(*ROOT*) > ");
    change_text_color(VGA_BLUE);
}


void print(const char *str)
{
    k_print("\n%s", str);
}

void version()
{
    k_print("\ncrepOS beta 1.3\n");
}

void help()
{
    putchar('\n');
    for(long unsigned int i = 0; i < sizeof command_list / sizeof *command_list - 1; i++) {
        if(end_of_screen(strlen(command_list[i])))
            putchar('\n');
        k_print("%s ", command_list[i]);
    }
    for(long unsigned int i = 0; i < sizeof arg_command / sizeof *arg_command; i++)
        k_print("%s ", arg_command[i]);
    putchar('\n');
}

void reboot()
{
    uint8_t check = 0x02;

    while (check & 0x02)
        check = inb(0x64);

    outb(0x64, 0xFE);
    asm volatile("hlt");
}

/* not ACPI, just qemu bochs and virtual machine */

void shutdown(void)
{
    while(1) {
        outw(0xB004, 0x2000);
        for(const char *s = "Shutdown"; *s; ++s)
            outb(0x8900, *s);
        asm volatile("cli; hlt");
    }
}

void add_command(const char base_name[10], int args, int size)
{
    static int counter = 0;

    if(counter == 2) {
        t_print("fatal: increase size of struct array to add another command");
        return;
    }

    strcpy(arg[counter].name, base_name);
    arg[counter].arguments_num = args;
    arg[counter].assumed_size = size;

    counter++;
}

int command_parse(char argument[5][10], int start, const char *input)
{
    int arg_count = 0;
    int counter = 0;

    int argsize = 0;

    int strle = strlen(input);
    bool got_base;

    for(int j = start+1; j < strle; j++) {
        if(input[j-1] == ' ')
        got_base = true;

        if(got_base) {
            if(input[j] == ' ') {
                arg_count++;
                counter = 0;
            }
            else if(counter < strle) {
                argument[arg_count][counter++] = input[j];
                argsize++;
            }
            else {
                t_print("Woah there you, you almost caused a seg fault");
                break;
            }
        }
    }
    return argsize;
}

/* tests malloc and free */

void test()
{
    k_print("\nTesting Malloc & Free:\n");
    /*uint64_t *ptr = (uint64_t*)pagalloc(1);  //tests page allocator
    uint64_t *ptr1 = (uint64_t*)pagalloc(1);
    pagfree(ptr);
    uint64_t *ptr2 = (uint64_t*)pagalloc(1);*/

    uint32_t *ptr = (uint32_t*)malloc(sizeof(uint32_t));
    uint16_t *ptr1 = (uint16_t*)malloc(sizeof(uint16_t));
    uint16_t *ptr2 = (uint16_t*)malloc(sizeof(uint16_t));
    k_print("Space allocated for ptr: %a\n", ptr);
    k_print("Space allocated for ptr1: %a\n", ptr1);
    k_print("Space allocated for ptr2: %a\n", ptr2);
    k_print("Freeing ptr2\n");
    free(ptr);
    uint16_t *ptr3 = (uint16_t*)malloc(sizeof(uint16_t));
    uint16_t *ptr4 = (uint16_t*)malloc(sizeof(uint16_t));
    uint16_t *ptr5 = (uint16_t*)malloc(sizeof(uint16_t));
	k_print("Space allocated for ptr3: %a\n", ptr3);
	k_print("Space allocated for ptr4: %a\n", ptr4);
    k_print("Space allocated for ptr5: %a\n", ptr5);
}

void print_uptime()
{
    t_print("Gamer");
}

void test_panic() {
    panic("test");
}

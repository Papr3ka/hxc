#include "hxc.h"

/*
 * Settings bits (uint32_t)
*/

#define HXC_SHOW_OFFSET 0x00000001
#define HXC_DECODE_CHAR 0x00000002
#define HXC_USE_ASCII   0x00000004

const char* hxc_version = "0.1.0";

int strdigits(char* str){
    int digits = 0;
    for(size_t i = 0; i < strlen(str); ++i){
        if(str[i] >= '0' && str[i] <= '9'){
            ++digits;
        }
    }
    return digits;
}

void input_loop(uint32_t settings, int columns, int groups){
    int col;
    int printable_columns;

    size_t offset = 0;
    size_t input_index;
    size_t input_size;

    byte_t* input_buffer;

    for(;;){
        input_buffer = input_dynamic(stdin, 32, 16);
        input_size = strlen((char*)input_buffer);
        input_index = 0;

        printable_columns = columns;


        while(input_index + columns < input_size){
            if(settings & HXC_SHOW_OFFSET){
                printf("%08lx: ", offset);
            }

            for(col = 0; col < columns; ++col){
                printf("%02x", input_buffer[input_index]);
                if((col + 1) % groups == 0){
                    printf(" ");
                }
                ++input_index;
            }

            if(settings & HXC_DECODE_CHAR){

                input_index -= columns;

                
                printf(" ");

                for(col = 0; col < columns; ++col){
                    if(isprint(input_buffer[input_index])){
                        printf("%c ", input_buffer[input_index]);
                    }else{
                        printf("\033[31m.\033[0m ");
                    }

                    ++input_index;
                }
            }


            offset += columns;

            puts("");

        }

        printable_columns = input_size - input_index;

        if(printable_columns){

            if(settings & HXC_SHOW_OFFSET){
                printf("%08lx: ", offset);
            }

            for(col = 0; col < printable_columns; ++col){
                printf("%02x", input_buffer[input_index]);
                ++input_index;
                if((col + 1) % groups == 0){
                    printf(" ");
                }
            }

            if(settings & HXC_DECODE_CHAR){

                input_index -= printable_columns;

                for(col = printable_columns; col < columns; ++col){
                    printf("  ");
                    if((col + 1) % groups == 0){
                        printf(" ");
                    }
                }

                printf(" ");

                for(col = 0; col < printable_columns; ++col){
                    if(isprint(input_buffer[input_index])){
                        printf("%c", input_buffer[input_index]);
                    }else{
                        printf("\033[31m.\033[0m");
                    }

                    ++input_index;
                }
            }
            offset += printable_columns;
        }
        puts("");
        free(input_buffer);
        
    }

}


void help(void){
    printf("Usage: hxc [OPTIONS] ... [FILE]\n\n"
           "With no FILE, or when FILE is -, read standard input.\n\n"

           "Options:\n"
           "  --columns N                        Display N columns\n"
           "  --group N                          Groups N bytes\n"
           "  --splice START:END                 Displays START to END bytes\n\n"
        
           "  -d, --decode                       Decode hex to characters\n"
           "  -s, --show-offset                  Display offset\n\n"

           "  -h, --help                         Display this information\n"
           "  -v, --version                      Display the version information\n");
}

void version(){
    printf("hxc %s\n", hxc_version);
}

int main(int argc, char** argv){

    char* target_file = "-";
    int ag_parse_idx  = 1;
    int columns       = 16;
    int groups        = 1;

    uint32_t settings = 0;

    size_t bytes_start = 0;
    size_t bytes_end = -1;

    while(ag_parse_idx < argc){
        if(!strcmp(argv[ag_parse_idx], "--show-offset")){
            settings |= HXC_SHOW_OFFSET;

        }else if(!strcmp(argv[ag_parse_idx], "-s")){
            settings |= HXC_SHOW_OFFSET;

        }else if(!strcmp(argv[ag_parse_idx], "--decode")){
            settings |= HXC_DECODE_CHAR;

        }else if(!strcmp(argv[ag_parse_idx], "-d")){
            settings |= HXC_DECODE_CHAR;

        }else if(!strcmp(argv[ag_parse_idx], "--columns")){
            if(argc > ag_parse_idx + 1){
                ++ag_parse_idx;
                if(strdigits(argv[ag_parse_idx])){
                    columns = atol(argv[ag_parse_idx]);
                }
            }
        }else if(!strcmp(argv[ag_parse_idx], "--group")){
            if(argc > ag_parse_idx + 1){
                ++ag_parse_idx;
                if(strdigits(argv[ag_parse_idx])){
                    groups = atol(argv[ag_parse_idx]);
                }
            }
            
        }else if(!strcmp(argv[ag_parse_idx], "--splice")){
            if(argc > ag_parse_idx + 1){
                ++ag_parse_idx;

                char* start_str = strtok(argv[ag_parse_idx], ":");
                char* end_str = strtok(NULL, ":");

                if(!strncmp(argv[ag_parse_idx], ":", 1)){
                    end_str = start_str;
                    start_str = NULL;
                }

                if(start_str && strlen(start_str)){
                    if(!strncmp(start_str, "0x", 2)){
                        bytes_start = strtol(start_str, NULL, 16);
                    }else if(strdigits(start_str)){
                        bytes_start = atol(start_str);
                    }
                }

                if(end_str && strlen(end_str)){

                    if(!strncmp(end_str, "0x", 2)){
                        bytes_end = strtol(end_str, NULL, 16);
                    }else if(strdigits(end_str)){
                        bytes_end = atol(end_str);
                    }
                }

                if(!(start_str || end_str)){
                    printf("hxc: error: failed parsing \"%s\"\n", argv[ag_parse_idx]);
                    exit(1);
                }

                if(bytes_start > bytes_end){
                    printf("hxc: error: invalid splice index");
                    exit(1);
                }
            }
            
        }else if(!strcmp(argv[ag_parse_idx], "--help")){

            help();
            exit(0);

        }else if(!strcmp(argv[ag_parse_idx], "-h")){

            help();
            exit(0);

        }else if(!strcmp(argv[ag_parse_idx], "--version")){

            version();
            exit(0);

        }else if(!strcmp(argv[ag_parse_idx], "-v")){

            version();
            exit(0);

        }else{
            if(ag_parse_idx + 1 == argc){
                target_file = argv[ag_parse_idx];
                break;
            }
            printf("hxc: error: unrecognized command-line option '%s'\n", argv[ag_parse_idx]);
            exit(1);
        }

        ++ag_parse_idx;
        
        /* 
        if(!strcmp(argv[ag_parse_idx], "-")){

        }else
        */
        
    }

    columns *= groups;

    if(!(settings & HXC_USE_ASCII)){
        setlocale(LC_ALL, "");
    }

    if(!strcmp(target_file, "-") || argc == 1){
        input_loop(settings, columns, groups);
        return 0;
    }

    FILE* handle = fopen(target_file, "r");

    fseek(handle, bytes_start, SEEK_SET);

    if(!handle){
        printf("hxc: error: %s: no such file or directory\n", target_file);
        exit(1);
    }

    File_buffer* fbuf = init(columns + 1, handle);

    int result = advance(fbuf, 1, bytes_end);
    size_t offset = bytes_start;

    int col = 0;
    int printable_columns = columns;

    do{
        /*
         * all the if-statements are easily branch predictable
        */

        result = advance(fbuf, columns, bytes_end);

        printable_columns -= result;

        if(!printable_columns){
            break;
        }

        if(settings & HXC_SHOW_OFFSET){
            printf("%08lx: ", offset);
        }

        for(;col < printable_columns; ++col){
            printf("%02x", buffer_get(fbuf, col));
            if((col + 1) % groups == 0){
                printf(" ");
            }
        }

        if(settings & HXC_DECODE_CHAR){
            col = 0;
            for(;printable_columns < columns; ++printable_columns){
                printf("  ");
                if(printable_columns % groups == 0){
                    printf(" ");
                }
            }

            printable_columns -= result;

            printf(" ");

            for(;col < printable_columns; ++col){
                if(isprint(buffer_get(fbuf, col))){
                    printf("%c", buffer_get(fbuf, col));
                }else{
                    printf("\033[31m.\033[0m");
                }
            }
        }

        col = 0;

        puts("");

        offset += columns;

    }while(!result);

    fbuf_close(fbuf);

    return 0;
}
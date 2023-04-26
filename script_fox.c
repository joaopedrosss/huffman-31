#include "zip.h"
#include "unzip.h"

int main()
{
    //APRESENTACAO DO PROGRAMA E OPCOES
    printf(" ==================================================================================================================================================\n");
    printf("| __          __         _                                       _                _____                 _           _       ______                 |\n");
    printf("| \\ \\        / /        | |                                     | |              / ____|               (_)         | |     |  ____|                |\n");
    printf("|  \\ \\  /\\  / /    ___  | |   ___    ___    _ __ ___     ___    | |_    ___     | (___     ___   _ __   _   _ __   | |_    | |__      ___   __  __ |\n");
    printf("|   \\ \\/  \\/ /    / _ \\ | |  / __|  / _ \\  | '_ ` _ \\   / _ \\   | __|  / _ \\     \\___ \\   / __| | '__| | | | '_ \\  | __|   |  __|    / _ \\  \\ \\/ / |\n");
    printf("|    \\  /\\  /    |  __/ | | | (__  | (_) | | | | | | | |  __/   | |_  | (_) |    ____) | | (__  | |    | | | |_) | | |_    | |      | (_) |  >  <  |\n");
    printf("|     \\/  \\/      \\___| |_|  \\___|  \\___/  |_| |_| |_|  \\___|    \\__|  \\___/    |_____/   \\___| |_|    |_| | .__/   \\__|   |_|       \\___/  /_/\\_\\ |\n");
    printf("|                                                                                                          | |                                     |\n");
    printf("|                                                                                                          |_|                                     |\n");
    printf(" ==================================================================================================================================================\n");
    printf("Create by: Wolfshohle\n");
    int opc = 0;
    sleep(1);
    while(opc != 3)
    {
        printf(" -----------------------------\n| SELECT ANY OF THE OPTIONS:  |\n -----------------------------\n| 1. compress                 |\n| 2. unzip                    |\n| 3. exit                     |\n -----------------------------\n");
        scanf("%d", &opc);
        char archive_name[106];
        switch (opc)
        {
        case 1:
            printf("\nstarting file compression\n");
            scanf("%s", archive_name);
            compress(archive_name);
            break;
        case 2:
            printf("\nstarting to unzip\n");
            scanf("%s", archive_name);
            descompressed(archive_name);
            break;
        case 3:
            printf("Thanks for using the fox script!!\n");
            break;
        default:
            printf("Invalid option!\n");
            break;
        }
    }

    return 0;
}
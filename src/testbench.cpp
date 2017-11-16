//
// Created by jf on 10/24/17.
//


#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "testbench.h"

int main(void){
    char saisie_de_user[300];
    char lecture_fichier[256];
    char input=0x00,mode=0;
    int i=0,nb_char=0,taille=0,retour=0;
    int fp;
    std::string saisie;
    while(i<300){
        saisie_de_user[i]='\0';
        i++;
    }

    std::cout << "\033[2J\033[1;1H";
    while(1){
        // std::cout << "\033[2J\033[1;1H";
        std::cout<<"1. Prendre une photo\n2. Ajuster le focus\n3."
                <<"Reset Focus\n4. Get parameter\n5. Set parameter\n6. Grab Data\n";
        std::cin>>input;
        switch (input) {
            case '1' :

                break;
            case '2':

                break;
            case '3':
                fp=open("/dev/cam_node",O_RDWR);
                if (fp < 0 ) {
                    printf("Error couldnt open the file\n");
                } else {
                    std::cout<<"Focus reset with value : "<<ioctl(fp,IOCTL_PANTILT_RESEST)<<"\n";
                    close(fp);
                }
                break;
            case '4':

                break;
            default:
                break;
        }

        if(input=='q'){
            break;
        }
        // std::this_thread::sleep_for (std::chrono::seconds(3));
    }

    //fprintf(fp, "%s",buffer);



    return 0;
}


//
// Created by jf on 10/24/17.
//


#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "dht_data.h"

#include "testbench.h"
#include "usbvideo.h"

int main(void){

    char input=0x00,mode=0;
    int i=0,nb_char=0,taille=0,retour=0,param;
    int fp;
    int choix;
    int test[3]={0x01,0x0A,0x05};
    struct termios old_setting, new_setting;
    FILE *foutput;
    unsigned char * inBuffer;
    unsigned char * finalBuf;
    unsigned int mySize;
    int reponse[4];
    inBuffer =(unsigned char *) malloc((42666)* sizeof(unsigned char));
    finalBuf =(unsigned char *) malloc((42666 * 2)* sizeof(unsigned char));
    if((inBuffer == NULL) || (finalBuf == NULL)){
        return -1;
    }

    tcgetattr(STDIN_FILENO,&old_setting);
    new_setting=old_setting;
    new_setting.c_lflag &=(~ICANON & ~ECHO);




    std::cout << "\033[2J\033[1;1H";
    while(1){
        // std::cout << "\033[2J\033[1;1H";
        std::cout<<"1. Prendre une photo\n2. Bouger le focus\n3. "
                <<"Reset Focus\n4. Get parameter\n5. Set parameter\n";
        std::cin>>input;
        switch (input) {
            case '1' :
                fp=open("/dev/cam_node",O_RDWR);
                if(fp >=0){
                    foutput = fopen("image.jpg", "wb");
                    if(foutput != NULL) {

                        std::cout << "IOCTL STREAMON : " << ioctl(fp, IOCTL_STREAMON) << "\n";
                        std::cout << "IOCTL GRAB : " << ioctl(fp, IOCTL_GRAB) << "\n";
                        mySize = read(fp, inBuffer, 42666);
                        std::cout<<"READ "<<mySize;
                        std::cout << "IOCTL STREAMOFF : " << ioctl(fp, IOCTL_STREAMOFF) << "\n";
                        memcpy(finalBuf, inBuffer, HEADERFRAME1);
                        memcpy(finalBuf + HEADERFRAME1, dht_data, DHT_SIZE);
                        memcpy(finalBuf + HEADERFRAME1 + DHT_SIZE,
                               inBuffer + HEADERFRAME1,
                               (mySize - HEADERFRAME1));
                        fwrite(finalBuf, mySize + DHT_SIZE, 1, foutput);
                        fclose(foutput);
                    }
                }
                close(fp);
                break;
            case '2':
            std::cout<<"Veuillez choisir l'orientation de la camera\n";
                std::cout<<"W. Haut\nS. Bas\nA. Gauche\nD. Droite\ne. Sortie\n";

                tcsetattr(STDIN_FILENO,TCSANOW,&new_setting);
                do{
                    input=getchar();
                    //std::cin>>input;
                    switch (input){
                        case 'w':
                            choix=HAUT;
                            break;
                        case 's':
                            choix=BAS;
                            break;
                        case 'a':
                            choix=GAUCHE;
                            break;
                        case 'd':
                            choix=DROIT;
                            break;
                        default:
                            choix=0xFF;
                            break;
                    }
                    if(choix != 0xff){
                        fp=open("/dev/cam_node",O_RDWR);
                        if (fp < 0 ) {
                            printf("Error couldnt open the file\n");
                        } else {
                            std::cout<<"Focus set with value : "<<ioctl(fp,IOCTL_PANTILT,choix)<<"\n";
                            close(fp);
                        }
                    }
                }while(input!='e');
                tcsetattr(STDIN_FILENO,TCSANOW,&old_setting);
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
                fp=open("/dev/cam_node",O_RDWR);
                if (fp < 0 ) {
                    printf("Error couldnt open the file\n");
                } else {
                    std::cout<<"Listing all Processing Unit Control Selectors value :\n";
                    std::cout<<"valeur\t\tCurrent\t\tMin\t\tMax\t\tRes\n";
                    for(int i=0;i<=0x12;i++){
                        reponse[0]=i;
                        ioctl(fp,IOCTL_GET,reponse);
                        if(reponse[0]<0){
                            printf("Data not available on this device\n");
                        }
                        else{
                            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",i,reponse[0],reponse[1],reponse[2],reponse[3]);
                        }

                    }

                    close(fp);
                }

                break;
            case '5':
            std::cout<<"Veuillez entrer le parametre que vous voulez modifier (0-18)\n";
                std::cin>>param;
                reponse[0]=param;
                     switch(param){
                         case 0: std::cout<<"PU_CONTROL_UNDEFINED please set value\n";
                             std::cin>>param;
                             break;
                         case 1: std::cout<<"PU_BACKLIGHT_COMPENSATION_VALUE please set value\n";
                             std::cin>>param;
                             break;
                         case 2:std::cout<<"PU_BRIGHTNESS_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 3:std::cout<<"PU_CONTRAST_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 4:std::cout<<"PU_GAIN_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 5:std::cout<<"PU_POWER_LINE_FREQUENCY_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 7:std::cout<<"PU_SATURATION_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 8:std::cout<<"PU_SHARPNESS_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 10:std::cout<<"PU_WHITE_BALANCE_TEMPERATURE_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         case 11:std::cout<<"PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL please set value\n";
                             std::cin>>param;
                             break;
                         default:
                             std::cout<<"Choix invalide pour lappareil\n";
                             param =-1;
                             break;
                     }
                if(param >=0){
                    reponse[1]=param>>8;
                    reponse[2]=param&0x00FF;
                    fp=open("/dev/cam_node",O_RDWR);
                    if (fp < 0 ) {
                        printf("Error couldnt open the file\n");
                    } else {
                        if(ioctl(fp,IOCTL_SET,reponse)>=0){
                            printf("Register %d was set successfully with value %d",reponse[0],((reponse[1]<<8)&reponse[2]));
                        }
                        close(fp);
                    }
                }
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


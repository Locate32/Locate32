#ifndef DTYPE_H
#define DTYPE_H

extern "C" {
void getlocaldrives(const char* drives);
unsigned char getdrivetype(const char* root);
int getvolumeinfo(const char* root,unsigned long* dwSerial,char* szLabel,char* szFS);
}

#endif
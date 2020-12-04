//Na poczatek jako argumenty programu pierwszy Tekst jawny, jako drugi do pliku wygenerowanych kluczy ok
//Tekst jest dzielony na 64 bitowe bloki ok
//Blok 64 jest poddawany permutacji ok
//Pobieranie klucza dla bloku z pliku ok
//Transformacja klucza na 56 bitów ok
//Inicjacja 16 rund
//
//-Podział Bloku tekstu jawnego na 2 32 bit L oraz R
//-Podział klucza na dwa 28 bitowe bloki
//
//Działania w kazdej rundzie:
//1. Transformacja klucza
//  -Rotacja w lewo bloków klucza o wartość 1 lub 2 zależne od tablicy LS 
//  -uzyskana wartość (połączona) przechodzi przez permutację PC-2 
//2. Funkcja f:
//  -Expansion E: R z 32 bit robi 48 poprzez tablicę E
//  -XOR Z podanym kluczem 48 bit
//  -dzielimy na 8 bloków po 6 bitów
//  -do każdego osobna tabela liczb 4 bitowej
//    -pobieranie wiersza: pierwszy i ostatni bit bloku
//    -pobieranie numeru kolumny: reszta bitów 
//    -pobieranie i zwracanie liczby z tablicy
//  -permutacja tablicą P
//3. XOR Wyjście funkcji F z L
//4. Zamiana L i R
//Po 16 rundzie znowu zamienić L i R 
//Ostatnia permutacja Finalna

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
/*********************************************************************************
 * All Tabs 
 * ******************************************************************************/

const uint8_t IP_InitPer[] = {58,50,42,34,26,18,10,2,
                              60,52,44,36,28,20,12,4,
                              62,54,46,38,30,22,14,6,
                              64,56,48,40,32,24,16,8,
                              57,49,41,33,25,17,9,1,
                              59,51,43,35,27,19,11,3,
                              61,53,45,37,29,21,13,5,
                              63,55,47,39,31,23,15,7};

const uint8_t IP_FinalPer[] = {40,8,48,16,56,24,64,32,
                               39,7,47,15,55,23,63,31,
                               38,6,46,14,54,22,62,30,
                               37,5,45,13,53,21,61,29,
                               36,4,44,12,52,20,60,28,
                               35,3,43,11,51,19,59,27,
                               34,2,42,10,50,18,58,26,
                               33,1,41,9,49,17,57,25};

const uint8_t E_Expansion[] = {32,1,2,3,4,5,
                         4,5,6,7,8,9,
                         8,9,10,11,12,13,
                         12,13,14,15,16,17,
                         16,17,18,19,20,21,
                         20,21,22,23,24,25,
                         24,25,26,27,28,29,
                         28,29,30,31,32,1};

const uint8_t S_Box[8][4][16] =   {{
                               {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
                               {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
                               {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
                               {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
                              },

                              {
                                {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
                                {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
                                {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
                                {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
                              },
                              
                              {
                                {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
                                {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
                                {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
                                {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
                              }, 

                              {
                                {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
                                {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
                                {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
                                {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
                              }, 

                              {
                                {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
                                {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
                                {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
                                {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
                              }, 

                              {
                                {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
                                {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
                                {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
                                {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
                               },

                              {
                                {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
                                {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
                                {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
                                {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
                              },

                              {
                                {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
                                {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
                                {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
                                {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
                              }};   

const uint8_t P_Permutation[] = {16,7,20,21,29,12,28,17,
                                 1,15,23,26,5,18,31,10,
                                 2,8,24,14,32,27,3,9,
                                 19,13,30,6,22,11,4,25};  

const uint8_t PC_1[] = {57,49,41,33,25,17,9,1,
                        58,50,42,34,26,18,10,2,
                        59,51,43,35,27,19,11,3,
                        60,52,44,36,63,55,47,39,
                        31,23,15,7,62,54,46,38,
                        30,22,14,6,61,53,45,37,
                        29,21,13,5,28,20,12,4}; 

const uint8_t PC_2[] = {14,17,11,24,1,5,3,28,
                        15,6,21,10,23,19,12,4,
                        26,8,16,7,27,20,13,2,
                        41,52,31,37,47,55,30,40,
                        51,45,33,48,44,49,39,56,
                        34,53,46,42,50,36,29,32};  

const uint8_t Shift_tab[] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};                      


/************************************************************************************/

uint64_t permutation(uint64_t buffer, uint8_t len, const uint8_t * tab);

uint32_t shiftKey(uint32_t buffer, uint8_t round);

uint32_t functionF(uint64_t bufTxt_R, uint64_t K);

uint8_t SBoxSub(uint8_t buf, uint8_t i);

int main(int argc, char **argv) {
  //Załadowanie plików do odczytu oraz zapisu
  FILE * fileText = fopen("txt.txt","rb");
  FILE * fileKey = fopen("key.txt","rb");
  FILE * fileDES = fopen("DES.txt","wb");
  if(fileKey == NULL){ printf("wrong file name\n"); return EXIT_FAILURE;}
  
  uint64_t bufferTxt;
  uint64_t bufferKey;

  while (!feof(fileText)){
    fread(&bufferTxt, sizeof(uint64_t), 1, fileText);
    fread(&bufferKey, sizeof(uint64_t), 1, fileKey);
    bufferTxt = permutation(bufferTxt,64,IP_InitPer);
    bufferKey = permutation(bufferKey,56,PC_1);
    uint64_t bufferTxt_L = (bufferTxt >> 32);
    uint64_t bufferTxt_R = (bufferTxt) & 0xFFFFFFFF;
    uint64_t bufferKey_C = (bufferKey >> 28);
    uint64_t bufferKey_D = (bufferKey) & 0xFFFFFFF;
    uint64_t swapBuff = 0;
    for(int i = 0;i<16;i++){
      bufferKey_C = shiftKey(bufferKey_C,i); 
      bufferKey_D = shiftKey(bufferKey_D,i);
      bufferKey = (bufferKey_C<<28) | (bufferKey_D & 0xFFFFFFF);  
      uint64_t bufferKeyPC_2 = permutation(bufferKey,48,PC_2);
      uint32_t postF = functionF(bufferTxt_R, bufferKeyPC_2);
      bufferTxt_L = bufferTxt_L ^ postF;
      swapBuff = bufferTxt_L;
      bufferTxt_L = bufferTxt_R;
      bufferTxt_R = swapBuff;
    }
    swapBuff = bufferTxt_L;
    bufferTxt_L = bufferTxt_R;
    bufferTxt_R = swapBuff;
    bufferTxt = bufferTxt_L;
    bufferTxt = (bufferTxt << 32) | bufferTxt_R;
    fwrite(&bufferTxt,sizeof(bufferTxt),1,fileDES);
  }
  fclose(fileText);
  fclose(fileKey);
  fclose(fileDES);
}

uint64_t permutation(uint64_t buffer, uint8_t len, const uint8_t * tab){
  uint64_t w = 1;
  uint64_t ret_val = 0;
  for(int i = 0; i < len ;i++){
    if(buffer & (w<<(tab[i] - 1))){
      ret_val = (ret_val<<1) | 1;
    }else{
      ret_val = (ret_val<<1);
    }
  }
  return ret_val;
}

uint32_t shiftKey(uint32_t buffer, uint8_t round){
  for(uint8_t i = 0;i<Shift_tab[round];i++){
    if((buffer>>27) > 0){
      buffer = (buffer << 1) | 1;
    }else{
      buffer = (buffer << 1);
    }
  }
  return buffer;
}

uint32_t functionF(uint64_t bufTxt_R, uint64_t K){
  bufTxt_R = permutation(bufTxt_R,48,E_Expansion);
  //XOR
  bufTxt_R = bufTxt_R ^ K;
  uint32_t postSBoxBuf = 0;
  uint8_t SBox_buff = 0;
  for(int i = 0;i<8;i++){
    SBox_buff = (bufTxt_R >> ((7-i)*6)) & 0x3F;
    SBox_buff = SBoxSub(SBox_buff,i);
    postSBoxBuf = (postSBoxBuf << 4) | SBox_buff;
  }
  postSBoxBuf = permutation(postSBoxBuf,32,P_Permutation);
  return postSBoxBuf;
}

uint8_t SBoxSub(uint8_t buf, uint8_t i){
  uint8_t row = 0;
  uint8_t column = 0;
  row = (buf & 1) | ((buf>>5)<<1) & 3;
  column = (buf>>1) & 15;
  return S_Box[i][row][column];
}
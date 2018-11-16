//Ejercicio 1 
//Tarea Programada 2 
//Estructuras de Computadores Digitales 2 II-2018 
//Alejandro Rojas Cordero B36049 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
    
// Constans values used along the program    
#define CACHE 16
#define LINEA 32
#define ASOC 2
#define OFFSETL1 13
#define INDEXL1 16
#define TAGL1 3
#define OFFSETL2 16
#define INDEXL2 256
#define TAGL2 3    

// std req    
using namespace std;

// Delimiter for check the trace
string delimiter = " ";

// Global vars and initialization
int index = 0;
int tag = 0;
int access = 0;
int RW1 = 0;
int RW2 = 0;
int indexAso1 = 0;
int indexAso2 = 0;
int totalL1 = 0;
int totalL2 = 0;
int missL1 = 0;
int missL2 = 0;
int missInv1 = 0;
int missInv2 = 0;
int totalL1yL2 = 0;
int stateCPU1 = 3; // States are 0:M, 1:E, 2:S, 3:I
int stateCPU2 = 3; // Same states as CPU1
double rateL1 = 0.0;
double rateL2 = 0.0;
int cacheL1CPU1 [65536][2] = {0}; // Memory size for 2^16 L1 both CPU
int cacheL1CPU2 [65536][2] = {0};

// Set vars to other funcs (just for L1) with every line read by readFile
// Recieves a string fileLine and returns a 0
int setVars (string fileLine) {
    unsigned int hexNumber;
    stringstream ss;
    ss << hex << fileLine;
    ss >> hexNumber;
    tag = hexNumber >> OFFSETL1 + INDEXL1; // Gets the tag
    index = (hexNumber >> OFFSETL1) & int(8191); // Gets the index, 8191 is dec for $1FFF
}

// Set mod var from line
// Recieves a string fileLine and returns a 0
int setAccess(string fileLine) {
    unsigned int hexNumber;
    stringstream ss;
    ss << dec << fileLine;
    ss >> access;
    return 0;
}

// Set if Read or Write from line
// Recieves a string fileLine and returns a 0
int setRW(string fileLine) {
    unsigned int hexNumber;
    stringstream ss;
    if (access == 4) {
        ss << dec << fileLine;
        ss >> RW1;
    } else {
        ss << dec << fileLine;
        ss >> RW2;
    }
    return 0;
}

// Checks L2
int checkL2() {
    return 0;
}

// FSM MESI handling for one memory space
int MESI() {
    if (stateCPU1 == 3 && stateCPU2 == 3) {
        if (RW1 == 1) {
            stateCPU1 = 0;
        } else if (RW1 == 0){
            stateCPU1 = 1;
        } else if (RW2 == 1) {
            stateCPU2 = 0;
        } else {
            stateCPU2 = 1;
        }
    } else if (stateCPU1 == 0) {
        if (RW2 == 1) {
            stateCPU1 = 3;
            missInv1++;
            stateCPU2 = 0;
        } else if (RW1 == 0) {
            stateCPU1 = 1;
        } else {}
    } else if (stateCPU1 == 1) {
        if (RW2 == 0 && stateCPU2 == 3) {
            stateCPU1 = 2;
            stateCPU2 = 2;
        } else if (RW2 == 1) {
            stateCPU1 = 0;
            stateCPU2 = 3;
            missInv2++;
        } else if (RW1 == 1) {
            stateCPU1 = 3;
            missInv1++;
            stateCPU2 = 0;
        } else {}
    } else if (stateCPU1 == 2) {
        if (RW1 == 1) {
            stateCPU1 = 0;
            stateCPU2 = 3;
            missInv2++;
        } else if (RW2 == 1) {
            stateCPU1 = 3;
            missInv1++;
            stateCPU2 = 0;
        } else {}
    } else if (stateCPU2 == 0) {
        if (RW1 == 1) {
            stateCPU2 = 3;
            missInv2++;
            stateCPU1 = 0;
        } else if (RW2 == 0) {
            stateCPU2 = 1;
        } else {}
    } else if (stateCPU2 == 1) {
        if (RW1 == 0 && stateCPU1 == 3) {
            stateCPU2 = 2;
            stateCPU1 = 2;
        } else if (RW1 == 1) {
            stateCPU2 = 0;
            stateCPU1 = 3;
            missInv1++;
        } else if (RW2 == 1) {
            stateCPU2 = 3;
            missInv2++;
            stateCPU1 = 0;
        } else {}
    } else if (stateCPU2 == 2) {
        if (RW2 == 1) {
            stateCPU2 = 0;
            stateCPU1 = 3;
            missInv1++;
        } else if (RW1 == 1) {
            stateCPU2 = 3;
            missInv2++;
            stateCPU1 = 0;
        } else {}
    }
    return 0;
}

// Checks L1 cache from CPU1 for a miss
int checkCacheL1CPU1() {
    if (RW1 == 0) {
        if (cacheL1CPU1[index][0] == 0) {
            missL1++;
            cacheL1CPU1[index][0] = tag;
        } else if (cacheL1CPU1[index][1] == 0) {
            missL1++;
            cacheL1CPU1[index][1] = tag;
        } else {
            if (cacheL1CPU1[index][0] != tag) {
                if (cacheL1CPU1[index][1] != tag) {
                    cacheL1CPU1[index][0] = tag;
                    missL1++;
                } else {
                    //cacheL1CPU1[index][1] = tag;
                }
            }
            
            else {
                //cacheL1CPU1[index][0] = tag;
            }
        }
    } else {
        if (cacheL1CPU1[index][0] == 0) {
            cacheL1CPU1[index][0] = tag;
        } else if (cacheL1CPU1[index][1] == 0){
            cacheL1CPU1[index][1] = tag;
        } else {
            if (indexAso1 == 0) {
                cacheL1CPU1[index][0] = tag;
                indexAso1 = 1;
            } else {
                cacheL1CPU1[index][1] = tag;
                indexAso1 = 0;
            }
        }
    }
    return 0;
}

// Checks L1 cache from CPU2 for a miss
int checkCacheL1CPU2() {
    if (RW2 == 0) {
        if (cacheL1CPU2[index][0] == 0) {
            missL2++;
            cacheL1CPU2[index][0] = tag;
        } else if (cacheL1CPU2[index][1] == 0) {
            missL2++;
            cacheL1CPU2[index][1] = tag;
        } else {
            if (cacheL1CPU2[index][0] != tag) {
                if (cacheL1CPU2[index][1] != tag) {
                    cacheL1CPU2[index][0] = tag;
                    missL2++;
                } else {
                    //cacheL1CPU2[index][1] = tag;
                }
            }
            
            else {
                //cacheL1CPU2[index][0] = tag;
            }
        }
    } else {
        if (cacheL1CPU2[index][0] == 0) {
            cacheL1CPU2[index][0] = tag;
        } else if (cacheL1CPU2[index][1] == 0){
            cacheL1CPU2[index][1] = tag;
        } else {
            if (indexAso2 == 0) {
                cacheL1CPU2[index][0] = tag;
                indexAso2 = 1;
            } else {
                cacheL1CPU2[index][1] = tag;
                indexAso2 = 0;
            }
        }
    }
    return 0;
}

// Check which CPU is getting the access 
int checkCPU() {
    if (access == 4) {
        checkCacheL1CPU1();
        totalL1++;
    } else {
        checkCacheL1CPU2();
        totalL2++;
    }
    return 0;
}

// Reads the file line per line
int readFile () { 
    string line;
    ifstream myfile ("trace.txt"); // You can change the trace here, needs to be a .txt
    if (myfile.is_open()) {
        while (getline(myfile,line)) {
            size_t last = 4; 
            size_t next = 4; 
            while ((next = line.find(delimiter, last)) != string::npos) { 
                setVars(line.substr(last, next-last));
                //printf("%d, %d\n",tag, index);
                last = next + 1; 
            } 
            
            while ((next = line.find(delimiter, last)) != string::npos) { 
                last = next + 1; 
            }
            
            setAccess(line.substr(last)); 
            //printf("%d\n",access);
            //printf("%d, %d, %d, %d\n",RW, tag, index, access);
            last = 2; 
            next = 2; 
            setRW(line.substr(2, 2));
            //printf("%d\n",RW);
            checkCPU();
            MESI();

        }
        myfile.close();
        rateL1 = missL1 / double(totalL1);
        rateL2 = missL2 / double(totalL2);
        //printf("Total de accesos: %d y %d\n", totalL1, totalL2);
        //printf("Total de misses: %d y %d\n", missL1, missL2);
        //printf("Miss rate: %.5lf y %.5lf\n", rateL1, rateL2);
        //printf("Miss rate por invalidez: %.5lf y %.5lf\n", double(missInv1 / double(totalL1 + totalL2)), double(missInv2 / double(totalL1 + totalL2)));
        printf("-------------------------------------------------\n");
        printf("Miss rate L1 CPU1:                       %.5lf\n", rateL1);
        printf("Miss rate L1 CPU2:                       %.5lf\n", rateL2);
        printf("Invalidaciones por coherencia CPU1:      %.5lf\n", double(missInv1 / double(totalL1 + totalL2)));
        printf("Invalidaciones por coherencia CPU2:      %.5lf\n", double(missInv2 / double(totalL1 + totalL2)));
        printf("-------------------------------------------------\n");
    }   
    else cout << "Error abriendo el archivo\n";
    return 0;   
}

// Main func
int main (int argc, char *argv[]) {
    readFile();
    return 0;
}

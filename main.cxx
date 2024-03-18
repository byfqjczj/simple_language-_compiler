// libc includes (available in both C and C++)
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
// Implementation includes
#include "slice.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;
//this initalizes the global variables, the stack is implemented by creating two large arrays that stores the PC of each stack and the localIt of each function call.
//the stack pointer is initialized to 0(SP), and everytime a function is added to the call stack, it gets incremented and vice versa.
//global it is done by reserving localIt[0] for the global variation of the it variable
    uint64_t localIt[10000];
    char * PCtoBranch[10000];
    int stackPointer = 0;
    SliceToIntHashMap* s_table;
    bool consume(const char*);
    void expression(bool);
    char const * program;
    char const * current;
    uint64_t returnRegister;
    bool statement(bool);
    bool isPreProcess = true;
    unordered_set<string> varM;
    int labelCounter = 1;
    int whileLabelCounter = 1;
    int sentinelCounter = 1;
    unordered_map<int,int> whileLayers;
    int currLayer = 0;
    unordered_map<int,int> ifLayers;
    int ifLayer = 0;
    unordered_map<int,int> functionLayers;
    int fCounter = 0;
    int shortCircuitCounter = 0;
    int moduloCounter = 0;
  void unused(uint64_t bruh)
    {
        bruh = bruh;
        return;
    }
    vector<string> uint64ToHexSegments(uint64_t value) {
    vector<string> segments;
    for (int i = 0; i < 4; ++i) {
        // Isolate 16 bits
        uint16_t part = (value >> (i * 16)) & 0xFFFF;
        
        // Convert to hexadecimal string
        stringstream stream;
        stream << "0x"
               << setfill('0') << setw(4) // 16 bits = 4 hex digits
               << hex << part;
        segments.push_back(stream.str());
    }
    
    return segments;
}

  void fail() {
    printf("failed at offset %ld\n",(size_t)(current-program));
    printf("%s\n",current);
    exit(1);
  }

  void end_or_fail() {
    //cout<<"here"<<endl;
    while (isspace(*current)) {
      current += 1;
    }
    if (*current != 0) fail();
  }

  void consume_or_fail(const char* str) {
    if (!consume(str)) {
      fail();
    }
  }

  void skip() {
      while (isspace(*current)) {
          current += 1;
      }
  }
  //this peeks at the next string, and returning true is the str that we want is peeked
  bool peek(const char* str) {
    char * temp = (char*)current;
    while(isspace(*temp)) {
        temp += 1;
    }
    size_t i = 0;
    while (true) {
      char const expected = str[i];
      char const found = temp[i];
      if (expected == 0) {
        /* survived to the end of the expected string */
            return true;  
        }
      if (expected != found) {
            return false;
        }
      // assertion: found != 0
      i += 1;
    }
  }
  bool consume(const char* str) {
    skip();
    //printf("%s\n",str);
    size_t i = 0;
    while (true) {
      char const expected = str[i];
      char const found = current[i];
      if (expected == 0) {
        /* survived to the end of the expected string */
        current += i;
        return true;
      }
      if (expected != found) {
        return false;
      }
      // assertion: found != 0
      i += 1;
    }
    
  }

  Slice* consume_identifier() {
    skip();

    if (isalpha(*current)) {
      char * start = (char*) current;
      do {
        current += 1;
      } while(isalnum(*current));
      Slice* ptr = (Slice*) malloc(sizeof(Slice));
      ptr->start=start;
      ptr->len=(size_t)(current-start);
      return ptr;
    } else {
      return NULL;
    }
  }

  uint64_t* consume_literal() {
    skip();
    if (isdigit(*current)) {
      uint64_t v = 0;
      do {
        v = 10*v + ((*current) - '0');
        current += 1;
      } while (isdigit(*current));
      uint64_t* ptr = (uint64_t*) malloc(sizeof(uint64_t));
      *ptr = v;
      return ptr;
    } else {
      return NULL;
    }
  }

    // The plan is to honor as many C operators as possible with
    // the same precedence and associativity
    // e<n> implements operators with precedence 'n' (smaller is higher)

    // () [] . -> ...
    
    //the logic here is that if effects is true, then we would actually evalute the logic. If it is false, we would just increment the PC until effects is true.

    void e1(bool effects) {
        //printf("%s\n","test");
        //the logic here checks for the case where IT is called.
        //honored by checking the stackPointer and then returning it from the localIt array
        /*
        if(peek("itFAKE"))
        {
            if(consume("itFAKE")) {
                if(effects)
                {
                    cout << "    SUB SP, SP, #16" << endl;
                    cout << "    MOVZ X7, #0" << endl;
                    cout << "    STR x7, [SP]"<< endl;
                    return;
                }
            }
            return;
        }
        */
        if(peek("it")&&!isalpha(*(current+3)))
        {
            if(consume("it")) {
                if(effects)
                {
                    cout << "    SUB SP, SP, #16" << endl;
                    cout << "    STR x11, [SP]"<< endl;
                }
                if(peek("("))
                {
                    while(consume("("))
                    {
                        expression(effects);
                        if(effects)
                        {
                            cout << "    LDR x7, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    LDR x12, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    SUB SP, SP, #16"<< endl;
                            cout << "    STR X11, [SP]" << endl;
                            cout << "    SUB SP, SP, #16"<< endl;
                            cout << "    STR X29, [SP]" << endl;
                            cout << "    SUB SP, SP, #16"<< endl;
                            cout << "    STR X30, [SP]" << endl;
                            cout << "    MOV X11, x7" << endl;
                            cout << "    BLR x12" << endl;
                            cout << "    LDR x7, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    LDR x30, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    LDR x29, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    LDR x11, [SP]"<< endl;
                            cout << "    ADD SP, SP, #16" << endl;
                            cout << "    SUB SP, SP, #16" << endl;
                            cout << "    STR x7, [SP]"<< endl;
                        }
                        consume(")");
                    }
                }
                return;
            }  
        }
        
        if (consume("fun")) {
            int temp = 0;
            if(effects)
            {
                fCounter++;
                temp = fCounter;
                //printf("    stp x29, x30, [sp, #-16]! \n");
                cout << "    b       .fEnd_"<< temp<<endl;
            }
            if(!consume("{"))
            {
                if(effects)
                {
                    cout << ".fBody_"<< temp<<":"<<endl;
                }
                statement(effects);
                if(effects)
                {
                    cout << "    mov x7, #0" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR X7, [SP]" << endl;
                    cout << "    ret" << endl;
                }

            }
            else
            {
                if(effects)
                {
                    cout << ".fBody_"<< temp<<":"<<endl;
                }

                while(!consume("}"))
                {
                    statement(effects);
                }
                if(effects)
                {
                    cout << "    mov x7, #0" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR X7, [SP]" << endl;
                    cout << "    ret" << endl;
                }
            }
            if(effects)
            {
                cout << ".fEnd_"<< temp << ":" <<endl;
                cout<< "     LDR x7, =.fBody_"<< temp << endl;
                cout << "    SUB SP, SP, #16"<< endl;
                cout << "    STR X7, [SP]" << endl;
                
            }
            if(peek("("))
            {
                while(consume("("))
                {
                    
                    expression(effects);
                    if(effects)
                    {
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x12, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X11, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X29, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X30, [SP]" << endl;
                        cout << "    MOV X11, x7" << endl;
                        cout << "    BLR x12" << endl;
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x30, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x29, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x11, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16" << endl;
                        cout << "    STR x7, [SP]"<< endl;
                    }
                    consume(")");
                }
            }
            return;
        }
        Slice* slicePtrTwo = consume_identifier();
        if (slicePtrTwo!=NULL) {
            if(isPreProcess==true)
            {
                string currStr = ".";
                for(int i = 0;(size_t)i<slicePtrTwo->len;i++)
                {
                    char c = slicePtrTwo->start[i];
                    currStr = currStr + c;
                }
                varM.insert(currStr);
            }
            if(effects)
            {
                //uint64_t v = sliceToIntHashMapGet(s_table,slicePtrTwo);
                string sliceStr = ".";
                for(int i = 0;(size_t)i<slicePtrTwo->len;i++)
                {
                    char c = slicePtrTwo->start[i];
                    sliceStr = sliceStr + c;
                }
                cout << "    LDR x9, " << sliceStr<< endl;
                cout << "    SUB SP, SP, #16"<< endl;
                cout << "    STR X9, [SP]" << endl;
                if(peek("("))
                {
                    while(consume("(")){
                        expression(effects);
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x12, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X11, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X29, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X30, [SP]" << endl;
                        cout << "    MOV X11, x7" << endl;
                        cout << "    BLR x12" << endl;
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x30, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x29, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x11, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16" << endl;
                        cout << "    STR x7, [SP]"<< endl;
                        consume(")");
                    }
                    return;
                }
                return;
            }
            else
            {
                if(peek("("))
                {
                    while(consume("("))
                    {
                        expression(effects);
                        consume(")");
                    }    
                }
                return;
            }   
        }
        uint64_t* cons_litPtr = consume_literal();
        if (cons_litPtr!=NULL) {
            uint64_t temp = *cons_litPtr;
            free(cons_litPtr);
            if(effects)
            {
                cout << "    SUB SP, SP, #16" << endl;
                vector<string> toMove = uint64ToHexSegments(temp);
                int counter = 0;
                for(const auto& segment : toMove)
                {
                    if(counter==0)
                    {
                        cout << "    MOVZ x9, " << segment << ", LSL 0"<< endl;
                    }
                    else
                    {
                        cout << "    MOVK x9, " << segment << ", LSL "<< counter<< endl;
                    }
                    counter +=16;
                }
                cout << "    STR x9, [SP]" << endl;
                return;
            }
            else
            {
                return;
            }
        }
        if (consume("(")) {
            expression(effects);
            consume(")");
            if(peek("("))
            {
                while(consume("("))
                {
                    if(effects)
                    {
                        cout << "    LDR x12, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                    }
                    expression(effects);
                    if(effects)
                    {
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X11, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X29, [SP]" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR X30, [SP]" << endl;
                        cout << "    MOV X11, x7" << endl;
                        cout << "    BLR x12" << endl;
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x30, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x29, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x11, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    SUB SP, SP, #16" << endl;
                        cout << "    STR x7, [SP]"<< endl;
                    }
                    consume(")");
                }
            }
            return;
        } 
        else {
            
            fail();
            return;
        }
        fail();
        return;
    }

    // ++ -- unary+ unary- ... (Right) deprecated
    void e2(bool effects) {
        e1(effects);
    }
    
    // * / % (Left)
    void e3(bool effects) {
        e2(effects);

        while (true) {
            if (consume("*")) {  
                e2(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    MUL x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }

            } else if (consume("/")) {
                    
                e2(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    UDIV x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }

            } else if (consume("%")) {
                moduloCounter++;
                int temp = moduloCounter;
                e2(effects);                
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;   
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    CBZ x8, .mLabel" << temp << endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    UDIV x9, x7, x8" << endl;
                    cout << "    MUL x10, x9, x8" << endl;
                    cout << "    SUB x9, x7, x10" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    MOV x8, x9"<< endl;
                    cout << ".mLabel" << temp << ":" << endl;
                    cout << "    STR x8, [SP]" << endl;

                }
                
            } else {
                return;
            }
        }
    }

    // (Left) + -
    void e4(bool effects) {
        e3(effects);
        //printf("%d\n",(int)v);
        while (true) {
            if (consume("+")) {
                e3(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    ADD x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }
                //printf("%d\n",(int)v);
            } else if (consume("-")) {
                e3(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    SUB x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }
                //printf("%d\n",(int)v);
            } else {
                return;
            }
        }
    }

    // << >> deprecated
    void e5(bool effects) {
        e4(effects);
    }

    //all the other arithematic/logical stuff is implemented the same was as the starter code.
    //a recursive call to the left, and then a recursive call to the right.

    // < <= > >=
    void e6(bool effects) {
        e5(effects);
        while(true) {
            if(consume("<=")) {
                {
                    e5(effects);
                    if(effects)
                    {
                        cout << "    LDR x8, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    CMP x7, x8" << endl;
                        cout << "    MOV X8, #1" << endl;
                        cout << "    CSEL x7, x8, xzr, LS" << endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR x7, [SP]" << endl;
                        // cout << "    BGT .L" << labelCounter<< endl;
                        // cout << "    MOV x7, 1"<<endl;
                        // cout << "    SUB SP, SP, #16"<< endl;
                        // cout << "    STR x7, [SP]" << endl;
                        // cout << "    B    .L" << labelCounter+1<<endl;
                        // cout << ".L"<<labelCounter<<":"<<endl;
                        // labelCounter++;
                        // cout << "    MOV x7, 0"<<endl;
                        // cout << "    SUB SP, SP, #16"<< endl;
                        // cout << "    STR x7, [SP]" << endl;
                        // cout << ".L"<<labelCounter<<":"<<endl;
                        // labelCounter++;
                    }
                }
            }
            else if(consume("<"))  {
                e5(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, x8" << endl;
                    cout << "    MOV X8, #1" << endl;
                    cout << "    CSEL x7, x8, xzr, LO" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    // cout << "    BGE .L" << labelCounter<< endl;
                    // cout << "    MOV x7, 1"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << "    B    .L" << labelCounter+1<<endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                    // cout << "    MOV x7, 0"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                }
            }
            else if(consume(">=")) {
                e5(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, x8" << endl;
                    cout << "    MOV X8, #1" << endl;
                    cout << "    CSEL x7, x8, xzr, HS" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    // cout << "    BLT .L" << labelCounter<< endl;
                    // cout << "    MOV x7, 1"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << "    B    .L" << labelCounter+1<<endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                    // cout << "    MOV x7, 0"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                }
            }
            else if(consume(">"))  {
                e5(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, x8" << endl;
                    cout << "    MOV X8, #1" << endl;
                    cout << "    CSEL x7, x8, xzr, HI" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    // cout << "    BLE .L" << labelCounter<< endl;
                    // cout << "    MOV x7, 1"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << "    B    .L" << labelCounter+1<<endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                    // cout << "    MOV x7, 0"<<endl;
                    // cout << "    SUB SP, SP, #16"<< endl;
                    // cout << "    STR x7, [SP]" << endl;
                    // cout << ".L"<<labelCounter<<":"<<endl;
                    // labelCounter++;
                }
            }
            else
            {
                return;
            }
        }
    }

    // == !=
    void e7(bool effects) {
        e6(effects);
        while(true) {
            if(consume("==")) {
                {
                    e6(effects);
                    if(effects)
                    {
                        cout << "    LDR x8, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    LDR x7, [SP]"<< endl;
                        cout << "    ADD SP, SP, #16" << endl;
                        cout << "    CMP x7, x8" << endl;
                        cout << "    BNE .L" << labelCounter<< endl;
                        cout << "    MOV x7, 1"<<endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR x7, [SP]" << endl;
                        cout << "    B    .L" << labelCounter+1<<endl;
                        cout << ".L"<<labelCounter<<":"<<endl;
                        labelCounter++;
                        cout << "    MOV x7, 0"<<endl;
                        cout << "    SUB SP, SP, #16"<< endl;
                        cout << "    STR x7, [SP]" << endl;
                        cout << ".L"<<labelCounter<<":"<<endl;
                        labelCounter++;
                    }
                }
            }
            else if(consume("!="))  {
                e6(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, x8" << endl;
                    cout << "    BEQ .L" << labelCounter<< endl;
                    cout << "    MOV x7, 1"<<endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    cout << "    B    .L" << labelCounter+1<<endl;
                    cout << ".L"<<labelCounter<<":"<<endl;
                    labelCounter++;
                    cout << "    MOV x7, 0"<<endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    cout << ".L"<<labelCounter<<":"<<endl;
                    labelCounter++;
                }
            }
            else
            {
                return;
            }
        }
    }
    // (left) &
    void e8(bool effects) {
        e7(effects);
        while(true)
        {   
            if(peek("&&"))
            {
                //cout << "true" << endl;
                return;
            }
            if(consume("&"))
            {
                e7(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    AND x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }

            }
            else{
                return;
            }
        }
    }

    // ^
    void e9(bool effects) {
        e8(effects);
        while(true)
        {
            if(consume("^"))
            {
                e8(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    EQR x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }
            }
            else{
                return;
            }
        }
    }

    // |
    void e10(bool effects) {
        e9(effects);
        while(true)
        {   
            if(peek("||"))
            {
                return;
            }
            if(consume("|"))
            {
                e9(effects);
                if(effects)
                {
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    ORR x7, x7, x8" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                }

            }
            else{
                return;
            }
        }
    }
    //short circuiting is done through an if statement, which would render the other half to have a false effect(only increment PC).
    // &&
    void e11(bool effects) {
        int temp = 0;
        int flag = 0;
        if(effects)
        {
            shortCircuitCounter++;
            temp = shortCircuitCounter;
        }
        e10(effects);
        if(effects&&peek("&&"))
        {
            cout << "    LDR x7, [SP]"<< endl;
            cout << "    ADD SP, SP, #16" << endl;
            cout << "    CMP x7, #0" << endl;
            cout << "    CSET x7, NE" << endl;
            cout << "    SUB SP, SP, #16"<< endl;
            cout << "    STR x7, [SP]" << endl;
            cout << "    CBZ x7, .shortCircuitEnd" << temp <<endl;
            flag = 1;
        }
        while(true)
        {
            if(consume("&&"))
            {
                e10(effects);
                if(effects)
                {
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, #0" << endl;
                    cout << "    CSET x7, NE" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    cout << "    CBZ x7, .shortCircuitEnd" << temp <<endl;
                }
            }
            else{
                if(effects&&flag==1)
                {
                    cout << ".shortCircuitEnd" << temp << ":" <<endl;
                }
                return;
            }
        }
    }
    // ||
    void e12(bool effects) {
        int temp = 0;
        int flag = 0;
        if(effects)
        {
            shortCircuitCounter++;
            temp = shortCircuitCounter;
        }
        e11(effects);
        if(effects&&peek("||"))
        {
            cout << "    LDR x7, [SP]"<< endl;
            cout << "    ADD SP, SP, #16" << endl;
            cout << "    CMP x7, #0" << endl;
            cout << "    CSET x7, NE" << endl;
            cout << "    SUB SP, SP, #16"<< endl;
            cout << "    STR x7, [SP]" << endl;
            cout << "    CBNZ x7, .shortCircuitEnd" << temp <<endl;
            flag = 1;
        }
        while(true)
        {
            if(consume("||"))
            {
                e11(effects);
                if(effects)
                {
                    cout << "    LDR x7, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    CMP x7, #0" << endl;
                    cout << "    CSET x7, NE" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x7, [SP]" << endl;
                    cout << "    CBNZ x7, .shortCircuitEnd" << temp <<endl;
                }
            }
            else{
                if(effects&&flag==1)
                {
                    cout << ".shortCircuitEnd" << temp << ":" <<endl;
                }
                return;
            }
        }
    }

    // (right with special treatment for middle expression) ?: deprecated
    void e13(bool effects) {
        e12(effects);
    }

    // = += -= ... deprecated
    void e14(bool effects) {
        e13(effects);
    }

    // ,
    void e15(bool effects) {
        e14(effects);
        while(true)
        {
            if(consume(","))
            {
                if(effects)
                {
                    cout << "    ADD SP, SP, #16" << endl;
                }
                e14(effects);
            }
            else{
                return;
            }
        }
    }

    void expression(bool effects) {
        e15(effects);
    }

    bool statement(bool effects) {

        //this deals with it as the start of a statement(aka a varaible), essentially implemented by just treating it as variable.
        if(peek("it")&&!isalpha(*(current+3)))
        {
            if(consume("it"))
        {
            if(consume("="))
            {
                expression(effects);
                if(effects)
                {
                    cout << "    LDR x11, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                }
                return true;
            }
            else
            {
                fail();
                return false;
            }
        }
        }
        if(consume("return")) {
            expression(effects);
            if(effects)
            {
                cout << "    ret" << endl;
            }
            return true;
        }
        if (consume("print")) {

            // print ...
            expression(effects);
            if (effects) {
                    printf("    LDR x0, =.$fmt\n");
                    cout << "    LDR x1, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    SUB SP, SP, #16"<< endl;
                    cout << "    STR x11, [SP]" << endl;
                    printf("    stp x29, x30, [sp, #-16]! \n");
                    printf("    BL printf\n");
                    printf("    ldp x29, x30, [sp], #16\n");
                    cout << "    LDR x11, [SP]" << endl;
                    cout << "    ADD SP, SP, #16" << endl;
            }
            return true;
        }
        //the while block here keeps a pointer to the start, and then constantly jumps to it until the logic fails
        if (consume("while"))
        {
            int temp = 0;
            if(effects)
            {
                currLayer = currLayer + 2;
                temp = currLayer;
                cout << ".wL_"<< temp << ":"<<endl;
            }
            expression(effects);
            if(effects)
            {
                cout << "    LDR x7, [SP]"<< endl;
                cout << "    ADD SP, SP, #16" << endl;
                cout << "    MOV x8, 1"<<endl;
                cout << "    CMP x7, x8"<<endl;
                cout << "    B.NE .wL_"<< temp + 1<<endl;
            }
            
            if(!consume("{"))
                {
                    statement(effects);
                }
            else
                {
                    while(!consume("}"))
                    {
                        statement(effects);
                    }
                }
            if(effects)
                {
                    cout << "    b       .wL_"<< temp << endl;
                    cout << ".wL_"<< temp + 1<<":"<<endl;
                }

            return true;
        }
        //the if block would have effects treated as "true" if the logic inside is evaluated to be true, and vice verse.
        //else is implemented the same way
        if (consume("if")) 
        {
            int temp = 0;
            if(effects)
            {
                ifLayer = ifLayer + 2;
                temp = ifLayer;
            }
            expression(effects);
            if(effects)
            {
                cout << "    LDR x7, [SP]"<< endl;
                cout << "    ADD SP, SP, #16" << endl;
                cout << "    MOV x8, 0"<<endl;
                cout << "    CMP x7, x8"<<endl;
                cout << "    B.EQ .iL_"<< temp <<endl;
            }
    
            if(!consume("{"))
            {
                statement(effects);
                if(effects)
                {
                    cout << "    b       .iL_"<< temp + 1<<endl;
                }
            }
            else
            {
                
                while(!consume("}"))
                {   
                    statement(effects);       
                }
                if(effects)
                {
                    cout << "    b       .iL_"<< temp + 1<<endl;
                }
            }
            if(effects)
            {
                cout << ".iL_"<< temp <<":"<<endl;
            }
            if(consume("else"))
            {
                if(!consume("{"))
                {
                    statement(effects);
                    
                }
                else
                {
                    while(!consume("}"))
                    {
                        statement(effects);
                    }
                    
                }
            }
            if(effects)
            {
                cout << ".iL_"<< temp + 1<<":"<<endl;
            }
            return true;
        }
        Slice* slicePtrOne = consume_identifier();
        if (slicePtrOne!=NULL) {
            if(isPreProcess==true)
            {
                string currStr = ".";
                for(int i = 0;(size_t)i<slicePtrOne->len;i++)
                {
                    char c = slicePtrOne->start[i];
                    currStr = currStr + c;
                }
                varM.insert(currStr);
            }
            // x = ...
            if (consume("=")) {
                //uint64_t v = expression(effects);
                expression(effects);
                if (effects) {
                    string currStr = ".";
                    for(int i = 0;(size_t)i<slicePtrOne->len;i++)
                    {
                        char c = slicePtrOne->start[i];
                        currStr = currStr + c;
                    }
                    
                    cout << "    ADRP x7, " << currStr<< endl;
                    cout << "    ADD x7, x7, :lo12:" << currStr << endl;
                    cout << "    LDR x8, [SP]"<< endl;
                    cout << "    ADD SP, SP, #16" << endl;
                    cout << "    STR x8, [x7]" << endl;
                }
                return true;
            } else {
                fail();
            }
        }
        return false;
    }

    void statements(bool effects) {
        while (statement(effects)){

        }
    }

    void run() {
        statements(true);
        end_or_fail();
    }
    
    void preprocess() {
        statements(false);
        end_or_fail();
    }

int main(int argc, const char *const *const argv) {
    if (argc != 2) {
        fprintf(stderr,"usage: %s <file name>\n",argv[0]);
        exit(1);
    }

    // open the file
    int fd = open(argv[1],O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // determine its size (std::filesystem::get_size?)
    struct stat file_stats;
    int rc = fstat(fd,&file_stats);
    if (rc != 0) {
        perror("fstat");
        exit(1);
    }

    // map the file in my address space
    char const* prog = (char const *)mmap(
        0,
        file_stats.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0);
    if (prog == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    program = prog;
    current =  prog;
    s_table = createSliceToIntHashMap(1000);
    printf(".extern printf\n");
    printf(".section .data\n");
    preprocess();
    isPreProcess = false;
    current = prog;
    printf(".$fmt:\n");
    printf("    .string \"%%ld\\n\"\n");
    printf(".align 8\n");
    printf(".argc:\n");
    printf("    .quad 0\n");
    for (auto itr = varM.begin(); itr != varM.end(); ++itr) {
    if((*itr) != "it" && (*itr) != ".argc")
        {    
            printf(".align 8\n");
            cout<<(*itr)<<":"<<endl;
            printf("    .quad 0\n");
        }
    }
    printf(".text\n");
    printf(".global main\n");
    printf("main:\n");
    cout << "    ADRP X1, .argc" << endl;
    cout << "    ADD X1, X1, :lo12:.argc" << endl;
    cout << "    STR x0, [X1]"<<endl;
    cout << "    MOV x11, #0" <<endl;
    run();
    printf("    ret // we're out of here\n");
    return 0;
}

// vim: tabstop=4 shiftwidth=2 expandtab
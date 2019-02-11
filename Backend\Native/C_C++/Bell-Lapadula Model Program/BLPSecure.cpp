//
//  main.cpp
//  Project1
//
//  Created by Lee Robertson on 10/13/18.
//  Copyright © 2018 Lee Robertson. All rights reserved.
//

#include "referencemonitor.cpp"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
using namespace std;

vector<string> explode(string const &s, char delim);

int main(int argc, const char **argv) {
    FILE *fp;
    int linesread = 0;
    char line[100];
    vector<string> currentline;
    
    rm *reference = 0;
    
    reference = (rm *)calloc(sizeof(rm), 1);
    
    if(argc > 2){
        cout << "*** ERROR *** Excess arguments given...\n";
        return 0;
    }else if(argc == 1){
        cout << "*** ERROR *** Not enough arguments given...\n";
        return 0;
    }
    fp = fopen(argv[1], "r");
    //fp = fopen("instrList", "r");
    
    if(!fp){
        //cout << "*** ERROR *** cannot read %s. Terminating..." << endl;
        cout << "*** ERROR *** cannot read file. Terminating..." << endl;
        return 0;
    } 
    
    while(fgets(line, 100, fp) != NULL){
        linesread++;
        
        if(linesread > 10){
            reference->printcurrent();
            linesread = 0;
        }
        currentline = explode(line, ' ');
        
        //cout << "current line : " << line << endl << endl;
        if(currentline.size() == 3){
            if(currentline[0] == "addsub"){
                 //cout << "adding sub...\n";
                if((currentline[1] != "") && (currentline[2] != "")){
                    currentline[1].erase(remove(currentline[1].begin(), currentline[1].end(), '\n'), currentline[1].end());
                    currentline[2].erase(remove(currentline[2].begin(), currentline[2].end(), '\n'), currentline[2].end());
                    //cout << "sub name.. " << currentline[1] << endl;
                    //cout << "security level " << currentline[2] << endl << endl;
                    
                    reference->addsub(currentline[1], currentline[2]);
                    
                    continue;
                }
            }else if (currentline[0] == "addobj"){
                //cout << "adding object...\n";
                if((currentline[1] != "") && (currentline[2] != "")){
                    currentline[1].erase(remove(currentline[1].begin(), currentline[1].end(), '\n'), currentline[1].end());
                    currentline[2].erase(remove(currentline[2].begin(), currentline[2].end(), '\n'), currentline[2].end());
                    //cout << "obj name.. " << currentline[1] << endl;
                    //cout << "security level " << currentline [2] << endl << endl;
                    
                    reference->addobj(currentline[1], currentline[2]);
                    
                    continue;
                }
            }else if (currentline[0] == "read"){
                if((currentline[1] != "") && (currentline[2] != "")){
                    currentline[1].erase(remove(currentline[1].begin(), currentline[1].end(), '\n'), currentline[1].end());
                    currentline[2].erase(remove(currentline[2].begin(), currentline[2].end(), '\n'), currentline[2].end());
                    //cout << "obj name.. " << currentline[1] << endl;
                    //cout << "security level " << currentline [2] << endl << endl;
                    
                    reference->read_obj(currentline[1], currentline[2]);
                    
                    continue;
                }
            }else{
                cout << "Bad Instruction : " << line << endl;
                continue;
            }
        }else if(currentline.size() == 4){
            //int last = 0;
            if(currentline[0] == "write"){
                if((currentline[1] != "") && (currentline[2] != "") && (currentline.back() != "")){
                    //for(int i = 0; i < currentline.size(); i++) last = i;
                    currentline[1].erase(remove(currentline[1].begin(), currentline[1].end(), '\n'), currentline[1].end());
                    currentline[2].erase(remove(currentline[2].begin(), currentline[2].end(), '\n'), currentline[2].end());
                    currentline[3].erase(remove(currentline[3].begin(), currentline[3].end(), '\n'), currentline[3].end());
                    
                    //cout << "obj name.. " << currentline[1] << endl;
                    //cout << "file name.. " << currentline[2] << endl;
                    try{
                        //cout << "write value.. " << stoi(currentline[last]) << endl << endl;
                        reference->write_obj(currentline[1], currentline[2], stoi(currentline[3]));
                    }catch(invalid_argument& e){
                        cout << "Bad Instruction : " << line << endl;
                    }
                    continue;
                }
            }else{
                cout << "Bad Instruction : " << line << endl;
                continue;
            }
        }else{
            cout << "Bad Instruction : " << line << endl;
        }
    }
    
    return 0;
}

vector<string> explode(string const &s, char delim){
    vector<string> result;
    istringstream iss(s);
    
    for (string token; getline(iss, token, delim); )
    {
        result.push_back(move(token));
    }
    
    return result;
}

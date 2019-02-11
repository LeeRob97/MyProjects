//
//  referencemonitor.cpp
//  Project1
//
//  Created by Lee Robertson on 10/14/18.
//  Copyright © 2018 Lee Robertson. All rights reserved.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include "users.cpp"
#include "userobject.cpp"
using namespace std;

typedef class referencemonitor {
    public:
    vector<users> user;
    vector<userobject> uo;

    void addsub(string name, string level){
        users user(name, level);
        this->user.push_back(user);
        
         cout << "Subject Added : addsub " << name << " " << level << endl;
    }

    void addobj(string name, string level){
        userobject obj(name, level);
        this->uo.push_back(obj);
        
        cout << "Object Added : addobj " << name << " " << level << endl;
    }
    
    void read_obj(const string name, const string obj){
        for(int i = 0; i < this->user.size(); i++){
            if(name == this->user[i].username){ //we have a valid subject
                for(int j = 0; j < this->uo.size(); j++){
                    if(obj == this->uo[j].username){ //we have a valid object
                        //cout << "Comparing : " << this->user[i].username << " and " << this->uo[j].username << endl;
                        //cout << "ACCESSLEVEL:  " << this->user[i].accesslevel << " and " << this->uo[j].accesslevel << endl;
                        
                        
                        if(((this->user[i].accesslevel == "HIGH") && (this->uo[j].accesslevel == "HIGH")) ||
                            ((this->user[i].accesslevel == "HIGH") && (this->uo[j].accesslevel == "MEDIUM"))||
                            ((this->user[i].accesslevel == "HIGH") && (this->uo[j].accesslevel == "LOW"))){
                            this->user[i].temp = this->uo[j].temp;
        
                            cout << "Access GRANTED : " << name << " reads " << obj << endl;
                            return;
                        }else if(((this->user[i].accesslevel == "MEDIUM") && (this->uo[j].accesslevel == "MEDIUM")) ||
                                 ((this->user[i].accesslevel == "MEDIUM") && (this->uo[j].accesslevel == "LOW"))){
                            this->user[i].temp = this->uo[j].temp;
                            cout << "Access GRANTED : " << name << " reads " << obj << endl;
                            return;
                        }else if((this->user[i].accesslevel == "LOW") && (this->uo[j].accesslevel == "LOW")){
                            this->user[i].temp = this->uo[j].temp;
                            cout << "Access GRANTED : " << name << " reads " << obj << endl;
                            return;
                        }else{
                            cout << "Access DENIED : read " << name << " " << obj << endl;
                            return;
                        }
                    }
                }
            }
        }
        cout << "Bad Instruction : read " << name << " " << obj << endl;
    }
    
    void write_obj(string name, string obj, int val){
        for(int i = 0; i < this->user.size(); i++){
            if(name == this->user[i].username){ //we have a valid subject
                for(int j = 0; j < this->uo.size(); j++){
                    if(obj == this->uo[j].username){ //we have a valid object
                        if((this->user[i].accesslevel == "HIGH") && (this->uo[j].accesslevel == "HIGH")){
                            this->uo[j].temp = val;
                            cout << "Access GRANTED : " << name << " writes value " << val << " to " << obj << endl;
                            return;
                        }else if(((this->user[i].accesslevel == "MEDIUM") && (this->uo[j].accesslevel == "HIGH")) ||
                                 ((this->user[i].accesslevel == "MEDIUM") && (this->uo[j].accesslevel == "MEDIUM"))){
                            this->uo[j].temp = val;
                             cout << "Access GRANTED : " << name << " writes value " << val << " to " << obj << endl;
                            return;
                        }else if(((this->user[i].accesslevel == "LOW") && (this->uo[j].accesslevel == "HIGH")) ||
                                 ((this->user[i].accesslevel == "LOW") && (this->uo[j].accesslevel == "MEDIUM"))||
                                 ((this->user[i].accesslevel == "LOW") && (this->uo[j].accesslevel == "LOW"))){
                            this->uo[j].temp = val;
                            cout << "Access GRANTED : " << name << " writes value " << val << " to " << obj << endl;
                            return;
                        }else{
                            cout << "Access DENIED : WRITE " << name << " " << obj << " " << val << endl;
                            return;
                        }
                    }
                }
            }
        }
        cout << "Bad Instruction : write " << name << " " << obj << " " << val << endl;
    }
    
    void printcurrent(){
        cout << "+---current state---+\n";
        cout << "|-subject-|---temp--|\n";
        for(int i = 0; i < this->user.size(); i++){
            cout <<"| " << this->user[i].username <<" |   " << this->user[i].temp << "    |\n";
        }
        cout << "|--object-|--value--|\n";
        for(int i = 0; i < this->uo.size(); i++){
            cout <<"| " << this->uo[i].username <<" |   " << this->uo[i].temp << "    |\n";
        }
        cout << "+-------------------+\n";
    }
    
    referencemonitor(){
        this->user.clear();
        this->uo.clear();
    }
} rm;

/*
    NAME: LEE ROBERTSON
    CLASS: CSCE 4550.002
    PROGRAM: Project2.cpp
*/

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <bitset>
#include <iomanip>
using namespace std;

/* GLOBAL DEFINED VARIABLES */
const string CHARACTERS= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int COLUMNS = 4;

struct paddedmatrix;
typedef struct paddedmatrix{ //structure is used to create an reg_array of structures based on the amount of 16 letter blocks
    /* These arrays are used to store and mix hex values */
	int reg_array[4][4]; 
	int mix_array[4][4]; 

    /* This array is used to store our mixture of hex values */
	string string_array[4][4]; 
} PMATRIX;

/* Function Prototypes */

/* File functions */
int getfilesize(char *filename);

/* String Functions */
void stripstring(char *string, char *retval);
char *subtext(char *pre_proc, char *key);

/* Bit-Wise Functions */
unsigned char bitoperation(string head, int prodval);
void createpadding(char *subval, int padval, FILE *fp); 

/* Program entry point  */
int main(){
    FILE *fp_plain, *fp_key, *fp_output; // File pointer used to open and close files.
    char filename[30], outname[30], *newptxt = 0, *ptxt = 0, *keyvalue = 0, *subval = 0;
    char *key = 0;
    string newsubval;
    int filesize = 0, encodelength = 0, mn = 0, padval = 0;
    size_t result = 0, sublength = 0;

    /* Open Files */

    /**** Here we have opened and tried to read from the given file ****/
    //Get data from plain text file
    memset(filename, 0, 30);
    cout << "Enter the name of the input plaintext file: ";
    cin >> filename;
    fp_plain = fopen(filename, "r");
    if(!fp_plain){
        cout << "** ERROR ** There is an issue with file: " << filename << endl;
        //fclose(fp_plain);
        return 0;
    }
    filesize = getfilesize(filename);
    if(filesize <= 0){
        cout << "*** ERROR *** FILE SIZE IS: 0...\n\n";
        return 0;
    }
    ptxt = (char *)calloc(filesize, 1); //allocate memory to pull from given file
    newptxt = (char *)calloc(filesize, 1); //allocate memory for stripped string
    result = fread(ptxt, 1, filesize, fp_plain); //store contents
    memset(filename, 0, 30);

    //Get data from our key file
    filesize = 0; //reset our current file size variables
    cout << "Enter the name of the input key file: ";
    cin >> filename;
    fp_key = fopen(filename, "r");
    if(!fp_key){
        cout << "** ERROR ** There is an issue with file: " << filename << endl;
        //fclose(fp_key);
        return 0;
    }
    filesize = getfilesize(filename);
    if(filesize <= 0){
        cout << "*** ERROR *** FILE SIZE IS: 0...\n\n";
        return 0;
    }
    keyvalue = (char *)calloc(filesize, 1); //allocate memory for 
    fread(keyvalue, 1, filesize, fp_key); //store key to our keyvalue variable
    memset(filename, 0, 30);

    //Get our file name so we can write to it.
    cout << "Enter the name of the output ciphertext file: ";
    cin >> filename;
    fp_output = fopen(filename, "w");
    if(!fp_output){
        cout << "** ERROR ** There is an issue with file: " << filename << endl;
        fclose(fp_output);
        return 0;
    }
    /*********************************************************************/


    /* Here we begin the program */
    
    /* HERE WE PRE-PROCESS our given */

    //newptxt = stripstring(ptxt); //function call
    stripstring(ptxt, newptxt); //function call
    cout << "\nPreprocessed:\n" << newptxt << "\n\n"; //output to screen
    fprintf(fp_output, "Preprocessed:\n%s\n\n", newptxt); //output to file
    
    /*********************************/

    /* HERE WE do our substution process */

    subval = subtext(newptxt, keyvalue);
    sublength = strlen(subval);
    cout << "Substitution:\n" << subval << "\n\n";
    fprintf(fp_output, "Substitution:\n%s\n\n", subval);

    /*************************************/

    /* HERE is when we create our 4 x 4 matrics from the encoded string */
    
    //the length of the substituted string is checked, depending on the length and if divisible by 16, A's will be appended to the end of the string
	mn = 16 - (sublength % 16);

    newsubval = subval;
    if(newsubval.length() == sublength){
        for (int i = 0; i < mn; i++){ //for loop appends A's as padding to make string length divisible by 16 for 4x4 blocks
            newsubval += "A"; //maybe an issue;
        }
        subval = (char *)newsubval.c_str();
        sublength = strlen(subval);
        padval = sublength / 16; //padval is the amount of 4x4 blocks in the encryption
    }else{
        cout << "**** ALERT **** main(): newsubval is NOT equal to current subval\n\n";
        for (int i = 0; i < mn; i++){ //for loop appends A's as padding to make string length divisible by 16 for 4x4 blocks
            newsubval += "A"; //maybe an issue;
        }
        subval = (char *)newsubval.c_str();
        sublength = strlen(subval);
        padval = sublength / 16; //padval is the amount of 4x4 blocks in the encryption
    }

    /* Here is our entry point for the actual padding creation */
	createpadding(subval, padval, fp_output);

    /*************************************/

    /* End of program  */
    /* close/free our pointers  */
    fclose(fp_plain);
    fclose(fp_key);
    fclose(fp_output);
    free(ptxt);
    free(newptxt);
    free(keyvalue);
    return 0;
}

/* Function Definitions */
/* 
    DESC: This function gets the file size
    Parameters: Char * | the filename 
    Return: an integer of the size of the given file 
*/
int getfilesize(char *filename){
    int fsize = 0;
    FILE *tempfp = 0;

    if(!filename){
        cout << " **** ERROR ****: Invaild filename..\n\n";
        return 0;
    }
    tempfp = fopen(filename, "rb");
    fseek(tempfp, 0, SEEK_END);
    fsize = ftell(tempfp);
    fclose(tempfp);
    return fsize;
}

/* 
    DESC: following function remove ALL special characters from the string collected from our input file 
    Parameters: Char * | needs to be stripped AND the address of a char * to place new string into
    Return: We dont return we just copy a new value to the given address
*/
void stripstring(char *temp_bit, char *retval){
    string modstring;
    /* Validate the string given is not null to prevent crash */
    if(!temp_bit){
        cout << "**** ERROR **** stripstring(): NULL STRING GIVEN\n\n";
        return;
    }
    modstring = temp_bit;  

      
    if(/* Verify string matches our c-string */ 
        modstring.length() == strlen(temp_bit)){
        boost::erase_all(modstring, "\r");
        boost::erase_all(modstring, "\t");
        boost::erase_all(modstring, "\n");
        boost::erase_all(modstring, " ");
        boost::erase_all(modstring, "!");
        boost::erase_all(modstring, ".");
        boost::erase_all(modstring, "?");
        boost::erase_all(modstring, "\"");
        boost::erase_all(modstring, "(");
        boost::erase_all(modstring, ")");
        boost::erase_all(modstring, "-");
        boost::erase_all(modstring, ";");
        boost::erase_all(modstring, ",");
        boost::erase_all(modstring, "'");
    }

	strcpy(retval, (char *)modstring.c_str()); //returns the preprocessed string
}

/* 
    DESC: this function uses the vigenere cipher to encode our string
    Parameters: Char * | contains our processed string AND CHAR * | our key that is used to base our encoding
    Return: A new string that does not include any special characters or spaces
*/
char *subtext(char *processed, char *key){
    /***** STRING VARIABLES *****/
	string encodedstr; // string to return
    string keyvalue; //convert our char * to string
    string pp;

    /****** INT VARIABLES ******/
    unsigned int i = 0, a = 0, b = 0, ind = 0;

    if(!processed){
        cout << "**** ERROR **** subtext(): NULL *processed* STRING GIVEN\n\n";
        return processed;
    }
    if(!key){
        cout << "**** ERROR **** subtext(): NULL *key* STRING GIVEN\n\n";
        return key;
    }

    /* HERE OUR STRINGS HAVE BEEN VALIDATED */
    //convert our char * to string
    keyvalue = key;
    pp = processed;

    /**** HERE we begin to encode our string ****/
	for(char &charaddr : pp) { //s
		if (/*if the key has been used 16 times to encrypt */ 
            i == 16){ 
			i = 0;
		}

        /* Search for the indexes of our letters */
		a = CHARACTERS.find(charaddr); 
		b = CHARACTERS.find(keyvalue[i]);

        /* ind is used to keep track of where our letter is in the string */
		ind = ((a + b) % 26); 

		encodedstr += CHARACTERS[ind]; //the new string has the letter at the indexed letter
		i++;
	}
	return (char *)encodedstr.c_str(); //return the encoded string as char *
}

/* 
    DESC: function returns an unsigned character based off of bitoperation based on multpilcation number, and does bitwise operations to carry out operations
    Parameters: String | used to determine the "exclusive or" bit AND int | to determine our shift
    Return: Our new bit
*/
unsigned char bitoperation(string head, int prodval){
    /* unsigned chars */
	unsigned char one = 0x1; //unsigned character one is used for left shift
	unsigned char ts = 0x1b; //00011011 is used for exclusive or if there is a leading one

    /* Integers variables */
    unsigned int temp = 0;
    unsigned int retval = 0;
	int need_drop = 0;

    /* Validate our header value to prevent crashing */
    if(head.length() == 0){
        cout << " **** ERROR **** NULL START VAL\n\n";
        return retval;
    }

	if (/* The first value is one, set the need_drop bit to 1 for the xor operation occurs */
        head[0] == '1'){ 
		need_drop = 1;
	}

    /* Here we change the string to an integer */
	retval = stoi(head, nullptr, 2);

    switch (prodval){
        case 2:
            retval <<= one; //Here move our bit by one
            if (/*if xor is true, then xor by twenty seven */
                need_drop == 1){ 
                retval ^= ts;
            }
            break;
        case 3:
            temp = retval; //unsigned int holds temp, a cop of the starting value
            retval <<= one; //left shift by 1
            retval ^= temp; //exclusing or by itself
            if (need_drop == 1){ //if xor is true, then xor by 27
                retval ^= ts; 
            }
            break;
        default:
            break;
    }
	return retval;
}


/* 
    DESC: createpadding function carries out padding, mix columns, parity bit, and mixing rows
    Parameters: Char * | contains our processed string AND INT | PADDING VALUE AND FILE * | So we can write to our current file
    Return: Nothing
*/
void createpadding(char *subval, int padval, FILE *fp){
    string sub(subval);
	string temp;
    string temp_bit; //temp_bit 8 bit representation

	int p_bit = 0; //p_bit used to keep track of ones
    int counter = 0; //keeps track of each letter that is assigned
    int hexint = 0; //variable is used to show our int representation of the hex value
    int colnum = 0;

    /* loop values */
    int i = 0, j = 0, z = 0;

    /* reg_array of our structs based on the amount of padval */
    PMATRIX list[padval];

    if(!fp){
        cout << "*** ERROR *** createpadding(): ERROR FILE..\n\n";
        return;
    }

    /*  ENTRY POINT FOR CREATING ASSIGNMENTS   */
	for(i = 0; i < padval; i++){ 
		for (j = 0; j < COLUMNS; j++){ 
			for (z = 0; z < COLUMNS; z++){
				list[i].reg_array[j][z] = sub[counter]; //each reg_array has a letter from our encoded string
				counter++; //increment so we can move to our next character to assign
			}
		}
	}
    /******************************************/

    /*  ENTRY POINT FOR CREATING OUR PADDING   */
	cout << "Padding:\n";
    fprintf(fp, "Padding:\n");
    for(i = 0; i < padval; i++){ //runs for the amount of 4x4 vectors
        for (j = 0; j < COLUMNS; j++){//runs for [j][z] in two d reg_array
            for (z = 0; z < COLUMNS; z++){
                cout << static_cast<char>(list[i].reg_array[j][z]); //prints all the padding arrays, casting to character
                fprintf(fp, "%c", static_cast<char>(list[i].reg_array[j][z]));
            }
            cout << "\n";
            fprintf(fp, "\n");
        }
        cout << "\n";
        fprintf(fp, "\n");
    }

    for(i = 0; i < padval; i++){ 
        for (j = 0; j < COLUMNS; j++){ 
            colnum = j;
            /* HERE we perform the actual shifting of the rows */
            switch(colnum){
                /* Each columns is shifted based on the col number */
                case 0:
                    continue;
                    break;
                case 1:
                    swap(list[i].reg_array[j][0],list[i].reg_array[j][3]);
                    swap(list[i].reg_array[j][0],list[i].reg_array[j][2]);
                    swap(list[i].reg_array[j][0],list[i].reg_array[j][1]);
                    break;
                case 2:
                    swap(list[i].reg_array[j][0],list[i].reg_array[j][2]);
                    swap(list[i].reg_array[j][1],list[i].reg_array[j][3]);
                    break;
                case 3:
                    swap(list[i].reg_array[j][0],list[i].reg_array[j][3]);
                    swap(list[i].reg_array[j][1],list[i].reg_array[j][3]);
                    swap(list[i].reg_array[j][2],list[i].reg_array[j][3]);
                    break;
                default:
                    break;
            }
        }
    }
    /******************************************/

    /*  ENTRY POINT FOR SHIFTING OUR ROWS  */
	cout << "ShiftRows:\n";
    fprintf(fp, "ShiftRows:\n");
	for(i = 0; i < padval; i++){
        for (j = 0; j < COLUMNS; j++){
            for (z = 0; z < COLUMNS; z++){
                cout << static_cast<char>(list[i].reg_array[j][z]);
                fprintf(fp, "%c", static_cast<char>(list[i].reg_array[j][z]));
			}
            cout << "\n";
            fprintf(fp, "\n");
        }
        cout << "\n";
        fprintf(fp,"\n");
    }

    for(i = 0; i < padval; i++){ 
        for (j = 0; j < COLUMNS; j++){ 
            for (z = 0; z < COLUMNS; z++){
                /* HERE we convert character to octate binary */
                temp_bit = bitset<8>(list[i].reg_array[j][z]).to_string(); //converts urrent character to 8 bit binary
                for(char &charaddr : temp_bit) { //for each 8 characters in that string
                    if (/*value is a 1 */
                        charaddr == '1'){
                        p_bit++; //increment the parity counter
                    }
                }

                if(/* even amount of p_bits, we will keep */
                    (p_bit % 2) == 0){
                    list[i].string_array[j][z] = temp_bit;
                }else{
                    temp_bit[0] = '1'; //set most significant bit to 1
                    list[i].string_array[j][z] = temp_bit; //assign to string_array
                }
                p_bit = 0;
            }   
        }
    }
    /******************************************/

    /*  ENTRY POINT FOR OUR PARITY BIT  */
	cout << "Parity Bit:\n";
    fprintf(fp, "Parity Bit:\n");
    hexint = 0; 
    for(i = 0; i < padval; i++){
        for (j = 0; j < COLUMNS; j++){
            for (z = 0; z < COLUMNS; z++){
                hexint = stoi(list[i].string_array[j][z], nullptr, 2);
                cout << hex << hexint << " ";
                fprintf(fp, "%x", hexint);
                fprintf(fp, " ");
            }
            cout << "\n";
            fprintf(fp, "\n");
        }
    }
    /******************************************/

    /*  ENTRY POINT FOR CREATING OUR DOING OUR ACTUAL BIT OPERATIONS   */
	for(i = 0; i < padval; i++){
		for (z = 0; z < COLUMNS; z++){ 
            /* HERE WE do the actual bit operation on all of our values */
            //val 0
			list[i].mix_array[0][z] = bitoperation(list[i].string_array[0][z], 2) ^ bitoperation(list[i].string_array[1][z], 3) ^ stoi(list[i].string_array[2][z], nullptr, 2) ^ stoi(list[i].string_array[3][z], nullptr, 2); 
			
            //val 1
            list[i].mix_array[1][z] = stoi(list[i].string_array[0][z], nullptr, 2) ^ bitoperation(list[i].string_array[1][z], 2) ^ bitoperation(list[i].string_array[2][z], 3) ^ stoi(list[i].string_array[3][z], nullptr, 2); 
			
            //val 2
            list[i].mix_array[2][z] = stoi(list[i].string_array[0][z] , nullptr, 2) ^ stoi(list[i].string_array[1][z],nullptr, 2) ^ bitoperation(list[i].string_array[2][z], 2) ^ bitoperation(list[i].string_array[3][z],3); 
			
            //val 3
            list[i].mix_array[3][z] = bitoperation(list[i].string_array[0][z], 3) ^ stoi(list[i].string_array[1][z], nullptr, 2) ^ stoi(list[i].string_array[2][z], nullptr, 2) ^ bitoperation(list[i].string_array[3][z], 2); 
		}
    }
    /******************************************/

    /*  ENTRY POINT FOR MIXING OUR COLUMNS */
	cout << "\nMixColumns:\n";
	fprintf(fp, "\nMixColumns:\n");
    hexint = 0;
    for(i = 0; i < padval; i++){
        for (j = 0; j < COLUMNS; j++){
            for (z = 0; z < COLUMNS; z++){
                hexint = list[i].mix_array[j][z];
                cout  << hex << hexint << " ";
                fprintf(fp, "%x", hexint);
                fprintf(fp, " ");
            }
            cout << "\n";
            fprintf(fp, "\n");
        }
    }
    /******************************************/

    return; //return statement just because its good to have
}
/*
Assignment:
lex - Lexical Analyzer for PL/0

Author: Danh Nguyen

Language: C (only)

To Compile:
  gcc -O2 -std=c11 -o lex lex.c

To Execute (on Eustis):
  ./lex <input file>

where:
  <input file> is the path to the PL/0 source program

Notes:
  - Implement a lexical analyser for the PL/0 language.
  - The program must detect errors such as
      - numbers longer than five digits
      - identifiers longer than eleven characters
      - invalid characters.
  - The output format must exactly match the specification.
  - Tested on Eustis.

Class: COP 3402 - System Software - Spring 2026
Instructor: Dr. Jie Lin
Due Date: Monday, March 2, 2026
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Maximum length allowed for identifiers (11 characters)
#define MAX_IDENTIFIER 11

// Maximum length allowed for numbers (5 digits)
#define MAX_NUM 5

// Maximum size of source code input
#define MAX_CODE 20000

// All token types
typedef enum
{
    skipsym = 1, identsym, numbersym, beginsym, endsym, ifsym, fisym,
    thensym, whilesym, dosym, odsym, callsym, constsym, varsym,
    procsym, writesym, readsym, elsesym, plussym, minussym, multsym,
    slashsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym,
    lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym
} TokenType;


// Name table to store identifiers

char name_table[500][MAX_IDENTIFIER + 1];

// Number of identifiers stored so far  
int name_count = 0;

// array for final tokens
int token_list[MAX_CODE];
int token_values[MAX_CODE];
int list_size = 0;


// validate if word is a reserved keyword
int getReservedToken(char *s)
{
    char *res[] =
    {
        "begin", "end", "if", "fi", "then", "while", "do", "od",
        "call", "const", "var", "procedure", "write", "read", "else"
    };

    for (int i = 0; i < 15; i++)
    {
        if (strcmp(s, res[i]) == 0)
            return (i + 4);
    }

    return identsym;
}

// add identifier to table
int updateTable(char *name)
{    
    //check if it's already in list
    for (int i = 0; i < name_count; i++)
    {
        if (strcmp(name_table[i], name) == 0)
            return i; //return to index i
    }

    strcpy(name_table[name_count], name); // copy to table
    return name_count++; //increase counter
}

int main(int argc, char *argv[])
{    
    // check if argument is valid
    if (argc != 2)
    {
        return 0;
    }
    
    // open file pointer
    FILE *filePointer = fopen(argv[1], "r");
    
    if (!filePointer) {
        return 0; 
    }

    // buffer to store code
    char code[MAX_CODE];
    int n = 0;
    char c;
    
    // read character in file
    while ((c = fgetc(filePointer)) != EOF && n < MAX_CODE - 1)
        code[n++] = c;

    code[n] = '\0';
    fclose(filePointer); // close pointer


    // Print Source Program
    printf("\nSource Program :\n%s\n\n", code);

    // Print Lexeme Table
    printf("Lexeme Table :\n");
    printf("lexeme      token type\n");

    int i = 0;

    while (i < n)
    {
        if (isspace(code[i]))
        {
            i++;
            continue;
        }

        
        if (code[i] == '/' && code[i + 1] == '*')
        {
            i += 2;

            while (!(code[i] == '*' && code[i + 1] == '/') && i < n)
                i++;

            i += 2;
            continue;
        }

        
        if (isalpha(code[i]))
        {
            char buff[100];
            int len = 0;

            while (isalnum(code[i]))
                buff[len++] = code[i++];

            buff[len] = '\0';

            if (len > MAX_IDENTIFIER)
            {
                printf("%-11s identifer is out of range\n", buff);
                token_list[list_size++] = skipsym;
            }
            else
            {
                int type = getReservedToken(buff);
                printf("%-11s %d\n", buff, type);

                token_list[list_size] = type;

                if (type == identsym)
                    token_values[list_size] = updateTable(buff);

                list_size++;
            }
        }

       
        else if (isdigit(code[i]))
        {
            char buff[100];
            int len = 0;

            while (isdigit(code[i]))
                buff[len++] = code[i++];

            buff[len] = '\0';

            if (len > MAX_NUM)
            {
                printf("%-11s number is out of range\n", buff);
                token_list[list_size++] = skipsym;
            }
            else
            {
                printf("%-11s 3\n", buff);
                token_list[list_size] = 3;
                token_values[list_size] = atoi(buff);
                list_size++;
            }
        }

        
        else
        {
            int type = 0;
            char sym[3] = { code[i], '\0', '\0' };

            if (code[i] == '+')
                type = plussym;

            else if (code[i] == '-')
                type = minussym;

            else if (code[i] == '*')
                type = multsym;

            else if (code[i] == '/')
                type = slashsym;

            else if (code[i] == '=')
                type = eqsym;

            else if (code[i] == '(')
                type = lparentsym;

            else if (code[i] == ')')
                type = rparentsym;

            else if (code[i] == ',')
                type = commasym;

            else if (code[i] == ';')
                type = semicolonsym;

            else if (code[i] == '.')
                type = periodsym;

            else if (code[i] == '<')
            {
                if (code[i + 1] == '>')
                {
                    type = neqsym;
                    strcpy(sym, "<>");
                    i++;
                }
                else if (code[i + 1] == '=')
                {
                    type = leqsym;
                    strcpy(sym, "<=");
                    i++;
                }
                else
                    type = lessym;
            }

            else if (code[i] == '>')
            {
                if (code[i + 1] == '=')
                {
                    type = geqsym;
                    strcpy(sym, ">=");
                    i++;
                }
                else
                    type = gtrsym;
            }

            else if (code[i] == '!' && code[i + 1] == '!')
            {
                type = becomessym;
                strcpy(sym, "!!");
                i++;
            }

            else if (code[i] == ':' && code[i + 1] == '=')
            {
                type = becomessym;
                strcpy(sym, ":=");
                i++;
            }

            if (type != 0)
            {
                printf("%-11s %d\n", sym, type);
                token_list[list_size++] = type;
            }
            else
            {
                printf("%-11s symbols not allowed \n", sym);
                token_list[list_size++] = skipsym;
            }

            i++;
        }
    }


    // Print Name Table
    printf("\nName Table :\n");
    printf("Index Name\n");

    for (int j = 0; j < name_count; j++)
        printf("%-5d %s\n", j, name_table[j]);


    // Print Token List
    printf("\nToken List :\n");

    for (int j = 0; j < list_size; j++)
    {
        printf("%d ", token_list[j]);

        if (token_list[j] == identsym || token_list[j] == numbersym)
            printf("%d ", token_values[j]);
    }

    printf("\n");

    return 0;
}
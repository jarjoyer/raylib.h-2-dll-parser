// -----------------------------------------------------------------------------------------//
// ------------------ Simple parser of Raylib.h for use in dll's. --------------------------//
// ---------------------------- made by warjoyer 6-2026 ------------------------------------//
// -----------------------------------------------------------------------------------------//

/*

// to use raylib funtions inside a dll, i declare the following:

// in the .h file common to both main.exe and the dll:



#include "list_of_raylib_functions.h"



// typedefs to all functions with the syntax FunctionType

#define NAME(name) 
#define X(name) name##Type
#define PROTOTYPE(name) typedef name;
LIST_OF_RAYLIB_FUNCTIONS
#undef PROTOTYPE
#undef X
#undef NAME


// declares function pointer struct in the form: FunctionType *Function;

typedef struct{
    #define NAME(name) name##Type *name; 
    #define X(name)
    #define PROTOTYPE(name)
    LIST_OF_RAYLIB_FUNCTIONS
    #undef PROTOTYPE
    #undef X
    #undef NAME
}functions;


// then, in my main.exe i have:

void InitFunctionPointers(functions *function){
    
    // writes all functions in the form: function->Function = (FunctionType*) Function;
    #define NAME(name) function->name = (name##Type *) name; 
    #define X(name)
    #define PROTOTYPE(name)
    LIST_OF_RAYLIB_FUNCTIONS
    #undef PROTOTYPE
    #undef X
    #undef NAME
}

Having raylib in the main translation unit and passing pointer to the functions struct
into the dll allows to use raylib functions by calling F->DrawRectangle(...).

*/

// -----------------------------------------------------------------------------------------//

#include "raylib.h"

// #include "tools.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 420

#define FONT_SIZE 12

char text_prototype[] = "PROTOTYPE";
char text_name[] = "NAME";

char text_define[] = "#define LIST_OF_RAYLIB_FUNCTIONS \\\n";


void goto_next_line(char *s, int *i, int length){
    while(s[(*i)++] != '\n'){
        if(*i == length-1){break;}
    }
}

// asumes both strings have the same length & i is shorter than i2.
void copy_string_till_next_line(char *s, char *s2, int *i, int *i2, int length){
    
    while(s[*i] != '\n'){
        s2[(*i2)++] = s[(*i)++];
        if(*i == length-1){break;}
    }
    s2[(*i2)++] = s[(*i)++]; // copies \n
}

int copy_string(char *s, char *s2){
    int i = 0;
    while(s2[i] != 0){
        s[i] = s2[i];
        i++;
    }
    return i;
}


void parse_line(char *s, char *s2, int *i, int *i2, int length){
    // extract type field, name field, brackets field.
    
    // STRUCTURE:
    // PROTOTYPE(type X(name)(bracket_field))NAME(name2) \
    // where name2 can't contain any *'s before it.
    
    #define FIND_NEXT(name)     while(s[(*i)++] != name){if(*i == length-1){break;}}
    #define FIND_PREVIOUS(name) while(s[--(*i)] != name){if(*i == length-1){break;}}
    char bracket_field[150];
    char type_field[32];
    char name_field[50];
    
    // if(0)
    {
        // since we got here when s[i] = 'R', we know that s[i+6] is where type_field begins. (RLAPI )
        int type_field_start = (*i)+6;
        
        int bfi = 0;
        
        int tfi = 0;
        
        int nfi = 0;
        
        int end;
        int start;
        
        FIND_NEXT('(');
        end = --(*i);
        FIND_PREVIOUS(' ');
        start = (*i)++;
        
        // Name Field
        while((*i) < end){
            // s2[(*i2)++] = s[(*i)++];
            name_field[nfi++] = s[(*i)++];
        }
        name_field[nfi++] = 0;
        
        // Type Field
        *i = type_field_start;
        while((*i) < start){
            // s2[(*i2)++] = s[(*i)++];
            type_field[tfi++] = s[(*i)++];
        }
        type_field[tfi++] = 0;
        
        
        (*i) = end;
        start = end;
        FIND_NEXT(')');
        end = (*i);
        (*i) = start;
        while((*i) < end){
            // s2[(*i2)++] = s[(*i)++];
            bracket_field[bfi++] = s[(*i)++];
        }
        bracket_field[bfi++] = 0;
    }
    
    // STRUCTURE:
    // PROTOTYPE(type X(name)(bracket_field))NAME(name2) \
    // where name2 can't contain any *'s before it.
    
    int ni;
    ni = copy_string(&s2[*i2], text_prototype);
    *i2 += ni;
    
    s2[(*i2)++] = '(';
    
    ni = copy_string(&s2[*i2], type_field);
    *i2 += ni;
    
    s2[(*i2)++] = ' ';
    s2[(*i2)++] = 'X';
    s2[(*i2)++] = '(';
    
    ni = copy_string(&s2[*i2], name_field);
    *i2 += ni;
    
    s2[(*i2)++] = ')';
    
    ni = copy_string(&s2[*i2], bracket_field);
    *i2 += ni;
    
    s2[(*i2)++] = ')';
    
    ni = copy_string(&s2[*i2], text_name);
    *i2 += ni;
    
    s2[(*i2)++] = '(';
    
    // delete *'s
    ni = 0;
    while(name_field[ni] == '*'){ni++;}
    ni = copy_string(&s2[*i2], &name_field[ni]);
    *i2 += ni;
    
    s2[(*i2)++] = ')';
    
    s2[(*i2)++] = '\\';
    
    
    s2[(*i2)++] = '\n';
    goto_next_line(s, i, length);
}
    

char *parse_into_header(char *s1, int s1_size, char *s2){
    s2 = (char*)MemAlloc(s1_size);
    
    int i = 0;
    int i2 = 0;
    
    i2 = copy_string(s2, text_define);
    
    while(i < s1_size-1){
        if(s1[i] == 0){break;}
        // Note: could be implemented as: if(beginning_contains("RLAPI")){
        if(s1[i] != 'R'){
            goto_next_line(s1, &i, s1_size);
        }else{
            // copy_string_till_next_line(s1, s2, &i, &i2, s1_size);
            
            parse_line(s1, s2, &i, &i2, s1_size);
            
        }
    }
    s2[++i2] = 0;
    return s2;
}

// ---------------------------------------------------------------------------------------------------------------------------//
int main(void){
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib.h parser");
    SetTraceLogLevel(LOG_ERROR);
    SetRandomSeed(GetTime());
	SetTargetFPS(60);
    
    #define TEXT_FILE "raylib.h"
    
    char *raylib_h_text = LoadFileText(TEXT_FILE);
    
    int raylib_h_length = GetFileLength(TEXT_FILE);
    
    char *raylib_h_parsed = 0;
    
    raylib_h_parsed = parse_into_header(raylib_h_text, raylib_h_length, raylib_h_parsed);
    
    
    float mw = 0;
    float vertical_off = 0;
    
	while (!WindowShouldClose()){
	// while (false){
        
        if(IsKeyPressed(KEY_PAGE_DOWN)){
            vertical_off -= WINDOW_HEIGHT;
        }
        if(IsKeyPressed(KEY_PAGE_UP)){
            vertical_off += WINDOW_HEIGHT;
        }
        
        mw = GetMouseWheelMove();
        vertical_off += mw * FONT_SIZE;
        //---------------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);
        
        DrawText(raylib_h_parsed, 0, 0 + FONT_SIZE + vertical_off, FONT_SIZE, WHITE);
        
        DrawRectangle(0,0, WINDOW_WIDTH, FONT_SIZE, GRAY);
        
        DrawText(TextFormat("size: %i", raylib_h_length), 10, 0, FONT_SIZE, WHITE);
        
        EndDrawing(); 
    //---------------------------------------------------------------------------------------
	}
    
    SaveFileText("list_of_raylib_functions.h", raylib_h_parsed);
    
	CloseWindow();
	return 0;
}

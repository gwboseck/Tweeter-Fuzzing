#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct 
{
  char* name;
  int count;
} Tweeter;

int numTweeters;

//Find name column in file
int namecolumn(FILE *file)
{
  //Read first line
  char line [500];
  fgets(line, 500, file);
  
  char comma [2] = ",";
  
  //Find the name column
  char *field = strtok(line, comma);
  int col = 0;
  
  while(strcmp(field, "name") != 0)
  {
    col++;
    field = strtok(NULL, comma);
  }
  return col;
}

//Check if file has invalid dimensions (over 20000 lines or over 500 characters in a line)
int invalidDimensions(FILE *file)
{
  int numlines = 1; //Number of lines
  int numchars = 0; //Number of characters in a line
  
  char c;
  c = fgetc(file);
  
  while(c != EOF)
  {
    if(c == '\n')
    {
      numlines++;
      numchars = 0;
    }
    else
    {
      numchars++;
    }
    
    if(numlines > 20000) //If more than 20000 lines
      return 1;
      
    if(numchars > 500) //If more than 500 characters
      return 1;
    
    c = fgetc(file);  
  } 
  
  return 0; 
}

//Check if header has no name field
int noNameField(FILE *file)
{
  //Read first line
  char line [500];
  fgets(line, 500, file);
  
  char comma [2] = ",";
  
  char *field = strtok(line, comma);
  
  if(strcmp(field, "name") == 0)
    return 0;
  
  //If first line doesn't contain "name", return 1, else return 0
  while((field = strtok(NULL, comma)))
    if(strcmp(field, "name") == 0)
      return 0;
  
  
  return 1; //Stub
}

//Check if lines have differing number of commas
int badCommas(FILE *file)
{
  int numcommasfirstline = 0; //Number of commas in first line
  int numcommas = 0; //Number of commas on current line
  char c; //Used to step through file
  
  c = fgetc(file);
  
  //Find number of commas in first line
  while(c != '\n' && c != EOF)
  {
    if(c == ',')
      numcommasfirstline++;
    c = fgetc(file);
  }

  //Find number of commas in subsequent lines
  c = fgetc(file);
  while(c != EOF)
  {
    if(c == '\n')
    {
      if(numcommas != numcommasfirstline) //Return 1 if different number of commas than first line
        return 1;
      
      numcommas = 0;  
    }
    if(c == ',')
      numcommas++;
    
    c = fgetc(file);  
  }
  
  
  return 0; //Stub
}

//Check if invalid file
int invalid(FILE *file)
{
  //Possible errors:
  
  //Too many lines (limit 20000)
  //Line too long (limit: 500 characters)
  if(invalidDimensions(file) == 1)
    return 1;
  
  rewind(file);
  
  //Inconsistent number of commas in lines
  if(badCommas(file) == 1)
    return 1;
  
  rewind(file);
    
  //No name field in header
  if(noNameField(file) == 1)
    return 1;
    
  rewind(file);
  
  //If error found, return 1. Else, return 0.
  return 0;
}

//Get name from line
char* getName(char* line, int namecolnum, char *name)
{
  //Wait until namecolnum + 1 commas encountered.
  int numcommas = 0;
  int i = 0;
  char c;
  
  while(numcommas < namecolnum + 1)
  {
    c = line[i];
    i++;
    
    if(c == ',')
    {
      numcommas++;
    }
  }
  
  int j = 0;
  //Read into name until next comma
  while((c = line[i]) != ',')
  {
    name[j] = c;
    j++;
    i++;
  }
  name[j] = '\0';
  //Return string
  return name;
}

//Search an array for a tweeter with name name
int searchTweeter(Tweeter* tweeterArray, char* name)
{
  int i; //Iteration variable
  int found = 0; //Determine if found
  int size = 0; //Size of tweeterArray
  
  for(i = 0; i < numTweeters; i++) //Loop until end of tweeter array
  {
    if(strcmp(tweeterArray[i].name, name) == 0) //Compare tweeterArray[i] and name
    {
      found = 1;
      break;
    }
  }
  
  //If found return i, else return -1
  if(found == 1)
  {
    return i;
  }
  
  else
  {
    return -1;
  }
}

//Insert tweeter with name name into array
void insertTweeter(Tweeter* tweeterArray, char* name)
{
  //Search array for tweeter with name name
  int index = searchTweeter(tweeterArray, name);
  //If found, increment count by 1
  //Otherwise, insert into end of array and initialize count to 1
  //If not found
  if(index == -1)
  {
    int i = 0;
    //Insert into end of array and initialize count to 1
    
    //Create new tweeter with name name and count 1, and place in array
    Tweeter newTweeter;
    newTweeter.name = (char *) malloc(strlen(name));
    strcpy(newTweeter.name, name);
    newTweeter.count = 1;
    
    //Place newTweeter at end of array
    tweeterArray[numTweeters] = newTweeter;
    numTweeters++;
  }
  //If found
  else
  {
    //Increment count by 1
    tweeterArray[index].count++;
  }
}

//Sort the array by count
void sortTweeters(Tweeter* tweeterArray)
{
  //Insertion sort
  int i;
  int j;
  int k;
  for(i = 1; i < numTweeters; i++)
  {
    j = i;
    Tweeter curr = tweeterArray[i];
    
    while(curr.count > tweeterArray[j-1].count)
    {
      j--;
      
      if(j == 0)
        break;
    }
    
    //Move everything between j and i-1 forward by 1
    
    for(k = i; k > j; k--)
      tweeterArray[k] = tweeterArray[k-1];
    
    tweeterArray[j] = curr;
  }
}

int main(int argc, char **argv)
{
  //Open csv file
  FILE *file = fopen(argv[1], "r");
  //Create array of tweeters
  Tweeter tweeterArray[20000];
  numTweeters = 0; //Initialize number of tweeters to 0
  
  if(access(argv[1],F_OK) != -1) { //check if file exists and is accessible
      
    if(argc == 2) {
      //Correct number of arguments, so do nothing and move on
      } else {
        //Any other number of arguments will be invalid. Either no argument or too many arguments.
        printf("Invalid Input Format\n");
        return 1;
    }
    
    //Catch errors
    if(invalid(file) == 1) {
      printf("Invalid Input Format\n");
      return 1;
    }
      
    
    
    rewind(file); 
    //Read first line
    
    //Find name column
    
    int namecol = namecolumn(file);
    char line [500];
    
    //For each line (not including the first)
    while(fgets(line, 500, file) != NULL)
    {
      //Get the name
      char name[500];
      getName(line, namecol, name);
      //insert into array
      insertTweeter(tweeterArray, name);
    }
    //Sort array by tweeter's count (in descending order)
    
    sortTweeters(tweeterArray);
    
    int i;
    
    //If less than 10 tweeters, output all tweeters
    if(numTweeters < 10)
      for(i = 0; i < numTweeters; i++)
        printf("%s: %d\n", tweeterArray[i].name, tweeterArray[i].count);
    
    //Else Output first 10 elements of array
    else
      for(i = 0; i < 10; i++)
        printf("%s: %d\n", tweeterArray[i].name, tweeterArray[i].count);
    
    //Free dynamically allocated names
    for(i = 0; i < numTweeters; i++)
      free(tweeterArray[i].name);
    
    //Close file
    fclose(file);
    
    return 0;
    
  } else { //file is invalid or inaccessible
    printf("Invalid Input Format\n");
    return 1;
  }
  
}
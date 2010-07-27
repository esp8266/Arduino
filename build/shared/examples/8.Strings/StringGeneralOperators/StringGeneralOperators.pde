String stringOne = String("string number one"); //  declares stringOne with an initial string
char[] theseChars;
void setup() {
  Serial.begin(9600); 
  
  String stringTwo = String(10000);  // stringTwo holds "10000", equivalent to: String stringTwo = 10000;
  Serial.println("stringTwo holds: " + stringTwo);
  
  String stringThree = String(567000, DEC);  // stringThree holds "567000"
  Serial.println("stringThree holds: " + stringThree);
  
  String stringFour = "string number four";  // stringFour holds "string number four"
  Serial.println("stringFour holds: " + stringFour);
  
  stringFour += " plus another part";  // use + operator to add a string
  Serial.println("now stringFour holds: " + stringFour);  // user + operator when printing to serial
  
  char c = stringFour.charAt(5);  // get character at position 5
  Serial.print("c holds: ");
  Serial.println(c);  // prints 'g'
  
  String stringFive = String("string number one");  // stringFive holds "string number one"
  
  int cmp = stringOne.compareTo(stringFive);  // compare stringOne to stringFive
  if(cmp == 0) {
    Serial.println("stringOne and stringFive are equal");  // prints equal
  } else {
    Serial.println("stringOne and stringFive are different");
  }
  
  if(stringFive.endsWith("one"))  // check if stringFive ends with "one"
    Serial.println("stringFive ends with \"one\"");  // prints "stringFive ends with "one""
  
  if(stringOne.equals(stringFive))  // check if stringOne equal to stringFive
    Serial.println("stringOne and stringFive are equal");  // prints equal
  
  String stringSix = String("string NUMBER one");  // stringSix holds "string NUMBER one"
  if(stringFive.equalsIgnoreCase(stringSix))  // check if stringFive and stringSix are equal ignoring case differences
    Serial.println("stringSix and stringFive are equal ignoring the case");  // prints equal
  
  Serial.print("index of char R pn stringSix is: ");
  Serial.println(stringSix.indexOf('R'), DEC);  // prints 12

  Serial.print("index of char R on stringSix from index 13 is: ");
  Serial.println(stringSix.indexOf('R', 13), DEC);  // prints -1, not found
  
  String s7 = stringSix.substring(7, 13);
  Serial.println("s7 is: "+s7);  // prints "NUMBER"
  
  Serial.print("index of string \"NUMBER\" on stringSix is: ");  
  Serial.println(stringSix.indexOf(s7), DEC);  // prints 7

  Serial.print("last index of char 'n' on stringSix is: ");  
  Serial.println(stringSix.lastIndexOf('n'), DEC);  // prints 15
  
  Serial.print("length of stringSix is: ");  
  Serial.println(stringSix.length(), DEC);  // prints 15

  stringSix.setCharAt(15, 'N');  // set character at index 15 to 'N'
  Serial.println("stringSix is: "+stringSix);  // prints "string NUMBER oNe"
  
  if(stringSix.startsWith("string"))  // check if stringSix starts with "string"
    Serial.println("stringSix starts with \"string\""); // stringSix starts with "string" string
    
  Serial.println("stringSix to lower case is: "+stringSix.toLowerCase());  // prints "string number one"
  
  Serial.println("stringSix to upper case is: "+stringSix.toUpperCase());  // prints "STRING NUMBER ONE"
  
  stringSix.concat(" plus spaces at the end   ");  // concat a string
  Serial.println("stringSix is: "+stringSix);  // prints "string NUMBER oNe plus spaces at the end   "
  
  Serial.println("trim of stringSix is: "+stringSix.trim());  // prints "string NUMBER oNe plus spaces at the end"

  Serial.println("stringSix is: "+stringSix.replace('e', '5'));  // prints "string NUMBER oN5 plus spac5s at th5 5nd"
  
  const char *str = stringSix.toCharArray();  // get the char array
  Serial.print("str is: ");
  Serial.println(str);  // prints "string NUMBER oNe plus spaces at the end   "
}

void loop() {
  
}

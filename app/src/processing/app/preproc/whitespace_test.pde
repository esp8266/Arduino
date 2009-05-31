// this is a whitespace and other invisible token torture test for the ANTLR-based
// preprocessor.  edit pde.properties and set "editor.save_build_files" to true.
// then, build this in processing.  next, use
//
// diff -u --strip-trailing-cr \ 
//   work/sketchbook/default/whitespace_test/whitespace_test.pde \ 
//   work/lib/build/MyDemo.java
//
// to compare the files before and after preprocessing.  There should not be 
// any differences.

import // comment test
  java.io.*;

// comment 1
public class // post-class comment

// comment2
  
MyDemo extends BApplet implements // foo
   java.lang. // bar
    Cloneable {
    
  //argh
    
  public  // foo
    String // bar
      fff = /*rheet */ "stuff";
    
  static /*a*/ {
    /*foo*/
    /*bar*/
    six = 6;
    } /* b*/
    
  static /*a*/ final /*b*/ int six;
  
  void setup()
  {
    size(200, 200);
    background(255);
    
    this . fff = /* ook */ (String)/*foo*/"yo";
    rectMode(CENTER_DIAMETER); // comment 1a
    noStroke();
    fill(255, 204, 0);
    
    int q = /*a*/ - /*b*/ 1;
    
    boolean c = /*a*/ ! /*b*/ true;
  }

  int foo() /*a*/ throws /*b*/ java.lang.Exception /*c*/
  {
    int b = 7;
    switch /*a*/ ( /*b*/ b /*c*/ ) {
    case /*d*/ 1 /*e*/: /*f*/
      int c=9;
        /*g*/
      break; /*h*/
    default /*i*/ :
      int d=9;
      break;
    }   
       
    try { /* qq */
      loop(); /* rr */
    } catch /*ss*/ (java.lang.Exception ex) /*tt*/ {
      b = 8; /*tut*/
      throw /*utu*/ ex;
    } /*uu*/ finally /*vv*/ { 
      b = 9;
    } /*ww*/

    b /*aaa*/ = /*bbb*/ true /*ccc*/ ? /*ddd*/ 0 /*eee*/
      : /* fff*/ 1 /*ggg*/;
    return /*a*/ 5 /*b*/;
  }
  
  // comment 2
  void loop()
  {
  
    int arr1 /* VVV */ [ /* XXX */] /*YYY*/  ;
    int[] arr2 = { /*a*/ 2, 3 /*b*/ } /*c*/ ;
    
    for /*a*/ (/*b*/ int j=0 /*c*/; /*d*/ j<2/*e*/ ; /*f*/ j++ /*g*/)
      /*h*/
      arr2[1] = 6;
      
     /*foo*/  
     ;
     /*bar*/
    rect(width-mouseX, height-mouseY, 50, 50);
    rect(mouseX, mouseY, 50, 50);
    
    if (/*a*/ arr2[1] == 6/*b*/) {
      /*c*/
      int d=7;
    } /*d*/else /*e*/{
      int e=8;
      /*f*/
    }
    
    int f;
    if (/*aa*/ arr2[1] ==6 /*bb*/ ) 
      /*cc*/
      f=8; /*dd*/
    else /*ee*/
      f=10; /*ff*/
      
    while ( /*aaa*/ f < 15) /*bbb*/ {
     f ++;
    } /*ggg*/
    
    do /* aaaa */ {
      f++;
    } /*bbbb*/ while /*cccc*/ ( /*a*/ - /*b*/ 20 > f) /*dddd*/;
    
    f = 2 * 3 + 4;
    
    f = ( 2 * 3 ) + 4 + /*aa*/ -/*bb*/1;
    
    f = 2 * ( 3 + 4 ) ;
    
    fff = /*a*/ new /*b*/ String(/*c*/"foo"/*d*/) /*e*/;
    
    int arr3[] = /*a*/ new /*b*/ int/*c*/[/*d*/] /*e*/ {1/*f*/,2};
    int arr4[][] = new /*a*/int/*b*/[1][2]/*c*/;

  }
  
  class Shoe
    {
      Shoe(String brand)
        {
          println(brand);
        }
    }

  class NikeAir extends Shoe
    {
      NikeAir()
        {
          /*a*/ super /*b*/ ( /*c*/ "Nike" /*d*/ ) /*e*/ ;
          
          /*aa*/ ( /*bb*/ new /*cc*/ MyDemo /*dd*/ (/*ee*/)/*ff*/)/*gg*/./*hh*/super/*ii*/(/*jj*/5/*kk*/);
        }
    }
}

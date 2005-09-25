/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        Copyright (c) Non, Inc. 1997 -- All Rights Reserved

PROJECT:        C Compiler
MODULE:         Parser
FILE:           stdc.g

AUTHOR:         John D. Mitchell (john@non.net), Jul 12, 1997

REVISION HISTORY:

        Name    Date            Description
        ----    ----            -----------
        JDM     97.07.12        Initial version.
        JTC     97.11.18        Declaration vs declarator & misc. hacking.
        JDM     97.11.20        Fixed:  declaration vs funcDef,
                                        parenthesized expressions,
                                        declarator iteration,
                                        varargs recognition,
                                        empty source file recognition,
                                        and some typos.
                                        

DESCRIPTION:

        This grammar supports the Standard C language.

        Note clearly that this grammar does *NOT* deal with
        preprocessor functionality (including things like trigraphs)
        Nor does this grammar deal with multi-byte characters nor strings
        containing multi-byte characters [these constructs are "exercises
        for the reader" as it were :-)].

        Please refer to the ISO/ANSI C Language Standard if you believe
        this grammar to be in error.  Please cite chapter and verse in any
        correspondence to the author to back up your claim.

TODO:

        - typedefName is commented out, needs a symbol table to resolve
        ambiguity.

        - trees

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


header{
package processing.app.preproc;
import java.io.*;

import antlr.CommonAST;
import antlr.DumpASTVisitor;
}

                     
class StdCParser extends Parser;

options
        {
        k = 2;
        exportVocab = STDC;
        buildAST = true;
        ASTLabelType = "TNode";

        // Copied following options from java grammar.
        codeGenMakeSwitchThreshold = 2;
        codeGenBitsetTestThreshold = 3;
        }


{
    // Suppport C++-style single-line comments?
    public static boolean CPPComments = true;

    // access to symbol table
    public CSymbolTable symbolTable = new CSymbolTable();

    // source for names to unnamed scopes
    protected int unnamedScopeCounter = 0;

    public boolean isTypedefName(String name) {
      boolean returnValue = false;
      TNode node = symbolTable.lookupNameInCurrentScope(name);
      for (; node != null; node = (TNode) node.getNextSibling() ) {
        if(node.getType() == LITERAL_typedef) {
            returnValue = true;
            break;
        }
      }
      return returnValue;
    }


    public String getAScopeName() {
      return "" + (unnamedScopeCounter++);
    }

    public void pushScope(String scopeName) {
      symbolTable.pushScope(scopeName);
    }

    public void popScope() {
      symbolTable.popScope();
    }

        int traceDepth = 0;
        public void reportError(RecognitionException ex) {
          try {
            System.err.println("ANTLR Parsing Error: "+ex + " token name:" + tokenNames[LA(1)]);
            ex.printStackTrace(System.err);
          }
	  catch (TokenStreamException e) {
            System.err.println("ANTLR Parsing Error: "+ex);
            ex.printStackTrace(System.err);              
          }
        }
        public void reportError(String s) {
            System.err.println("ANTLR Parsing Error from String: " + s);
        }
        public void reportWarning(String s) {
            System.err.println("ANTLR Parsing Warning from String: " + s);
        }
        public void match(int t) throws MismatchedTokenException {
          boolean debugging = false;
          
          if ( debugging ) {
           for (int x=0; x<traceDepth; x++) System.out.print(" ");
           try {
            System.out.println("Match("+tokenNames[t]+") with LA(1)="+
                tokenNames[LA(1)] + ((inputState.guessing>0)?" [inputState.guessing "+ inputState.guessing + "]":""));
           }
           catch (TokenStreamException e) {
            System.out.println("Match("+tokenNames[t]+") " + ((inputState.guessing>0)?" [inputState.guessing "+ inputState.guessing + "]":""));

           }
    
          }
          try {
            if ( LA(1)!=t ) {
                if ( debugging ){
                    for (int x=0; x<traceDepth; x++) System.out.print(" ");
                    System.out.println("token mismatch: "+tokenNames[LA(1)]
                                + "!="+tokenNames[t]);
                }
	        throw new MismatchedTokenException(tokenNames, LT(1), t, false, getFilename());

            } else {
                // mark token as consumed -- fetch next token deferred until LA/LT
                consume();
            }
          }
          catch (TokenStreamException e) {
          }
    
        }
        public void traceIn(String rname) {
          traceDepth += 1;
          for (int x=0; x<traceDepth; x++) System.out.print(" ");
          try {
            System.out.println("> "+rname+"; LA(1)==("+ tokenNames[LT(1).getType()] 
                + ") " + LT(1).getText() + " [inputState.guessing "+ inputState.guessing + "]");
          }
          catch (TokenStreamException e) {
          }
        }
        public void traceOut(String rname) {
          for (int x=0; x<traceDepth; x++) System.out.print(" ");
          try {
            System.out.println("< "+rname+"; LA(1)==("+ tokenNames[LT(1).getType()] 
                + ") "+LT(1).getText() + " [inputState.guessing "+ inputState.guessing + "]");
          }
          catch (TokenStreamException e) {
          }
          traceDepth -= 1;
        }
    
}



translationUnit
        :       externalList

        |       /* Empty source files are *not* allowed.  */
                {
                System.err.println ( "Empty source file!" );
                }
        ;


externalList
        :       ( externalDef )+
        ;


externalDef
        :       ( "typedef" | declaration )=> declaration
        |       functionDef
        |       asm_expr
        ;


asm_expr
        :       "asm"^ 
                ("volatile")? LCURLY! expr RCURLY! SEMI!
        ;


declaration
                                        { AST ds1 = null; }
        :       ds:declSpecifiers       { ds1 = astFactory.dupList(#ds); }
                (                       
                    initDeclList[ds1]
                )?
                SEMI!
                                        { ## = #( #[NDeclaration], ##); }
                
        ;


declSpecifiers 
                                { int specCount=0; }
        :       (               options { // this loop properly aborts when
                                          //  it finds a non-typedefName ID MBZ
                                          warnWhenFollowAmbig = false;
                                        } :
                  s:storageClassSpecifier
                | typeQualifier
                | ( "struct" | "union" | "enum" | typeSpecifier[specCount] )=>
                        specCount = typeSpecifier[specCount]
                )+
        ;

storageClassSpecifier
        :       "auto"                  
        |       "register"              
        |       "typedef"               
        |       functionStorageClassSpecifier
        ;


functionStorageClassSpecifier
        :       "extern"
        |       "static"
        ;


typeQualifier
        :       "const"
        |       "volatile"
        ;

typeSpecifier [int specCount] returns [int retSpecCount]
                                                        { retSpecCount = specCount + 1; }
        :
        (       "void"
        |       "char"
        |       "short"
        |       "int"
        |       "long"
        |       "float"
        |       "double"
        |       "signed"
        |       "unsigned"
        |       structOrUnionSpecifier
        |       enumSpecifier
        |       { specCount == 0 }? typedefName
        )
        ;


typedefName
        :       { isTypedefName ( LT(1).getText() ) }?
                i:ID                    { ## = #(#[NTypedefName], #i); }
        ;

structOrUnionSpecifier
                                        { String scopeName; }
        :       sou:structOrUnion!
                ( ( ID LCURLY )=> i:ID l:LCURLY
                                            {
                                            scopeName = #sou.getText() + " " + #i.getText();
                                            #l.setText(scopeName);
                                            pushScope(scopeName);
                                            }
                        structDeclarationList
                                            { popScope();}
                        RCURLY!
                |   l1:LCURLY
                                            {
                                            scopeName = getAScopeName();
                                            #l1.setText(scopeName);
                                            pushScope(scopeName);
                                            }
                    structDeclarationList
                                            { popScope(); }
                    RCURLY!
                | ID
                )
                                            {
                                            ## = #( #sou, ## );
                                            }
        ;


structOrUnion
        :       "struct"
        |       "union"
        ;


structDeclarationList
        :       ( structDeclaration )+
        ;


structDeclaration
        :       specifierQualifierList structDeclaratorList ( SEMI! )+
        ;


specifierQualifierList
                                { int specCount = 0; }
        :       (               options {   // this loop properly aborts when
                                            // it finds a non-typedefName ID MBZ
                                            warnWhenFollowAmbig = false;
                                        } :
                ( "struct" | "union" | "enum" | typeSpecifier[specCount] )=>
                        specCount = typeSpecifier[specCount]
                | typeQualifier
                )+
        ;


structDeclaratorList
        :       structDeclarator ( COMMA! structDeclarator )*
        ;


structDeclarator
        :
        (       COLON constExpr
        |       declarator[false] ( COLON constExpr )?
        )
                                    { ## = #( #[NStructDeclarator], ##); }
        ;


enumSpecifier
        :       "enum"^
                ( ( ID LCURLY )=> i:ID LCURLY enumList[i.getText()] RCURLY!
                | LCURLY enumList["anonymous"] RCURLY!
                | ID
                )
        ;


enumList[String enumName]
        :       enumerator[enumName] ( COMMA! enumerator[enumName] )*  
        ;

enumerator[String enumName]
        :       i:ID                { symbolTable.add(  i.getText(),
                                                        #(   null,
                                                            #[LITERAL_enum, "enum"],
                                                            #[ ID, enumName]
                                                         )
                                                     );
                                    }
                (ASSIGN constExpr)?
        ;


initDeclList[AST declarationSpecifiers]
        :       initDecl[declarationSpecifiers] 
                ( COMMA! initDecl[declarationSpecifiers] )*
        ;


initDecl[AST declarationSpecifiers]
                                        { String declName = ""; }
        :       declName = d:declarator[false]
                                        {   AST ds1, d1;
                                            ds1 = astFactory.dupList(declarationSpecifiers);
                                            d1 = astFactory.dupList(#d);
                                            symbolTable.add(declName, #(null, ds1, d1) );
                                        }
                ( ASSIGN initializer
                | COLON expr
                )?
                                        { ## = #( #[NInitDecl], ## ); }

        ;

pointerGroup
        :       ( STAR ( typeQualifier )* )+    { ## = #( #[NPointerGroup], ##); }
        ;



idList
        :       ID ( COMMA! ID )*
        ;


initializer
        :       ( assignExpr
                |       LCURLY initializerList ( COMMA! )? RCURLY!
                )
                        { ## = #( #[NInitializer], ## ); }
        ;


initializerList
        :       initializer ( COMMA! initializer )*
        ;


declarator[boolean isFunctionDefinition] returns [String declName]
                                                { declName = ""; }
        :
                ( pointerGroup )?               

                ( id:ID                         { declName = id.getText(); }
                | LPAREN declName = declarator[false] RPAREN
                )

                ( !  LPAREN
                                                { 
                                                    if (isFunctionDefinition) {
                                                        pushScope(declName);
                                                    }
                                                    else {
                                                        pushScope("!"+declName); 
                                                    }
                                                }
                    (                           
                        (declSpecifiers)=> p:parameterTypeList
                                                {
                                                ## = #( null, ##, #( #[NParameterTypeList], #p ) );
                                                }

                        | (i:idList)?
                                                {
                                                ## = #( null, ##, #( #[NParameterTypeList], #i ) );
                                                }
                    )
                                                {
                                                popScope();
                                                }    
                  RPAREN                        
                | LBRACKET ( constExpr )? RBRACKET
                )*
                                                { ## = #( #[NDeclarator], ## ); }
        ;
 
parameterTypeList
        :       parameterDeclaration
                (   options {
                            warnWhenFollowAmbig = false;
                        } : 
                  COMMA!
                  parameterDeclaration
                )*
                ( COMMA!
                  VARARGS
                )?
        ;


parameterDeclaration
                            { String declName; }
        :       ds:declSpecifiers
                ( ( declarator[false] )=> declName = d:declarator[false]
                            {
                            AST d2, ds2;
                            d2 = astFactory.dupList(#d);
                            ds2 = astFactory.dupList(#ds);
                            symbolTable.add(declName, #(null, ds2, d2));
                            }
                | nonemptyAbstractDeclarator
                )?
                            {
                            ## = #( #[NParameterDeclaration], ## );
                            }
        ;

/* JTC:
 * This handles both new and old style functions.
 * see declarator rule to see differences in parameters
 * and here (declaration SEMI)* is the param type decls for the
 * old style.  may want to do some checking to check for illegal
 * combinations (but I assume all parsed code will be legal?)
 */

functionDef
                            { String declName; }
        :       ( (functionDeclSpecifiers)=> ds:functionDeclSpecifiers
                |  //epsilon
                )
                declName = d:declarator[true]
                            {
                            AST d2, ds2;
                            d2 = astFactory.dupList(#d);
                            ds2 = astFactory.dupList(#ds);
                            symbolTable.add(declName, #(null, ds2, d2));
                            pushScope(declName);
                            }
                ( declaration )* (VARARGS)? ( SEMI! )*
                            { popScope(); }
                compoundStatement[declName]
                            { ## = #( #[NFunctionDef], ## );}
        ;

functionDeclSpecifiers
                                { int specCount = 0; }
        :       (               options {   // this loop properly aborts when
                                            // it finds a non-typedefName ID MBZ
                                            warnWhenFollowAmbig = false;
                                        } :
                  functionStorageClassSpecifier
                | typeQualifier
                | ( "struct" | "union" | "enum" | typeSpecifier[specCount] )=>
                        specCount = typeSpecifier[specCount]
                )+
        ;

declarationList
        :       (               options {   // this loop properly aborts when
                                            // it finds a non-typedefName ID MBZ
                                            warnWhenFollowAmbig = false;
                                        } :
                ( declarationPredictor )=> declaration
                )+
        ;

declarationPredictor
        :       (options {      //only want to look at declaration if I don't see typedef
                    warnWhenFollowAmbig = false;
                }:
                "typedef"
                | declaration
                )
        ;


compoundStatement[String scopeName]
        :       LCURLY!
                            {
                                pushScope(scopeName);
                            }
                ( ( declarationPredictor)=> declarationList )?
                ( statementList )?
                            { popScope(); }
                RCURLY!
                            { ## = #( #[NCompoundStatement, scopeName], ##); }
        ;

    
statementList
        :       ( statement )+
        ;
statement
        :       SEMI                    // Empty statements

        |       compoundStatement[getAScopeName()]       // Group of statements

        |       expr SEMI!               { ## = #( #[NStatementExpr], ## ); } // Expressions

// Iteration statements:

        |       "while"^ LPAREN! expr RPAREN! statement
        |       "do"^ statement "while"! LPAREN! expr RPAREN! SEMI!
        |!       "for"
                LPAREN ( e1:expr )? SEMI ( e2:expr )? SEMI ( e3:expr )? RPAREN
                s:statement
                                    {
                                        if ( #e1 == null) { #e1 = #[ NEmptyExpression ]; }
                                        if ( #e2 == null) { #e2 = #[ NEmptyExpression ]; }
                                        if ( #e3 == null) { #e3 = #[ NEmptyExpression ]; }
                                        ## = #( #[LITERAL_for, "for"], #e1, #e2, #e3, #s );
                                    }


// Jump statements:

        |       "goto"^ ID SEMI!
        |       "continue" SEMI!
        |       "break" SEMI!
        |       "return"^ ( expr )? SEMI!


// Labeled statements:
        |       ID COLON! (options {warnWhenFollowAmbig=false;}:statement)? { ## = #( #[NLabel], ## ); }
        |       "case"^ constExpr COLON! statement
        |       "default"^ COLON! statement



// Selection statements:

        |       "if"^
                 LPAREN! expr RPAREN! statement  
                ( //standard if-else ambiguity
                        options {
                            warnWhenFollowAmbig = false;
                        } :
                "else" statement )?
        |       "switch"^ LPAREN! expr RPAREN! statement
        ;






expr
        :       assignExpr (options {
                                /* MBZ:
                                    COMMA is ambiguous between comma expressions and
                                    argument lists.  argExprList should get priority,
                                    and it does by being deeper in the expr rule tree
                                    and using (COMMA assignExpr)*
                                */
                                warnWhenFollowAmbig = false;
                            } :
                            c:COMMA^ { #c.setType(NCommaExpr); } assignExpr         
                            )*
        ;


assignExpr
        :       conditionalExpr ( a:assignOperator! assignExpr { ## = #( #a, ## );} )?
        ;

assignOperator
        :       ASSIGN
        |       DIV_ASSIGN
        |       PLUS_ASSIGN
        |       MINUS_ASSIGN
        |       STAR_ASSIGN
        |       MOD_ASSIGN
        |       RSHIFT_ASSIGN
        |       LSHIFT_ASSIGN
        |       BAND_ASSIGN
        |       BOR_ASSIGN
        |       BXOR_ASSIGN
        ;


conditionalExpr
        :       logicalOrExpr
                ( QUESTION^ expr COLON! conditionalExpr )?
        ;


constExpr
        :       conditionalExpr
        ;

logicalOrExpr
        :       logicalAndExpr ( LOR^ logicalAndExpr )*
        ;


logicalAndExpr
        :       inclusiveOrExpr ( LAND^ inclusiveOrExpr )*
        ;

inclusiveOrExpr
        :       exclusiveOrExpr ( BOR^ exclusiveOrExpr )*
        ;


exclusiveOrExpr
        :       bitAndExpr ( BXOR^ bitAndExpr )*
        ;


bitAndExpr
        :       equalityExpr ( BAND^ equalityExpr )*
        ;



equalityExpr
        :       relationalExpr
                ( ( EQUAL^ | NOT_EQUAL^ ) relationalExpr )*
        ;


relationalExpr
        :       shiftExpr
                ( ( LT^ | LTE^ | GT^ | GTE^ ) shiftExpr )*
        ;



shiftExpr
        :       additiveExpr
                ( ( LSHIFT^ | RSHIFT^ ) additiveExpr )*
        ;


additiveExpr
        :       multExpr
                ( ( PLUS^ | MINUS^ ) multExpr )*
        ;


multExpr
        :       castExpr
                ( ( STAR^ | DIV^ | MOD^ ) castExpr )*
        ;


castExpr
        :       ( LPAREN typeName RPAREN )=>
                LPAREN! typeName RPAREN! ( castExpr )
                            { ## = #( #[NCast, "("], ## ); }

        |       unaryExpr
        ;


typeName
        :       specifierQualifierList (nonemptyAbstractDeclarator)?
        ;

nonemptyAbstractDeclarator
        :   (
                pointerGroup
                (   (LPAREN  
                    (   nonemptyAbstractDeclarator
                        | parameterTypeList
                    )?
                    RPAREN)
                | (LBRACKET (expr)? RBRACKET)
                )*

            |   (   (LPAREN  
                    (   nonemptyAbstractDeclarator
                        | parameterTypeList
                    )?
                    RPAREN)
                | (LBRACKET (expr)? RBRACKET)
                )+
            )
                            {   ## = #( #[NNonemptyAbstractDeclarator], ## ); }
                                
        ;

/* JTC:

LR rules:

abstractDeclarator
        :       nonemptyAbstractDeclarator
        |       // null
        ;

nonemptyAbstractDeclarator
        :       LPAREN  nonemptyAbstractDeclarator RPAREN
        |       abstractDeclarator LPAREN RPAREN
        |       abstractDeclarator (LBRACKET (expr)? RBRACKET)
        |       STAR abstractDeclarator
        ;
*/

unaryExpr
        :       postfixExpr
        |       INC^ unaryExpr
        |       DEC^ unaryExpr
        |       u:unaryOperator castExpr { ## = #( #[NUnaryExpr], ## ); }

        |       "sizeof"^
                ( ( LPAREN typeName )=> LPAREN typeName RPAREN
                | unaryExpr
                )
        ;


unaryOperator
        :       BAND
        |       STAR
        |       PLUS
        |       MINUS
        |       BNOT
        |       LNOT
        ;

postfixExpr
        :       primaryExpr
                ( 
                postfixSuffix                   {## = #( #[NPostfixExpr], ## );} 
                )?
        ;
postfixSuffix
        :
                ( PTR ID
                | DOT ID
                | functionCall
                | LBRACKET expr RBRACKET
                | INC
                | DEC
                )+
        ;

functionCall
        :
                LPAREN^ (a:argExprList)? RPAREN
                        {
                        ##.setType( NFunctionCallArgs );
                        }
        ;
    

primaryExpr
        :       ID
        |       charConst
        |       intConst
        |       floatConst
        |       stringConst

// JTC:
// ID should catch the enumerator
// leaving it in gives ambiguous err
//      | enumerator
        |       LPAREN! expr RPAREN!        { ## = #( #[NExpressionGroup, "("], ## ); }
        ;

argExprList
        :       assignExpr ( COMMA! assignExpr )*
        ;



protected
charConst
        :       CharLiteral
        ;


protected
stringConst
        :       (StringLiteral)+                { ## = #(#[NStringSeq], ##); }
        ;


protected
intConst
        :       IntOctalConst
        |       LongOctalConst
        |       UnsignedOctalConst
        |       IntIntConst
        |       LongIntConst
        |       UnsignedIntConst
        |       IntHexConst
        |       LongHexConst
        |       UnsignedHexConst
        ;


protected
floatConst
        :       FloatDoubleConst
        |       DoubleDoubleConst
        |       LongDoubleConst
        ;




    

dummy
        :       NTypedefName
        |       NInitDecl
        |       NDeclarator
        |       NStructDeclarator
        |       NDeclaration
        |       NCast
        |       NPointerGroup
        |       NExpressionGroup
        |       NFunctionCallArgs
        |       NNonemptyAbstractDeclarator
        |       NInitializer
        |       NStatementExpr
        |       NEmptyExpression
        |       NParameterTypeList
        |       NFunctionDef
        |       NCompoundStatement
        |       NParameterDeclaration
        |       NCommaExpr
        |       NUnaryExpr
        |       NLabel
        |       NPostfixExpr
        |       NRangeExpr
        |       NStringSeq
        |       NInitializerElementLabel
        |       NLcurlyInitializer
        |       NAsmAttribute
        |       NGnuAsmExpr
        |       NTypeMissing
        ;



    


{
//        import CToken;
        import java.io.*;
//        import LineObject;
        import antlr.*;
}

class StdCLexer extends Lexer;

options
        {
        k = 3;
        exportVocab = STDC;
        testLiterals = false;
        }

{
  LineObject lineObject = new LineObject();
  String originalSource = "";
  PreprocessorInfoChannel preprocessorInfoChannel = new PreprocessorInfoChannel();
  int tokenNumber = 0;
  boolean countingTokens = true;
  int deferredLineCount = 0;

  public void setCountingTokens(boolean ct) 
  {
    countingTokens = ct;
    if ( countingTokens ) {
      tokenNumber = 0;
    }
    else {
      tokenNumber = 1;
    }
  }

  public void setOriginalSource(String src) 
  {
    originalSource = src;
    lineObject.setSource(src);
  }
  public void setSource(String src) 
  {
    lineObject.setSource(src);
  }
  
  public PreprocessorInfoChannel getPreprocessorInfoChannel() 
  {
    return preprocessorInfoChannel;
  }

  public void setPreprocessingDirective(String pre)
  {
    preprocessorInfoChannel.addLineForTokenNumber( pre, new Integer(tokenNumber) );
  }
  
  protected Token makeToken(int t)
  {
    if ( t != Token.SKIP && countingTokens) {
        tokenNumber++;
    }
    CToken tok = (CToken) super.makeToken(t);
    tok.setLine(lineObject.line);
    tok.setSource(lineObject.source);
    tok.setTokenNumber(tokenNumber);

    lineObject.line += deferredLineCount;
    deferredLineCount = 0;
    return tok;
  }

    public void deferredNewline() { 
        deferredLineCount++;
    }

    public void newline() { 
        lineObject.newline();
    }






}

protected
Vocabulary
        :       '\3'..'\377'
        ;


/* Operators: */

ASSIGN          : '=' ;
COLON           : ':' ;
COMMA           : ',' ;
QUESTION        : '?' ;
SEMI            : ';' ;
PTR             : "->" ;


// DOT & VARARGS are commented out since they are generated as part of
// the Number rule below due to some bizarre lexical ambiguity shme.

// DOT  :       '.' ;
protected
DOT:;

// VARARGS      : "..." ;
protected
VARARGS:;


LPAREN          : '(' ;
RPAREN          : ')' ;
LBRACKET        : '[' ;
RBRACKET        : ']' ;
LCURLY          : '{' ;
RCURLY          : '}' ;

EQUAL           : "==" ;
NOT_EQUAL       : "!=" ;
LTE             : "<=" ;
LT              : "<" ;
GTE             : ">=" ;
GT              : ">" ;

DIV             : '/' ;
DIV_ASSIGN      : "/=" ;
PLUS            : '+' ;
PLUS_ASSIGN     : "+=" ;
INC             : "++" ;
MINUS           : '-' ;
MINUS_ASSIGN    : "-=" ;
DEC             : "--" ;
STAR            : '*' ;
STAR_ASSIGN     : "*=" ;
MOD             : '%' ;
MOD_ASSIGN      : "%=" ;
RSHIFT          : ">>" ;
RSHIFT_ASSIGN   : ">>=" ;
LSHIFT          : "<<" ;
LSHIFT_ASSIGN   : "<<=" ;

LAND            : "&&" ;
LNOT            : '!' ;
LOR             : "||" ;

BAND            : '&' ;
BAND_ASSIGN     : "&=" ;
BNOT            : '~' ;
BOR             : '|' ;
BOR_ASSIGN      : "|=" ;
BXOR            : '^' ;
BXOR_ASSIGN     : "^=" ;


Whitespace
        :       ( ( '\003'..'\010' | '\t' | '\013' | '\f' | '\016'.. '\037' | '\177'..'\377' | ' ' )
                | "\r\n"                { newline(); }
                | ( '\n' | '\r' )       { newline(); }
                )                       { _ttype = Token.SKIP;  }
        ;


Comment
        :       "/*"
                ( { LA(2) != '/' }? '*'
                | "\r\n"                { deferredNewline(); }
                | ( '\r' | '\n' )       { deferredNewline();    }
                | ~( '*'| '\r' | '\n' )
                )*
                "*/"                    { _ttype = Token.SKIP;  
                                        }
        ;


CPPComment
        :
                "//" ( ~('\n') )* 
                        {
                        _ttype = Token.SKIP;
                        }
        ;

PREPROC_DIRECTIVE
options {
  paraphrase = "a line directive";
}

        :
        '#'
        ( ( "line" || (( ' ' | '\t' | '\014')+ '0'..'9')) => LineDirective      
            | (~'\n')*                                  { setPreprocessingDirective(getText()); }
        )
                {  
                    _ttype = Token.SKIP;
                }
        ;

protected  Space:
        ( ' ' | '\t' | '\014')
        ;

protected LineDirective
{
        boolean oldCountingTokens = countingTokens;
        countingTokens = false;
}
:
                {
                        lineObject = new LineObject();
                        deferredLineCount = 0;
                }
        ("line")?  //this would be for if the directive started "#line", but not there for GNU directives
        (Space)+
        n:Number { lineObject.setLine(Integer.parseInt(n.getText())); } 
        (Space)+
        (       fn:StringLiteral {  try { 
                                          lineObject.setSource(fn.getText().substring(1,fn.getText().length()-1)); 
                                    } 
                                    catch (StringIndexOutOfBoundsException e) { /*not possible*/ } 
                                 }
                | fi:ID { lineObject.setSource(fi.getText()); }
        )?
        (Space)*
        ("1"            { lineObject.setEnteringFile(true); } )?
        (Space)*
        ("2"            { lineObject.setReturningToFile(true); } )?
        (Space)*
        ("3"            { lineObject.setSystemHeader(true); } )?
        (Space)*
        ("4"            { lineObject.setTreatAsC(true); } )?
        (~('\r' | '\n'))*
        ("\r\n" | "\r" | "\n")
                {
                        preprocessorInfoChannel.addLineForTokenNumber(new LineObject(lineObject), new Integer(tokenNumber));
                        countingTokens = oldCountingTokens;
                }
        ;



/* Literals: */

/*
 * Note that we do NOT handle tri-graphs nor multi-byte sequences.
 */


/*
 * Note that we can't have empty character constants (even though we
 * can have empty strings :-).
 */
CharLiteral
        :       '\'' ( Escape | ~( '\'' ) ) '\''
        ;


/*
 * Can't have raw imbedded newlines in string constants.  Strict reading of
 * the standard gives odd dichotomy between newlines & carriage returns.
 * Go figure.
 */
StringLiteral
        :       '"'
                ( Escape
                | ( 
                    '\r'        { deferredNewline(); }
                  | '\n'        {
                                deferredNewline();
                                _ttype = BadStringLiteral;
                                }
                  | '\\' '\n'   {
                                deferredNewline();
                                }
                  )
                | ~( '"' | '\r' | '\n' | '\\' )
                )*
                '"'
        ;


protected BadStringLiteral
        :       // Imaginary token.
        ;


/*
 * Handle the various escape sequences.
 *
 * Note carefully that these numeric escape *sequences* are *not* of the
 * same form as the C language numeric *constants*.
 *
 * There is no such thing as a binary numeric escape sequence.
 *
 * Octal escape sequences are either 1, 2, or 3 octal digits exactly.
 *
 * There is no such thing as a decimal escape sequence.
 *
 * Hexadecimal escape sequences are begun with a leading \x and continue
 * until a non-hexadecimal character is found.
 *
 * No real handling of tri-graph sequences, yet.
 */

protected
Escape  
        :       '\\'
                ( options{warnWhenFollowAmbig=false;}:
                  'a'
                | 'b'
                | 'f'
                | 'n'
                | 'r'
                | 't'
                | 'v'
                | '"'
                | '\''
                | '\\'
                | '?'
                | ('0'..'3') ( options{warnWhenFollowAmbig=false;}: Digit ( options{warnWhenFollowAmbig=false;}: Digit )? )?
                | ('4'..'7') ( options{warnWhenFollowAmbig=false;}: Digit )?
                | 'x' ( options{warnWhenFollowAmbig=false;}: Digit | 'a'..'f' | 'A'..'F' )+
                )
        ;


/* Numeric Constants: */

protected
Digit
        :       '0'..'9'
        ;

protected
LongSuffix
        :       'l'
        |       'L'
        ;

protected
UnsignedSuffix
        :       'u'
        |       'U'
        ;

protected
FloatSuffix
        :       'f'
        |       'F'
        ;

protected
Exponent
        :       ( 'e' | 'E' ) ( '+' | '-' )? ( Digit )+
        ;


protected
DoubleDoubleConst:;

protected
FloatDoubleConst:;

protected
LongDoubleConst:;

protected
IntOctalConst:;

protected
LongOctalConst:;

protected
UnsignedOctalConst:;

protected
IntIntConst:;

protected
LongIntConst:;

protected
UnsignedIntConst:;

protected
IntHexConst:;

protected
LongHexConst:;

protected
UnsignedHexConst:;




Number
        :       ( ( Digit )+ ( '.' | 'e' | 'E' ) )=> ( Digit )+
                ( '.' ( Digit )* ( Exponent )?
                | Exponent
                )                       { _ttype = DoubleDoubleConst;   }
                ( FloatSuffix           { _ttype = FloatDoubleConst;    }
                | LongSuffix            { _ttype = LongDoubleConst;     }
                )?

        |       ( "..." )=> "..."       { _ttype = VARARGS;     }

        |       '.'                     { _ttype = DOT; }
                ( ( Digit )+ ( Exponent )?
                                        { _ttype = DoubleDoubleConst;   }
                  ( FloatSuffix         { _ttype = FloatDoubleConst;    }
                  | LongSuffix          { _ttype = LongDoubleConst;     }
                  )?
                )?

        |       '0' ( '0'..'7' )*       { _ttype = IntOctalConst;       }
                ( LongSuffix            { _ttype = LongOctalConst;      }
                | UnsignedSuffix        { _ttype = UnsignedOctalConst;  }
                )?

        |       '1'..'9' ( Digit )*     { _ttype = IntIntConst;         }
                ( LongSuffix            { _ttype = LongIntConst;        }
                | UnsignedSuffix        { _ttype = UnsignedIntConst;    }
                )?

        |       '0' ( 'x' | 'X' ) ( 'a'..'f' | 'A'..'F' | Digit )+
                                        { _ttype = IntHexConst;         }
                ( LongSuffix            { _ttype = LongHexConst;        }
                | UnsignedSuffix        { _ttype = UnsignedHexConst;    }
                )?
        ;


ID
        options 
                {
                testLiterals = true; 
                }
        :       ( 'a'..'z' | 'A'..'Z' | '_' )
                ( 'a'..'z' | 'A'..'Z' | '_' | '0'..'9' )*
        ;



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        Copyright (c) Non, Inc. 1998 -- All Rights Reserved

PROJECT:        C Compiler
MODULE:         WParser
FILE:           WParser.g

AUTHOR:         Monty Zukowski (jamz@cdsnet.net) April 28, 1998
MODIFICATIONS:  Hernando Barragan added support for the Wiring language
DESCRIPTION:
                This is a grammar for the GNU C compiler.  It is a
                grammar subclass of StdCParser, overriding only those
                rules which are different from Standard C.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

    
header {
package processing.app.preproc;

import java.io.*;

import antlr.CommonAST;
import antlr.DumpASTVisitor;
}

           
class WParser extends StdCParser;

options
        {
        k = 2;
        exportVocab = W;
        buildAST = true;
        ASTLabelType = "TNode";
        defaultErrorHandler = false;
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
        :       ( externalList )?       /* Empty source files are allowed.  */
        ;
asm_expr
        :       "asm"^ 
                ("volatile")? LCURLY expr RCURLY ( SEMI )+
        ;

idList
        :       ID ( options{warnWhenFollowAmbig=false;}: COMMA ID )*
        ;

externalDef
        :       ( "typedef" | declaration )=> declaration
        |       ( functionPrefix )=> functionDef
        |       typelessDeclaration
        |       asm_expr
        |       SEMI
        ;

/* these two are here because GCC allows "cat = 13;" as a valid program! */
functionPrefix
                            { String declName; }
        :       ( (functionDeclSpecifiers)=> ds:functionDeclSpecifiers
                |  //epsilon
                )
                declName = d:declarator[true]
                ( declaration )* (VARARGS)? ( SEMI )*
                LCURLY
        ;

typelessDeclaration
                        { AST typeMissing = #[NTypeMissing]; }
        :       initDeclList[typeMissing] SEMI          { ## = #( #[NTypeMissing], ##); }
        ;

initializer
        : ( ( ( (initializerElementLabel)=> initializerElementLabel )?
                ( assignExpr | lcurlyInitializer )  { ## = #( #[NInitializer], ## ); }
              )
              | lcurlyInitializer
              )
        ;

// GCC allows more specific initializers
initializerElementLabel
        :   (   ( LBRACKET ((constExpr VARARGS)=> rangeExpr | constExpr) RBRACKET (ASSIGN)? )
                | ID COLON
                | DOT ID ASSIGN
            )
                                    { ## = #( #[NInitializerElementLabel], ##) ; }
        ;

// GCC allows empty initializer lists
lcurlyInitializer 
        :    
                LCURLY^ (initializerList ( COMMA! )? )? RCURLY
                            { ##.setType( NLcurlyInitializer ); }
        ;
    
initializerList
        :       initializer ( options{warnWhenFollowAmbig=false;}:COMMA! initializer )*
        ;
    

declarator[boolean isFunctionDefinition] returns [String declName]
                                                { declName = ""; }
        :
                ( pointerGroup )?               

                ( id:ID                         { declName = id.getText(); }
                | LPAREN declName = declarator[false] RPAREN
                )

                ( declaratorParamaterList[isFunctionDefinition, declName]
                | LBRACKET ( expr )? RBRACKET
                )*
                                                { ## = #( #[NDeclarator], ## ); }
        ;

declaratorParamaterList[boolean isFunctionDefinition, String declName]
        :
                LPAREN^
                                                { 
                                                    if (isFunctionDefinition) {
                                                        pushScope(declName);
                                                    }
                                                    else {
                                                        pushScope("!"+declName); 
                                                    }
                                                }
                (                           
                        (declSpecifiers)=> parameterTypeList
                        | (idList)?
                )
                                                {
                                                popScope();
                                                }    
                ( COMMA! )?
                RPAREN       
                                                { ##.setType(NParameterTypeList); }      
        ;
           
parameterTypeList
        :       parameterDeclaration
                (   options {
                            warnWhenFollowAmbig = false;
                        } : 
                  ( COMMA | SEMI )  
                  parameterDeclaration
                )*
                ( ( COMMA | SEMI ) 
                  VARARGS
                )?
        ;


declarationList
         :       (               options {   // this loop properly aborts when
                                            // it finds a non-typedefName ID MBZ
                                            warnWhenFollowAmbig = false;
                                        } :
    
                localLabelDeclaration
                |  ( declarationPredictor )=> declaration
                )+
        ;
localLabelDeclaration    
        :       ( //GNU note:  any __label__ declarations must come before regular declarations.
                "__label__"^ ID (options{warnWhenFollowAmbig=false;}: COMMA! ID)* ( COMMA! )? ( SEMI! )+
                )
        ;

    
declaration
                                        { AST ds1 = null; }
        :       ds:declSpecifiers       { ds1 = astFactory.dupList(#ds); }
                (                       
                    initDeclList[ds1]
                )?
                ( SEMI )+
                                        { ## = #( #[NDeclaration], ##); }
                
        ;

functionStorageClassSpecifier
        :       "extern"
        |       "static"
        |       "inline"
        ;

typeSpecifier [int specCount] returns [int retSpecCount]
                                                        { retSpecCount = specCount + 1; }
        :       
        ( "void"
        |       "char"
        |       "short"
        |       "int"
        |       "long"
        |       "float"
        |       "double"
        |       "signed"
        |       "unsigned"
        |       "byte"
        |       "boolean"
        |       "Servo"
        |       "Wire"
        |       structOrUnionSpecifier  ( options{warnWhenFollowAmbig=false;}: attributeDecl )*
        |       enumSpecifier
        |       { specCount==0 }? typedefName
        |       "typeof"^ LPAREN
                ( ( typeName )=> typeName
                | expr
                )
                RPAREN
        |       "__complex"
        )
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
                        ( structDeclarationList )?
                                            { popScope();}
                        RCURLY
                |   l1:LCURLY
                                            {
                                            scopeName = getAScopeName();
                                            #l1.setText(scopeName);
                                            pushScope(scopeName);
                                            }
                    ( structDeclarationList )?
                                            { popScope(); }
                    RCURLY
                | ID
                )
                                            {
                                            ## = #( #sou, ## );
                                            }
        ;


structDeclaration
        :       specifierQualifierList structDeclaratorList ( COMMA! )? ( SEMI! )+
        ;

structDeclaratorList
        :       structDeclarator ( options{warnWhenFollowAmbig=false;}: COMMA! structDeclarator )*
        ;

structDeclarator
        :       ( declarator[false] )?
                ( COLON constExpr )?
                ( attributeDecl )*
                                    { ## = #( #[NStructDeclarator], ##); }
        ;



enumSpecifier
        :       "enum"^
                ( ( ID LCURLY )=> i:ID LCURLY enumList[i.getText()] RCURLY
                | LCURLY enumList["anonymous"] RCURLY
                | ID
                )
        ;
enumList[String enumName]
        :       enumerator[enumName] ( options{warnWhenFollowAmbig=false;}: COMMA! enumerator[enumName] )* ( COMMA! )?
        ;


initDeclList[AST declarationSpecifiers]
        :       initDecl[declarationSpecifiers] 
                ( options{warnWhenFollowAmbig=false;}: COMMA! initDecl[declarationSpecifiers] )*
                ( COMMA! )?
        ;

initDecl[AST declarationSpecifiers]
                                        { String declName = ""; }
        :       declName = d:declarator[false]
                                        {   AST ds1, d1;
                                            ds1 = astFactory.dupList(declarationSpecifiers);
                                            d1 = astFactory.dupList(#d);
                                            symbolTable.add(declName, #(null, ds1, d1) );
                                        }
                ( attributeDecl )*
                ( ASSIGN initializer
                | COLON expr
                )?
                                        { ## = #( #[NInitDecl], ## ); }
        ;

attributeDecl
        :       "__attribute"^ LPAREN LPAREN attributeList RPAREN RPAREN
                | "asm"^ LPAREN stringConst RPAREN { ##.setType( NAsmAttribute ); }
        ;

attributeList
        :       attribute ( options{warnWhenFollowAmbig=false;}: COMMA attribute)*  ( COMMA )?
        ;

attribute
        :       ( ~(LPAREN | RPAREN | COMMA)
                |  LPAREN attributeList RPAREN
                )*
        ;
compoundStatement[String scopeName]
        :       LCURLY^

                            {
                                pushScope(scopeName);
                            }
                (       //this ambiguity is ok, declarationList and nestedFunctionDef end properly
                        options {
                            warnWhenFollowAmbig = false;
                        } :
                    ( "typedef" | "__label__" | declaration )=> declarationList
                    | (nestedFunctionDef)=> nestedFunctionDef
                )*
                ( statementList )?
                            { popScope(); }
                RCURLY
                            { ##.setType( NCompoundStatement ); ##.setAttribute( "scopeName", scopeName ); }
        ;

nestedFunctionDef
                            { String declName; }
        :       ( "auto" )? //only for nested functions
                ( (functionDeclSpecifiers)=> ds:functionDeclSpecifiers
                )?
                declName = d:declarator[false]
                            {
                            AST d2, ds2;
                            d2 = astFactory.dupList(#d);
                            ds2 = astFactory.dupList(#ds);
                            symbolTable.add(declName, #(null, ds2, d2));
                            pushScope(declName);
                            }
                ( declaration )*
                            { popScope(); }
                compoundStatement[declName]
                            { ## = #( #[NFunctionDef], ## );}
        ;

statement
        :       SEMI                    // Empty statements
        
        |       compoundStatement[getAScopeName()]       // Group of statements

        |       expr SEMI!               { ## = #( #[NStatementExpr], ## );} // Expressions

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

        |       "goto"^ expr SEMI!
        |       "continue" SEMI!
        |       "break" SEMI!
        |       "return"^ ( expr )? SEMI!


        |       ID COLON! (options {warnWhenFollowAmbig=false;}: statement)?  { ## = #( #[NLabel], ## ); }
// GNU allows range expressions in case statements
        |       "case"^ ((constExpr VARARGS)=> rangeExpr | constExpr) COLON! ( options{warnWhenFollowAmbig=false;}:statement )?
        |       "default"^ COLON! ( options{warnWhenFollowAmbig=false;}: statement )?

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



conditionalExpr
        :       logicalOrExpr
                ( QUESTION^ (expr)? COLON conditionalExpr )?
        ;

rangeExpr   //used in initializers only
        :  constExpr VARARGS constExpr
                                { ## = #(#[NRangeExpr], ##); }
        ;

castExpr
        :       ( LPAREN typeName RPAREN )=>
                LPAREN^ typeName RPAREN ( castExpr | lcurlyInitializer )
                            { ##.setType(NCast); }

        |       unaryExpr
        ;
nonemptyAbstractDeclarator
        :   (
                pointerGroup
                (   (LPAREN  
                    (   nonemptyAbstractDeclarator
                        | parameterTypeList
                    )?
                    ( COMMA! )?
                    RPAREN)
                | (LBRACKET (expr)? RBRACKET)
                )*

            |   (   (LPAREN  
                    (   nonemptyAbstractDeclarator
                        | parameterTypeList
                    )?
                    ( COMMA! )?
                    RPAREN)
                | (LBRACKET (expr)? RBRACKET)
                )+
            )
                            {   ## = #( #[NNonemptyAbstractDeclarator], ## ); }
                                
        ;



unaryExpr
        :       postfixExpr
        |       INC^ castExpr
        |       DEC^ castExpr
        |       u:unaryOperator castExpr { ## = #( #[NUnaryExpr], ## ); }

        |       "sizeof"^
                ( ( LPAREN typeName )=> LPAREN typeName RPAREN
                | unaryExpr
                )
        |       "__alignof"^
                ( ( LPAREN typeName )=> LPAREN typeName RPAREN
                | unaryExpr
                )       
        |       gnuAsmExpr
        ;

unaryOperator
        :       BAND
        |       STAR
        |       PLUS
        |       MINUS
        |       BNOT    //also stands for complex conjugation
        |       LNOT
        |       LAND    //for label dereference (&&label)
        |       "__real"
        |       "__imag"
        ;

gnuAsmExpr
        :       "asm"^ ("volatile")? 
                LPAREN stringConst
                ( options { warnWhenFollowAmbig = false; }:
                  COLON (strOptExprPair ( COMMA strOptExprPair)* )?
                  ( options { warnWhenFollowAmbig = false; }:
                    COLON (strOptExprPair ( COMMA strOptExprPair)* )?
                  )?
                )?
                ( COLON stringConst ( COMMA stringConst)* )?
                RPAREN
                                { ##.setType(NGnuAsmExpr); }
        ;

//GCC requires the PARENs
strOptExprPair
        :  stringConst ( LPAREN expr RPAREN )?
        ;


primaryExpr
        :       ID
        |       Number
        |       charConst
        |       stringConst
// JTC:
// ID should catch the enumerator
// leaving it in gives ambiguous err
//      | enumerator
        |       (LPAREN LCURLY) => LPAREN^ compoundStatement[getAScopeName()] RPAREN
        |       LPAREN^ expr RPAREN        { ##.setType(NExpressionGroup); }
        ;
    

{
//        import CToken;
        import java.io.*;
//        import LineObject;
        import antlr.*;
}

class WLexer extends StdCLexer;
options 
        {
        k = 3;
        importVocab = W;
        testLiterals = false;
        }    
tokens {
        LITERAL___extension__ = "__extension__";
}

{
  public void initialize(String src)
  {
    setOriginalSource(src);
    initialize();
  }

  public void initialize() 
  {
    literals.put(new ANTLRHashString("__alignof__", this), new Integer(LITERAL___alignof));
    literals.put(new ANTLRHashString("__asm", this), new Integer(LITERAL_asm));
    literals.put(new ANTLRHashString("__asm__", this), new Integer(LITERAL_asm));
    literals.put(new ANTLRHashString("__attribute__", this), new Integer(LITERAL___attribute));
    literals.put(new ANTLRHashString("__complex__", this), new Integer(LITERAL___complex));
    literals.put(new ANTLRHashString("__const", this), new Integer(LITERAL_const));
    literals.put(new ANTLRHashString("__const__", this), new Integer(LITERAL_const));
    literals.put(new ANTLRHashString("__imag__", this), new Integer(LITERAL___imag));
    literals.put(new ANTLRHashString("__inline", this), new Integer(LITERAL_inline));
    literals.put(new ANTLRHashString("__inline__", this), new Integer(LITERAL_inline));
    literals.put(new ANTLRHashString("__real__", this), new Integer(LITERAL___real));
    literals.put(new ANTLRHashString("__signed", this), new Integer(LITERAL_signed));
    literals.put(new ANTLRHashString("__signed__", this), new Integer(LITERAL_signed));
    literals.put(new ANTLRHashString("__typeof", this), new Integer(LITERAL_typeof));
    literals.put(new ANTLRHashString("__typeof__", this), new Integer(LITERAL_typeof));
    literals.put(new ANTLRHashString("__volatile", this), new Integer(LITERAL_volatile));
    literals.put(new ANTLRHashString("__volatile__", this), new Integer(LITERAL_volatile));
  }


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
Whitespace
        :       ( ( ' ' | '\t' | '\014')
                | "\r\n"                { newline(); }
                | ( '\n' | '\r' )       { newline();    }
                )                       { _ttype = Token.SKIP;  }
        ;


protected
Escape
        :       '\\'
                ( options{warnWhenFollowAmbig=false;}: 
                  ~('0'..'7' | 'x')
                | ('0'..'3') ( options{warnWhenFollowAmbig=false;}: Digit )*
                | ('4'..'7') ( options{warnWhenFollowAmbig=false;}: Digit )*
                | 'x' ( options{warnWhenFollowAmbig=false;}: Digit | 'a'..'f' | 'A'..'F' )+
                )
        ;

protected IntSuffix
        :   'L'
            | 'l'
            | 'U'
            | 'u'
            | 'I'
            | 'i'
            | 'J'
            | 'j'
        ;
protected NumberSuffix
        :
            IntSuffix
            | 'F'
            | 'f'
        ;
    
Number
        :       ( ( Digit )+ ( '.' | 'e' | 'E' ) )=> ( Digit )+
                ( '.' ( Digit )* ( Exponent )?
                | Exponent
                ) 
                ( NumberSuffix
                )*

        |       ( "..." )=> "..."       { _ttype = VARARGS;     }

        |       '.'                     { _ttype = DOT; }
                ( ( Digit )+ ( Exponent )?
                                        { _ttype = Number;   }
                    ( NumberSuffix
                    )*
                )?

        |       '0' ( '0'..'7' )*       
                ( NumberSuffix
                )*

        |       '1'..'9' ( Digit )*     
                ( NumberSuffix
                )*

        |       '0' ( 'x' | 'X' ) ( 'a'..'f' | 'A'..'F' | Digit )+
                ( IntSuffix
                )*
        ;

IDMEAT
        :
                i:ID                {
                                        
                                        if ( i.getType() == LITERAL___extension__ ) {
                                                $setType(Token.SKIP);
                                        }
                                        else {
                                                $setType(i.getType());
                                        }
                                        
                                    }
        ;

protected ID
        options 
                {
                testLiterals = true; 
                }
        :       ( 'a'..'z' | 'A'..'Z' | '_' | '$')
                ( 'a'..'z' | 'A'..'Z' | '_' | '$' | '0'..'9' )*
        ;

WideCharLiteral
        :
                'L' CharLiteral
                                { $setType(CharLiteral); }
        ;



WideStringLiteral
        :
                'L' StringLiteral
                                { $setType(StringLiteral); }
        ;

StringLiteral
        :
                '"'
                ( ('\\' ~('\n'))=> Escape
                | ( '\r'        { newline(); }
                  | '\n'        {
                                newline();
                                }
                  | '\\' '\n'   {
                                newline();
                                }
                  )
                | ~( '"' | '\r' | '\n' | '\\' )
                )*
                '"'
        ;

            



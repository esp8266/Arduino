/* -*- mode: antlr; c-basic-offset: 4; indent-tabs-mode: nil -*- */
header {
package processing.app.preproc;

import processing.app.*;
}

class PdeRecognizer extends JavaRecognizer;

options {
    importVocab = Java;
    exportVocab = PdePartial;

    codeGenMakeSwitchThreshold=10; // this is set high for debugging
    codeGenBitsetTestThreshold=10; // this is set high for debugging

    // developers may to want to set this to true for better
    // debugging messages, however, doing so disables highlighting errors
    // in the editor.
    defaultErrorHandler = false; //true;
}

tokens {
    CONSTRUCTOR_CAST; EMPTY_FIELD;
}

pdeProgram
        // only java mode programs will have their own public classes or
        // imports (and they must have at least one)
    :   ( "public" "class" | "import" ) => javaProgram
        { PdePreprocessor.programType = PdePreprocessor.JAVA; }

        // the syntactic predicate here looks for any minimal (thus
        // the non-greedy qualifier) number of fields, followed by
        // the tokens that represent the definition of loop() or
        // some other member function.  java mode programs may have such
        // definitions, but they won't reach this point, having already been
        // selected in the previous alternative.  static mode programs 
        // don't have member functions.
        //
    |   ( ( options {greedy=false;}: possiblyEmptyField)* "void" IDENT LPAREN ) 
        => activeProgram
        { PdePreprocessor.programType = PdePreprocessor.ACTIVE; }

    |   staticProgram
        { PdePreprocessor.programType = PdePreprocessor.STATIC; }
    ;

// advanced mode is really just a normal java file
javaProgram
    :   compilationUnit
    ;

activeProgram
    :  (possiblyEmptyField)+
    ;

staticProgram
    :  (statement)*
    ; 

// copy of the java.g rule with WEBCOLOR_LITERAL added
constant
    :   NUM_INT
    |   CHAR_LITERAL
    |   STRING_LITERAL
    |   NUM_FLOAT
    |   NUM_LONG
    |   NUM_DOUBLE
    |   webcolor_literal
    ; 

// of the form #cc008f in PDE
webcolor_literal
    :   w:WEBCOLOR_LITERAL 
    { Preferences.getBoolean("preproc.web_colors") && 
      w.getText().length() == 6 }?  // must be exactly 6 hex digits
    ;

// copy of the java.g builtInType rule
builtInConsCastType
    :   "void"
    |   "boolean"
    |   "byte"
    |   "char"
    |   "short"
    |   "int"
    |   "float"
    |   "long"
    |   "double"
    ;

// our types include the java types and "color".  this is separated into two
// rules so that constructor casts can just use the original typelist, since
// we don't want to support the color type as a constructor cast.
//
builtInType
    :   builtInConsCastType
    |   "color"              // aliased to an int in PDE
        { Preferences.getBoolean("preproc.color_datatype") }? 
    ;

// constructor style casts.
constructorCast!
    :   t:consCastTypeSpec[true]
        LPAREN!
        e:expression
        RPAREN!
        // if this is a string literal, make sure the type we're trying to cast
        // to is one of the supported ones
        //
        { #e.getType() != STRING_LITERAL ||
            ( #t.getType() == LITERAL_byte ||
              #t.getType() == LITERAL_double ||
              #t.getType() == LITERAL_float ||
              #t.getType() == LITERAL_int ||
              #t.getType() == LITERAL_long ||
              #t.getType() == LITERAL_short ) }?
        // create the node
        //
        {#constructorCast = #(#[CONSTRUCTOR_CAST,"CONSTRUCTOR_CAST"], t, e);}
    ;

// A list of types that be used as the destination type in a constructor-style
// cast.  Ideally, this would include all class types, not just "String".  
// Unfortunately, it's not possible to tell whether Foo(5) is supposed to be
// a method call or a constructor cast without have a table of all valid
// types or methods, which requires semantic analysis (eg processing of import
// statements).  So we accept the set of built-in types plus "String".
//
consCastTypeSpec[boolean addImagNode]
//	: stringTypeSpec[addImagNode]
//	| builtInConsCastTypeSpec[addImagNode]
        : builtInConsCastTypeSpec[addImagNode]
// trying to remove String() cast [fry]
	;

//stringTypeSpec[boolean addImagNode]
//    : id:IDENT { #id.getText().equals("String") }?
//        {
//            if ( addImagNode ) {
//                #stringTypeSpec = #(#[TYPE,"TYPE"], 
//                                   #stringTypeSpec);
//            }
//        }
//    ;

builtInConsCastTypeSpec[boolean addImagNode]
    :    builtInConsCastType
             {
                 if ( addImagNode ) {
                     #builtInConsCastTypeSpec = #(#[TYPE,"TYPE"],
                                                  #builtInConsCastTypeSpec);
                 }
             }
    ;

// Since "color" tokens are lexed as LITERAL_color now, we need to have a rule
// that can generate a method call from an expression that starts with this
// token
//
colorMethodCall
    : c:"color" {#c.setType(IDENT);} // this would default to LITERAL_color
      lp:LPAREN^ {#lp.setType(METHOD_CALL);}
      argList
      RPAREN!
    ;  

// copy of the java.g rule with added constructorCast and colorMethodCall 
// alternatives
primaryExpression
    :   (consCastTypeSpec[false] LPAREN) => constructorCast   
            { Preferences.getBoolean("preproc.enhanced_casting") }?
    |   identPrimary ( options {greedy=true;} : DOT^ "class" )?
    |   constant
    |   "true"
    |   "false"
    |   "null"
    |   newExpression
    |   "this"
    |   "super"
    |   LPAREN! assignmentExpression RPAREN!
    |   colorMethodCall
        // look for int.class and int[].class
    |   builtInType
        ( lbt:LBRACK^ {#lbt.setType(ARRAY_DECLARATOR);} RBRACK! )*
        DOT^ "class"
    ;

// the below variable rule hacks are needed so that it's possible for the
// emitter to correctly output variable declarations of the form "float a, b"
// from the AST.  This means that our AST has a somewhat different form in
// these rules than the java one does, and this new form may have its own 
// semantic issues.  But it seems to fix the comma declaration issues.
//
variableDefinitions![AST mods, AST t]
    :	vd:variableDeclarator[getASTFactory().dupTree(mods),
                              getASTFactory().dupTree(t)]
        {#variableDefinitions = #(#[VARIABLE_DEF,"VARIABLE_DEF"], mods, 
                                  t, vd);}
    ;
variableDeclarator[AST mods, AST t]
    :	( id:IDENT 	(lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);} RBRACK!)*
        v:varInitializer (COMMA!)? )+
    ; 

// java.g builds syntax trees with an inconsistent structure.  override one of
// the rules there to fix this.
//
explicitConstructorInvocation!
    :   t:"this" LPAREN a1:argList RPAREN SEMI
        {#explicitConstructorInvocation = #(#[CTOR_CALL, "CTOR_CALL"], 
                                            #t, #a1);}
    |   s:"super" LPAREN a2:argList RPAREN SEMI
        {#explicitConstructorInvocation = #(#[SUPER_CTOR_CALL, 
                                              "SUPER_CTOR_CALL"], 
                                            #s, #a2);}
    ;

// quick-n-dirty hack to the get the advanced class name.  we should 
// really be getting it from the AST and not forking this rule from
// the java.g copy at all.  Since this is a recursive descent parser, we get
// the last class name in the file so that we don't end up with the classname
// of an inner class.  If there is more than one "outer" class in a file,
// this heuristic will fail.
//
classDefinition![AST modifiers]
    :   "class" i:IDENT
        // it _might_ have a superclass...
        sc:superClassClause
        // it might implement some interfaces...
        ic:implementsClause
        // now parse the body of the class
        cb:classBlock
        {#classDefinition = #(#[CLASS_DEF,"CLASS_DEF"],
                              modifiers,i,sc,ic,cb);
         PdePreprocessor.advClassName = i.getText();}
    ;

possiblyEmptyField
    : field
    | s:SEMI {#s.setType(EMPTY_FIELD);}
    ;

class PdeLexer extends JavaLexer;

options {
    importVocab=PdePartial;
    exportVocab=Pde;
}

// We need to preserve whitespace and commentary instead of ignoring
// like the supergrammar does.  Otherwise Jikes won't be able to give
// us error messages that point to the equivalent PDE code.

// WS, SL_COMMENT, ML_COMMENT are copies of the original productions,
// but with the SKIP assigment removed.

WS  :   (   ' '
        |   '\t'
        |   '\f'
            // handle newlines
        |   (   options {generateAmbigWarnings=false;}
            :   "\r\n"  // Evil DOS
            |   '\r'    // Macintosh
            |   '\n'    // Unix (the right way)
            )
            { newline(); }
        )+
    ;

// Single-line comments
SL_COMMENT
    :   "//"
        (~('\n'|'\r'))* ('\n'|'\r'('\n')?)
        {newline();}
    ;

// multiple-line comments
ML_COMMENT
    :   "/*"
        (   /*  '\r' '\n' can be matched in one alternative or by matching
                '\r' in one iteration and '\n' in another.  I am trying to
                handle any flavor of newline that comes in, but the language
                that allows both "\r\n" and "\r" and "\n" to all be valid
                newline is ambiguous.  Consequently, the resulting grammar
                must be ambiguous.  I'm shutting this warning off.
             */
            options {
                generateAmbigWarnings=false;
            }
        :
            { LA(2)!='/' }? '*'
        |   '\r' '\n'       {newline();}
        |   '\r'            {newline();}
        |   '\n'            {newline();}
        |   ~('*'|'\n'|'\r')
        )*
        "*/"
    ;

WEBCOLOR_LITERAL
    : '#'! (HEX_DIGIT)+
    ;

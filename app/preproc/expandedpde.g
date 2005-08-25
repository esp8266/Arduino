header {
package processing.app.preproc;

import processing.app.*;
}
class PdeRecognizer extends Parser;

options {
	importVocab= Java;
	exportVocab= PdePartial;
	codeGenMakeSwitchThreshold=10;
	codeGenBitsetTestThreshold=10;
	defaultErrorHandler= false;
	k= 2;
	buildAST= true;
}

tokens {
    CONSTRUCTOR_CAST; EMPTY_FIELD;
}
pdeProgram :( "public" "class" | "import" ) => javaProgram
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

javaProgram :compilationUnit
    ;

activeProgram :(possiblyEmptyField)+
    ;

staticProgram :(statement)*
    ;

constant :NUM_INT
    |   CHAR_LITERAL
    |   STRING_LITERAL
    |   NUM_FLOAT
    |   NUM_LONG
    |   NUM_DOUBLE
    |   webcolor_literal
    ;

webcolor_literal :w:WEBCOLOR_LITERAL 
    { Preferences.getBoolean("preproc.web_colors") && 
      w.getText().length() == 6 }?  // must be exactly 6 hex digits
    ;

builtInConsCastType :"void"
    |   "boolean"
    |   "byte"
    |   "char"
    |   "short"
    |   "int"
    |   "float"
    |   "long"
    |   "double"
    ;

builtInType :builtInConsCastType
    |   "color"              // aliased to an int in PDE
        { Preferences.getBoolean("preproc.color_datatype") }? 
    ;

constructorCast! :t:consCastTypeSpec[true]
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

consCastTypeSpec[boolean addImagNode] :builtInConsCastTypeSpec[addImagNode]
// trying to remove String() cast [fry]
	;

builtInConsCastTypeSpec[boolean addImagNode] :builtInConsCastType
             {
                 if ( addImagNode ) {
                     #builtInConsCastTypeSpec = #(#[TYPE,"TYPE"],
                                                  #builtInConsCastTypeSpec);
                 }
             }
    ;

colorMethodCall :c:"color" {#c.setType(IDENT);} // this would default to LITERAL_color
      lp:LPAREN^ {#lp.setType(METHOD_CALL);}
      argList
      RPAREN!
    ;

primaryExpression :(consCastTypeSpec[false] LPAREN) => constructorCast   
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

variableDefinitions![AST mods, AST t] :vd:variableDeclarator[getASTFactory().dupTree(mods),
                              getASTFactory().dupTree(t)]
        {#variableDefinitions = #(#[VARIABLE_DEF,"VARIABLE_DEF"], mods, 
                                  t, vd);}
    ;

variableDeclarator[AST mods, AST t] :( id:IDENT 	(lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);} RBRACK!)*
        v:varInitializer (COMMA!)? )+
    ;

explicitConstructorInvocation! :t:"this" LPAREN a1:argList RPAREN SEMI
        {#explicitConstructorInvocation = #(#[CTOR_CALL, "CTOR_CALL"], 
                                            #t, #a1);}
    |   s:"super" LPAREN a2:argList RPAREN SEMI
        {#explicitConstructorInvocation = #(#[SUPER_CTOR_CALL, 
                                              "SUPER_CTOR_CALL"], 
                                            #s, #a2);}
    ;

classDefinition![AST modifiers] :"class" i:IDENT
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

possiblyEmptyField :field
    | s:SEMI {#s.setType(EMPTY_FIELD);}
    ;

// inherited from grammar JavaRecognizer
compilationUnit :// A compilation unit starts with an optional package definition
		(	packageDefinition
		|	/* nothing */
		)

		// Next we have a series of zero or more import statements
		( importDefinition )*

		// Wrapping things up with any number of class or interface
		//    definitions
		( typeDefinition )*

		EOF!
	;

// inherited from grammar JavaRecognizer
packageDefinition 
options {
	defaultErrorHandler= true;
}
:p:"package"^ {#p.setType(PACKAGE_DEF);} identifier SEMI!
	;

// inherited from grammar JavaRecognizer
importDefinition 
options {
	defaultErrorHandler= true;
}
:i:"import"^ {#i.setType(IMPORT);} identifierStar SEMI!
	;

// inherited from grammar JavaRecognizer
typeDefinition 
options {
	defaultErrorHandler= true;
}
:m:modifiers!
		( classDefinition[#m]
		| interfaceDefinition[#m]
		)
	|	SEMI!
	;

// inherited from grammar JavaRecognizer
declaration! :m:modifiers t:typeSpec[false] v:variableDefinitions[#m,#t]
		{#declaration = #v;}
	;

// inherited from grammar JavaRecognizer
typeSpec[boolean addImagNode] :classTypeSpec[addImagNode]
	| builtInTypeSpec[addImagNode]
	;

// inherited from grammar JavaRecognizer
classTypeSpec[boolean addImagNode] :identifier (lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);} RBRACK!)*
		{
			if ( addImagNode ) {
				#classTypeSpec = #(#[TYPE,"TYPE"], #classTypeSpec);
			}
		}
	;

// inherited from grammar JavaRecognizer
builtInTypeSpec[boolean addImagNode] :builtInType (lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);} RBRACK!)*
		{
			if ( addImagNode ) {
				#builtInTypeSpec = #(#[TYPE,"TYPE"], #builtInTypeSpec);
			}
		}
	;

// inherited from grammar JavaRecognizer
type :identifier
	|	builtInType
	;

// inherited from grammar JavaRecognizer
identifier :IDENT  ( DOT^ IDENT )*
	;

// inherited from grammar JavaRecognizer
identifierStar :IDENT
		( DOT^ IDENT )*
		( DOT^ STAR  )?
	;

// inherited from grammar JavaRecognizer
modifiers :( modifier )*
		{#modifiers = #([MODIFIERS, "MODIFIERS"], #modifiers);}
	;

// inherited from grammar JavaRecognizer
modifier :"private"
	|	"public"
	|	"protected"
	|	"static"
	|	"transient"
	|	"final"
	|	"abstract"
	|	"native"
	|	"threadsafe"
	|	"synchronized"
//	|	"const"			// reserved word, but not valid
	|	"volatile"
	|	"strictfp"
	;

// inherited from grammar JavaRecognizer
superClassClause! :( "extends" id:identifier )?
		{#superClassClause = #(#[EXTENDS_CLAUSE,"EXTENDS_CLAUSE"],id);}
	;

// inherited from grammar JavaRecognizer
interfaceDefinition![AST modifiers] :"interface" IDENT
		// it might extend some other interfaces
		ie:interfaceExtends
		// now parse the body of the interface (looks like a class...)
		cb:classBlock
		{#interfaceDefinition = #(#[INTERFACE_DEF,"INTERFACE_DEF"],
									modifiers,IDENT,ie,cb);}
	;

// inherited from grammar JavaRecognizer
classBlock :LCURLY!
			( field | SEMI! )*
		RCURLY!
		{#classBlock = #([OBJBLOCK, "OBJBLOCK"], #classBlock);}
	;

// inherited from grammar JavaRecognizer
interfaceExtends :(
		e:"extends"!
		identifier ( COMMA! identifier )*
		)?
		{#interfaceExtends = #(#[EXTENDS_CLAUSE,"EXTENDS_CLAUSE"],
							#interfaceExtends);}
	;

// inherited from grammar JavaRecognizer
implementsClause :(
			i:"implements"! identifier ( COMMA! identifier )*
		)?
		{#implementsClause = #(#[IMPLEMENTS_CLAUSE,"IMPLEMENTS_CLAUSE"],
								 #implementsClause);}
	;

// inherited from grammar JavaRecognizer
field! :// method, constructor, or variable declaration
		mods:modifiers
		(	h:ctorHead s:constructorBody // constructor
			{#field = #(#[CTOR_DEF,"CTOR_DEF"], mods, h, s);}

		|	cd:classDefinition[#mods]       // inner class
			{#field = #cd;}

		|	id:interfaceDefinition[#mods]   // inner interface
			{#field = #id;}

		|	t:typeSpec[false]  // method or variable declaration(s)
			(	IDENT  // the name of the method

				// parse the formal parameter declarations.
				LPAREN! param:parameterDeclarationList RPAREN!

				rt:declaratorBrackets[#t]

				// get the list of exceptions that this method is
				// declared to throw
				(tc:throwsClause)?

				( s2:compoundStatement | SEMI )
				{#field = #(#[METHOD_DEF,"METHOD_DEF"],
						     mods,
							 #(#[TYPE,"TYPE"],rt),
							 IDENT,
							 param,
							 tc,
							 s2);}
			|	v:variableDefinitions[#mods,#t] SEMI
//				{#field = #(#[VARIABLE_DEF,"VARIABLE_DEF"], v);}
				{#field = #v;}
			)
		)

    // "static { ... }" class initializer
	|	"static" s3:compoundStatement
		{#field = #(#[STATIC_INIT,"STATIC_INIT"], s3);}

    // "{ ... }" instance initializer
	|	s4:compoundStatement
		{#field = #(#[INSTANCE_INIT,"INSTANCE_INIT"], s4);}
	;

// inherited from grammar JavaRecognizer
constructorBody :lc:LCURLY^ {#lc.setType(SLIST);}
            ( options { greedy=true; } : explicitConstructorInvocation)?
            (statement)*
        RCURLY!
    ;

// inherited from grammar JavaRecognizer
declaratorBrackets[AST typ] :{#declaratorBrackets=typ;}
		(lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);} RBRACK!)*
	;

// inherited from grammar JavaRecognizer
varInitializer :( ASSIGN^ initializer )?
	;

// inherited from grammar JavaRecognizer
arrayInitializer :lc:LCURLY^ {#lc.setType(ARRAY_INIT);}
			(	initializer
				(
					// CONFLICT: does a COMMA after an initializer start a new
					//           initializer or start the option ',' at end?
					//           ANTLR generates proper code by matching
					//			 the comma as soon as possible.
					options {
						warnWhenFollowAmbig = false;
					}
				:
					COMMA! initializer
				)*
				(COMMA!)?
			)?
		RCURLY!
	;

// inherited from grammar JavaRecognizer
initializer :expression
	|	arrayInitializer
	;

// inherited from grammar JavaRecognizer
ctorHead :IDENT  // the name of the method

		// parse the formal parameter declarations.
		LPAREN! parameterDeclarationList RPAREN!

		// get the list of exceptions that this method is declared to throw
		(throwsClause)?
	;

// inherited from grammar JavaRecognizer
throwsClause :"throws"^ identifier ( COMMA! identifier )*
	;

// inherited from grammar JavaRecognizer
parameterDeclarationList :( parameterDeclaration ( COMMA! parameterDeclaration )* )?
		{#parameterDeclarationList = #(#[PARAMETERS,"PARAMETERS"],
									#parameterDeclarationList);}
	;

// inherited from grammar JavaRecognizer
parameterDeclaration! :pm:parameterModifier t:typeSpec[false] id:IDENT
		pd:declaratorBrackets[#t]
		{#parameterDeclaration = #(#[PARAMETER_DEF,"PARAMETER_DEF"],
									pm, #([TYPE,"TYPE"],pd), id);}
	;

// inherited from grammar JavaRecognizer
parameterModifier :(f:"final")?
		{#parameterModifier = #(#[MODIFIERS,"MODIFIERS"], f);}
	;

// inherited from grammar JavaRecognizer
compoundStatement :lc:LCURLY^ {#lc.setType(SLIST);}
			// include the (possibly-empty) list of statements
			(statement)*
		RCURLY!
	;

// inherited from grammar JavaRecognizer
statement :compoundStatement

	// declarations are ambiguous with "ID DOT" relative to expression
	// statements.  Must backtrack to be sure.  Could use a semantic
	// predicate to test symbol table to see what the type was coming
	// up, but that's pretty hard without a symbol table ;)
	|	(declaration)=> declaration SEMI!

	// An expression statement.  This could be a method call,
	// assignment statement, or any other expression evaluated for
	// side-effects.
	|	expression SEMI!

	// class definition
	|	m:modifiers! classDefinition[#m]

	// Attach a label to the front of a statement
	|	IDENT c:COLON^ {#c.setType(LABELED_STAT);} statement

	// If-else statement
	|	"if"^ LPAREN! expression RPAREN! statement
		(
			// CONFLICT: the old "dangling-else" problem...
			//           ANTLR generates proper code matching
			//			 as soon as possible.  Hush warning.
			options {
				warnWhenFollowAmbig = false;
			}
		:
			"else"! statement
		)?

	// For statement
	|	"for"^
			LPAREN!
				forInit SEMI!   // initializer
				forCond	SEMI!   // condition test
				forIter         // updater
			RPAREN!
			statement                     // statement to loop over

	// While statement
	|	"while"^ LPAREN! expression RPAREN! statement

	// do-while statement
	|	"do"^ statement "while"! LPAREN! expression RPAREN! SEMI!

	// get out of a loop (or switch)
	|	"break"^ (IDENT)? SEMI!

	// do next iteration of a loop
	|	"continue"^ (IDENT)? SEMI!

	// Return an expression
	|	"return"^ (expression)? SEMI!

	// switch/case statement
	|	"switch"^ LPAREN! expression RPAREN! LCURLY!
			( casesGroup )*
		RCURLY!

	// exception try-catch block
	|	tryBlock

	// throw an exception
	|	"throw"^ expression SEMI!

	// synchronize a statement
	|	"synchronized"^ LPAREN! expression RPAREN! compoundStatement

	// asserts (uncomment if you want 1.4 compatibility)
	|	"assert"^ expression ( COLON! expression )? SEMI!

	// empty statement
	|	s:SEMI {#s.setType(EMPTY_STAT);}
	;

// inherited from grammar JavaRecognizer
casesGroup :(	// CONFLICT: to which case group do the statements bind?
			//           ANTLR generates proper code: it groups the
			//           many "case"/"default" labels together then
			//           follows them with the statements
			options {
				greedy = true;
			}
			:
			aCase
		)+
		caseSList
		{#casesGroup = #([CASE_GROUP, "CASE_GROUP"], #casesGroup);}
	;

// inherited from grammar JavaRecognizer
aCase :("case"^ expression | "default") COLON!
	;

// inherited from grammar JavaRecognizer
caseSList :(statement)*
		{#caseSList = #(#[SLIST,"SLIST"],#caseSList);}
	;

// inherited from grammar JavaRecognizer
forInit :(	(declaration)=> declaration
		// otherwise it could be an expression list...
		|	expressionList
		)?
		{#forInit = #(#[FOR_INIT,"FOR_INIT"],#forInit);}
	;

// inherited from grammar JavaRecognizer
forCond :(expression)?
		{#forCond = #(#[FOR_CONDITION,"FOR_CONDITION"],#forCond);}
	;

// inherited from grammar JavaRecognizer
forIter :(expressionList)?
		{#forIter = #(#[FOR_ITERATOR,"FOR_ITERATOR"],#forIter);}
	;

// inherited from grammar JavaRecognizer
tryBlock :"try"^ compoundStatement
		(handler)*
		( finallyClause )?
	;

// inherited from grammar JavaRecognizer
finallyClause :"finally"^ compoundStatement
	;

// inherited from grammar JavaRecognizer
handler :"catch"^ LPAREN! parameterDeclaration RPAREN! compoundStatement
	;

// inherited from grammar JavaRecognizer
expression :assignmentExpression
		{#expression = #(#[EXPR,"EXPR"],#expression);}
	;

// inherited from grammar JavaRecognizer
expressionList :expression (COMMA! expression)*
		{#expressionList = #(#[ELIST,"ELIST"], expressionList);}
	;

// inherited from grammar JavaRecognizer
assignmentExpression :conditionalExpression
		(	(	ASSIGN^
            |   PLUS_ASSIGN^
            |   MINUS_ASSIGN^
            |   STAR_ASSIGN^
            |   DIV_ASSIGN^
            |   MOD_ASSIGN^
            |   SR_ASSIGN^
            |   BSR_ASSIGN^
            |   SL_ASSIGN^
            |   BAND_ASSIGN^
            |   BXOR_ASSIGN^
            |   BOR_ASSIGN^
            )
			assignmentExpression
		)?
	;

// inherited from grammar JavaRecognizer
conditionalExpression :logicalOrExpression
		( QUESTION^ assignmentExpression COLON! conditionalExpression )?
	;

// inherited from grammar JavaRecognizer
logicalOrExpression :logicalAndExpression (LOR^ logicalAndExpression)*
	;

// inherited from grammar JavaRecognizer
logicalAndExpression :inclusiveOrExpression (LAND^ inclusiveOrExpression)*
	;

// inherited from grammar JavaRecognizer
inclusiveOrExpression :exclusiveOrExpression (BOR^ exclusiveOrExpression)*
	;

// inherited from grammar JavaRecognizer
exclusiveOrExpression :andExpression (BXOR^ andExpression)*
	;

// inherited from grammar JavaRecognizer
andExpression :equalityExpression (BAND^ equalityExpression)*
	;

// inherited from grammar JavaRecognizer
equalityExpression :relationalExpression ((NOT_EQUAL^ | EQUAL^) relationalExpression)*
	;

// inherited from grammar JavaRecognizer
relationalExpression :shiftExpression
		(	(	(	LT^
				|	GT^
				|	LE^
				|	GE^
				)
				shiftExpression
			)*
		|	"instanceof"^ typeSpec[true]
		)
	;

// inherited from grammar JavaRecognizer
shiftExpression :additiveExpression ((SL^ | SR^ | BSR^) additiveExpression)*
	;

// inherited from grammar JavaRecognizer
additiveExpression :multiplicativeExpression ((PLUS^ | MINUS^) multiplicativeExpression)*
	;

// inherited from grammar JavaRecognizer
multiplicativeExpression :unaryExpression ((STAR^ | DIV^ | MOD^ ) unaryExpression)*
	;

// inherited from grammar JavaRecognizer
unaryExpression :INC^ unaryExpression
	|	DEC^ unaryExpression
	|	MINUS^ {#MINUS.setType(UNARY_MINUS);} unaryExpression
	|	PLUS^  {#PLUS.setType(UNARY_PLUS);} unaryExpression
	|	unaryExpressionNotPlusMinus
	;

// inherited from grammar JavaRecognizer
unaryExpressionNotPlusMinus :BNOT^ unaryExpression
	|	LNOT^ unaryExpression

	|	(	// subrule allows option to shut off warnings
			options {
				// "(int" ambig with postfixExpr due to lack of sequence
				// info in linear approximate LL(k).  It's ok.  Shut up.
				generateAmbigWarnings=false;
			}
		:	// If typecast is built in type, must be numeric operand
			// Also, no reason to backtrack if type keyword like int, float...
			lpb:LPAREN^ {#lpb.setType(TYPECAST);} builtInTypeSpec[true] RPAREN!
			unaryExpression

			// Have to backtrack to see if operator follows.  If no operator
			// follows, it's a typecast.  No semantic checking needed to parse.
			// if it _looks_ like a cast, it _is_ a cast; else it's a "(expr)"
		|	(LPAREN classTypeSpec[true] RPAREN unaryExpressionNotPlusMinus)=>
			lp:LPAREN^ {#lp.setType(TYPECAST);} classTypeSpec[true] RPAREN!
			unaryExpressionNotPlusMinus

		|	postfixExpression
		)
	;

// inherited from grammar JavaRecognizer
postfixExpression :/*
    "this"! lp1:LPAREN^ argList RPAREN!
		{#lp1.setType(CTOR_CALL);}

    |   "super"! lp2:LPAREN^ argList RPAREN!
		{#lp2.setType(SUPER_CTOR_CALL);}
    |
    */
        primaryExpression

		(
            /*
            options {
				// the use of postfixExpression in SUPER_CTOR_CALL adds DOT
				// to the lookahead set, and gives loads of false non-det
				// warnings.
				// shut them off.
				generateAmbigWarnings=false;
			}
		:	*/
            DOT^ IDENT
			(	lp:LPAREN^ {#lp.setType(METHOD_CALL);}
				argList
				RPAREN!
			)?
		|	DOT^ "this"

		|	DOT^ "super"
            (   // (new Outer()).super()  (create enclosing instance)
                lp3:LPAREN^ argList RPAREN!
                {#lp3.setType(SUPER_CTOR_CALL);}
			|   DOT^ IDENT
                (	lps:LPAREN^ {#lps.setType(METHOD_CALL);}
                    argList
                    RPAREN!
                )?
            )
		|	DOT^ newExpression
		|	lb:LBRACK^ {#lb.setType(INDEX_OP);} expression RBRACK!
		)*

		(   // possibly add on a post-increment or post-decrement.
            // allows INC/DEC on too much, but semantics can check
			in:INC^ {#in.setType(POST_INC);}
	 	|	de:DEC^ {#de.setType(POST_DEC);}
		)?
 	;

// inherited from grammar JavaRecognizer
identPrimary :IDENT
		(
            options {
				// .ident could match here or in postfixExpression.
				// We do want to match here.  Turn off warning.
				greedy=true;
			}
		:	DOT^ IDENT
		)*
		(
            options {
				// ARRAY_DECLARATOR here conflicts with INDEX_OP in
				// postfixExpression on LBRACK RBRACK.
				// We want to match [] here, so greedy.  This overcomes
                // limitation of linear approximate lookahead.
				greedy=true;
		    }
		:   ( lp:LPAREN^ {#lp.setType(METHOD_CALL);} argList RPAREN! )
		|	( options {greedy=true;} :
              lbc:LBRACK^ {#lbc.setType(ARRAY_DECLARATOR);} RBRACK!
            )+
		)?
    ;

// inherited from grammar JavaRecognizer
newExpression :"new"^ type
		(	LPAREN! argList RPAREN! (classBlock)?

			//java 1.1
			// Note: This will allow bad constructs like
			//    new int[4][][3] {exp,exp}.
			//    There needs to be a semantic check here...
			// to make sure:
			//   a) [ expr ] and [ ] are not mixed
			//   b) [ expr ] and an init are not used together

		|	newArrayDeclarator (arrayInitializer)?
		)
	;

// inherited from grammar JavaRecognizer
argList :(	expressionList
		|	/*nothing*/
			{#argList = #[ELIST,"ELIST"];}
		)
	;

// inherited from grammar JavaRecognizer
newArrayDeclarator :(
			// CONFLICT:
			// newExpression is a primaryExpression which can be
			// followed by an array index reference.  This is ok,
			// as the generated code will stay in this loop as
			// long as it sees an LBRACK (proper behavior)
			options {
				warnWhenFollowAmbig = false;
			}
		:
			lb:LBRACK^ {#lb.setType(ARRAY_DECLARATOR);}
				(expression)?
			RBRACK!
		)+
	;

class PdeLexer extends Lexer;

options {
	importVocab=PdePartial;
	exportVocab=Pde;
	testLiterals=false;
	k=4;
	charVocabulary='\u0003'..'\uFFFF';
	codeGenBitsetTestThreshold=20;
}

WS :(   ' '
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

SL_COMMENT :"//"
        (~('\n'|'\r'))* ('\n'|'\r'('\n')?)
        {newline();}
    ;

ML_COMMENT :"/*"
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

WEBCOLOR_LITERAL :'#'! (HEX_DIGIT)+
    ;

// inherited from grammar JavaLexer
QUESTION :'?'		;

// inherited from grammar JavaLexer
LPAREN :'('		;

// inherited from grammar JavaLexer
RPAREN :')'		;

// inherited from grammar JavaLexer
LBRACK :'['		;

// inherited from grammar JavaLexer
RBRACK :']'		;

// inherited from grammar JavaLexer
LCURLY :'{'		;

// inherited from grammar JavaLexer
RCURLY :'}'		;

// inherited from grammar JavaLexer
COLON :':'		;

// inherited from grammar JavaLexer
COMMA :','		;

// inherited from grammar JavaLexer
ASSIGN :'='		;

// inherited from grammar JavaLexer
EQUAL :"=="	;

// inherited from grammar JavaLexer
LNOT :'!'		;

// inherited from grammar JavaLexer
BNOT :'~'		;

// inherited from grammar JavaLexer
NOT_EQUAL :"!="	;

// inherited from grammar JavaLexer
DIV :'/'		;

// inherited from grammar JavaLexer
DIV_ASSIGN :"/="	;

// inherited from grammar JavaLexer
PLUS :'+'		;

// inherited from grammar JavaLexer
PLUS_ASSIGN :"+="	;

// inherited from grammar JavaLexer
INC :"++"	;

// inherited from grammar JavaLexer
MINUS :'-'		;

// inherited from grammar JavaLexer
MINUS_ASSIGN :"-="	;

// inherited from grammar JavaLexer
DEC :"--"	;

// inherited from grammar JavaLexer
STAR :'*'		;

// inherited from grammar JavaLexer
STAR_ASSIGN :"*="	;

// inherited from grammar JavaLexer
MOD :'%'		;

// inherited from grammar JavaLexer
MOD_ASSIGN :"%="	;

// inherited from grammar JavaLexer
SR :">>"	;

// inherited from grammar JavaLexer
SR_ASSIGN :">>="	;

// inherited from grammar JavaLexer
BSR :">>>"	;

// inherited from grammar JavaLexer
BSR_ASSIGN :">>>="	;

// inherited from grammar JavaLexer
GE :">="	;

// inherited from grammar JavaLexer
GT :">"		;

// inherited from grammar JavaLexer
SL :"<<"	;

// inherited from grammar JavaLexer
SL_ASSIGN :"<<="	;

// inherited from grammar JavaLexer
LE :"<="	;

// inherited from grammar JavaLexer
LT :'<'		;

// inherited from grammar JavaLexer
BXOR :'^'		;

// inherited from grammar JavaLexer
BXOR_ASSIGN :"^="	;

// inherited from grammar JavaLexer
BOR :'|'		;

// inherited from grammar JavaLexer
BOR_ASSIGN :"|="	;

// inherited from grammar JavaLexer
LOR :"||"	;

// inherited from grammar JavaLexer
BAND :'&'		;

// inherited from grammar JavaLexer
BAND_ASSIGN :"&="	;

// inherited from grammar JavaLexer
LAND :"&&"	;

// inherited from grammar JavaLexer
SEMI :';'		;

// inherited from grammar JavaLexer
CHAR_LITERAL :'\'' ( ESC | ~'\'' ) '\''
	;

// inherited from grammar JavaLexer
STRING_LITERAL :'"' (ESC|~('"'|'\\'))* '"'
	;

// inherited from grammar JavaLexer
protected ESC :'\\'
		(	'n'
		|	'r'
		|	't'
		|	'b'
		|	'f'
		|	'"'
		|	'\''
		|	'\\'
		|	('u')+ HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
		|	'0'..'3'
			(
				options {
					warnWhenFollowAmbig = false;
				}
			:	'0'..'7'
				(
					options {
						warnWhenFollowAmbig = false;
					}
				:	'0'..'7'
				)?
			)?
		|	'4'..'7'
			(
				options {
					warnWhenFollowAmbig = false;
				}
			:	'0'..'7'
			)?
		)
	;

// inherited from grammar JavaLexer
protected HEX_DIGIT :('0'..'9'|'A'..'F'|'a'..'f')
	;

// inherited from grammar JavaLexer
protected VOCAB :'\3'..'\377'
	;

// inherited from grammar JavaLexer
IDENT 
options {
	testLiterals=true;
}
:('a'..'z'|'A'..'Z'|'_'|'$') ('a'..'z'|'A'..'Z'|'_'|'0'..'9'|'$')*
	;

// inherited from grammar JavaLexer
NUM_INT {boolean isDecimal=false; Token t=null;}
:'.' {_ttype = DOT;}
            (	('0'..'9')+ (EXPONENT)? (f1:FLOAT_SUFFIX {t=f1;})?
                {
				if (t != null && t.getText().toUpperCase().indexOf('F')>=0) {
                	_ttype = NUM_FLOAT;
				}
				else {
                	_ttype = NUM_DOUBLE; // assume double
				}
				}
            )?

	|	(	'0' {isDecimal = true;} // special case for just '0'
			(	('x'|'X')
				(											// hex
					// the 'e'|'E' and float suffix stuff look
					// like hex digits, hence the (...)+ doesn't
					// know when to stop: ambig.  ANTLR resolves
					// it correctly by matching immediately.  It
					// is therefor ok to hush warning.
					options {
						warnWhenFollowAmbig=false;
					}
				:	HEX_DIGIT
				)+
			|	('0'..'7')+									// octal
			)?
		|	('1'..'9') ('0'..'9')*  {isDecimal=true;}		// non-zero decimal
		)
		(	('l'|'L') { _ttype = NUM_LONG; }

		// only check to see if it's a float if looks like decimal so far
		|	{isDecimal}?
            (   '.' ('0'..'9')* (EXPONENT)? (f2:FLOAT_SUFFIX {t=f2;})?
            |   EXPONENT (f3:FLOAT_SUFFIX {t=f3;})?
            |   f4:FLOAT_SUFFIX {t=f4;}
            )
            {
			if (t != null && t.getText().toUpperCase() .indexOf('F') >= 0) {
                _ttype = NUM_FLOAT;
			}
            else {
	           	_ttype = NUM_DOUBLE; // assume double
			}
			}
        )?
	;

// inherited from grammar JavaLexer
protected EXPONENT :('e'|'E') ('+'|'-')? ('0'..'9')+
	;

// inherited from grammar JavaLexer
protected FLOAT_SUFFIX :'f'|'F'|'d'|'D'
	;



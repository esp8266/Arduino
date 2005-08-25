// $ANTLR 2.7.2: "java.g" -> "JavaRecognizer.java"$

package antlr.java;

import antlr.TokenBuffer;
import antlr.TokenStreamException;
import antlr.TokenStreamIOException;
import antlr.ANTLRException;
import antlr.LLkParser;
import antlr.Token;
import antlr.TokenStream;
import antlr.RecognitionException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.ParserSharedInputState;
import antlr.collections.impl.BitSet;
import antlr.collections.AST;
import java.util.Hashtable;
import antlr.ASTFactory;
import antlr.ASTPair;
import antlr.collections.impl.ASTArray;

/** Java 1.3 Recognizer
 *
 * Run 'java Main [-showtree] directory-full-of-java-files'
 *
 * [The -showtree option pops up a Swing frame that shows
 *  the AST constructed from the parser.]
 *
 * Run 'java Main <directory full of java files>'
 *
 * Contributing authors:
 *		John Mitchell		johnm@non.net
 *		Terence Parr		parrt@magelang.com
 *		John Lilley			jlilley@empathy.com
 *		Scott Stanchfield	thetick@magelang.com
 *		Markus Mohnen       mohnen@informatik.rwth-aachen.de
 *      Peter Williams      pete.williams@sun.com
 *      Allan Jacobs        Allan.Jacobs@eng.sun.com
 *      Steve Messick       messick@redhills.com
 *      John Pybus			john@pybus.org
 *
 * Version 1.00 December 9, 1997 -- initial release
 * Version 1.01 December 10, 1997
 *		fixed bug in octal def (0..7 not 0..8)
 * Version 1.10 August 1998 (parrt)
 *		added tree construction
 *		fixed definition of WS,comments for mac,pc,unix newlines
 *		added unary plus
 * Version 1.11 (Nov 20, 1998)
 *		Added "shutup" option to turn off last ambig warning.
 *		Fixed inner class def to allow named class defs as statements
 *		synchronized requires compound not simple statement
 *		add [] after builtInType DOT class in primaryExpression
 *		"const" is reserved but not valid..removed from modifiers
 * Version 1.12 (Feb 2, 1999)
 *		Changed LITERAL_xxx to xxx in tree grammar.
 *		Updated java.g to use tokens {...} now for 2.6.0 (new feature).
 *
 * Version 1.13 (Apr 23, 1999)
 *		Didn't have (stat)? for else clause in tree parser.
 *		Didn't gen ASTs for interface extends.  Updated tree parser too.
 *		Updated to 2.6.0.
 * Version 1.14 (Jun 20, 1999)
 *		Allowed final/abstract on local classes.
 *		Removed local interfaces from methods
 *		Put instanceof precedence where it belongs...in relationalExpr
 *			It also had expr not type as arg; fixed it.
 *		Missing ! on SEMI in classBlock
 *		fixed: (expr) + "string" was parsed incorrectly (+ as unary plus).
 *		fixed: didn't like Object[].class in parser or tree parser
 * Version 1.15 (Jun 26, 1999)
 *		Screwed up rule with instanceof in it. :(  Fixed.
 *		Tree parser didn't like (expr).something; fixed.
 *		Allowed multiple inheritance in tree grammar. oops.
 * Version 1.16 (August 22, 1999)
 *		Extending an interface built a wacky tree: had extra EXTENDS.
 *		Tree grammar didn't allow multiple superinterfaces.
 *		Tree grammar didn't allow empty var initializer: {}
 * Version 1.17 (October 12, 1999)
 *		ESC lexer rule allowed 399 max not 377 max.
 *		java.tree.g didn't handle the expression of synchronized
 *		statements.
 * Version 1.18 (August 12, 2001)
 *      	Terence updated to Java 2 Version 1.3 by
 *		observing/combining work of Allan Jacobs and Steve
 *		Messick.  Handles 1.3 src.  Summary:
 *		o  primary didn't include boolean.class kind of thing
 *      	o  constructor calls parsed explicitly now:
 * 		   see explicitConstructorInvocation
 *		o  add strictfp modifier
 *      	o  missing objBlock after new expression in tree grammar
 *		o  merged local class definition alternatives, moved after declaration
 *		o  fixed problem with ClassName.super.field
 *      	o  reordered some alternatives to make things more efficient
 *		o  long and double constants were not differentiated from int/float
 *		o  whitespace rule was inefficient: matched only one char
 *		o  add an examples directory with some nasty 1.3 cases
 *		o  made Main.java use buffered IO and a Reader for Unicode support
 *		o  supports UNICODE?
 *		   Using Unicode charVocabulay makes code file big, but only
 *		   in the bitsets at the end. I need to make ANTLR generate
 *		   unicode bitsets more efficiently.
 * Version 1.19 (April 25, 2002)
 *		Terence added in nice fixes by John Pybus concerning floating
 *		constants and problems with super() calls.  John did a nice
 *		reorg of the primary/postfix expression stuff to read better
 *		and makes f.g.super() parse properly (it was METHOD_CALL not
 *		a SUPER_CTOR_CALL).  Also:
 *
 *		o  "finally" clause was a root...made it a child of "try"
 *		o  Added stuff for asserts too for Java 1.4, but *commented out*
 *		   as it is not backward compatible.
 *
 * Version 1.20 (October 27, 2002)
 *
 *      Terence ended up reorging John Pybus' stuff to
 *      remove some nondeterminisms and some syntactic predicates.
 *      Note that the grammar is stricter now; e.g., this(...) must
 *	be the first statement.
 *
 *      Trinary ?: operator wasn't working as array name:
 *          (isBig ? bigDigits : digits)[i];
 *
 *      Checked parser/tree parser on source for
 *          Resin-2.0.5, jive-2.1.1, jdk 1.3.1, Lucene, antlr 2.7.2a4,
 *	    and the 110k-line jGuru server source.
 *
 * This grammar is in the PUBLIC DOMAIN
 */
public class JavaRecognizer extends antlr.LLkParser       implements JavaTokenTypes
 {

protected JavaRecognizer(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public JavaRecognizer(TokenBuffer tokenBuf) {
  this(tokenBuf,2);
}

protected JavaRecognizer(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public JavaRecognizer(TokenStream lexer) {
  this(lexer,2);
}

public JavaRecognizer(ParserSharedInputState state) {
  super(state,2);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

	public final void compilationUnit() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST compilationUnit_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_package:
		{
			packageDefinition();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case EOF:
		case FINAL:
		case ABSTRACT:
		case STRICTFP:
		case SEMI:
		case LITERAL_import:
		case LITERAL_private:
		case LITERAL_public:
		case LITERAL_protected:
		case LITERAL_static:
		case LITERAL_transient:
		case LITERAL_native:
		case LITERAL_threadsafe:
		case LITERAL_synchronized:
		case LITERAL_volatile:
		case LITERAL_class:
		case LITERAL_interface:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		_loop4:
		do {
			if ((LA(1)==LITERAL_import)) {
				importDefinition();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop4;
			}
			
		} while (true);
		}
		{
		_loop6:
		do {
			if ((_tokenSet_0.member(LA(1)))) {
				typeDefinition();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop6;
			}
			
		} while (true);
		}
		match(Token.EOF_TYPE);
		compilationUnit_AST = (AST)currentAST.root;
		returnAST = compilationUnit_AST;
	}
	
	public final void packageDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST packageDefinition_AST = null;
		Token  p = null;
		AST p_AST = null;
		
		try {      // for error handling
			p = LT(1);
			p_AST = astFactory.create(p);
			astFactory.makeASTRoot(currentAST, p_AST);
			match(LITERAL_package);
			if ( inputState.guessing==0 ) {
				p_AST.setType(PACKAGE_DEF);
			}
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			packageDefinition_AST = (AST)currentAST.root;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_1);
			} else {
			  throw ex;
			}
		}
		returnAST = packageDefinition_AST;
	}
	
	public final void importDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST importDefinition_AST = null;
		Token  i = null;
		AST i_AST = null;
		
		try {      // for error handling
			i = LT(1);
			i_AST = astFactory.create(i);
			astFactory.makeASTRoot(currentAST, i_AST);
			match(LITERAL_import);
			if ( inputState.guessing==0 ) {
				i_AST.setType(IMPORT);
			}
			identifierStar();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			importDefinition_AST = (AST)currentAST.root;
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_1);
			} else {
			  throw ex;
			}
		}
		returnAST = importDefinition_AST;
	}
	
	public final void typeDefinition() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST typeDefinition_AST = null;
		AST m_AST = null;
		
		try {      // for error handling
			switch ( LA(1)) {
			case FINAL:
			case ABSTRACT:
			case STRICTFP:
			case LITERAL_private:
			case LITERAL_public:
			case LITERAL_protected:
			case LITERAL_static:
			case LITERAL_transient:
			case LITERAL_native:
			case LITERAL_threadsafe:
			case LITERAL_synchronized:
			case LITERAL_volatile:
			case LITERAL_class:
			case LITERAL_interface:
			{
				modifiers();
				m_AST = (AST)returnAST;
				{
				switch ( LA(1)) {
				case LITERAL_class:
				{
					classDefinition(m_AST);
					astFactory.addASTChild(currentAST, returnAST);
					break;
				}
				case LITERAL_interface:
				{
					interfaceDefinition(m_AST);
					astFactory.addASTChild(currentAST, returnAST);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				typeDefinition_AST = (AST)currentAST.root;
				break;
			}
			case SEMI:
			{
				match(SEMI);
				typeDefinition_AST = (AST)currentAST.root;
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
		}
		catch (RecognitionException ex) {
			if (inputState.guessing==0) {
				reportError(ex);
				consume();
				consumeUntil(_tokenSet_2);
			} else {
			  throw ex;
			}
		}
		returnAST = typeDefinition_AST;
	}
	
	public final void identifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identifier_AST = null;
		
		AST tmp5_AST = null;
		tmp5_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp5_AST);
		match(IDENT);
		{
		_loop23:
		do {
			if ((LA(1)==DOT)) {
				AST tmp6_AST = null;
				tmp6_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp6_AST);
				match(DOT);
				AST tmp7_AST = null;
				tmp7_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp7_AST);
				match(IDENT);
			}
			else {
				break _loop23;
			}
			
		} while (true);
		}
		identifier_AST = (AST)currentAST.root;
		returnAST = identifier_AST;
	}
	
	public final void identifierStar() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identifierStar_AST = null;
		
		AST tmp8_AST = null;
		tmp8_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp8_AST);
		match(IDENT);
		{
		_loop26:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp9_AST = null;
				tmp9_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp9_AST);
				match(DOT);
				AST tmp10_AST = null;
				tmp10_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp10_AST);
				match(IDENT);
			}
			else {
				break _loop26;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case DOT:
		{
			AST tmp11_AST = null;
			tmp11_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp11_AST);
			match(DOT);
			AST tmp12_AST = null;
			tmp12_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp12_AST);
			match(STAR);
			break;
		}
		case SEMI:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		identifierStar_AST = (AST)currentAST.root;
		returnAST = identifierStar_AST;
	}
	
	public final void modifiers() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST modifiers_AST = null;
		
		{
		_loop30:
		do {
			if ((_tokenSet_3.member(LA(1)))) {
				modifier();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop30;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			modifiers_AST = (AST)currentAST.root;
			modifiers_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(MODIFIERS,"MODIFIERS")).add(modifiers_AST));
			currentAST.root = modifiers_AST;
			currentAST.child = modifiers_AST!=null &&modifiers_AST.getFirstChild()!=null ?
				modifiers_AST.getFirstChild() : modifiers_AST;
			currentAST.advanceChildToEnd();
		}
		modifiers_AST = (AST)currentAST.root;
		returnAST = modifiers_AST;
	}
	
	public final void classDefinition(
		AST modifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classDefinition_AST = null;
		AST sc_AST = null;
		AST ic_AST = null;
		AST cb_AST = null;
		
		match(LITERAL_class);
		AST tmp14_AST = null;
		tmp14_AST = astFactory.create(LT(1));
		match(IDENT);
		superClassClause();
		sc_AST = (AST)returnAST;
		implementsClause();
		ic_AST = (AST)returnAST;
		classBlock();
		cb_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			classDefinition_AST = (AST)currentAST.root;
			classDefinition_AST = (AST)astFactory.make( (new ASTArray(6)).add(astFactory.create(CLASS_DEF,"CLASS_DEF")).add(modifiers).add(tmp14_AST).add(sc_AST).add(ic_AST).add(cb_AST));
			currentAST.root = classDefinition_AST;
			currentAST.child = classDefinition_AST!=null &&classDefinition_AST.getFirstChild()!=null ?
				classDefinition_AST.getFirstChild() : classDefinition_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = classDefinition_AST;
	}
	
	public final void interfaceDefinition(
		AST modifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST interfaceDefinition_AST = null;
		AST ie_AST = null;
		AST cb_AST = null;
		
		match(LITERAL_interface);
		AST tmp16_AST = null;
		tmp16_AST = astFactory.create(LT(1));
		match(IDENT);
		interfaceExtends();
		ie_AST = (AST)returnAST;
		classBlock();
		cb_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			interfaceDefinition_AST = (AST)currentAST.root;
			interfaceDefinition_AST = (AST)astFactory.make( (new ASTArray(5)).add(astFactory.create(INTERFACE_DEF,"INTERFACE_DEF")).add(modifiers).add(tmp16_AST).add(ie_AST).add(cb_AST));
			currentAST.root = interfaceDefinition_AST;
			currentAST.child = interfaceDefinition_AST!=null &&interfaceDefinition_AST.getFirstChild()!=null ?
				interfaceDefinition_AST.getFirstChild() : interfaceDefinition_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = interfaceDefinition_AST;
	}
	
/** A declaration is the creation of a reference or primitive-type variable
 *  Create a separate Type/Var tree for each var in the var list.
 */
	public final void declaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST declaration_AST = null;
		AST m_AST = null;
		AST t_AST = null;
		AST v_AST = null;
		
		modifiers();
		m_AST = (AST)returnAST;
		typeSpec(false);
		t_AST = (AST)returnAST;
		variableDefinitions(m_AST,t_AST);
		v_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			declaration_AST = (AST)currentAST.root;
			declaration_AST = v_AST;
			currentAST.root = declaration_AST;
			currentAST.child = declaration_AST!=null &&declaration_AST.getFirstChild()!=null ?
				declaration_AST.getFirstChild() : declaration_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = declaration_AST;
	}
	
	public final void typeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST typeSpec_AST = null;
		
		switch ( LA(1)) {
		case IDENT:
		{
			classTypeSpec(addImagNode);
			astFactory.addASTChild(currentAST, returnAST);
			typeSpec_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		{
			builtInTypeSpec(addImagNode);
			astFactory.addASTChild(currentAST, returnAST);
			typeSpec_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = typeSpec_AST;
	}
	
	public final void variableDefinitions(
		AST mods, AST t
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST variableDefinitions_AST = null;
		
		variableDeclarator(getASTFactory().dupTree(mods),
						   getASTFactory().dupTree(t));
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop59:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				variableDeclarator(getASTFactory().dupTree(mods),
							   getASTFactory().dupTree(t));
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop59;
			}
			
		} while (true);
		}
		variableDefinitions_AST = (AST)currentAST.root;
		returnAST = variableDefinitions_AST;
	}
	
	public final void classTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classTypeSpec_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		identifier();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop15:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop15;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			classTypeSpec_AST = (AST)currentAST.root;
			
						if ( addImagNode ) {
							classTypeSpec_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(classTypeSpec_AST));
						}
					
			currentAST.root = classTypeSpec_AST;
			currentAST.child = classTypeSpec_AST!=null &&classTypeSpec_AST.getFirstChild()!=null ?
				classTypeSpec_AST.getFirstChild() : classTypeSpec_AST;
			currentAST.advanceChildToEnd();
		}
		classTypeSpec_AST = (AST)currentAST.root;
		returnAST = classTypeSpec_AST;
	}
	
	public final void builtInTypeSpec(
		boolean addImagNode
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInTypeSpec_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		builtInType();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop18:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop18;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			builtInTypeSpec_AST = (AST)currentAST.root;
			
						if ( addImagNode ) {
							builtInTypeSpec_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(builtInTypeSpec_AST));
						}
					
			currentAST.root = builtInTypeSpec_AST;
			currentAST.child = builtInTypeSpec_AST!=null &&builtInTypeSpec_AST.getFirstChild()!=null ?
				builtInTypeSpec_AST.getFirstChild() : builtInTypeSpec_AST;
			currentAST.advanceChildToEnd();
		}
		builtInTypeSpec_AST = (AST)currentAST.root;
		returnAST = builtInTypeSpec_AST;
	}
	
	public final void builtInType() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST builtInType_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_void:
		{
			AST tmp20_AST = null;
			tmp20_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp20_AST);
			match(LITERAL_void);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_boolean:
		{
			AST tmp21_AST = null;
			tmp21_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp21_AST);
			match(LITERAL_boolean);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_byte:
		{
			AST tmp22_AST = null;
			tmp22_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp22_AST);
			match(LITERAL_byte);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_char:
		{
			AST tmp23_AST = null;
			tmp23_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp23_AST);
			match(LITERAL_char);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_short:
		{
			AST tmp24_AST = null;
			tmp24_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp24_AST);
			match(LITERAL_short);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_int:
		{
			AST tmp25_AST = null;
			tmp25_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp25_AST);
			match(LITERAL_int);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_float:
		{
			AST tmp26_AST = null;
			tmp26_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp26_AST);
			match(LITERAL_float);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_long:
		{
			AST tmp27_AST = null;
			tmp27_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp27_AST);
			match(LITERAL_long);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_double:
		{
			AST tmp28_AST = null;
			tmp28_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp28_AST);
			match(LITERAL_double);
			builtInType_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = builtInType_AST;
	}
	
	public final void type() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST type_AST = null;
		
		switch ( LA(1)) {
		case IDENT:
		{
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			type_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		{
			builtInType();
			astFactory.addASTChild(currentAST, returnAST);
			type_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = type_AST;
	}
	
	public final void modifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST modifier_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_private:
		{
			AST tmp29_AST = null;
			tmp29_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp29_AST);
			match(LITERAL_private);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_public:
		{
			AST tmp30_AST = null;
			tmp30_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp30_AST);
			match(LITERAL_public);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_protected:
		{
			AST tmp31_AST = null;
			tmp31_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp31_AST);
			match(LITERAL_protected);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_static:
		{
			AST tmp32_AST = null;
			tmp32_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp32_AST);
			match(LITERAL_static);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_transient:
		{
			AST tmp33_AST = null;
			tmp33_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp33_AST);
			match(LITERAL_transient);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case FINAL:
		{
			AST tmp34_AST = null;
			tmp34_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp34_AST);
			match(FINAL);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case ABSTRACT:
		{
			AST tmp35_AST = null;
			tmp35_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp35_AST);
			match(ABSTRACT);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_native:
		{
			AST tmp36_AST = null;
			tmp36_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp36_AST);
			match(LITERAL_native);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_threadsafe:
		{
			AST tmp37_AST = null;
			tmp37_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp37_AST);
			match(LITERAL_threadsafe);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_synchronized:
		{
			AST tmp38_AST = null;
			tmp38_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp38_AST);
			match(LITERAL_synchronized);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_volatile:
		{
			AST tmp39_AST = null;
			tmp39_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp39_AST);
			match(LITERAL_volatile);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		case STRICTFP:
		{
			AST tmp40_AST = null;
			tmp40_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp40_AST);
			match(STRICTFP);
			modifier_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = modifier_AST;
	}
	
	public final void superClassClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST superClassClause_AST = null;
		AST id_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_extends:
		{
			match(LITERAL_extends);
			identifier();
			id_AST = (AST)returnAST;
			break;
		}
		case LCURLY:
		case LITERAL_implements:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			superClassClause_AST = (AST)currentAST.root;
			superClassClause_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXTENDS_CLAUSE,"EXTENDS_CLAUSE")).add(id_AST));
			currentAST.root = superClassClause_AST;
			currentAST.child = superClassClause_AST!=null &&superClassClause_AST.getFirstChild()!=null ?
				superClassClause_AST.getFirstChild() : superClassClause_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = superClassClause_AST;
	}
	
	public final void implementsClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST implementsClause_AST = null;
		Token  i = null;
		AST i_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_implements:
		{
			i = LT(1);
			i_AST = astFactory.create(i);
			match(LITERAL_implements);
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop46:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					identifier();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop46;
				}
				
			} while (true);
			}
			break;
		}
		case LCURLY:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			implementsClause_AST = (AST)currentAST.root;
			implementsClause_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(IMPLEMENTS_CLAUSE,"IMPLEMENTS_CLAUSE")).add(implementsClause_AST));
			currentAST.root = implementsClause_AST;
			currentAST.child = implementsClause_AST!=null &&implementsClause_AST.getFirstChild()!=null ?
				implementsClause_AST.getFirstChild() : implementsClause_AST;
			currentAST.advanceChildToEnd();
		}
		implementsClause_AST = (AST)currentAST.root;
		returnAST = implementsClause_AST;
	}
	
	public final void classBlock() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST classBlock_AST = null;
		
		match(LCURLY);
		{
		_loop38:
		do {
			switch ( LA(1)) {
			case FINAL:
			case ABSTRACT:
			case STRICTFP:
			case LITERAL_void:
			case LITERAL_boolean:
			case LITERAL_byte:
			case LITERAL_char:
			case LITERAL_short:
			case LITERAL_int:
			case LITERAL_float:
			case LITERAL_long:
			case LITERAL_double:
			case IDENT:
			case LITERAL_private:
			case LITERAL_public:
			case LITERAL_protected:
			case LITERAL_static:
			case LITERAL_transient:
			case LITERAL_native:
			case LITERAL_threadsafe:
			case LITERAL_synchronized:
			case LITERAL_volatile:
			case LITERAL_class:
			case LITERAL_interface:
			case LCURLY:
			{
				field();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case SEMI:
			{
				match(SEMI);
				break;
			}
			default:
			{
				break _loop38;
			}
			}
		} while (true);
		}
		match(RCURLY);
		if ( inputState.guessing==0 ) {
			classBlock_AST = (AST)currentAST.root;
			classBlock_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(OBJBLOCK,"OBJBLOCK")).add(classBlock_AST));
			currentAST.root = classBlock_AST;
			currentAST.child = classBlock_AST!=null &&classBlock_AST.getFirstChild()!=null ?
				classBlock_AST.getFirstChild() : classBlock_AST;
			currentAST.advanceChildToEnd();
		}
		classBlock_AST = (AST)currentAST.root;
		returnAST = classBlock_AST;
	}
	
	public final void interfaceExtends() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST interfaceExtends_AST = null;
		Token  e = null;
		AST e_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_extends:
		{
			e = LT(1);
			e_AST = astFactory.create(e);
			match(LITERAL_extends);
			identifier();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop42:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					identifier();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop42;
				}
				
			} while (true);
			}
			break;
		}
		case LCURLY:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			interfaceExtends_AST = (AST)currentAST.root;
			interfaceExtends_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXTENDS_CLAUSE,"EXTENDS_CLAUSE")).add(interfaceExtends_AST));
			currentAST.root = interfaceExtends_AST;
			currentAST.child = interfaceExtends_AST!=null &&interfaceExtends_AST.getFirstChild()!=null ?
				interfaceExtends_AST.getFirstChild() : interfaceExtends_AST;
			currentAST.advanceChildToEnd();
		}
		interfaceExtends_AST = (AST)currentAST.root;
		returnAST = interfaceExtends_AST;
	}
	
	public final void field() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST field_AST = null;
		AST mods_AST = null;
		AST h_AST = null;
		AST s_AST = null;
		AST cd_AST = null;
		AST id_AST = null;
		AST t_AST = null;
		AST param_AST = null;
		AST rt_AST = null;
		AST tc_AST = null;
		AST s2_AST = null;
		AST v_AST = null;
		AST s3_AST = null;
		AST s4_AST = null;
		
		if ((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2)))) {
			modifiers();
			mods_AST = (AST)returnAST;
			{
			switch ( LA(1)) {
			case LITERAL_class:
			{
				classDefinition(mods_AST);
				cd_AST = (AST)returnAST;
				if ( inputState.guessing==0 ) {
					field_AST = (AST)currentAST.root;
					field_AST = cd_AST;
					currentAST.root = field_AST;
					currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
						field_AST.getFirstChild() : field_AST;
					currentAST.advanceChildToEnd();
				}
				break;
			}
			case LITERAL_interface:
			{
				interfaceDefinition(mods_AST);
				id_AST = (AST)returnAST;
				if ( inputState.guessing==0 ) {
					field_AST = (AST)currentAST.root;
					field_AST = id_AST;
					currentAST.root = field_AST;
					currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
						field_AST.getFirstChild() : field_AST;
					currentAST.advanceChildToEnd();
				}
				break;
			}
			default:
				if ((LA(1)==IDENT) && (LA(2)==LPAREN)) {
					ctorHead();
					h_AST = (AST)returnAST;
					constructorBody();
					s_AST = (AST)returnAST;
					if ( inputState.guessing==0 ) {
						field_AST = (AST)currentAST.root;
						field_AST = (AST)astFactory.make( (new ASTArray(4)).add(astFactory.create(CTOR_DEF,"CTOR_DEF")).add(mods_AST).add(h_AST).add(s_AST));
						currentAST.root = field_AST;
						currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
							field_AST.getFirstChild() : field_AST;
						currentAST.advanceChildToEnd();
					}
				}
				else if (((LA(1) >= LITERAL_void && LA(1) <= IDENT)) && (_tokenSet_6.member(LA(2)))) {
					typeSpec(false);
					t_AST = (AST)returnAST;
					{
					if ((LA(1)==IDENT) && (LA(2)==LPAREN)) {
						AST tmp47_AST = null;
						tmp47_AST = astFactory.create(LT(1));
						match(IDENT);
						match(LPAREN);
						parameterDeclarationList();
						param_AST = (AST)returnAST;
						match(RPAREN);
						declaratorBrackets(t_AST);
						rt_AST = (AST)returnAST;
						{
						switch ( LA(1)) {
						case LITERAL_throws:
						{
							throwsClause();
							tc_AST = (AST)returnAST;
							break;
						}
						case SEMI:
						case LCURLY:
						{
							break;
						}
						default:
						{
							throw new NoViableAltException(LT(1), getFilename());
						}
						}
						}
						{
						switch ( LA(1)) {
						case LCURLY:
						{
							compoundStatement();
							s2_AST = (AST)returnAST;
							break;
						}
						case SEMI:
						{
							AST tmp50_AST = null;
							tmp50_AST = astFactory.create(LT(1));
							match(SEMI);
							break;
						}
						default:
						{
							throw new NoViableAltException(LT(1), getFilename());
						}
						}
						}
						if ( inputState.guessing==0 ) {
							field_AST = (AST)currentAST.root;
							field_AST = (AST)astFactory.make( (new ASTArray(7)).add(astFactory.create(METHOD_DEF,"METHOD_DEF")).add(mods_AST).add((AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(rt_AST))).add(tmp47_AST).add(param_AST).add(tc_AST).add(s2_AST));
							currentAST.root = field_AST;
							currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
								field_AST.getFirstChild() : field_AST;
							currentAST.advanceChildToEnd();
						}
					}
					else if ((LA(1)==IDENT) && (_tokenSet_7.member(LA(2)))) {
						variableDefinitions(mods_AST,t_AST);
						v_AST = (AST)returnAST;
						AST tmp51_AST = null;
						tmp51_AST = astFactory.create(LT(1));
						match(SEMI);
						if ( inputState.guessing==0 ) {
							field_AST = (AST)currentAST.root;
							field_AST = v_AST;
							currentAST.root = field_AST;
							currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
								field_AST.getFirstChild() : field_AST;
							currentAST.advanceChildToEnd();
						}
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else if ((LA(1)==LITERAL_static) && (LA(2)==LCURLY)) {
			match(LITERAL_static);
			compoundStatement();
			s3_AST = (AST)returnAST;
			if ( inputState.guessing==0 ) {
				field_AST = (AST)currentAST.root;
				field_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(STATIC_INIT,"STATIC_INIT")).add(s3_AST));
				currentAST.root = field_AST;
				currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
					field_AST.getFirstChild() : field_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else if ((LA(1)==LCURLY)) {
			compoundStatement();
			s4_AST = (AST)returnAST;
			if ( inputState.guessing==0 ) {
				field_AST = (AST)currentAST.root;
				field_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(INSTANCE_INIT,"INSTANCE_INIT")).add(s4_AST));
				currentAST.root = field_AST;
				currentAST.child = field_AST!=null &&field_AST.getFirstChild()!=null ?
					field_AST.getFirstChild() : field_AST;
				currentAST.advanceChildToEnd();
			}
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = field_AST;
	}
	
	public final void ctorHead() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST ctorHead_AST = null;
		
		AST tmp53_AST = null;
		tmp53_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp53_AST);
		match(IDENT);
		match(LPAREN);
		parameterDeclarationList();
		astFactory.addASTChild(currentAST, returnAST);
		match(RPAREN);
		{
		switch ( LA(1)) {
		case LITERAL_throws:
		{
			throwsClause();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LCURLY:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		ctorHead_AST = (AST)currentAST.root;
		returnAST = ctorHead_AST;
	}
	
	public final void constructorBody() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST constructorBody_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(SLIST);
		}
		{
		if ((LA(1)==LITERAL_this||LA(1)==LITERAL_super) && (LA(2)==LPAREN)) {
			explicitConstructorInvocation();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_8.member(LA(1))) && (_tokenSet_9.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		_loop55:
		do {
			if ((_tokenSet_10.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop55;
			}
			
		} while (true);
		}
		match(RCURLY);
		constructorBody_AST = (AST)currentAST.root;
		returnAST = constructorBody_AST;
	}
	
	public final void parameterDeclarationList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterDeclarationList_AST = null;
		
		{
		switch ( LA(1)) {
		case FINAL:
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		{
			parameterDeclaration();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop80:
			do {
				if ((LA(1)==COMMA)) {
					match(COMMA);
					parameterDeclaration();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop80;
				}
				
			} while (true);
			}
			break;
		}
		case RPAREN:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			parameterDeclarationList_AST = (AST)currentAST.root;
			parameterDeclarationList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(PARAMETERS,"PARAMETERS")).add(parameterDeclarationList_AST));
			currentAST.root = parameterDeclarationList_AST;
			currentAST.child = parameterDeclarationList_AST!=null &&parameterDeclarationList_AST.getFirstChild()!=null ?
				parameterDeclarationList_AST.getFirstChild() : parameterDeclarationList_AST;
			currentAST.advanceChildToEnd();
		}
		parameterDeclarationList_AST = (AST)currentAST.root;
		returnAST = parameterDeclarationList_AST;
	}
	
	public final void declaratorBrackets(
		AST typ
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST declaratorBrackets_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		if ( inputState.guessing==0 ) {
			declaratorBrackets_AST = (AST)currentAST.root;
			declaratorBrackets_AST=typ;
			currentAST.root = declaratorBrackets_AST;
			currentAST.child = declaratorBrackets_AST!=null &&declaratorBrackets_AST.getFirstChild()!=null ?
				declaratorBrackets_AST.getFirstChild() : declaratorBrackets_AST;
			currentAST.advanceChildToEnd();
		}
		{
		_loop63:
		do {
			if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				match(RBRACK);
			}
			else {
				break _loop63;
			}
			
		} while (true);
		}
		declaratorBrackets_AST = (AST)currentAST.root;
		returnAST = declaratorBrackets_AST;
	}
	
	public final void throwsClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST throwsClause_AST = null;
		
		AST tmp59_AST = null;
		tmp59_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp59_AST);
		match(LITERAL_throws);
		identifier();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop76:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				identifier();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop76;
			}
			
		} while (true);
		}
		throwsClause_AST = (AST)currentAST.root;
		returnAST = throwsClause_AST;
	}
	
	public final void compoundStatement() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST compoundStatement_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(SLIST);
		}
		{
		_loop86:
		do {
			if ((_tokenSet_10.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop86;
			}
			
		} while (true);
		}
		match(RCURLY);
		compoundStatement_AST = (AST)currentAST.root;
		returnAST = compoundStatement_AST;
	}
	
/** Catch obvious constructor calls, but not the expr.super(...) calls */
	public final void explicitConstructorInvocation() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST explicitConstructorInvocation_AST = null;
		Token  lp1 = null;
		AST lp1_AST = null;
		Token  lp2 = null;
		AST lp2_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_this:
		{
			match(LITERAL_this);
			lp1 = LT(1);
			lp1_AST = astFactory.create(lp1);
			astFactory.makeASTRoot(currentAST, lp1_AST);
			match(LPAREN);
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(SEMI);
			if ( inputState.guessing==0 ) {
				lp1_AST.setType(CTOR_CALL);
			}
			explicitConstructorInvocation_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_super:
		{
			match(LITERAL_super);
			lp2 = LT(1);
			lp2_AST = astFactory.create(lp2);
			astFactory.makeASTRoot(currentAST, lp2_AST);
			match(LPAREN);
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(SEMI);
			if ( inputState.guessing==0 ) {
				lp2_AST.setType(SUPER_CTOR_CALL);
			}
			explicitConstructorInvocation_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = explicitConstructorInvocation_AST;
	}
	
	public final void statement() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST statement_AST = null;
		AST m_AST = null;
		Token  c = null;
		AST c_AST = null;
		Token  s = null;
		AST s_AST = null;
		
		switch ( LA(1)) {
		case LCURLY:
		{
			compoundStatement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_if:
		{
			AST tmp68_AST = null;
			tmp68_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp68_AST);
			match(LITERAL_if);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==LITERAL_else) && (_tokenSet_10.member(LA(2)))) {
				match(LITERAL_else);
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_12.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_for:
		{
			AST tmp72_AST = null;
			tmp72_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp72_AST);
			match(LITERAL_for);
			match(LPAREN);
			forInit();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			forCond();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			forIter();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_while:
		{
			AST tmp77_AST = null;
			tmp77_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp77_AST);
			match(LITERAL_while);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_do:
		{
			AST tmp80_AST = null;
			tmp80_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp80_AST);
			match(LITERAL_do);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			match(LITERAL_while);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_break:
		{
			AST tmp85_AST = null;
			tmp85_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp85_AST);
			match(LITERAL_break);
			{
			switch ( LA(1)) {
			case IDENT:
			{
				AST tmp86_AST = null;
				tmp86_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp86_AST);
				match(IDENT);
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_continue:
		{
			AST tmp88_AST = null;
			tmp88_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp88_AST);
			match(LITERAL_continue);
			{
			switch ( LA(1)) {
			case IDENT:
			{
				AST tmp89_AST = null;
				tmp89_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp89_AST);
				match(IDENT);
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_return:
		{
			AST tmp91_AST = null;
			tmp91_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp91_AST);
			match(LITERAL_return);
			{
			switch ( LA(1)) {
			case LITERAL_void:
			case LITERAL_boolean:
			case LITERAL_byte:
			case LITERAL_char:
			case LITERAL_short:
			case LITERAL_int:
			case LITERAL_float:
			case LITERAL_long:
			case LITERAL_double:
			case IDENT:
			case LPAREN:
			case LITERAL_this:
			case LITERAL_super:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case BNOT:
			case LNOT:
			case LITERAL_true:
			case LITERAL_false:
			case LITERAL_null:
			case LITERAL_new:
			case NUM_INT:
			case CHAR_LITERAL:
			case STRING_LITERAL:
			case NUM_FLOAT:
			case NUM_LONG:
			case NUM_DOUBLE:
			{
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_switch:
		{
			AST tmp93_AST = null;
			tmp93_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp93_AST);
			match(LITERAL_switch);
			match(LPAREN);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(LCURLY);
			{
			_loop95:
			do {
				if ((LA(1)==LITERAL_case||LA(1)==LITERAL_default)) {
					casesGroup();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop95;
				}
				
			} while (true);
			}
			match(RCURLY);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_try:
		{
			tryBlock();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_throw:
		{
			AST tmp98_AST = null;
			tmp98_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp98_AST);
			match(LITERAL_throw);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_assert:
		{
			AST tmp100_AST = null;
			tmp100_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp100_AST);
			match(LITERAL_assert);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			{
			switch ( LA(1)) {
			case COLON:
			{
				match(COLON);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case SEMI:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			match(SEMI);
			statement_AST = (AST)currentAST.root;
			break;
		}
		case SEMI:
		{
			s = LT(1);
			s_AST = astFactory.create(s);
			astFactory.addASTChild(currentAST, s_AST);
			match(SEMI);
			if ( inputState.guessing==0 ) {
				s_AST.setType(EMPTY_STAT);
			}
			statement_AST = (AST)currentAST.root;
			break;
		}
		default:
			boolean synPredMatched89 = false;
			if (((_tokenSet_13.member(LA(1))) && (_tokenSet_14.member(LA(2))))) {
				int _m89 = mark();
				synPredMatched89 = true;
				inputState.guessing++;
				try {
					{
					declaration();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched89 = false;
				}
				rewind(_m89);
				inputState.guessing--;
			}
			if ( synPredMatched89 ) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
				match(SEMI);
				statement_AST = (AST)currentAST.root;
			}
			else if ((_tokenSet_15.member(LA(1))) && (_tokenSet_16.member(LA(2)))) {
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(SEMI);
				statement_AST = (AST)currentAST.root;
			}
			else if ((_tokenSet_17.member(LA(1))) && (_tokenSet_18.member(LA(2)))) {
				modifiers();
				m_AST = (AST)returnAST;
				classDefinition(m_AST);
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
			else if ((LA(1)==IDENT) && (LA(2)==COLON)) {
				AST tmp105_AST = null;
				tmp105_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp105_AST);
				match(IDENT);
				c = LT(1);
				c_AST = astFactory.create(c);
				astFactory.makeASTRoot(currentAST, c_AST);
				match(COLON);
				if ( inputState.guessing==0 ) {
					c_AST.setType(LABELED_STAT);
				}
				statement();
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
			else if ((LA(1)==LITERAL_synchronized) && (LA(2)==LPAREN)) {
				AST tmp106_AST = null;
				tmp106_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp106_AST);
				match(LITERAL_synchronized);
				match(LPAREN);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(RPAREN);
				compoundStatement();
				astFactory.addASTChild(currentAST, returnAST);
				statement_AST = (AST)currentAST.root;
			}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = statement_AST;
	}
	
	public final void argList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST argList_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case RPAREN:
		{
			if ( inputState.guessing==0 ) {
				argList_AST = (AST)currentAST.root;
				argList_AST = astFactory.create(ELIST,"ELIST");
				currentAST.root = argList_AST;
				currentAST.child = argList_AST!=null &&argList_AST.getFirstChild()!=null ?
					argList_AST.getFirstChild() : argList_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		argList_AST = (AST)currentAST.root;
		returnAST = argList_AST;
	}
	
/** Declaration of a variable.  This can be a class/instance variable,
 *   or a local variable in a method
 * It can also include possible initialization.
 */
	public final void variableDeclarator(
		AST mods, AST t
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST variableDeclarator_AST = null;
		Token  id = null;
		AST id_AST = null;
		AST d_AST = null;
		AST v_AST = null;
		
		id = LT(1);
		id_AST = astFactory.create(id);
		match(IDENT);
		declaratorBrackets(t);
		d_AST = (AST)returnAST;
		varInitializer();
		v_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			variableDeclarator_AST = (AST)currentAST.root;
			variableDeclarator_AST = (AST)astFactory.make( (new ASTArray(5)).add(astFactory.create(VARIABLE_DEF,"VARIABLE_DEF")).add(mods).add((AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(d_AST))).add(id_AST).add(v_AST));
			currentAST.root = variableDeclarator_AST;
			currentAST.child = variableDeclarator_AST!=null &&variableDeclarator_AST.getFirstChild()!=null ?
				variableDeclarator_AST.getFirstChild() : variableDeclarator_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = variableDeclarator_AST;
	}
	
	public final void varInitializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST varInitializer_AST = null;
		
		{
		switch ( LA(1)) {
		case ASSIGN:
		{
			AST tmp109_AST = null;
			tmp109_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp109_AST);
			match(ASSIGN);
			initializer();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case SEMI:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		varInitializer_AST = (AST)currentAST.root;
		returnAST = varInitializer_AST;
	}
	
	public final void initializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST initializer_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			initializer_AST = (AST)currentAST.root;
			break;
		}
		case LCURLY:
		{
			arrayInitializer();
			astFactory.addASTChild(currentAST, returnAST);
			initializer_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = initializer_AST;
	}
	
	public final void arrayInitializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST arrayInitializer_AST = null;
		Token  lc = null;
		AST lc_AST = null;
		
		lc = LT(1);
		lc_AST = astFactory.create(lc);
		astFactory.makeASTRoot(currentAST, lc_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			lc_AST.setType(ARRAY_INIT);
		}
		{
		switch ( LA(1)) {
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LCURLY:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			initializer();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop69:
			do {
				if ((LA(1)==COMMA) && (_tokenSet_19.member(LA(2)))) {
					match(COMMA);
					initializer();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop69;
				}
				
			} while (true);
			}
			{
			switch ( LA(1)) {
			case COMMA:
			{
				match(COMMA);
				break;
			}
			case RCURLY:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		case RCURLY:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RCURLY);
		arrayInitializer_AST = (AST)currentAST.root;
		returnAST = arrayInitializer_AST;
	}
	
	public final void expression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST expression_AST = null;
		
		assignmentExpression();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			expression_AST = (AST)currentAST.root;
			expression_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(EXPR,"EXPR")).add(expression_AST));
			currentAST.root = expression_AST;
			currentAST.child = expression_AST!=null &&expression_AST.getFirstChild()!=null ?
				expression_AST.getFirstChild() : expression_AST;
			currentAST.advanceChildToEnd();
		}
		expression_AST = (AST)currentAST.root;
		returnAST = expression_AST;
	}
	
	public final void parameterDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterDeclaration_AST = null;
		AST pm_AST = null;
		AST t_AST = null;
		Token  id = null;
		AST id_AST = null;
		AST pd_AST = null;
		
		parameterModifier();
		pm_AST = (AST)returnAST;
		typeSpec(false);
		t_AST = (AST)returnAST;
		id = LT(1);
		id_AST = astFactory.create(id);
		match(IDENT);
		declaratorBrackets(t_AST);
		pd_AST = (AST)returnAST;
		if ( inputState.guessing==0 ) {
			parameterDeclaration_AST = (AST)currentAST.root;
			parameterDeclaration_AST = (AST)astFactory.make( (new ASTArray(4)).add(astFactory.create(PARAMETER_DEF,"PARAMETER_DEF")).add(pm_AST).add((AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(TYPE,"TYPE")).add(pd_AST))).add(id_AST));
			currentAST.root = parameterDeclaration_AST;
			currentAST.child = parameterDeclaration_AST!=null &&parameterDeclaration_AST.getFirstChild()!=null ?
				parameterDeclaration_AST.getFirstChild() : parameterDeclaration_AST;
			currentAST.advanceChildToEnd();
		}
		returnAST = parameterDeclaration_AST;
	}
	
	public final void parameterModifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST parameterModifier_AST = null;
		Token  f = null;
		AST f_AST = null;
		
		{
		switch ( LA(1)) {
		case FINAL:
		{
			f = LT(1);
			f_AST = astFactory.create(f);
			astFactory.addASTChild(currentAST, f_AST);
			match(FINAL);
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			parameterModifier_AST = (AST)currentAST.root;
			parameterModifier_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(MODIFIERS,"MODIFIERS")).add(f_AST));
			currentAST.root = parameterModifier_AST;
			currentAST.child = parameterModifier_AST!=null &&parameterModifier_AST.getFirstChild()!=null ?
				parameterModifier_AST.getFirstChild() : parameterModifier_AST;
			currentAST.advanceChildToEnd();
		}
		parameterModifier_AST = (AST)currentAST.root;
		returnAST = parameterModifier_AST;
	}
	
	public final void forInit() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forInit_AST = null;
		
		{
		boolean synPredMatched108 = false;
		if (((_tokenSet_13.member(LA(1))) && (_tokenSet_14.member(LA(2))))) {
			int _m108 = mark();
			synPredMatched108 = true;
			inputState.guessing++;
			try {
				{
				declaration();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched108 = false;
			}
			rewind(_m108);
			inputState.guessing--;
		}
		if ( synPredMatched108 ) {
			declaration();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_15.member(LA(1))) && (_tokenSet_20.member(LA(2)))) {
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((LA(1)==SEMI)) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			forInit_AST = (AST)currentAST.root;
			forInit_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_INIT,"FOR_INIT")).add(forInit_AST));
			currentAST.root = forInit_AST;
			currentAST.child = forInit_AST!=null &&forInit_AST.getFirstChild()!=null ?
				forInit_AST.getFirstChild() : forInit_AST;
			currentAST.advanceChildToEnd();
		}
		forInit_AST = (AST)currentAST.root;
		returnAST = forInit_AST;
	}
	
	public final void forCond() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forCond_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case SEMI:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			forCond_AST = (AST)currentAST.root;
			forCond_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_CONDITION,"FOR_CONDITION")).add(forCond_AST));
			currentAST.root = forCond_AST;
			currentAST.child = forCond_AST!=null &&forCond_AST.getFirstChild()!=null ?
				forCond_AST.getFirstChild() : forCond_AST;
			currentAST.advanceChildToEnd();
		}
		forCond_AST = (AST)currentAST.root;
		returnAST = forCond_AST;
	}
	
	public final void forIter() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST forIter_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			expressionList();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case RPAREN:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			forIter_AST = (AST)currentAST.root;
			forIter_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(FOR_ITERATOR,"FOR_ITERATOR")).add(forIter_AST));
			currentAST.root = forIter_AST;
			currentAST.child = forIter_AST!=null &&forIter_AST.getFirstChild()!=null ?
				forIter_AST.getFirstChild() : forIter_AST;
			currentAST.advanceChildToEnd();
		}
		forIter_AST = (AST)currentAST.root;
		returnAST = forIter_AST;
	}
	
	public final void casesGroup() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST casesGroup_AST = null;
		
		{
		int _cnt99=0;
		_loop99:
		do {
			if ((LA(1)==LITERAL_case||LA(1)==LITERAL_default) && (_tokenSet_21.member(LA(2)))) {
				aCase();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt99>=1 ) { break _loop99; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt99++;
		} while (true);
		}
		caseSList();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			casesGroup_AST = (AST)currentAST.root;
			casesGroup_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(CASE_GROUP,"CASE_GROUP")).add(casesGroup_AST));
			currentAST.root = casesGroup_AST;
			currentAST.child = casesGroup_AST!=null &&casesGroup_AST.getFirstChild()!=null ?
				casesGroup_AST.getFirstChild() : casesGroup_AST;
			currentAST.advanceChildToEnd();
		}
		casesGroup_AST = (AST)currentAST.root;
		returnAST = casesGroup_AST;
	}
	
	public final void tryBlock() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST tryBlock_AST = null;
		
		AST tmp113_AST = null;
		tmp113_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp113_AST);
		match(LITERAL_try);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop115:
		do {
			if ((LA(1)==LITERAL_catch)) {
				handler();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop115;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case LITERAL_finally:
		{
			finallyClause();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case FINAL:
		case ABSTRACT:
		case STRICTFP:
		case SEMI:
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LITERAL_private:
		case LITERAL_public:
		case LITERAL_protected:
		case LITERAL_static:
		case LITERAL_transient:
		case LITERAL_native:
		case LITERAL_threadsafe:
		case LITERAL_synchronized:
		case LITERAL_volatile:
		case LITERAL_class:
		case LCURLY:
		case RCURLY:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case LITERAL_if:
		case LITERAL_else:
		case LITERAL_for:
		case LITERAL_while:
		case LITERAL_do:
		case LITERAL_break:
		case LITERAL_continue:
		case LITERAL_return:
		case LITERAL_switch:
		case LITERAL_throw:
		case LITERAL_assert:
		case LITERAL_case:
		case LITERAL_default:
		case LITERAL_try:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		tryBlock_AST = (AST)currentAST.root;
		returnAST = tryBlock_AST;
	}
	
	public final void aCase() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST aCase_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_case:
		{
			AST tmp114_AST = null;
			tmp114_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp114_AST);
			match(LITERAL_case);
			expression();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LITERAL_default:
		{
			AST tmp115_AST = null;
			tmp115_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp115_AST);
			match(LITERAL_default);
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(COLON);
		aCase_AST = (AST)currentAST.root;
		returnAST = aCase_AST;
	}
	
	public final void caseSList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST caseSList_AST = null;
		
		{
		_loop104:
		do {
			if ((_tokenSet_10.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop104;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			caseSList_AST = (AST)currentAST.root;
			caseSList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(SLIST,"SLIST")).add(caseSList_AST));
			currentAST.root = caseSList_AST;
			currentAST.child = caseSList_AST!=null &&caseSList_AST.getFirstChild()!=null ?
				caseSList_AST.getFirstChild() : caseSList_AST;
			currentAST.advanceChildToEnd();
		}
		caseSList_AST = (AST)currentAST.root;
		returnAST = caseSList_AST;
	}
	
	public final void expressionList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST expressionList_AST = null;
		
		expression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop122:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				expression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop122;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			expressionList_AST = (AST)currentAST.root;
			expressionList_AST = (AST)astFactory.make( (new ASTArray(2)).add(astFactory.create(ELIST,"ELIST")).add(expressionList_AST));
			currentAST.root = expressionList_AST;
			currentAST.child = expressionList_AST!=null &&expressionList_AST.getFirstChild()!=null ?
				expressionList_AST.getFirstChild() : expressionList_AST;
			currentAST.advanceChildToEnd();
		}
		expressionList_AST = (AST)currentAST.root;
		returnAST = expressionList_AST;
	}
	
	public final void handler() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST handler_AST = null;
		
		AST tmp118_AST = null;
		tmp118_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp118_AST);
		match(LITERAL_catch);
		match(LPAREN);
		parameterDeclaration();
		astFactory.addASTChild(currentAST, returnAST);
		match(RPAREN);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		handler_AST = (AST)currentAST.root;
		returnAST = handler_AST;
	}
	
	public final void finallyClause() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST finallyClause_AST = null;
		
		AST tmp121_AST = null;
		tmp121_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp121_AST);
		match(LITERAL_finally);
		compoundStatement();
		astFactory.addASTChild(currentAST, returnAST);
		finallyClause_AST = (AST)currentAST.root;
		returnAST = finallyClause_AST;
	}
	
	public final void assignmentExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST assignmentExpression_AST = null;
		
		conditionalExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case ASSIGN:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case DIV_ASSIGN:
		case MOD_ASSIGN:
		case SR_ASSIGN:
		case BSR_ASSIGN:
		case SL_ASSIGN:
		case BAND_ASSIGN:
		case BXOR_ASSIGN:
		case BOR_ASSIGN:
		{
			{
			switch ( LA(1)) {
			case ASSIGN:
			{
				AST tmp122_AST = null;
				tmp122_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp122_AST);
				match(ASSIGN);
				break;
			}
			case PLUS_ASSIGN:
			{
				AST tmp123_AST = null;
				tmp123_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp123_AST);
				match(PLUS_ASSIGN);
				break;
			}
			case MINUS_ASSIGN:
			{
				AST tmp124_AST = null;
				tmp124_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp124_AST);
				match(MINUS_ASSIGN);
				break;
			}
			case STAR_ASSIGN:
			{
				AST tmp125_AST = null;
				tmp125_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp125_AST);
				match(STAR_ASSIGN);
				break;
			}
			case DIV_ASSIGN:
			{
				AST tmp126_AST = null;
				tmp126_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp126_AST);
				match(DIV_ASSIGN);
				break;
			}
			case MOD_ASSIGN:
			{
				AST tmp127_AST = null;
				tmp127_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp127_AST);
				match(MOD_ASSIGN);
				break;
			}
			case SR_ASSIGN:
			{
				AST tmp128_AST = null;
				tmp128_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp128_AST);
				match(SR_ASSIGN);
				break;
			}
			case BSR_ASSIGN:
			{
				AST tmp129_AST = null;
				tmp129_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp129_AST);
				match(BSR_ASSIGN);
				break;
			}
			case SL_ASSIGN:
			{
				AST tmp130_AST = null;
				tmp130_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp130_AST);
				match(SL_ASSIGN);
				break;
			}
			case BAND_ASSIGN:
			{
				AST tmp131_AST = null;
				tmp131_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp131_AST);
				match(BAND_ASSIGN);
				break;
			}
			case BXOR_ASSIGN:
			{
				AST tmp132_AST = null;
				tmp132_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp132_AST);
				match(BXOR_ASSIGN);
				break;
			}
			case BOR_ASSIGN:
			{
				AST tmp133_AST = null;
				tmp133_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp133_AST);
				match(BOR_ASSIGN);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case SEMI:
		case RBRACK:
		case RCURLY:
		case COMMA:
		case RPAREN:
		case COLON:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		assignmentExpression_AST = (AST)currentAST.root;
		returnAST = assignmentExpression_AST;
	}
	
	public final void conditionalExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST conditionalExpression_AST = null;
		
		logicalOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case QUESTION:
		{
			AST tmp134_AST = null;
			tmp134_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp134_AST);
			match(QUESTION);
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
			match(COLON);
			conditionalExpression();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case SEMI:
		case RBRACK:
		case RCURLY:
		case COMMA:
		case RPAREN:
		case ASSIGN:
		case COLON:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case DIV_ASSIGN:
		case MOD_ASSIGN:
		case SR_ASSIGN:
		case BSR_ASSIGN:
		case SL_ASSIGN:
		case BAND_ASSIGN:
		case BXOR_ASSIGN:
		case BOR_ASSIGN:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		conditionalExpression_AST = (AST)currentAST.root;
		returnAST = conditionalExpression_AST;
	}
	
	public final void logicalOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST logicalOrExpression_AST = null;
		
		logicalAndExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop130:
		do {
			if ((LA(1)==LOR)) {
				AST tmp136_AST = null;
				tmp136_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp136_AST);
				match(LOR);
				logicalAndExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop130;
			}
			
		} while (true);
		}
		logicalOrExpression_AST = (AST)currentAST.root;
		returnAST = logicalOrExpression_AST;
	}
	
	public final void logicalAndExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST logicalAndExpression_AST = null;
		
		inclusiveOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop133:
		do {
			if ((LA(1)==LAND)) {
				AST tmp137_AST = null;
				tmp137_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp137_AST);
				match(LAND);
				inclusiveOrExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop133;
			}
			
		} while (true);
		}
		logicalAndExpression_AST = (AST)currentAST.root;
		returnAST = logicalAndExpression_AST;
	}
	
	public final void inclusiveOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST inclusiveOrExpression_AST = null;
		
		exclusiveOrExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop136:
		do {
			if ((LA(1)==BOR)) {
				AST tmp138_AST = null;
				tmp138_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp138_AST);
				match(BOR);
				exclusiveOrExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop136;
			}
			
		} while (true);
		}
		inclusiveOrExpression_AST = (AST)currentAST.root;
		returnAST = inclusiveOrExpression_AST;
	}
	
	public final void exclusiveOrExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST exclusiveOrExpression_AST = null;
		
		andExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop139:
		do {
			if ((LA(1)==BXOR)) {
				AST tmp139_AST = null;
				tmp139_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp139_AST);
				match(BXOR);
				andExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop139;
			}
			
		} while (true);
		}
		exclusiveOrExpression_AST = (AST)currentAST.root;
		returnAST = exclusiveOrExpression_AST;
	}
	
	public final void andExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST andExpression_AST = null;
		
		equalityExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop142:
		do {
			if ((LA(1)==BAND)) {
				AST tmp140_AST = null;
				tmp140_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp140_AST);
				match(BAND);
				equalityExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop142;
			}
			
		} while (true);
		}
		andExpression_AST = (AST)currentAST.root;
		returnAST = andExpression_AST;
	}
	
	public final void equalityExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST equalityExpression_AST = null;
		
		relationalExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop146:
		do {
			if ((LA(1)==NOT_EQUAL||LA(1)==EQUAL)) {
				{
				switch ( LA(1)) {
				case NOT_EQUAL:
				{
					AST tmp141_AST = null;
					tmp141_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp141_AST);
					match(NOT_EQUAL);
					break;
				}
				case EQUAL:
				{
					AST tmp142_AST = null;
					tmp142_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp142_AST);
					match(EQUAL);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				relationalExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop146;
			}
			
		} while (true);
		}
		equalityExpression_AST = (AST)currentAST.root;
		returnAST = equalityExpression_AST;
	}
	
	public final void relationalExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST relationalExpression_AST = null;
		
		shiftExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case SEMI:
		case RBRACK:
		case RCURLY:
		case COMMA:
		case RPAREN:
		case ASSIGN:
		case COLON:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case DIV_ASSIGN:
		case MOD_ASSIGN:
		case SR_ASSIGN:
		case BSR_ASSIGN:
		case SL_ASSIGN:
		case BAND_ASSIGN:
		case BXOR_ASSIGN:
		case BOR_ASSIGN:
		case QUESTION:
		case LOR:
		case LAND:
		case BOR:
		case BXOR:
		case BAND:
		case NOT_EQUAL:
		case EQUAL:
		case LT:
		case GT:
		case LE:
		case GE:
		{
			{
			_loop151:
			do {
				if (((LA(1) >= LT && LA(1) <= GE))) {
					{
					switch ( LA(1)) {
					case LT:
					{
						AST tmp143_AST = null;
						tmp143_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp143_AST);
						match(LT);
						break;
					}
					case GT:
					{
						AST tmp144_AST = null;
						tmp144_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp144_AST);
						match(GT);
						break;
					}
					case LE:
					{
						AST tmp145_AST = null;
						tmp145_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp145_AST);
						match(LE);
						break;
					}
					case GE:
					{
						AST tmp146_AST = null;
						tmp146_AST = astFactory.create(LT(1));
						astFactory.makeASTRoot(currentAST, tmp146_AST);
						match(GE);
						break;
					}
					default:
					{
						throw new NoViableAltException(LT(1), getFilename());
					}
					}
					}
					shiftExpression();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop151;
				}
				
			} while (true);
			}
			break;
		}
		case LITERAL_instanceof:
		{
			AST tmp147_AST = null;
			tmp147_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp147_AST);
			match(LITERAL_instanceof);
			typeSpec(true);
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		relationalExpression_AST = (AST)currentAST.root;
		returnAST = relationalExpression_AST;
	}
	
	public final void shiftExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST shiftExpression_AST = null;
		
		additiveExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop155:
		do {
			if (((LA(1) >= SL && LA(1) <= BSR))) {
				{
				switch ( LA(1)) {
				case SL:
				{
					AST tmp148_AST = null;
					tmp148_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp148_AST);
					match(SL);
					break;
				}
				case SR:
				{
					AST tmp149_AST = null;
					tmp149_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp149_AST);
					match(SR);
					break;
				}
				case BSR:
				{
					AST tmp150_AST = null;
					tmp150_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp150_AST);
					match(BSR);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				additiveExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop155;
			}
			
		} while (true);
		}
		shiftExpression_AST = (AST)currentAST.root;
		returnAST = shiftExpression_AST;
	}
	
	public final void additiveExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST additiveExpression_AST = null;
		
		multiplicativeExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop159:
		do {
			if ((LA(1)==PLUS||LA(1)==MINUS)) {
				{
				switch ( LA(1)) {
				case PLUS:
				{
					AST tmp151_AST = null;
					tmp151_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp151_AST);
					match(PLUS);
					break;
				}
				case MINUS:
				{
					AST tmp152_AST = null;
					tmp152_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp152_AST);
					match(MINUS);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				multiplicativeExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop159;
			}
			
		} while (true);
		}
		additiveExpression_AST = (AST)currentAST.root;
		returnAST = additiveExpression_AST;
	}
	
	public final void multiplicativeExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST multiplicativeExpression_AST = null;
		
		unaryExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop163:
		do {
			if ((_tokenSet_22.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case STAR:
				{
					AST tmp153_AST = null;
					tmp153_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp153_AST);
					match(STAR);
					break;
				}
				case DIV:
				{
					AST tmp154_AST = null;
					tmp154_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp154_AST);
					match(DIV);
					break;
				}
				case MOD:
				{
					AST tmp155_AST = null;
					tmp155_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp155_AST);
					match(MOD);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				unaryExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop163;
			}
			
		} while (true);
		}
		multiplicativeExpression_AST = (AST)currentAST.root;
		returnAST = multiplicativeExpression_AST;
	}
	
	public final void unaryExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST unaryExpression_AST = null;
		
		switch ( LA(1)) {
		case INC:
		{
			AST tmp156_AST = null;
			tmp156_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp156_AST);
			match(INC);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case DEC:
		{
			AST tmp157_AST = null;
			tmp157_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp157_AST);
			match(DEC);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case MINUS:
		{
			AST tmp158_AST = null;
			tmp158_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp158_AST);
			match(MINUS);
			if ( inputState.guessing==0 ) {
				tmp158_AST.setType(UNARY_MINUS);
			}
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case PLUS:
		{
			AST tmp159_AST = null;
			tmp159_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp159_AST);
			match(PLUS);
			if ( inputState.guessing==0 ) {
				tmp159_AST.setType(UNARY_PLUS);
			}
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case BNOT:
		case LNOT:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			unaryExpressionNotPlusMinus();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpression_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = unaryExpression_AST;
	}
	
	public final void unaryExpressionNotPlusMinus() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST unaryExpressionNotPlusMinus_AST = null;
		Token  lpb = null;
		AST lpb_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		
		switch ( LA(1)) {
		case BNOT:
		{
			AST tmp160_AST = null;
			tmp160_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp160_AST);
			match(BNOT);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
			break;
		}
		case LNOT:
		{
			AST tmp161_AST = null;
			tmp161_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp161_AST);
			match(LNOT);
			unaryExpression();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		case IDENT:
		case LPAREN:
		case LITERAL_this:
		case LITERAL_super:
		case LITERAL_true:
		case LITERAL_false:
		case LITERAL_null:
		case LITERAL_new:
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			{
			if ((LA(1)==LPAREN) && ((LA(2) >= LITERAL_void && LA(2) <= LITERAL_double))) {
				lpb = LT(1);
				lpb_AST = astFactory.create(lpb);
				astFactory.makeASTRoot(currentAST, lpb_AST);
				match(LPAREN);
				if ( inputState.guessing==0 ) {
					lpb_AST.setType(TYPECAST);
				}
				builtInTypeSpec(true);
				astFactory.addASTChild(currentAST, returnAST);
				match(RPAREN);
				unaryExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				boolean synPredMatched168 = false;
				if (((LA(1)==LPAREN) && (LA(2)==IDENT))) {
					int _m168 = mark();
					synPredMatched168 = true;
					inputState.guessing++;
					try {
						{
						match(LPAREN);
						classTypeSpec(true);
						match(RPAREN);
						unaryExpressionNotPlusMinus();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched168 = false;
					}
					rewind(_m168);
					inputState.guessing--;
				}
				if ( synPredMatched168 ) {
					lp = LT(1);
					lp_AST = astFactory.create(lp);
					astFactory.makeASTRoot(currentAST, lp_AST);
					match(LPAREN);
					if ( inputState.guessing==0 ) {
						lp_AST.setType(TYPECAST);
					}
					classTypeSpec(true);
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
					unaryExpressionNotPlusMinus();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else if ((_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
					postfixExpression();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				unaryExpressionNotPlusMinus_AST = (AST)currentAST.root;
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			returnAST = unaryExpressionNotPlusMinus_AST;
		}
		
	public final void postfixExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST postfixExpression_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		Token  lp3 = null;
		AST lp3_AST = null;
		Token  lps = null;
		AST lps_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		Token  in = null;
		AST in_AST = null;
		Token  de = null;
		AST de_AST = null;
		
		primaryExpression();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop174:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp164_AST = null;
				tmp164_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp164_AST);
				match(DOT);
				AST tmp165_AST = null;
				tmp165_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp165_AST);
				match(IDENT);
				{
				switch ( LA(1)) {
				case LPAREN:
				{
					lp = LT(1);
					lp_AST = astFactory.create(lp);
					astFactory.makeASTRoot(currentAST, lp_AST);
					match(LPAREN);
					if ( inputState.guessing==0 ) {
						lp_AST.setType(METHOD_CALL);
					}
					argList();
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
					break;
				}
				case SEMI:
				case LBRACK:
				case RBRACK:
				case DOT:
				case STAR:
				case RCURLY:
				case COMMA:
				case RPAREN:
				case ASSIGN:
				case COLON:
				case PLUS_ASSIGN:
				case MINUS_ASSIGN:
				case STAR_ASSIGN:
				case DIV_ASSIGN:
				case MOD_ASSIGN:
				case SR_ASSIGN:
				case BSR_ASSIGN:
				case SL_ASSIGN:
				case BAND_ASSIGN:
				case BXOR_ASSIGN:
				case BOR_ASSIGN:
				case QUESTION:
				case LOR:
				case LAND:
				case BOR:
				case BXOR:
				case BAND:
				case NOT_EQUAL:
				case EQUAL:
				case LT:
				case GT:
				case LE:
				case GE:
				case LITERAL_instanceof:
				case SL:
				case SR:
				case BSR:
				case PLUS:
				case MINUS:
				case DIV:
				case MOD:
				case INC:
				case DEC:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_this)) {
				AST tmp167_AST = null;
				tmp167_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp167_AST);
				match(DOT);
				AST tmp168_AST = null;
				tmp168_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp168_AST);
				match(LITERAL_this);
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_super)) {
				AST tmp169_AST = null;
				tmp169_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp169_AST);
				match(DOT);
				AST tmp170_AST = null;
				tmp170_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp170_AST);
				match(LITERAL_super);
				{
				switch ( LA(1)) {
				case LPAREN:
				{
					lp3 = LT(1);
					lp3_AST = astFactory.create(lp3);
					astFactory.makeASTRoot(currentAST, lp3_AST);
					match(LPAREN);
					argList();
					astFactory.addASTChild(currentAST, returnAST);
					match(RPAREN);
					if ( inputState.guessing==0 ) {
						lp3_AST.setType(SUPER_CTOR_CALL);
					}
					break;
				}
				case DOT:
				{
					AST tmp172_AST = null;
					tmp172_AST = astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp172_AST);
					match(DOT);
					AST tmp173_AST = null;
					tmp173_AST = astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp173_AST);
					match(IDENT);
					{
					switch ( LA(1)) {
					case LPAREN:
					{
						lps = LT(1);
						lps_AST = astFactory.create(lps);
						astFactory.makeASTRoot(currentAST, lps_AST);
						match(LPAREN);
						if ( inputState.guessing==0 ) {
							lps_AST.setType(METHOD_CALL);
						}
						argList();
						astFactory.addASTChild(currentAST, returnAST);
						match(RPAREN);
						break;
					}
					case SEMI:
					case LBRACK:
					case RBRACK:
					case DOT:
					case STAR:
					case RCURLY:
					case COMMA:
					case RPAREN:
					case ASSIGN:
					case COLON:
					case PLUS_ASSIGN:
					case MINUS_ASSIGN:
					case STAR_ASSIGN:
					case DIV_ASSIGN:
					case MOD_ASSIGN:
					case SR_ASSIGN:
					case BSR_ASSIGN:
					case SL_ASSIGN:
					case BAND_ASSIGN:
					case BXOR_ASSIGN:
					case BOR_ASSIGN:
					case QUESTION:
					case LOR:
					case LAND:
					case BOR:
					case BXOR:
					case BAND:
					case NOT_EQUAL:
					case EQUAL:
					case LT:
					case GT:
					case LE:
					case GE:
					case LITERAL_instanceof:
					case SL:
					case SR:
					case BSR:
					case PLUS:
					case MINUS:
					case DIV:
					case MOD:
					case INC:
					case DEC:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(LT(1), getFilename());
					}
					}
					}
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
			}
			else if ((LA(1)==DOT) && (LA(2)==LITERAL_new)) {
				AST tmp175_AST = null;
				tmp175_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp175_AST);
				match(DOT);
				newExpression();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==LBRACK)) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(INDEX_OP);
				}
				expression();
				astFactory.addASTChild(currentAST, returnAST);
				match(RBRACK);
			}
			else {
				break _loop174;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case INC:
		{
			in = LT(1);
			in_AST = astFactory.create(in);
			astFactory.makeASTRoot(currentAST, in_AST);
			match(INC);
			if ( inputState.guessing==0 ) {
				in_AST.setType(POST_INC);
			}
			break;
		}
		case DEC:
		{
			de = LT(1);
			de_AST = astFactory.create(de);
			astFactory.makeASTRoot(currentAST, de_AST);
			match(DEC);
			if ( inputState.guessing==0 ) {
				de_AST.setType(POST_DEC);
			}
			break;
		}
		case SEMI:
		case RBRACK:
		case STAR:
		case RCURLY:
		case COMMA:
		case RPAREN:
		case ASSIGN:
		case COLON:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case DIV_ASSIGN:
		case MOD_ASSIGN:
		case SR_ASSIGN:
		case BSR_ASSIGN:
		case SL_ASSIGN:
		case BAND_ASSIGN:
		case BXOR_ASSIGN:
		case BOR_ASSIGN:
		case QUESTION:
		case LOR:
		case LAND:
		case BOR:
		case BXOR:
		case BAND:
		case NOT_EQUAL:
		case EQUAL:
		case LT:
		case GT:
		case LE:
		case GE:
		case LITERAL_instanceof:
		case SL:
		case SR:
		case BSR:
		case PLUS:
		case MINUS:
		case DIV:
		case MOD:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		postfixExpression_AST = (AST)currentAST.root;
		returnAST = postfixExpression_AST;
	}
	
	public final void primaryExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST primaryExpression_AST = null;
		Token  lbt = null;
		AST lbt_AST = null;
		
		switch ( LA(1)) {
		case IDENT:
		{
			identPrimary();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==DOT) && (LA(2)==LITERAL_class)) {
				AST tmp177_AST = null;
				tmp177_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp177_AST);
				match(DOT);
				AST tmp178_AST = null;
				tmp178_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp178_AST);
				match(LITERAL_class);
			}
			else if ((_tokenSet_25.member(LA(1))) && (_tokenSet_26.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case NUM_INT:
		case CHAR_LITERAL:
		case STRING_LITERAL:
		case NUM_FLOAT:
		case NUM_LONG:
		case NUM_DOUBLE:
		{
			constant();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_true:
		{
			AST tmp179_AST = null;
			tmp179_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp179_AST);
			match(LITERAL_true);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_false:
		{
			AST tmp180_AST = null;
			tmp180_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp180_AST);
			match(LITERAL_false);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_null:
		{
			AST tmp181_AST = null;
			tmp181_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp181_AST);
			match(LITERAL_null);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_new:
		{
			newExpression();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_this:
		{
			AST tmp182_AST = null;
			tmp182_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp182_AST);
			match(LITERAL_this);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_super:
		{
			AST tmp183_AST = null;
			tmp183_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp183_AST);
			match(LITERAL_super);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LPAREN:
		{
			match(LPAREN);
			assignmentExpression();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		case LITERAL_void:
		case LITERAL_boolean:
		case LITERAL_byte:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_float:
		case LITERAL_long:
		case LITERAL_double:
		{
			builtInType();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop179:
			do {
				if ((LA(1)==LBRACK)) {
					lbt = LT(1);
					lbt_AST = astFactory.create(lbt);
					astFactory.makeASTRoot(currentAST, lbt_AST);
					match(LBRACK);
					if ( inputState.guessing==0 ) {
						lbt_AST.setType(ARRAY_DECLARATOR);
					}
					match(RBRACK);
				}
				else {
					break _loop179;
				}
				
			} while (true);
			}
			AST tmp187_AST = null;
			tmp187_AST = astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp187_AST);
			match(DOT);
			AST tmp188_AST = null;
			tmp188_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp188_AST);
			match(LITERAL_class);
			primaryExpression_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = primaryExpression_AST;
	}
	
/** object instantiation.
 *  Trees are built as illustrated by the following input/tree pairs:
 *
 *  new T()
 *
 *  new
 *   |
 *   T --  ELIST
 *           |
 *          arg1 -- arg2 -- .. -- argn
 *
 *  new int[]
 *
 *  new
 *   |
 *  int -- ARRAY_DECLARATOR
 *
 *  new int[] {1,2}
 *
 *  new
 *   |
 *  int -- ARRAY_DECLARATOR -- ARRAY_INIT
 *                                  |
 *                                EXPR -- EXPR
 *                                  |      |
 *                                  1      2
 *
 *  new int[3]
 *  new
 *   |
 *  int -- ARRAY_DECLARATOR
 *                |
 *              EXPR
 *                |
 *                3
 *
 *  new int[1][2]
 *
 *  new
 *   |
 *  int -- ARRAY_DECLARATOR
 *               |
 *         ARRAY_DECLARATOR -- EXPR
 *               |              |
 *             EXPR             1
 *               |
 *               2
 *
 */
	public final void newExpression() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST newExpression_AST = null;
		
		AST tmp189_AST = null;
		tmp189_AST = astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp189_AST);
		match(LITERAL_new);
		type();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case LPAREN:
		{
			match(LPAREN);
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			{
			switch ( LA(1)) {
			case LCURLY:
			{
				classBlock();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case SEMI:
			case LBRACK:
			case RBRACK:
			case DOT:
			case STAR:
			case RCURLY:
			case COMMA:
			case RPAREN:
			case ASSIGN:
			case COLON:
			case PLUS_ASSIGN:
			case MINUS_ASSIGN:
			case STAR_ASSIGN:
			case DIV_ASSIGN:
			case MOD_ASSIGN:
			case SR_ASSIGN:
			case BSR_ASSIGN:
			case SL_ASSIGN:
			case BAND_ASSIGN:
			case BXOR_ASSIGN:
			case BOR_ASSIGN:
			case QUESTION:
			case LOR:
			case LAND:
			case BOR:
			case BXOR:
			case BAND:
			case NOT_EQUAL:
			case EQUAL:
			case LT:
			case GT:
			case LE:
			case GE:
			case LITERAL_instanceof:
			case SL:
			case SR:
			case BSR:
			case PLUS:
			case MINUS:
			case DIV:
			case MOD:
			case INC:
			case DEC:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		case LBRACK:
		{
			newArrayDeclarator();
			astFactory.addASTChild(currentAST, returnAST);
			{
			switch ( LA(1)) {
			case LCURLY:
			{
				arrayInitializer();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case SEMI:
			case LBRACK:
			case RBRACK:
			case DOT:
			case STAR:
			case RCURLY:
			case COMMA:
			case RPAREN:
			case ASSIGN:
			case COLON:
			case PLUS_ASSIGN:
			case MINUS_ASSIGN:
			case STAR_ASSIGN:
			case DIV_ASSIGN:
			case MOD_ASSIGN:
			case SR_ASSIGN:
			case BSR_ASSIGN:
			case SL_ASSIGN:
			case BAND_ASSIGN:
			case BXOR_ASSIGN:
			case BOR_ASSIGN:
			case QUESTION:
			case LOR:
			case LAND:
			case BOR:
			case BXOR:
			case BAND:
			case NOT_EQUAL:
			case EQUAL:
			case LT:
			case GT:
			case LE:
			case GE:
			case LITERAL_instanceof:
			case SL:
			case SR:
			case BSR:
			case PLUS:
			case MINUS:
			case DIV:
			case MOD:
			case INC:
			case DEC:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		newExpression_AST = (AST)currentAST.root;
		returnAST = newExpression_AST;
	}
	
/** Match a, a.b.c refs, a.b.c(...) refs, a.b.c[], a.b.c[].class,
 *  and a.b.c.class refs.  Also this(...) and super(...).  Match
 *  this or super.
 */
	public final void identPrimary() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST identPrimary_AST = null;
		Token  lp = null;
		AST lp_AST = null;
		Token  lbc = null;
		AST lbc_AST = null;
		
		AST tmp192_AST = null;
		tmp192_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp192_AST);
		match(IDENT);
		{
		_loop182:
		do {
			if ((LA(1)==DOT) && (LA(2)==IDENT)) {
				AST tmp193_AST = null;
				tmp193_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp193_AST);
				match(DOT);
				AST tmp194_AST = null;
				tmp194_AST = astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp194_AST);
				match(IDENT);
			}
			else {
				break _loop182;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==LPAREN)) {
			{
			lp = LT(1);
			lp_AST = astFactory.create(lp);
			astFactory.makeASTRoot(currentAST, lp_AST);
			match(LPAREN);
			if ( inputState.guessing==0 ) {
				lp_AST.setType(METHOD_CALL);
			}
			argList();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			}
		}
		else if ((LA(1)==LBRACK) && (LA(2)==RBRACK)) {
			{
			int _cnt186=0;
			_loop186:
			do {
				if ((LA(1)==LBRACK) && (LA(2)==RBRACK)) {
					lbc = LT(1);
					lbc_AST = astFactory.create(lbc);
					astFactory.makeASTRoot(currentAST, lbc_AST);
					match(LBRACK);
					if ( inputState.guessing==0 ) {
						lbc_AST.setType(ARRAY_DECLARATOR);
					}
					match(RBRACK);
				}
				else {
					if ( _cnt186>=1 ) { break _loop186; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt186++;
			} while (true);
			}
		}
		else if ((_tokenSet_25.member(LA(1))) && (_tokenSet_26.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		identPrimary_AST = (AST)currentAST.root;
		returnAST = identPrimary_AST;
	}
	
	public final void constant() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST constant_AST = null;
		
		switch ( LA(1)) {
		case NUM_INT:
		{
			AST tmp197_AST = null;
			tmp197_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp197_AST);
			match(NUM_INT);
			constant_AST = (AST)currentAST.root;
			break;
		}
		case CHAR_LITERAL:
		{
			AST tmp198_AST = null;
			tmp198_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp198_AST);
			match(CHAR_LITERAL);
			constant_AST = (AST)currentAST.root;
			break;
		}
		case STRING_LITERAL:
		{
			AST tmp199_AST = null;
			tmp199_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp199_AST);
			match(STRING_LITERAL);
			constant_AST = (AST)currentAST.root;
			break;
		}
		case NUM_FLOAT:
		{
			AST tmp200_AST = null;
			tmp200_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp200_AST);
			match(NUM_FLOAT);
			constant_AST = (AST)currentAST.root;
			break;
		}
		case NUM_LONG:
		{
			AST tmp201_AST = null;
			tmp201_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp201_AST);
			match(NUM_LONG);
			constant_AST = (AST)currentAST.root;
			break;
		}
		case NUM_DOUBLE:
		{
			AST tmp202_AST = null;
			tmp202_AST = astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp202_AST);
			match(NUM_DOUBLE);
			constant_AST = (AST)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = constant_AST;
	}
	
	public final void newArrayDeclarator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		AST newArrayDeclarator_AST = null;
		Token  lb = null;
		AST lb_AST = null;
		
		{
		int _cnt196=0;
		_loop196:
		do {
			if ((LA(1)==LBRACK) && (_tokenSet_27.member(LA(2)))) {
				lb = LT(1);
				lb_AST = astFactory.create(lb);
				astFactory.makeASTRoot(currentAST, lb_AST);
				match(LBRACK);
				if ( inputState.guessing==0 ) {
					lb_AST.setType(ARRAY_DECLARATOR);
				}
				{
				switch ( LA(1)) {
				case LITERAL_void:
				case LITERAL_boolean:
				case LITERAL_byte:
				case LITERAL_char:
				case LITERAL_short:
				case LITERAL_int:
				case LITERAL_float:
				case LITERAL_long:
				case LITERAL_double:
				case IDENT:
				case LPAREN:
				case LITERAL_this:
				case LITERAL_super:
				case PLUS:
				case MINUS:
				case INC:
				case DEC:
				case BNOT:
				case LNOT:
				case LITERAL_true:
				case LITERAL_false:
				case LITERAL_null:
				case LITERAL_new:
				case NUM_INT:
				case CHAR_LITERAL:
				case STRING_LITERAL:
				case NUM_FLOAT:
				case NUM_LONG:
				case NUM_DOUBLE:
				{
					expression();
					astFactory.addASTChild(currentAST, returnAST);
					break;
				}
				case RBRACK:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				match(RBRACK);
			}
			else {
				if ( _cnt196>=1 ) { break _loop196; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt196++;
		} while (true);
		}
		newArrayDeclarator_AST = (AST)currentAST.root;
		returnAST = newArrayDeclarator_AST;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"BLOCK",
		"MODIFIERS",
		"OBJBLOCK",
		"SLIST",
		"CTOR_DEF",
		"METHOD_DEF",
		"VARIABLE_DEF",
		"INSTANCE_INIT",
		"STATIC_INIT",
		"TYPE",
		"CLASS_DEF",
		"INTERFACE_DEF",
		"PACKAGE_DEF",
		"ARRAY_DECLARATOR",
		"EXTENDS_CLAUSE",
		"IMPLEMENTS_CLAUSE",
		"PARAMETERS",
		"PARAMETER_DEF",
		"LABELED_STAT",
		"TYPECAST",
		"INDEX_OP",
		"POST_INC",
		"POST_DEC",
		"METHOD_CALL",
		"EXPR",
		"ARRAY_INIT",
		"IMPORT",
		"UNARY_MINUS",
		"UNARY_PLUS",
		"CASE_GROUP",
		"ELIST",
		"FOR_INIT",
		"FOR_CONDITION",
		"FOR_ITERATOR",
		"EMPTY_STAT",
		"\"final\"",
		"\"abstract\"",
		"\"strictfp\"",
		"SUPER_CTOR_CALL",
		"CTOR_CALL",
		"\"package\"",
		"SEMI",
		"\"import\"",
		"LBRACK",
		"RBRACK",
		"\"void\"",
		"\"boolean\"",
		"\"byte\"",
		"\"char\"",
		"\"short\"",
		"\"int\"",
		"\"float\"",
		"\"long\"",
		"\"double\"",
		"IDENT",
		"DOT",
		"STAR",
		"\"private\"",
		"\"public\"",
		"\"protected\"",
		"\"static\"",
		"\"transient\"",
		"\"native\"",
		"\"threadsafe\"",
		"\"synchronized\"",
		"\"volatile\"",
		"\"class\"",
		"\"extends\"",
		"\"interface\"",
		"LCURLY",
		"RCURLY",
		"COMMA",
		"\"implements\"",
		"LPAREN",
		"RPAREN",
		"\"this\"",
		"\"super\"",
		"ASSIGN",
		"\"throws\"",
		"COLON",
		"\"if\"",
		"\"else\"",
		"\"for\"",
		"\"while\"",
		"\"do\"",
		"\"break\"",
		"\"continue\"",
		"\"return\"",
		"\"switch\"",
		"\"throw\"",
		"\"assert\"",
		"\"case\"",
		"\"default\"",
		"\"try\"",
		"\"finally\"",
		"\"catch\"",
		"PLUS_ASSIGN",
		"MINUS_ASSIGN",
		"STAR_ASSIGN",
		"DIV_ASSIGN",
		"MOD_ASSIGN",
		"SR_ASSIGN",
		"BSR_ASSIGN",
		"SL_ASSIGN",
		"BAND_ASSIGN",
		"BXOR_ASSIGN",
		"BOR_ASSIGN",
		"QUESTION",
		"LOR",
		"LAND",
		"BOR",
		"BXOR",
		"BAND",
		"NOT_EQUAL",
		"EQUAL",
		"LT",
		"GT",
		"LE",
		"GE",
		"\"instanceof\"",
		"SL",
		"SR",
		"BSR",
		"PLUS",
		"MINUS",
		"DIV",
		"MOD",
		"INC",
		"DEC",
		"BNOT",
		"LNOT",
		"\"true\"",
		"\"false\"",
		"\"null\"",
		"\"new\"",
		"NUM_INT",
		"CHAR_LITERAL",
		"STRING_LITERAL",
		"NUM_FLOAT",
		"NUM_LONG",
		"NUM_DOUBLE",
		"WS",
		"SL_COMMENT",
		"ML_COMMENT",
		"ESC",
		"HEX_DIGIT",
		"VOCAB",
		"EXPONENT",
		"FLOAT_SUFFIX"
	};
	
	protected void buildTokenTypeASTClassMap() {
		tokenTypeToASTClassMap=null;
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { -2305803976550907904L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { -2305733607806730238L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { -2305803976550907902L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { -2305839160922996736L, 63L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	private static final long[] mk_tokenSet_4() {
		long[] data = { -1729941358572994560L, 383L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_4 = new BitSet(mk_tokenSet_4());
	private static final long[] mk_tokenSet_5() {
		long[] data = { -1153339868781215744L, 8575L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_5 = new BitSet(mk_tokenSet_5());
	private static final long[] mk_tokenSet_6() {
		long[] data = { 864831865943490560L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_6 = new BitSet(mk_tokenSet_6());
	private static final long[] mk_tokenSet_7() {
		long[] data = { 175921860444160L, 133120L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_7 = new BitSet(mk_tokenSet_7());
	private static final long[] mk_tokenSet_8() {
		long[] data = { -1729906174200905728L, -9223372026120395137L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_8 = new BitSet(mk_tokenSet_8());
	private static final long[] mk_tokenSet_9() {
		long[] data = { -383179802279936L, -57984440449L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_9 = new BitSet(mk_tokenSet_9());
	private static final long[] mk_tokenSet_10() {
		long[] data = { -1729906174200905728L, -9223372026120396161L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_10 = new BitSet(mk_tokenSet_10());
	private static final long[] mk_tokenSet_11() {
		long[] data = { -1729906174200905728L, -9223372019675847041L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_11 = new BitSet(mk_tokenSet_11());
	private static final long[] mk_tokenSet_12() {
		long[] data = { -383179802279936L, -284801L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_12 = new BitSet(mk_tokenSet_12());
	private static final long[] mk_tokenSet_13() {
		long[] data = { -1729941358572994560L, 63L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_13 = new BitSet(mk_tokenSet_13());
	private static final long[] mk_tokenSet_14() {
		long[] data = { -1153339868781215744L, 63L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_14 = new BitSet(mk_tokenSet_14());
	private static final long[] mk_tokenSet_15() {
		long[] data = { 575897802350002176L, -9223372036854669312L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_15 = new BitSet(mk_tokenSet_15());
	private static final long[] mk_tokenSet_16() {
		long[] data = { 2305455981120716800L, -68719239168L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_16 = new BitSet(mk_tokenSet_16());
	private static final long[] mk_tokenSet_17() {
		long[] data = { -2305839160922996736L, 127L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_17 = new BitSet(mk_tokenSet_17());
	private static final long[] mk_tokenSet_18() {
		long[] data = { -2017608784771284992L, 127L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_18 = new BitSet(mk_tokenSet_18());
	private static final long[] mk_tokenSet_19() {
		long[] data = { 575897802350002176L, -9223372036854668800L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_19 = new BitSet(mk_tokenSet_19());
	private static final long[] mk_tokenSet_20() {
		long[] data = { 2305455981120716800L, -68719237120L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_20 = new BitSet(mk_tokenSet_20());
	private static final long[] mk_tokenSet_21() {
		long[] data = { 575897802350002176L, -9223372036854145024L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_21 = new BitSet(mk_tokenSet_21());
	private static final long[] mk_tokenSet_22() {
		long[] data = { 1152921504606846976L, 0L, 6L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_22 = new BitSet(mk_tokenSet_22());
	private static final long[] mk_tokenSet_23() {
		long[] data = { 575897802350002176L, 106496L, 130944L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_23 = new BitSet(mk_tokenSet_23());
	private static final long[] mk_tokenSet_24() {
		long[] data = { 2305737456097427456L, -68718695424L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_24 = new BitSet(mk_tokenSet_24());
	private static final long[] mk_tokenSet_25() {
		long[] data = { 1729839653747425280L, -68718801920L, 31L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_25 = new BitSet(mk_tokenSet_25());
	private static final long[] mk_tokenSet_26() {
		long[] data = { -101704825569280L, -51539873921L, 131071L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_26 = new BitSet(mk_tokenSet_26());
	private static final long[] mk_tokenSet_27() {
		long[] data = { 576179277326712832L, -9223372036854669312L, 131065L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_27 = new BitSet(mk_tokenSet_27());
	
	}

// $ANTLR 2.7.2: "expandedWParser.g" -> "WParser.java"$

package processing.app.preproc;

import java.io.*;

import antlr.CommonAST;
import antlr.DumpASTVisitor;

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

public class WParser extends antlr.LLkParser       implements WTokenTypes
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
    

protected WParser(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public WParser(TokenBuffer tokenBuf) {
  this(tokenBuf,2);
}

protected WParser(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

public WParser(TokenStream lexer) {
  this(lexer,2);
}

public WParser(ParserSharedInputState state) {
  super(state,2);
  tokenNames = _tokenNames;
  buildTokenTypeASTClassMap();
  astFactory = new ASTFactory(getTokenTypeToASTClassMap());
}

	public final void translationUnit() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode translationUnit_AST = null;
		
		{
		switch ( LA(1)) {
		case LITERAL_typedef:
		case LITERAL_asm:
		case LITERAL_volatile:
		case SEMI:
		case LITERAL_struct:
		case LITERAL_union:
		case LITERAL_enum:
		case LITERAL_auto:
		case LITERAL_register:
		case LITERAL_extern:
		case LITERAL_static:
		case LITERAL_const:
		case LITERAL_void:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_long:
		case LITERAL_float:
		case LITERAL_double:
		case LITERAL_signed:
		case LITERAL_unsigned:
		case ID:
		case STAR:
		case LPAREN:
		case LITERAL_inline:
		case LITERAL_byte:
		case LITERAL_boolean:
		case LITERAL_Servo:
		case LITERAL_Wire:
		case LITERAL_typeof:
		case LITERAL___complex:
		{
			externalList();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case EOF:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		translationUnit_AST = (TNode)currentAST.root;
		returnAST = translationUnit_AST;
	}
	
	public final void externalList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode externalList_AST = null;
		
		{
		int _cnt209=0;
		_loop209:
		do {
			if ((_tokenSet_0.member(LA(1)))) {
				externalDef();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt209>=1 ) { break _loop209; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt209++;
		} while (true);
		}
		externalList_AST = (TNode)currentAST.root;
		returnAST = externalList_AST;
	}
	
	public final void asm_expr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode asm_expr_AST = null;
		
		TNode tmp1_AST = null;
		tmp1_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp1_AST);
		match(LITERAL_asm);
		{
		switch ( LA(1)) {
		case LITERAL_volatile:
		{
			TNode tmp2_AST = null;
			tmp2_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp2_AST);
			match(LITERAL_volatile);
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
		TNode tmp3_AST = null;
		tmp3_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp3_AST);
		match(LCURLY);
		expr();
		astFactory.addASTChild(currentAST, returnAST);
		TNode tmp4_AST = null;
		tmp4_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp4_AST);
		match(RCURLY);
		{
		int _cnt6=0;
		_loop6:
		do {
			if ((LA(1)==SEMI) && (_tokenSet_1.member(LA(2)))) {
				TNode tmp5_AST = null;
				tmp5_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp5_AST);
				match(SEMI);
			}
			else {
				if ( _cnt6>=1 ) { break _loop6; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt6++;
		} while (true);
		}
		asm_expr_AST = (TNode)currentAST.root;
		returnAST = asm_expr_AST;
	}
	
	public final void expr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode expr_AST = null;
		Token  c = null;
		TNode c_AST = null;
		
		assignExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop259:
		do {
			if ((LA(1)==COMMA) && (_tokenSet_2.member(LA(2)))) {
				c = LT(1);
				c_AST = (TNode)astFactory.create(c);
				astFactory.makeASTRoot(currentAST, c_AST);
				match(COMMA);
				if ( inputState.guessing==0 ) {
					c_AST.setType(NCommaExpr);
				}
				assignExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop259;
			}
			
		} while (true);
		}
		expr_AST = (TNode)currentAST.root;
		returnAST = expr_AST;
	}
	
	public final void idList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode idList_AST = null;
		
		TNode tmp6_AST = null;
		tmp6_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp6_AST);
		match(ID);
		{
		_loop9:
		do {
			if ((LA(1)==COMMA) && (LA(2)==ID)) {
				TNode tmp7_AST = null;
				tmp7_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp7_AST);
				match(COMMA);
				TNode tmp8_AST = null;
				tmp8_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp8_AST);
				match(ID);
			}
			else {
				break _loop9;
			}
			
		} while (true);
		}
		idList_AST = (TNode)currentAST.root;
		returnAST = idList_AST;
	}
	
	public final void externalDef() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode externalDef_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_asm:
		{
			asm_expr();
			astFactory.addASTChild(currentAST, returnAST);
			externalDef_AST = (TNode)currentAST.root;
			break;
		}
		case SEMI:
		{
			TNode tmp9_AST = null;
			tmp9_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp9_AST);
			match(SEMI);
			externalDef_AST = (TNode)currentAST.root;
			break;
		}
		default:
			boolean synPredMatched12 = false;
			if (((_tokenSet_3.member(LA(1))) && (_tokenSet_4.member(LA(2))))) {
				int _m12 = mark();
				synPredMatched12 = true;
				inputState.guessing++;
				try {
					{
					if ((LA(1)==LITERAL_typedef) && (true)) {
						match(LITERAL_typedef);
					}
					else if ((_tokenSet_3.member(LA(1))) && (_tokenSet_4.member(LA(2)))) {
						declaration();
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
				catch (RecognitionException pe) {
					synPredMatched12 = false;
				}
				rewind(_m12);
				inputState.guessing--;
			}
			if ( synPredMatched12 ) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
				externalDef_AST = (TNode)currentAST.root;
			}
			else {
				boolean synPredMatched14 = false;
				if (((_tokenSet_5.member(LA(1))) && (_tokenSet_6.member(LA(2))))) {
					int _m14 = mark();
					synPredMatched14 = true;
					inputState.guessing++;
					try {
						{
						functionPrefix();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched14 = false;
					}
					rewind(_m14);
					inputState.guessing--;
				}
				if ( synPredMatched14 ) {
					functionDef();
					astFactory.addASTChild(currentAST, returnAST);
					externalDef_AST = (TNode)currentAST.root;
				}
				else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_8.member(LA(2)))) {
					typelessDeclaration();
					astFactory.addASTChild(currentAST, returnAST);
					externalDef_AST = (TNode)currentAST.root;
				}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			}}
			returnAST = externalDef_AST;
		}
		
	public final void declaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declaration_AST = null;
		TNode ds_AST = null;
		AST ds1 = null;
		
		declSpecifiers();
		ds_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			ds1 = astFactory.dupList(ds_AST);
		}
		{
		switch ( LA(1)) {
		case ID:
		case STAR:
		case LPAREN:
		{
			initDeclList(ds1);
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
		{
		int _cnt78=0;
		_loop78:
		do {
			if ((LA(1)==SEMI) && (_tokenSet_9.member(LA(2)))) {
				TNode tmp10_AST = null;
				tmp10_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp10_AST);
				match(SEMI);
			}
			else {
				if ( _cnt78>=1 ) { break _loop78; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt78++;
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			declaration_AST = (TNode)currentAST.root;
			declaration_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NDeclaration)).add(declaration_AST));
			currentAST.root = declaration_AST;
			currentAST.child = declaration_AST!=null &&declaration_AST.getFirstChild()!=null ?
				declaration_AST.getFirstChild() : declaration_AST;
			currentAST.advanceChildToEnd();
		}
		declaration_AST = (TNode)currentAST.root;
		returnAST = declaration_AST;
	}
	
	public final void functionPrefix() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode functionPrefix_AST = null;
		TNode ds_AST = null;
		TNode d_AST = null;
		String declName;
		
		{
		boolean synPredMatched18 = false;
		if (((_tokenSet_10.member(LA(1))) && (_tokenSet_11.member(LA(2))))) {
			int _m18 = mark();
			synPredMatched18 = true;
			inputState.guessing++;
			try {
				{
				functionDeclSpecifiers();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched18 = false;
			}
			rewind(_m18);
			inputState.guessing--;
		}
		if ( synPredMatched18 ) {
			functionDeclSpecifiers();
			ds_AST = (TNode)returnAST;
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		declName=declarator(true);
		d_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop20:
		do {
			if ((_tokenSet_3.member(LA(1)))) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop20;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case VARARGS:
		{
			TNode tmp11_AST = null;
			tmp11_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp11_AST);
			match(VARARGS);
			break;
		}
		case LCURLY:
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
		{
		_loop23:
		do {
			if ((LA(1)==SEMI)) {
				TNode tmp12_AST = null;
				tmp12_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp12_AST);
				match(SEMI);
			}
			else {
				break _loop23;
			}
			
		} while (true);
		}
		TNode tmp13_AST = null;
		tmp13_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp13_AST);
		match(LCURLY);
		functionPrefix_AST = (TNode)currentAST.root;
		returnAST = functionPrefix_AST;
	}
	
	public final void functionDef() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode functionDef_AST = null;
		TNode ds_AST = null;
		TNode d_AST = null;
		String declName;
		
		{
		boolean synPredMatched241 = false;
		if (((_tokenSet_10.member(LA(1))) && (_tokenSet_11.member(LA(2))))) {
			int _m241 = mark();
			synPredMatched241 = true;
			inputState.guessing++;
			try {
				{
				functionDeclSpecifiers();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched241 = false;
			}
			rewind(_m241);
			inputState.guessing--;
		}
		if ( synPredMatched241 ) {
			functionDeclSpecifiers();
			ds_AST = (TNode)returnAST;
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		declName=declarator(true);
		d_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			
			AST d2, ds2;
			d2 = astFactory.dupList(d_AST);
			ds2 = astFactory.dupList(ds_AST);
			symbolTable.add(declName, (TNode)astFactory.make( (new ASTArray(3)).add(null).add(ds2).add(d2)));
			pushScope(declName);
			
		}
		{
		_loop243:
		do {
			if ((_tokenSet_3.member(LA(1)))) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop243;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case VARARGS:
		{
			TNode tmp14_AST = null;
			tmp14_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp14_AST);
			match(VARARGS);
			break;
		}
		case LCURLY:
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
		{
		_loop246:
		do {
			if ((LA(1)==SEMI)) {
				match(SEMI);
			}
			else {
				break _loop246;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			popScope();
		}
		compoundStatement(declName);
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			functionDef_AST = (TNode)currentAST.root;
			functionDef_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NFunctionDef)).add(functionDef_AST));
			currentAST.root = functionDef_AST;
			currentAST.child = functionDef_AST!=null &&functionDef_AST.getFirstChild()!=null ?
				functionDef_AST.getFirstChild() : functionDef_AST;
			currentAST.advanceChildToEnd();
		}
		functionDef_AST = (TNode)currentAST.root;
		returnAST = functionDef_AST;
	}
	
	public final void typelessDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode typelessDeclaration_AST = null;
		AST typeMissing = (TNode)astFactory.create(NTypeMissing);
		
		initDeclList(typeMissing);
		astFactory.addASTChild(currentAST, returnAST);
		TNode tmp16_AST = null;
		tmp16_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp16_AST);
		match(SEMI);
		if ( inputState.guessing==0 ) {
			typelessDeclaration_AST = (TNode)currentAST.root;
			typelessDeclaration_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NTypeMissing)).add(typelessDeclaration_AST));
			currentAST.root = typelessDeclaration_AST;
			currentAST.child = typelessDeclaration_AST!=null &&typelessDeclaration_AST.getFirstChild()!=null ?
				typelessDeclaration_AST.getFirstChild() : typelessDeclaration_AST;
			currentAST.advanceChildToEnd();
		}
		typelessDeclaration_AST = (TNode)currentAST.root;
		returnAST = typelessDeclaration_AST;
	}
	
	public final void functionDeclSpecifiers() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode functionDeclSpecifiers_AST = null;
		int specCount = 0;
		
		{
		int _cnt251=0;
		_loop251:
		do {
			switch ( LA(1)) {
			case LITERAL_extern:
			case LITERAL_static:
			case LITERAL_inline:
			{
				functionStorageClassSpecifier();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LITERAL_volatile:
			case LITERAL_const:
			{
				typeQualifier();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			default:
				boolean synPredMatched250 = false;
				if (((_tokenSet_12.member(LA(1))) && (_tokenSet_11.member(LA(2))))) {
					int _m250 = mark();
					synPredMatched250 = true;
					inputState.guessing++;
					try {
						{
						if ((LA(1)==LITERAL_struct) && (true)) {
							match(LITERAL_struct);
						}
						else if ((LA(1)==LITERAL_union) && (true)) {
							match(LITERAL_union);
						}
						else if ((LA(1)==LITERAL_enum) && (true)) {
							match(LITERAL_enum);
						}
						else if ((_tokenSet_12.member(LA(1))) && (true)) {
							typeSpecifier(specCount);
						}
						else {
							throw new NoViableAltException(LT(1), getFilename());
						}
						
						}
					}
					catch (RecognitionException pe) {
						synPredMatched250 = false;
					}
					rewind(_m250);
					inputState.guessing--;
				}
				if ( synPredMatched250 ) {
					specCount=typeSpecifier(specCount);
					astFactory.addASTChild(currentAST, returnAST);
				}
			else {
				if ( _cnt251>=1 ) { break _loop251; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt251++;
		} while (true);
		}
		functionDeclSpecifiers_AST = (TNode)currentAST.root;
		returnAST = functionDeclSpecifiers_AST;
	}
	
	public final String  declarator(
		boolean isFunctionDefinition
	) throws RecognitionException, TokenStreamException {
		String declName;
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declarator_AST = null;
		Token  id = null;
		TNode id_AST = null;
		declName = "";
		
		{
		switch ( LA(1)) {
		case STAR:
		{
			pointerGroup();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case ID:
		case LPAREN:
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
		case ID:
		{
			id = LT(1);
			id_AST = (TNode)astFactory.create(id);
			astFactory.addASTChild(currentAST, id_AST);
			match(ID);
			if ( inputState.guessing==0 ) {
				declName = id.getText();
			}
			break;
		}
		case LPAREN:
		{
			TNode tmp17_AST = null;
			tmp17_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp17_AST);
			match(LPAREN);
			declName=declarator(false);
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp18_AST = null;
			tmp18_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp18_AST);
			match(RPAREN);
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		_loop50:
		do {
			switch ( LA(1)) {
			case LPAREN:
			{
				declaratorParamaterList(isFunctionDefinition, declName);
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LBRACKET:
			{
				TNode tmp19_AST = null;
				tmp19_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp19_AST);
				match(LBRACKET);
				{
				switch ( LA(1)) {
				case LITERAL_asm:
				case ID:
				case STAR:
				case LPAREN:
				case LAND:
				case BAND:
				case PLUS:
				case MINUS:
				case INC:
				case DEC:
				case LITERAL_sizeof:
				case BNOT:
				case LNOT:
				case CharLiteral:
				case StringLiteral:
				case Number:
				case LITERAL___alignof:
				case LITERAL___real:
				case LITERAL___imag:
				{
					expr();
					astFactory.addASTChild(currentAST, returnAST);
					break;
				}
				case RBRACKET:
				{
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				TNode tmp20_AST = null;
				tmp20_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp20_AST);
				match(RBRACKET);
				break;
			}
			default:
			{
				break _loop50;
			}
			}
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			declarator_AST = (TNode)currentAST.root;
			declarator_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NDeclarator)).add(declarator_AST));
			currentAST.root = declarator_AST;
			currentAST.child = declarator_AST!=null &&declarator_AST.getFirstChild()!=null ?
				declarator_AST.getFirstChild() : declarator_AST;
			currentAST.advanceChildToEnd();
		}
		declarator_AST = (TNode)currentAST.root;
		returnAST = declarator_AST;
		return declName;
	}
	
	public final void initDeclList(
		AST declarationSpecifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode initDeclList_AST = null;
		
		initDecl(declarationSpecifiers);
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop115:
		do {
			if ((LA(1)==COMMA) && (_tokenSet_7.member(LA(2)))) {
				match(COMMA);
				initDecl(declarationSpecifiers);
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop115;
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
		initDeclList_AST = (TNode)currentAST.root;
		returnAST = initDeclList_AST;
	}
	
	public final void initializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode initializer_AST = null;
		
		{
		if ((_tokenSet_13.member(LA(1))) && (_tokenSet_14.member(LA(2)))) {
			{
			{
			boolean synPredMatched30 = false;
			if (((_tokenSet_15.member(LA(1))) && (_tokenSet_16.member(LA(2))))) {
				int _m30 = mark();
				synPredMatched30 = true;
				inputState.guessing++;
				try {
					{
					initializerElementLabel();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched30 = false;
				}
				rewind(_m30);
				inputState.guessing--;
			}
			if ( synPredMatched30 ) {
				initializerElementLabel();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_17.member(LA(1))) && (_tokenSet_18.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				assignExpr();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LCURLY:
			{
				lcurlyInitializer();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState.guessing==0 ) {
				initializer_AST = (TNode)currentAST.root;
				initializer_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NInitializer)).add(initializer_AST));
				currentAST.root = initializer_AST;
				currentAST.child = initializer_AST!=null &&initializer_AST.getFirstChild()!=null ?
					initializer_AST.getFirstChild() : initializer_AST;
				currentAST.advanceChildToEnd();
			}
			}
		}
		else if ((LA(1)==LCURLY) && (_tokenSet_19.member(LA(2)))) {
			lcurlyInitializer();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		initializer_AST = (TNode)currentAST.root;
		returnAST = initializer_AST;
	}
	
	public final void initializerElementLabel() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode initializerElementLabel_AST = null;
		
		{
		switch ( LA(1)) {
		case LBRACKET:
		{
			{
			TNode tmp23_AST = null;
			tmp23_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp23_AST);
			match(LBRACKET);
			{
			boolean synPredMatched37 = false;
			if (((_tokenSet_2.member(LA(1))) && (_tokenSet_20.member(LA(2))))) {
				int _m37 = mark();
				synPredMatched37 = true;
				inputState.guessing++;
				try {
					{
					constExpr();
					match(VARARGS);
					}
				}
				catch (RecognitionException pe) {
					synPredMatched37 = false;
				}
				rewind(_m37);
				inputState.guessing--;
			}
			if ( synPredMatched37 ) {
				rangeExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_21.member(LA(2)))) {
				constExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			TNode tmp24_AST = null;
			tmp24_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp24_AST);
			match(RBRACKET);
			{
			switch ( LA(1)) {
			case ASSIGN:
			{
				TNode tmp25_AST = null;
				tmp25_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp25_AST);
				match(ASSIGN);
				break;
			}
			case LITERAL_asm:
			case LCURLY:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
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
			break;
		}
		case ID:
		{
			TNode tmp26_AST = null;
			tmp26_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp26_AST);
			match(ID);
			TNode tmp27_AST = null;
			tmp27_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp27_AST);
			match(COLON);
			break;
		}
		case DOT:
		{
			TNode tmp28_AST = null;
			tmp28_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp28_AST);
			match(DOT);
			TNode tmp29_AST = null;
			tmp29_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp29_AST);
			match(ID);
			TNode tmp30_AST = null;
			tmp30_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp30_AST);
			match(ASSIGN);
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			initializerElementLabel_AST = (TNode)currentAST.root;
			initializerElementLabel_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NInitializerElementLabel)).add(initializerElementLabel_AST)) ;
			currentAST.root = initializerElementLabel_AST;
			currentAST.child = initializerElementLabel_AST!=null &&initializerElementLabel_AST.getFirstChild()!=null ?
				initializerElementLabel_AST.getFirstChild() : initializerElementLabel_AST;
			currentAST.advanceChildToEnd();
		}
		initializerElementLabel_AST = (TNode)currentAST.root;
		returnAST = initializerElementLabel_AST;
	}
	
	public final void assignExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode assignExpr_AST = null;
		TNode a_AST = null;
		
		conditionalExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case ASSIGN:
		case DIV_ASSIGN:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case MOD_ASSIGN:
		case RSHIFT_ASSIGN:
		case LSHIFT_ASSIGN:
		case BAND_ASSIGN:
		case BOR_ASSIGN:
		case BXOR_ASSIGN:
		{
			assignOperator();
			a_AST = (TNode)returnAST;
			assignExpr();
			astFactory.addASTChild(currentAST, returnAST);
			if ( inputState.guessing==0 ) {
				assignExpr_AST = (TNode)currentAST.root;
				assignExpr_AST = (TNode)astFactory.make( (new ASTArray(2)).add(a_AST).add(assignExpr_AST));
				currentAST.root = assignExpr_AST;
				currentAST.child = assignExpr_AST!=null &&assignExpr_AST.getFirstChild()!=null ?
					assignExpr_AST.getFirstChild() : assignExpr_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		case RCURLY:
		case SEMI:
		case COMMA:
		case COLON:
		case RPAREN:
		case RBRACKET:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		assignExpr_AST = (TNode)currentAST.root;
		returnAST = assignExpr_AST;
	}
	
	public final void lcurlyInitializer() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode lcurlyInitializer_AST = null;
		
		TNode tmp31_AST = null;
		tmp31_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp31_AST);
		match(LCURLY);
		{
		switch ( LA(1)) {
		case LITERAL_asm:
		case LCURLY:
		case ID:
		case STAR:
		case LPAREN:
		case LBRACKET:
		case LAND:
		case BAND:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case LITERAL_sizeof:
		case BNOT:
		case LNOT:
		case DOT:
		case CharLiteral:
		case StringLiteral:
		case Number:
		case LITERAL___alignof:
		case LITERAL___real:
		case LITERAL___imag:
		{
			initializerList();
			astFactory.addASTChild(currentAST, returnAST);
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
		TNode tmp33_AST = null;
		tmp33_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp33_AST);
		match(RCURLY);
		if ( inputState.guessing==0 ) {
			lcurlyInitializer_AST = (TNode)currentAST.root;
			lcurlyInitializer_AST.setType( NLcurlyInitializer );
		}
		lcurlyInitializer_AST = (TNode)currentAST.root;
		returnAST = lcurlyInitializer_AST;
	}
	
	public final void constExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode constExpr_AST = null;
		
		conditionalExpr();
		astFactory.addASTChild(currentAST, returnAST);
		constExpr_AST = (TNode)currentAST.root;
		returnAST = constExpr_AST;
	}
	
	public final void rangeExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode rangeExpr_AST = null;
		
		constExpr();
		astFactory.addASTChild(currentAST, returnAST);
		TNode tmp34_AST = null;
		tmp34_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp34_AST);
		match(VARARGS);
		constExpr();
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			rangeExpr_AST = (TNode)currentAST.root;
			rangeExpr_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NRangeExpr)).add(rangeExpr_AST));
			currentAST.root = rangeExpr_AST;
			currentAST.child = rangeExpr_AST!=null &&rangeExpr_AST.getFirstChild()!=null ?
				rangeExpr_AST.getFirstChild() : rangeExpr_AST;
			currentAST.advanceChildToEnd();
		}
		rangeExpr_AST = (TNode)currentAST.root;
		returnAST = rangeExpr_AST;
	}
	
	public final void initializerList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode initializerList_AST = null;
		
		initializer();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop44:
		do {
			if ((LA(1)==COMMA) && (_tokenSet_13.member(LA(2)))) {
				match(COMMA);
				initializer();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop44;
			}
			
		} while (true);
		}
		initializerList_AST = (TNode)currentAST.root;
		returnAST = initializerList_AST;
	}
	
	public final void pointerGroup() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode pointerGroup_AST = null;
		
		{
		int _cnt233=0;
		_loop233:
		do {
			if ((LA(1)==STAR)) {
				TNode tmp36_AST = null;
				tmp36_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp36_AST);
				match(STAR);
				{
				_loop232:
				do {
					if ((LA(1)==LITERAL_volatile||LA(1)==LITERAL_const)) {
						typeQualifier();
						astFactory.addASTChild(currentAST, returnAST);
					}
					else {
						break _loop232;
					}
					
				} while (true);
				}
			}
			else {
				if ( _cnt233>=1 ) { break _loop233; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt233++;
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			pointerGroup_AST = (TNode)currentAST.root;
			pointerGroup_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NPointerGroup)).add(pointerGroup_AST));
			currentAST.root = pointerGroup_AST;
			currentAST.child = pointerGroup_AST!=null &&pointerGroup_AST.getFirstChild()!=null ?
				pointerGroup_AST.getFirstChild() : pointerGroup_AST;
			currentAST.advanceChildToEnd();
		}
		pointerGroup_AST = (TNode)currentAST.root;
		returnAST = pointerGroup_AST;
	}
	
	public final void declaratorParamaterList(
		boolean isFunctionDefinition, String declName
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declaratorParamaterList_AST = null;
		
		TNode tmp37_AST = null;
		tmp37_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp37_AST);
		match(LPAREN);
		if ( inputState.guessing==0 ) {
			
			if (isFunctionDefinition) {
			pushScope(declName);
			}
			else {
			pushScope("!"+declName); 
			}
			
		}
		{
		boolean synPredMatched54 = false;
		if (((_tokenSet_3.member(LA(1))) && (_tokenSet_22.member(LA(2))))) {
			int _m54 = mark();
			synPredMatched54 = true;
			inputState.guessing++;
			try {
				{
				declSpecifiers();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched54 = false;
			}
			rewind(_m54);
			inputState.guessing--;
		}
		if ( synPredMatched54 ) {
			parameterTypeList();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
			{
			switch ( LA(1)) {
			case ID:
			{
				idList();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case COMMA:
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
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			
			popScope();
			
		}
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
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
		TNode tmp39_AST = null;
		tmp39_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp39_AST);
		match(RPAREN);
		if ( inputState.guessing==0 ) {
			declaratorParamaterList_AST = (TNode)currentAST.root;
			declaratorParamaterList_AST.setType(NParameterTypeList);
		}
		declaratorParamaterList_AST = (TNode)currentAST.root;
		returnAST = declaratorParamaterList_AST;
	}
	
	public final void declSpecifiers() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declSpecifiers_AST = null;
		TNode s_AST = null;
		int specCount=0;
		
		{
		int _cnt214=0;
		_loop214:
		do {
			switch ( LA(1)) {
			case LITERAL_typedef:
			case LITERAL_auto:
			case LITERAL_register:
			case LITERAL_extern:
			case LITERAL_static:
			case LITERAL_inline:
			{
				storageClassSpecifier();
				s_AST = (TNode)returnAST;
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LITERAL_volatile:
			case LITERAL_const:
			{
				typeQualifier();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			default:
				boolean synPredMatched213 = false;
				if (((_tokenSet_12.member(LA(1))) && (_tokenSet_22.member(LA(2))))) {
					int _m213 = mark();
					synPredMatched213 = true;
					inputState.guessing++;
					try {
						{
						if ((LA(1)==LITERAL_struct) && (true)) {
							match(LITERAL_struct);
						}
						else if ((LA(1)==LITERAL_union) && (true)) {
							match(LITERAL_union);
						}
						else if ((LA(1)==LITERAL_enum) && (true)) {
							match(LITERAL_enum);
						}
						else if ((_tokenSet_12.member(LA(1))) && (true)) {
							typeSpecifier(specCount);
						}
						else {
							throw new NoViableAltException(LT(1), getFilename());
						}
						
						}
					}
					catch (RecognitionException pe) {
						synPredMatched213 = false;
					}
					rewind(_m213);
					inputState.guessing--;
				}
				if ( synPredMatched213 ) {
					specCount=typeSpecifier(specCount);
					astFactory.addASTChild(currentAST, returnAST);
				}
			else {
				if ( _cnt214>=1 ) { break _loop214; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt214++;
		} while (true);
		}
		declSpecifiers_AST = (TNode)currentAST.root;
		returnAST = declSpecifiers_AST;
	}
	
	public final void parameterTypeList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode parameterTypeList_AST = null;
		
		parameterDeclaration();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop60:
		do {
			if ((LA(1)==SEMI||LA(1)==COMMA) && (_tokenSet_3.member(LA(2)))) {
				{
				switch ( LA(1)) {
				case COMMA:
				{
					TNode tmp40_AST = null;
					tmp40_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp40_AST);
					match(COMMA);
					break;
				}
				case SEMI:
				{
					TNode tmp41_AST = null;
					tmp41_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp41_AST);
					match(SEMI);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				parameterDeclaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop60;
			}
			
		} while (true);
		}
		{
		if ((LA(1)==SEMI||LA(1)==COMMA) && (LA(2)==VARARGS)) {
			{
			switch ( LA(1)) {
			case COMMA:
			{
				TNode tmp42_AST = null;
				tmp42_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp42_AST);
				match(COMMA);
				break;
			}
			case SEMI:
			{
				TNode tmp43_AST = null;
				tmp43_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp43_AST);
				match(SEMI);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			TNode tmp44_AST = null;
			tmp44_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp44_AST);
			match(VARARGS);
		}
		else if ((LA(1)==COMMA||LA(1)==RPAREN) && (_tokenSet_24.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		parameterTypeList_AST = (TNode)currentAST.root;
		returnAST = parameterTypeList_AST;
	}
	
	public final void parameterDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode parameterDeclaration_AST = null;
		TNode ds_AST = null;
		TNode d_AST = null;
		String declName;
		
		declSpecifiers();
		ds_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		{
		boolean synPredMatched237 = false;
		if (((_tokenSet_7.member(LA(1))) && (_tokenSet_25.member(LA(2))))) {
			int _m237 = mark();
			synPredMatched237 = true;
			inputState.guessing++;
			try {
				{
				declarator(false);
				}
			}
			catch (RecognitionException pe) {
				synPredMatched237 = false;
			}
			rewind(_m237);
			inputState.guessing--;
		}
		if ( synPredMatched237 ) {
			declName=declarator(false);
			d_AST = (TNode)returnAST;
			astFactory.addASTChild(currentAST, returnAST);
			if ( inputState.guessing==0 ) {
				
				AST d2, ds2;
				d2 = astFactory.dupList(d_AST);
				ds2 = astFactory.dupList(ds_AST);
				symbolTable.add(declName, (TNode)astFactory.make( (new ASTArray(3)).add(null).add(ds2).add(d2)));
				
			}
		}
		else if ((_tokenSet_26.member(LA(1))) && (_tokenSet_27.member(LA(2)))) {
			nonemptyAbstractDeclarator();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_28.member(LA(1)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			parameterDeclaration_AST = (TNode)currentAST.root;
			
			parameterDeclaration_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NParameterDeclaration)).add(parameterDeclaration_AST));
			
			currentAST.root = parameterDeclaration_AST;
			currentAST.child = parameterDeclaration_AST!=null &&parameterDeclaration_AST.getFirstChild()!=null ?
				parameterDeclaration_AST.getFirstChild() : parameterDeclaration_AST;
			currentAST.advanceChildToEnd();
		}
		parameterDeclaration_AST = (TNode)currentAST.root;
		returnAST = parameterDeclaration_AST;
	}
	
	public final void declarationList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declarationList_AST = null;
		
		{
		int _cnt67=0;
		_loop67:
		do {
			if ((LA(1)==LITERAL___label__) && (LA(2)==ID)) {
				localLabelDeclaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				boolean synPredMatched66 = false;
				if (((_tokenSet_3.member(LA(1))) && (_tokenSet_4.member(LA(2))))) {
					int _m66 = mark();
					synPredMatched66 = true;
					inputState.guessing++;
					try {
						{
						declarationPredictor();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched66 = false;
					}
					rewind(_m66);
					inputState.guessing--;
				}
				if ( synPredMatched66 ) {
					declaration();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					if ( _cnt67>=1 ) { break _loop67; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				}
				_cnt67++;
			} while (true);
			}
			declarationList_AST = (TNode)currentAST.root;
			returnAST = declarationList_AST;
		}
		
	public final void localLabelDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode localLabelDeclaration_AST = null;
		
		{
		TNode tmp45_AST = null;
		tmp45_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp45_AST);
		match(LITERAL___label__);
		TNode tmp46_AST = null;
		tmp46_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp46_AST);
		match(ID);
		{
		_loop71:
		do {
			if ((LA(1)==COMMA) && (LA(2)==ID)) {
				match(COMMA);
				TNode tmp48_AST = null;
				tmp48_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp48_AST);
				match(ID);
			}
			else {
				break _loop71;
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
		{
		int _cnt74=0;
		_loop74:
		do {
			if ((LA(1)==SEMI) && (_tokenSet_29.member(LA(2)))) {
				match(SEMI);
			}
			else {
				if ( _cnt74>=1 ) { break _loop74; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt74++;
		} while (true);
		}
		}
		localLabelDeclaration_AST = (TNode)currentAST.root;
		returnAST = localLabelDeclaration_AST;
	}
	
	public final void declarationPredictor() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode declarationPredictor_AST = null;
		
		{
		if ((LA(1)==LITERAL_typedef) && (LA(2)==EOF)) {
			TNode tmp51_AST = null;
			tmp51_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp51_AST);
			match(LITERAL_typedef);
		}
		else if ((_tokenSet_3.member(LA(1))) && (_tokenSet_4.member(LA(2)))) {
			declaration();
			astFactory.addASTChild(currentAST, returnAST);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		declarationPredictor_AST = (TNode)currentAST.root;
		returnAST = declarationPredictor_AST;
	}
	
	public final void functionStorageClassSpecifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode functionStorageClassSpecifier_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_extern:
		{
			TNode tmp52_AST = null;
			tmp52_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp52_AST);
			match(LITERAL_extern);
			functionStorageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_static:
		{
			TNode tmp53_AST = null;
			tmp53_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp53_AST);
			match(LITERAL_static);
			functionStorageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_inline:
		{
			TNode tmp54_AST = null;
			tmp54_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp54_AST);
			match(LITERAL_inline);
			functionStorageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = functionStorageClassSpecifier_AST;
	}
	
	public final int  typeSpecifier(
		int specCount
	) throws RecognitionException, TokenStreamException {
		int retSpecCount;
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode typeSpecifier_AST = null;
		retSpecCount = specCount + 1;
		
		{
		switch ( LA(1)) {
		case LITERAL_void:
		{
			TNode tmp55_AST = null;
			tmp55_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp55_AST);
			match(LITERAL_void);
			break;
		}
		case LITERAL_char:
		{
			TNode tmp56_AST = null;
			tmp56_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp56_AST);
			match(LITERAL_char);
			break;
		}
		case LITERAL_short:
		{
			TNode tmp57_AST = null;
			tmp57_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp57_AST);
			match(LITERAL_short);
			break;
		}
		case LITERAL_int:
		{
			TNode tmp58_AST = null;
			tmp58_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp58_AST);
			match(LITERAL_int);
			break;
		}
		case LITERAL_long:
		{
			TNode tmp59_AST = null;
			tmp59_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp59_AST);
			match(LITERAL_long);
			break;
		}
		case LITERAL_float:
		{
			TNode tmp60_AST = null;
			tmp60_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp60_AST);
			match(LITERAL_float);
			break;
		}
		case LITERAL_double:
		{
			TNode tmp61_AST = null;
			tmp61_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp61_AST);
			match(LITERAL_double);
			break;
		}
		case LITERAL_signed:
		{
			TNode tmp62_AST = null;
			tmp62_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp62_AST);
			match(LITERAL_signed);
			break;
		}
		case LITERAL_unsigned:
		{
			TNode tmp63_AST = null;
			tmp63_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp63_AST);
			match(LITERAL_unsigned);
			break;
		}
		case LITERAL_byte:
		{
			TNode tmp64_AST = null;
			tmp64_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp64_AST);
			match(LITERAL_byte);
			break;
		}
		case LITERAL_boolean:
		{
			TNode tmp65_AST = null;
			tmp65_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp65_AST);
			match(LITERAL_boolean);
			break;
		}
		case LITERAL_Servo:
		{
			TNode tmp66_AST = null;
			tmp66_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp66_AST);
			match(LITERAL_Servo);
			break;
		}
		case LITERAL_Wire:
		{
			TNode tmp67_AST = null;
			tmp67_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp67_AST);
			match(LITERAL_Wire);
			break;
		}
		case LITERAL_struct:
		case LITERAL_union:
		{
			structOrUnionSpecifier();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop83:
			do {
				if ((LA(1)==LITERAL_asm||LA(1)==LITERAL___attribute) && (LA(2)==LPAREN)) {
					attributeDecl();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop83;
				}
				
			} while (true);
			}
			break;
		}
		case LITERAL_enum:
		{
			enumSpecifier();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LITERAL_typeof:
		{
			TNode tmp68_AST = null;
			tmp68_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp68_AST);
			match(LITERAL_typeof);
			TNode tmp69_AST = null;
			tmp69_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp69_AST);
			match(LPAREN);
			{
			boolean synPredMatched86 = false;
			if (((_tokenSet_30.member(LA(1))) && (_tokenSet_31.member(LA(2))))) {
				int _m86 = mark();
				synPredMatched86 = true;
				inputState.guessing++;
				try {
					{
					typeName();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched86 = false;
				}
				rewind(_m86);
				inputState.guessing--;
			}
			if ( synPredMatched86 ) {
				typeName();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_32.member(LA(2)))) {
				expr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			TNode tmp70_AST = null;
			tmp70_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp70_AST);
			match(RPAREN);
			break;
		}
		case LITERAL___complex:
		{
			TNode tmp71_AST = null;
			tmp71_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp71_AST);
			match(LITERAL___complex);
			break;
		}
		default:
			if (((LA(1)==ID))&&( specCount==0 )) {
				typedefName();
				astFactory.addASTChild(currentAST, returnAST);
			}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		typeSpecifier_AST = (TNode)currentAST.root;
		returnAST = typeSpecifier_AST;
		return retSpecCount;
	}
	
	public final void structOrUnionSpecifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structOrUnionSpecifier_AST = null;
		TNode sou_AST = null;
		Token  i = null;
		TNode i_AST = null;
		Token  l = null;
		TNode l_AST = null;
		Token  l1 = null;
		TNode l1_AST = null;
		String scopeName;
		
		structOrUnion();
		sou_AST = (TNode)returnAST;
		{
		boolean synPredMatched90 = false;
		if (((LA(1)==ID) && (LA(2)==LCURLY))) {
			int _m90 = mark();
			synPredMatched90 = true;
			inputState.guessing++;
			try {
				{
				match(ID);
				match(LCURLY);
				}
			}
			catch (RecognitionException pe) {
				synPredMatched90 = false;
			}
			rewind(_m90);
			inputState.guessing--;
		}
		if ( synPredMatched90 ) {
			i = LT(1);
			i_AST = (TNode)astFactory.create(i);
			astFactory.addASTChild(currentAST, i_AST);
			match(ID);
			l = LT(1);
			l_AST = (TNode)astFactory.create(l);
			astFactory.addASTChild(currentAST, l_AST);
			match(LCURLY);
			if ( inputState.guessing==0 ) {
				
				scopeName = sou_AST.getText() + " " + i_AST.getText();
				l_AST.setText(scopeName);
				pushScope(scopeName);
				
			}
			{
			switch ( LA(1)) {
			case LITERAL_volatile:
			case LITERAL_struct:
			case LITERAL_union:
			case LITERAL_enum:
			case LITERAL_const:
			case LITERAL_void:
			case LITERAL_char:
			case LITERAL_short:
			case LITERAL_int:
			case LITERAL_long:
			case LITERAL_float:
			case LITERAL_double:
			case LITERAL_signed:
			case LITERAL_unsigned:
			case ID:
			case LITERAL_byte:
			case LITERAL_boolean:
			case LITERAL_Servo:
			case LITERAL_Wire:
			case LITERAL_typeof:
			case LITERAL___complex:
			{
				structDeclarationList();
				astFactory.addASTChild(currentAST, returnAST);
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
			if ( inputState.guessing==0 ) {
				popScope();
			}
			TNode tmp72_AST = null;
			tmp72_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp72_AST);
			match(RCURLY);
		}
		else if ((LA(1)==LCURLY)) {
			l1 = LT(1);
			l1_AST = (TNode)astFactory.create(l1);
			astFactory.addASTChild(currentAST, l1_AST);
			match(LCURLY);
			if ( inputState.guessing==0 ) {
				
				scopeName = getAScopeName();
				l1_AST.setText(scopeName);
				pushScope(scopeName);
				
			}
			{
			switch ( LA(1)) {
			case LITERAL_volatile:
			case LITERAL_struct:
			case LITERAL_union:
			case LITERAL_enum:
			case LITERAL_const:
			case LITERAL_void:
			case LITERAL_char:
			case LITERAL_short:
			case LITERAL_int:
			case LITERAL_long:
			case LITERAL_float:
			case LITERAL_double:
			case LITERAL_signed:
			case LITERAL_unsigned:
			case ID:
			case LITERAL_byte:
			case LITERAL_boolean:
			case LITERAL_Servo:
			case LITERAL_Wire:
			case LITERAL_typeof:
			case LITERAL___complex:
			{
				structDeclarationList();
				astFactory.addASTChild(currentAST, returnAST);
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
			if ( inputState.guessing==0 ) {
				popScope();
			}
			TNode tmp73_AST = null;
			tmp73_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp73_AST);
			match(RCURLY);
		}
		else if ((LA(1)==ID) && (_tokenSet_33.member(LA(2)))) {
			TNode tmp74_AST = null;
			tmp74_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp74_AST);
			match(ID);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		if ( inputState.guessing==0 ) {
			structOrUnionSpecifier_AST = (TNode)currentAST.root;
			
			structOrUnionSpecifier_AST = (TNode)astFactory.make( (new ASTArray(2)).add(sou_AST).add(structOrUnionSpecifier_AST));
			
			currentAST.root = structOrUnionSpecifier_AST;
			currentAST.child = structOrUnionSpecifier_AST!=null &&structOrUnionSpecifier_AST.getFirstChild()!=null ?
				structOrUnionSpecifier_AST.getFirstChild() : structOrUnionSpecifier_AST;
			currentAST.advanceChildToEnd();
		}
		structOrUnionSpecifier_AST = (TNode)currentAST.root;
		returnAST = structOrUnionSpecifier_AST;
	}
	
	public final void attributeDecl() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode attributeDecl_AST = null;
		
		switch ( LA(1)) {
		case LITERAL___attribute:
		{
			TNode tmp75_AST = null;
			tmp75_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp75_AST);
			match(LITERAL___attribute);
			TNode tmp76_AST = null;
			tmp76_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp76_AST);
			match(LPAREN);
			TNode tmp77_AST = null;
			tmp77_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp77_AST);
			match(LPAREN);
			attributeList();
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp78_AST = null;
			tmp78_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp78_AST);
			match(RPAREN);
			TNode tmp79_AST = null;
			tmp79_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp79_AST);
			match(RPAREN);
			attributeDecl_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_asm:
		{
			TNode tmp80_AST = null;
			tmp80_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp80_AST);
			match(LITERAL_asm);
			TNode tmp81_AST = null;
			tmp81_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp81_AST);
			match(LPAREN);
			stringConst();
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp82_AST = null;
			tmp82_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp82_AST);
			match(RPAREN);
			if ( inputState.guessing==0 ) {
				attributeDecl_AST = (TNode)currentAST.root;
				attributeDecl_AST.setType( NAsmAttribute );
			}
			attributeDecl_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = attributeDecl_AST;
	}
	
	public final void enumSpecifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode enumSpecifier_AST = null;
		Token  i = null;
		TNode i_AST = null;
		
		TNode tmp83_AST = null;
		tmp83_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp83_AST);
		match(LITERAL_enum);
		{
		boolean synPredMatched108 = false;
		if (((LA(1)==ID) && (LA(2)==LCURLY))) {
			int _m108 = mark();
			synPredMatched108 = true;
			inputState.guessing++;
			try {
				{
				match(ID);
				match(LCURLY);
				}
			}
			catch (RecognitionException pe) {
				synPredMatched108 = false;
			}
			rewind(_m108);
			inputState.guessing--;
		}
		if ( synPredMatched108 ) {
			i = LT(1);
			i_AST = (TNode)astFactory.create(i);
			astFactory.addASTChild(currentAST, i_AST);
			match(ID);
			TNode tmp84_AST = null;
			tmp84_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp84_AST);
			match(LCURLY);
			enumList(i.getText());
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp85_AST = null;
			tmp85_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp85_AST);
			match(RCURLY);
		}
		else if ((LA(1)==LCURLY)) {
			TNode tmp86_AST = null;
			tmp86_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp86_AST);
			match(LCURLY);
			enumList("anonymous");
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp87_AST = null;
			tmp87_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp87_AST);
			match(RCURLY);
		}
		else if ((LA(1)==ID) && (_tokenSet_33.member(LA(2)))) {
			TNode tmp88_AST = null;
			tmp88_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp88_AST);
			match(ID);
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		enumSpecifier_AST = (TNode)currentAST.root;
		returnAST = enumSpecifier_AST;
	}
	
	public final void typedefName() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode typedefName_AST = null;
		Token  i = null;
		TNode i_AST = null;
		
		if (!( isTypedefName ( LT(1).getText() ) ))
		  throw new SemanticException(" isTypedefName ( LT(1).getText() ) ");
		i = LT(1);
		i_AST = (TNode)astFactory.create(i);
		astFactory.addASTChild(currentAST, i_AST);
		match(ID);
		if ( inputState.guessing==0 ) {
			typedefName_AST = (TNode)currentAST.root;
			typedefName_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NTypedefName)).add(i_AST));
			currentAST.root = typedefName_AST;
			currentAST.child = typedefName_AST!=null &&typedefName_AST.getFirstChild()!=null ?
				typedefName_AST.getFirstChild() : typedefName_AST;
			currentAST.advanceChildToEnd();
		}
		typedefName_AST = (TNode)currentAST.root;
		returnAST = typedefName_AST;
	}
	
	public final void typeName() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode typeName_AST = null;
		
		specifierQualifierList();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case STAR:
		case LPAREN:
		case LBRACKET:
		{
			nonemptyAbstractDeclarator();
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
		typeName_AST = (TNode)currentAST.root;
		returnAST = typeName_AST;
	}
	
	public final void structOrUnion() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structOrUnion_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_struct:
		{
			TNode tmp89_AST = null;
			tmp89_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp89_AST);
			match(LITERAL_struct);
			structOrUnion_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_union:
		{
			TNode tmp90_AST = null;
			tmp90_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp90_AST);
			match(LITERAL_union);
			structOrUnion_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = structOrUnion_AST;
	}
	
	public final void structDeclarationList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structDeclarationList_AST = null;
		
		{
		int _cnt221=0;
		_loop221:
		do {
			if ((_tokenSet_30.member(LA(1)))) {
				structDeclaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt221>=1 ) { break _loop221; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt221++;
		} while (true);
		}
		structDeclarationList_AST = (TNode)currentAST.root;
		returnAST = structDeclarationList_AST;
	}
	
	public final void structDeclaration() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structDeclaration_AST = null;
		
		specifierQualifierList();
		astFactory.addASTChild(currentAST, returnAST);
		structDeclaratorList();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
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
		{
		int _cnt96=0;
		_loop96:
		do {
			if ((LA(1)==SEMI)) {
				match(SEMI);
			}
			else {
				if ( _cnt96>=1 ) { break _loop96; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt96++;
		} while (true);
		}
		structDeclaration_AST = (TNode)currentAST.root;
		returnAST = structDeclaration_AST;
	}
	
	public final void specifierQualifierList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode specifierQualifierList_AST = null;
		int specCount = 0;
		
		{
		int _cnt226=0;
		_loop226:
		do {
			boolean synPredMatched225 = false;
			if (((_tokenSet_12.member(LA(1))) && (_tokenSet_34.member(LA(2))))) {
				int _m225 = mark();
				synPredMatched225 = true;
				inputState.guessing++;
				try {
					{
					if ((LA(1)==LITERAL_struct) && (true)) {
						match(LITERAL_struct);
					}
					else if ((LA(1)==LITERAL_union) && (true)) {
						match(LITERAL_union);
					}
					else if ((LA(1)==LITERAL_enum) && (true)) {
						match(LITERAL_enum);
					}
					else if ((_tokenSet_12.member(LA(1))) && (true)) {
						typeSpecifier(specCount);
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
				catch (RecognitionException pe) {
					synPredMatched225 = false;
				}
				rewind(_m225);
				inputState.guessing--;
			}
			if ( synPredMatched225 ) {
				specCount=typeSpecifier(specCount);
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((LA(1)==LITERAL_volatile||LA(1)==LITERAL_const)) {
				typeQualifier();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt226>=1 ) { break _loop226; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt226++;
		} while (true);
		}
		specifierQualifierList_AST = (TNode)currentAST.root;
		returnAST = specifierQualifierList_AST;
	}
	
	public final void structDeclaratorList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structDeclaratorList_AST = null;
		
		structDeclarator();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop99:
		do {
			if ((LA(1)==COMMA) && (_tokenSet_35.member(LA(2)))) {
				match(COMMA);
				structDeclarator();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop99;
			}
			
		} while (true);
		}
		structDeclaratorList_AST = (TNode)currentAST.root;
		returnAST = structDeclaratorList_AST;
	}
	
	public final void structDeclarator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode structDeclarator_AST = null;
		
		{
		switch ( LA(1)) {
		case ID:
		case STAR:
		case LPAREN:
		{
			declarator(false);
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LITERAL_asm:
		case SEMI:
		case COMMA:
		case COLON:
		case LITERAL___attribute:
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
		case COLON:
		{
			TNode tmp94_AST = null;
			tmp94_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp94_AST);
			match(COLON);
			constExpr();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LITERAL_asm:
		case SEMI:
		case COMMA:
		case LITERAL___attribute:
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
		_loop104:
		do {
			if ((LA(1)==LITERAL_asm||LA(1)==LITERAL___attribute)) {
				attributeDecl();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop104;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			structDeclarator_AST = (TNode)currentAST.root;
			structDeclarator_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NStructDeclarator)).add(structDeclarator_AST));
			currentAST.root = structDeclarator_AST;
			currentAST.child = structDeclarator_AST!=null &&structDeclarator_AST.getFirstChild()!=null ?
				structDeclarator_AST.getFirstChild() : structDeclarator_AST;
			currentAST.advanceChildToEnd();
		}
		structDeclarator_AST = (TNode)currentAST.root;
		returnAST = structDeclarator_AST;
	}
	
	public final void enumList(
		String enumName
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode enumList_AST = null;
		
		enumerator(enumName);
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop111:
		do {
			if ((LA(1)==COMMA) && (LA(2)==ID)) {
				match(COMMA);
				enumerator(enumName);
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop111;
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
		enumList_AST = (TNode)currentAST.root;
		returnAST = enumList_AST;
	}
	
	public final void enumerator(
		String enumName
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode enumerator_AST = null;
		Token  i = null;
		TNode i_AST = null;
		
		i = LT(1);
		i_AST = (TNode)astFactory.create(i);
		astFactory.addASTChild(currentAST, i_AST);
		match(ID);
		if ( inputState.guessing==0 ) {
			symbolTable.add(  i.getText(),
			(TNode)astFactory.make( (new ASTArray(3)).add(null).add((TNode)astFactory.create(LITERAL_enum,"enum")).add((TNode)astFactory.create(ID,enumName)))
			);
			
		}
		{
		switch ( LA(1)) {
		case ASSIGN:
		{
			TNode tmp97_AST = null;
			tmp97_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp97_AST);
			match(ASSIGN);
			constExpr();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case RCURLY:
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
		enumerator_AST = (TNode)currentAST.root;
		returnAST = enumerator_AST;
	}
	
	public final void initDecl(
		AST declarationSpecifiers
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode initDecl_AST = null;
		TNode d_AST = null;
		String declName = "";
		
		declName=declarator(false);
		d_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			AST ds1, d1;
			ds1 = astFactory.dupList(declarationSpecifiers);
			d1 = astFactory.dupList(d_AST);
			symbolTable.add(declName, (TNode)astFactory.make( (new ASTArray(3)).add(null).add(ds1).add(d1)) );
			
		}
		{
		_loop119:
		do {
			if ((LA(1)==LITERAL_asm||LA(1)==LITERAL___attribute)) {
				attributeDecl();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop119;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case ASSIGN:
		{
			TNode tmp98_AST = null;
			tmp98_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp98_AST);
			match(ASSIGN);
			initializer();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case COLON:
		{
			TNode tmp99_AST = null;
			tmp99_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp99_AST);
			match(COLON);
			expr();
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
		if ( inputState.guessing==0 ) {
			initDecl_AST = (TNode)currentAST.root;
			initDecl_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NInitDecl)).add(initDecl_AST));
			currentAST.root = initDecl_AST;
			currentAST.child = initDecl_AST!=null &&initDecl_AST.getFirstChild()!=null ?
				initDecl_AST.getFirstChild() : initDecl_AST;
			currentAST.advanceChildToEnd();
		}
		initDecl_AST = (TNode)currentAST.root;
		returnAST = initDecl_AST;
	}
	
	public final void attributeList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode attributeList_AST = null;
		
		attribute();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop124:
		do {
			if ((LA(1)==COMMA) && ((LA(2) >= LITERAL_typedef && LA(2) <= LITERAL___imag))) {
				TNode tmp100_AST = null;
				tmp100_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp100_AST);
				match(COMMA);
				attribute();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop124;
			}
			
		} while (true);
		}
		{
		switch ( LA(1)) {
		case COMMA:
		{
			TNode tmp101_AST = null;
			tmp101_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp101_AST);
			match(COMMA);
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
		attributeList_AST = (TNode)currentAST.root;
		returnAST = attributeList_AST;
	}
	
	protected final void stringConst() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode stringConst_AST = null;
		
		{
		int _cnt314=0;
		_loop314:
		do {
			if ((LA(1)==StringLiteral)) {
				TNode tmp102_AST = null;
				tmp102_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp102_AST);
				match(StringLiteral);
			}
			else {
				if ( _cnt314>=1 ) { break _loop314; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt314++;
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			stringConst_AST = (TNode)currentAST.root;
			stringConst_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NStringSeq)).add(stringConst_AST));
			currentAST.root = stringConst_AST;
			currentAST.child = stringConst_AST!=null &&stringConst_AST.getFirstChild()!=null ?
				stringConst_AST.getFirstChild() : stringConst_AST;
			currentAST.advanceChildToEnd();
		}
		stringConst_AST = (TNode)currentAST.root;
		returnAST = stringConst_AST;
	}
	
	public final void attribute() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode attribute_AST = null;
		
		{
		_loop129:
		do {
			if ((_tokenSet_36.member(LA(1)))) {
				{
				TNode tmp103_AST = null;
				tmp103_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp103_AST);
				match(_tokenSet_36);
				}
			}
			else if ((LA(1)==LPAREN)) {
				TNode tmp104_AST = null;
				tmp104_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp104_AST);
				match(LPAREN);
				attributeList();
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp105_AST = null;
				tmp105_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp105_AST);
				match(RPAREN);
			}
			else {
				break _loop129;
			}
			
		} while (true);
		}
		attribute_AST = (TNode)currentAST.root;
		returnAST = attribute_AST;
	}
	
	public final void compoundStatement(
		String scopeName
	) throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode compoundStatement_AST = null;
		
		TNode tmp106_AST = null;
		tmp106_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp106_AST);
		match(LCURLY);
		if ( inputState.guessing==0 ) {
			
			pushScope(scopeName);
			
		}
		{
		_loop136:
		do {
			boolean synPredMatched133 = false;
			if (((_tokenSet_37.member(LA(1))) && (_tokenSet_4.member(LA(2))))) {
				int _m133 = mark();
				synPredMatched133 = true;
				inputState.guessing++;
				try {
					{
					if ((LA(1)==LITERAL_typedef) && (true)) {
						match(LITERAL_typedef);
					}
					else if ((LA(1)==LITERAL___label__)) {
						match(LITERAL___label__);
					}
					else if ((_tokenSet_3.member(LA(1))) && (_tokenSet_4.member(LA(2)))) {
						declaration();
					}
					else {
						throw new NoViableAltException(LT(1), getFilename());
					}
					
					}
				}
				catch (RecognitionException pe) {
					synPredMatched133 = false;
				}
				rewind(_m133);
				inputState.guessing--;
			}
			if ( synPredMatched133 ) {
				declarationList();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				boolean synPredMatched135 = false;
				if (((_tokenSet_38.member(LA(1))) && (_tokenSet_39.member(LA(2))))) {
					int _m135 = mark();
					synPredMatched135 = true;
					inputState.guessing++;
					try {
						{
						nestedFunctionDef();
						}
					}
					catch (RecognitionException pe) {
						synPredMatched135 = false;
					}
					rewind(_m135);
					inputState.guessing--;
				}
				if ( synPredMatched135 ) {
					nestedFunctionDef();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop136;
				}
				}
			} while (true);
			}
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case LCURLY:
			case SEMI:
			case ID:
			case STAR:
			case LPAREN:
			case LITERAL_while:
			case LITERAL_do:
			case LITERAL_for:
			case LITERAL_goto:
			case LITERAL_continue:
			case LITERAL_break:
			case LITERAL_return:
			case LITERAL_case:
			case LITERAL_default:
			case LITERAL_if:
			case LITERAL_switch:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				statementList();
				astFactory.addASTChild(currentAST, returnAST);
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
			if ( inputState.guessing==0 ) {
				popScope();
			}
			TNode tmp107_AST = null;
			tmp107_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp107_AST);
			match(RCURLY);
			if ( inputState.guessing==0 ) {
				compoundStatement_AST = (TNode)currentAST.root;
				compoundStatement_AST.setType( NCompoundStatement ); compoundStatement_AST.setAttribute( "scopeName", scopeName );
			}
			compoundStatement_AST = (TNode)currentAST.root;
			returnAST = compoundStatement_AST;
		}
		
	public final void nestedFunctionDef() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode nestedFunctionDef_AST = null;
		TNode ds_AST = null;
		TNode d_AST = null;
		String declName;
		
		{
		switch ( LA(1)) {
		case LITERAL_auto:
		{
			TNode tmp108_AST = null;
			tmp108_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp108_AST);
			match(LITERAL_auto);
			break;
		}
		case LITERAL_volatile:
		case LITERAL_struct:
		case LITERAL_union:
		case LITERAL_enum:
		case LITERAL_extern:
		case LITERAL_static:
		case LITERAL_const:
		case LITERAL_void:
		case LITERAL_char:
		case LITERAL_short:
		case LITERAL_int:
		case LITERAL_long:
		case LITERAL_float:
		case LITERAL_double:
		case LITERAL_signed:
		case LITERAL_unsigned:
		case ID:
		case STAR:
		case LPAREN:
		case LITERAL_inline:
		case LITERAL_byte:
		case LITERAL_boolean:
		case LITERAL_Servo:
		case LITERAL_Wire:
		case LITERAL_typeof:
		case LITERAL___complex:
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
		boolean synPredMatched142 = false;
		if (((_tokenSet_10.member(LA(1))) && (_tokenSet_11.member(LA(2))))) {
			int _m142 = mark();
			synPredMatched142 = true;
			inputState.guessing++;
			try {
				{
				functionDeclSpecifiers();
				}
			}
			catch (RecognitionException pe) {
				synPredMatched142 = false;
			}
			rewind(_m142);
			inputState.guessing--;
		}
		if ( synPredMatched142 ) {
			functionDeclSpecifiers();
			ds_AST = (TNode)returnAST;
			astFactory.addASTChild(currentAST, returnAST);
		}
		else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_39.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		declName=declarator(false);
		d_AST = (TNode)returnAST;
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			
			AST d2, ds2;
			d2 = astFactory.dupList(d_AST);
			ds2 = astFactory.dupList(ds_AST);
			symbolTable.add(declName, (TNode)astFactory.make( (new ASTArray(3)).add(null).add(ds2).add(d2)));
			pushScope(declName);
			
		}
		{
		_loop144:
		do {
			if ((_tokenSet_3.member(LA(1)))) {
				declaration();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop144;
			}
			
		} while (true);
		}
		if ( inputState.guessing==0 ) {
			popScope();
		}
		compoundStatement(declName);
		astFactory.addASTChild(currentAST, returnAST);
		if ( inputState.guessing==0 ) {
			nestedFunctionDef_AST = (TNode)currentAST.root;
			nestedFunctionDef_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NFunctionDef)).add(nestedFunctionDef_AST));
			currentAST.root = nestedFunctionDef_AST;
			currentAST.child = nestedFunctionDef_AST!=null &&nestedFunctionDef_AST.getFirstChild()!=null ?
				nestedFunctionDef_AST.getFirstChild() : nestedFunctionDef_AST;
			currentAST.advanceChildToEnd();
		}
		nestedFunctionDef_AST = (TNode)currentAST.root;
		returnAST = nestedFunctionDef_AST;
	}
	
	public final void statementList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode statementList_AST = null;
		
		{
		int _cnt256=0;
		_loop256:
		do {
			if ((_tokenSet_40.member(LA(1)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				if ( _cnt256>=1 ) { break _loop256; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt256++;
		} while (true);
		}
		statementList_AST = (TNode)currentAST.root;
		returnAST = statementList_AST;
	}
	
	public final void statement() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode statement_AST = null;
		TNode e1_AST = null;
		TNode e2_AST = null;
		TNode e3_AST = null;
		TNode s_AST = null;
		
		switch ( LA(1)) {
		case SEMI:
		{
			TNode tmp109_AST = null;
			tmp109_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp109_AST);
			match(SEMI);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LCURLY:
		{
			compoundStatement(getAScopeName());
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_while:
		{
			TNode tmp110_AST = null;
			tmp110_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp110_AST);
			match(LITERAL_while);
			match(LPAREN);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_do:
		{
			TNode tmp113_AST = null;
			tmp113_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp113_AST);
			match(LITERAL_do);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			match(LITERAL_while);
			match(LPAREN);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			match(SEMI);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_for:
		{
			match(LITERAL_for);
			TNode tmp119_AST = null;
			tmp119_AST = (TNode)astFactory.create(LT(1));
			match(LPAREN);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				expr();
				e1_AST = (TNode)returnAST;
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
			TNode tmp120_AST = null;
			tmp120_AST = (TNode)astFactory.create(LT(1));
			match(SEMI);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				expr();
				e2_AST = (TNode)returnAST;
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
			TNode tmp121_AST = null;
			tmp121_AST = (TNode)astFactory.create(LT(1));
			match(SEMI);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				expr();
				e3_AST = (TNode)returnAST;
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
			TNode tmp122_AST = null;
			tmp122_AST = (TNode)astFactory.create(LT(1));
			match(RPAREN);
			statement();
			s_AST = (TNode)returnAST;
			if ( inputState.guessing==0 ) {
				statement_AST = (TNode)currentAST.root;
				
				if ( e1_AST == null) { e1_AST = (TNode)astFactory.create(NEmptyExpression); }
				if ( e2_AST == null) { e2_AST = (TNode)astFactory.create(NEmptyExpression); }
				if ( e3_AST == null) { e3_AST = (TNode)astFactory.create(NEmptyExpression); }
				statement_AST = (TNode)astFactory.make( (new ASTArray(5)).add((TNode)astFactory.create(LITERAL_for,"for")).add(e1_AST).add(e2_AST).add(e3_AST).add(s_AST));
				
				currentAST.root = statement_AST;
				currentAST.child = statement_AST!=null &&statement_AST.getFirstChild()!=null ?
					statement_AST.getFirstChild() : statement_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		case LITERAL_goto:
		{
			TNode tmp123_AST = null;
			tmp123_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp123_AST);
			match(LITERAL_goto);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			match(SEMI);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_continue:
		{
			TNode tmp125_AST = null;
			tmp125_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp125_AST);
			match(LITERAL_continue);
			match(SEMI);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_break:
		{
			TNode tmp127_AST = null;
			tmp127_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp127_AST);
			match(LITERAL_break);
			match(SEMI);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_return:
		{
			TNode tmp129_AST = null;
			tmp129_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp129_AST);
			match(LITERAL_return);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				expr();
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
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_case:
		{
			TNode tmp131_AST = null;
			tmp131_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp131_AST);
			match(LITERAL_case);
			{
			boolean synPredMatched153 = false;
			if (((_tokenSet_2.member(LA(1))) && (_tokenSet_20.member(LA(2))))) {
				int _m153 = mark();
				synPredMatched153 = true;
				inputState.guessing++;
				try {
					{
					constExpr();
					match(VARARGS);
					}
				}
				catch (RecognitionException pe) {
					synPredMatched153 = false;
				}
				rewind(_m153);
				inputState.guessing--;
			}
			if ( synPredMatched153 ) {
				rangeExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_41.member(LA(2)))) {
				constExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			match(COLON);
			{
			if ((_tokenSet_40.member(LA(1))) && (_tokenSet_42.member(LA(2)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_43.member(LA(1))) && (_tokenSet_44.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_default:
		{
			TNode tmp133_AST = null;
			tmp133_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp133_AST);
			match(LITERAL_default);
			match(COLON);
			{
			if ((_tokenSet_40.member(LA(1))) && (_tokenSet_42.member(LA(2)))) {
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_43.member(LA(1))) && (_tokenSet_44.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_if:
		{
			TNode tmp135_AST = null;
			tmp135_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp135_AST);
			match(LITERAL_if);
			match(LPAREN);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			{
			if ((LA(1)==LITERAL_else) && (_tokenSet_40.member(LA(2)))) {
				TNode tmp138_AST = null;
				tmp138_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp138_AST);
				match(LITERAL_else);
				statement();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else if ((_tokenSet_43.member(LA(1))) && (_tokenSet_44.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			statement_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_switch:
		{
			TNode tmp139_AST = null;
			tmp139_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp139_AST);
			match(LITERAL_switch);
			match(LPAREN);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			match(RPAREN);
			statement();
			astFactory.addASTChild(currentAST, returnAST);
			statement_AST = (TNode)currentAST.root;
			break;
		}
		default:
			if ((_tokenSet_2.member(LA(1))) && (_tokenSet_45.member(LA(2)))) {
				expr();
				astFactory.addASTChild(currentAST, returnAST);
				match(SEMI);
				if ( inputState.guessing==0 ) {
					statement_AST = (TNode)currentAST.root;
					statement_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NStatementExpr)).add(statement_AST));
					currentAST.root = statement_AST;
					currentAST.child = statement_AST!=null &&statement_AST.getFirstChild()!=null ?
						statement_AST.getFirstChild() : statement_AST;
					currentAST.advanceChildToEnd();
				}
				statement_AST = (TNode)currentAST.root;
			}
			else if ((LA(1)==ID) && (LA(2)==COLON)) {
				TNode tmp143_AST = null;
				tmp143_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp143_AST);
				match(ID);
				match(COLON);
				{
				if ((_tokenSet_40.member(LA(1))) && (_tokenSet_42.member(LA(2)))) {
					statement();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else if ((_tokenSet_43.member(LA(1))) && (_tokenSet_44.member(LA(2)))) {
				}
				else {
					throw new NoViableAltException(LT(1), getFilename());
				}
				
				}
				if ( inputState.guessing==0 ) {
					statement_AST = (TNode)currentAST.root;
					statement_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NLabel)).add(statement_AST));
					currentAST.root = statement_AST;
					currentAST.child = statement_AST!=null &&statement_AST.getFirstChild()!=null ?
						statement_AST.getFirstChild() : statement_AST;
					currentAST.advanceChildToEnd();
				}
				statement_AST = (TNode)currentAST.root;
			}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = statement_AST;
	}
	
	public final void conditionalExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode conditionalExpr_AST = null;
		
		logicalOrExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case QUESTION:
		{
			TNode tmp145_AST = null;
			tmp145_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp145_AST);
			match(QUESTION);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				expr();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
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
			TNode tmp146_AST = null;
			tmp146_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp146_AST);
			match(COLON);
			conditionalExpr();
			astFactory.addASTChild(currentAST, returnAST);
			break;
		}
		case LITERAL_asm:
		case RCURLY:
		case SEMI:
		case COMMA:
		case COLON:
		case ASSIGN:
		case RPAREN:
		case RBRACKET:
		case VARARGS:
		case DIV_ASSIGN:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case MOD_ASSIGN:
		case RSHIFT_ASSIGN:
		case LSHIFT_ASSIGN:
		case BAND_ASSIGN:
		case BOR_ASSIGN:
		case BXOR_ASSIGN:
		case LITERAL___attribute:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		conditionalExpr_AST = (TNode)currentAST.root;
		returnAST = conditionalExpr_AST;
	}
	
	public final void logicalOrExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode logicalOrExpr_AST = null;
		
		logicalAndExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop266:
		do {
			if ((LA(1)==LOR)) {
				TNode tmp147_AST = null;
				tmp147_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp147_AST);
				match(LOR);
				logicalAndExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop266;
			}
			
		} while (true);
		}
		logicalOrExpr_AST = (TNode)currentAST.root;
		returnAST = logicalOrExpr_AST;
	}
	
	public final void castExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode castExpr_AST = null;
		
		boolean synPredMatched163 = false;
		if (((LA(1)==LPAREN) && (_tokenSet_30.member(LA(2))))) {
			int _m163 = mark();
			synPredMatched163 = true;
			inputState.guessing++;
			try {
				{
				match(LPAREN);
				typeName();
				match(RPAREN);
				}
			}
			catch (RecognitionException pe) {
				synPredMatched163 = false;
			}
			rewind(_m163);
			inputState.guessing--;
		}
		if ( synPredMatched163 ) {
			TNode tmp148_AST = null;
			tmp148_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp148_AST);
			match(LPAREN);
			typeName();
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp149_AST = null;
			tmp149_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp149_AST);
			match(RPAREN);
			{
			switch ( LA(1)) {
			case LITERAL_asm:
			case ID:
			case STAR:
			case LPAREN:
			case LAND:
			case BAND:
			case PLUS:
			case MINUS:
			case INC:
			case DEC:
			case LITERAL_sizeof:
			case BNOT:
			case LNOT:
			case CharLiteral:
			case StringLiteral:
			case Number:
			case LITERAL___alignof:
			case LITERAL___real:
			case LITERAL___imag:
			{
				castExpr();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LCURLY:
			{
				lcurlyInitializer();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			if ( inputState.guessing==0 ) {
				castExpr_AST = (TNode)currentAST.root;
				castExpr_AST.setType(NCast);
			}
			castExpr_AST = (TNode)currentAST.root;
		}
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_46.member(LA(2)))) {
			unaryExpr();
			astFactory.addASTChild(currentAST, returnAST);
			castExpr_AST = (TNode)currentAST.root;
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		returnAST = castExpr_AST;
	}
	
	public final void unaryExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode unaryExpr_AST = null;
		TNode u_AST = null;
		
		switch ( LA(1)) {
		case ID:
		case LPAREN:
		case CharLiteral:
		case StringLiteral:
		case Number:
		{
			postfixExpr();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case INC:
		{
			TNode tmp150_AST = null;
			tmp150_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp150_AST);
			match(INC);
			castExpr();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case DEC:
		{
			TNode tmp151_AST = null;
			tmp151_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp151_AST);
			match(DEC);
			castExpr();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case STAR:
		case LAND:
		case BAND:
		case PLUS:
		case MINUS:
		case BNOT:
		case LNOT:
		case LITERAL___real:
		case LITERAL___imag:
		{
			unaryOperator();
			u_AST = (TNode)returnAST;
			astFactory.addASTChild(currentAST, returnAST);
			castExpr();
			astFactory.addASTChild(currentAST, returnAST);
			if ( inputState.guessing==0 ) {
				unaryExpr_AST = (TNode)currentAST.root;
				unaryExpr_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NUnaryExpr)).add(unaryExpr_AST));
				currentAST.root = unaryExpr_AST;
				currentAST.child = unaryExpr_AST!=null &&unaryExpr_AST.getFirstChild()!=null ?
					unaryExpr_AST.getFirstChild() : unaryExpr_AST;
				currentAST.advanceChildToEnd();
			}
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_sizeof:
		{
			TNode tmp152_AST = null;
			tmp152_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp152_AST);
			match(LITERAL_sizeof);
			{
			boolean synPredMatched184 = false;
			if (((LA(1)==LPAREN) && (_tokenSet_30.member(LA(2))))) {
				int _m184 = mark();
				synPredMatched184 = true;
				inputState.guessing++;
				try {
					{
					match(LPAREN);
					typeName();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched184 = false;
				}
				rewind(_m184);
				inputState.guessing--;
			}
			if ( synPredMatched184 ) {
				TNode tmp153_AST = null;
				tmp153_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp153_AST);
				match(LPAREN);
				typeName();
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp154_AST = null;
				tmp154_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp154_AST);
				match(RPAREN);
			}
			else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_46.member(LA(2)))) {
				unaryExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL___alignof:
		{
			TNode tmp155_AST = null;
			tmp155_AST = (TNode)astFactory.create(LT(1));
			astFactory.makeASTRoot(currentAST, tmp155_AST);
			match(LITERAL___alignof);
			{
			boolean synPredMatched187 = false;
			if (((LA(1)==LPAREN) && (_tokenSet_30.member(LA(2))))) {
				int _m187 = mark();
				synPredMatched187 = true;
				inputState.guessing++;
				try {
					{
					match(LPAREN);
					typeName();
					}
				}
				catch (RecognitionException pe) {
					synPredMatched187 = false;
				}
				rewind(_m187);
				inputState.guessing--;
			}
			if ( synPredMatched187 ) {
				TNode tmp156_AST = null;
				tmp156_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp156_AST);
				match(LPAREN);
				typeName();
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp157_AST = null;
				tmp157_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp157_AST);
				match(RPAREN);
			}
			else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_46.member(LA(2)))) {
				unaryExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_asm:
		{
			gnuAsmExpr();
			astFactory.addASTChild(currentAST, returnAST);
			unaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = unaryExpr_AST;
	}
	
	public final void nonemptyAbstractDeclarator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode nonemptyAbstractDeclarator_AST = null;
		
		{
		switch ( LA(1)) {
		case STAR:
		{
			pointerGroup();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop173:
			do {
				switch ( LA(1)) {
				case LPAREN:
				{
					{
					TNode tmp158_AST = null;
					tmp158_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp158_AST);
					match(LPAREN);
					{
					switch ( LA(1)) {
					case STAR:
					case LPAREN:
					case LBRACKET:
					{
						nonemptyAbstractDeclarator();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case LITERAL_typedef:
					case LITERAL_volatile:
					case LITERAL_struct:
					case LITERAL_union:
					case LITERAL_enum:
					case LITERAL_auto:
					case LITERAL_register:
					case LITERAL_extern:
					case LITERAL_static:
					case LITERAL_const:
					case LITERAL_void:
					case LITERAL_char:
					case LITERAL_short:
					case LITERAL_int:
					case LITERAL_long:
					case LITERAL_float:
					case LITERAL_double:
					case LITERAL_signed:
					case LITERAL_unsigned:
					case ID:
					case LITERAL_inline:
					case LITERAL_byte:
					case LITERAL_boolean:
					case LITERAL_Servo:
					case LITERAL_Wire:
					case LITERAL_typeof:
					case LITERAL___complex:
					{
						parameterTypeList();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case COMMA:
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
					{
					switch ( LA(1)) {
					case COMMA:
					{
						match(COMMA);
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
					TNode tmp160_AST = null;
					tmp160_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp160_AST);
					match(RPAREN);
					}
					break;
				}
				case LBRACKET:
				{
					{
					TNode tmp161_AST = null;
					tmp161_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp161_AST);
					match(LBRACKET);
					{
					switch ( LA(1)) {
					case LITERAL_asm:
					case ID:
					case STAR:
					case LPAREN:
					case LAND:
					case BAND:
					case PLUS:
					case MINUS:
					case INC:
					case DEC:
					case LITERAL_sizeof:
					case BNOT:
					case LNOT:
					case CharLiteral:
					case StringLiteral:
					case Number:
					case LITERAL___alignof:
					case LITERAL___real:
					case LITERAL___imag:
					{
						expr();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case RBRACKET:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(LT(1), getFilename());
					}
					}
					}
					TNode tmp162_AST = null;
					tmp162_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp162_AST);
					match(RBRACKET);
					}
					break;
				}
				default:
				{
					break _loop173;
				}
				}
			} while (true);
			}
			break;
		}
		case LPAREN:
		case LBRACKET:
		{
			{
			int _cnt180=0;
			_loop180:
			do {
				switch ( LA(1)) {
				case LPAREN:
				{
					{
					TNode tmp163_AST = null;
					tmp163_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp163_AST);
					match(LPAREN);
					{
					switch ( LA(1)) {
					case STAR:
					case LPAREN:
					case LBRACKET:
					{
						nonemptyAbstractDeclarator();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case LITERAL_typedef:
					case LITERAL_volatile:
					case LITERAL_struct:
					case LITERAL_union:
					case LITERAL_enum:
					case LITERAL_auto:
					case LITERAL_register:
					case LITERAL_extern:
					case LITERAL_static:
					case LITERAL_const:
					case LITERAL_void:
					case LITERAL_char:
					case LITERAL_short:
					case LITERAL_int:
					case LITERAL_long:
					case LITERAL_float:
					case LITERAL_double:
					case LITERAL_signed:
					case LITERAL_unsigned:
					case ID:
					case LITERAL_inline:
					case LITERAL_byte:
					case LITERAL_boolean:
					case LITERAL_Servo:
					case LITERAL_Wire:
					case LITERAL_typeof:
					case LITERAL___complex:
					{
						parameterTypeList();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case COMMA:
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
					{
					switch ( LA(1)) {
					case COMMA:
					{
						match(COMMA);
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
					TNode tmp165_AST = null;
					tmp165_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp165_AST);
					match(RPAREN);
					}
					break;
				}
				case LBRACKET:
				{
					{
					TNode tmp166_AST = null;
					tmp166_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp166_AST);
					match(LBRACKET);
					{
					switch ( LA(1)) {
					case LITERAL_asm:
					case ID:
					case STAR:
					case LPAREN:
					case LAND:
					case BAND:
					case PLUS:
					case MINUS:
					case INC:
					case DEC:
					case LITERAL_sizeof:
					case BNOT:
					case LNOT:
					case CharLiteral:
					case StringLiteral:
					case Number:
					case LITERAL___alignof:
					case LITERAL___real:
					case LITERAL___imag:
					{
						expr();
						astFactory.addASTChild(currentAST, returnAST);
						break;
					}
					case RBRACKET:
					{
						break;
					}
					default:
					{
						throw new NoViableAltException(LT(1), getFilename());
					}
					}
					}
					TNode tmp167_AST = null;
					tmp167_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp167_AST);
					match(RBRACKET);
					}
					break;
				}
				default:
				{
					if ( _cnt180>=1 ) { break _loop180; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				}
				_cnt180++;
			} while (true);
			}
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState.guessing==0 ) {
			nonemptyAbstractDeclarator_AST = (TNode)currentAST.root;
			nonemptyAbstractDeclarator_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NNonemptyAbstractDeclarator)).add(nonemptyAbstractDeclarator_AST));
			currentAST.root = nonemptyAbstractDeclarator_AST;
			currentAST.child = nonemptyAbstractDeclarator_AST!=null &&nonemptyAbstractDeclarator_AST.getFirstChild()!=null ?
				nonemptyAbstractDeclarator_AST.getFirstChild() : nonemptyAbstractDeclarator_AST;
			currentAST.advanceChildToEnd();
		}
		nonemptyAbstractDeclarator_AST = (TNode)currentAST.root;
		returnAST = nonemptyAbstractDeclarator_AST;
	}
	
	public final void postfixExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode postfixExpr_AST = null;
		
		primaryExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case LPAREN:
		case LBRACKET:
		case INC:
		case DEC:
		case PTR:
		case DOT:
		{
			postfixSuffix();
			astFactory.addASTChild(currentAST, returnAST);
			if ( inputState.guessing==0 ) {
				postfixExpr_AST = (TNode)currentAST.root;
				postfixExpr_AST = (TNode)astFactory.make( (new ASTArray(2)).add((TNode)astFactory.create(NPostfixExpr)).add(postfixExpr_AST));
				currentAST.root = postfixExpr_AST;
				currentAST.child = postfixExpr_AST!=null &&postfixExpr_AST.getFirstChild()!=null ?
					postfixExpr_AST.getFirstChild() : postfixExpr_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		case LITERAL_asm:
		case RCURLY:
		case SEMI:
		case COMMA:
		case COLON:
		case ASSIGN:
		case STAR:
		case RPAREN:
		case RBRACKET:
		case VARARGS:
		case DIV_ASSIGN:
		case PLUS_ASSIGN:
		case MINUS_ASSIGN:
		case STAR_ASSIGN:
		case MOD_ASSIGN:
		case RSHIFT_ASSIGN:
		case LSHIFT_ASSIGN:
		case BAND_ASSIGN:
		case BOR_ASSIGN:
		case BXOR_ASSIGN:
		case QUESTION:
		case LOR:
		case LAND:
		case BOR:
		case BXOR:
		case BAND:
		case EQUAL:
		case NOT_EQUAL:
		case LT:
		case LTE:
		case GT:
		case GTE:
		case LSHIFT:
		case RSHIFT:
		case PLUS:
		case MINUS:
		case DIV:
		case MOD:
		case LITERAL___attribute:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		postfixExpr_AST = (TNode)currentAST.root;
		returnAST = postfixExpr_AST;
	}
	
	public final void unaryOperator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode unaryOperator_AST = null;
		
		switch ( LA(1)) {
		case BAND:
		{
			TNode tmp168_AST = null;
			tmp168_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp168_AST);
			match(BAND);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case STAR:
		{
			TNode tmp169_AST = null;
			tmp169_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp169_AST);
			match(STAR);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case PLUS:
		{
			TNode tmp170_AST = null;
			tmp170_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp170_AST);
			match(PLUS);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case MINUS:
		{
			TNode tmp171_AST = null;
			tmp171_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp171_AST);
			match(MINUS);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case BNOT:
		{
			TNode tmp172_AST = null;
			tmp172_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp172_AST);
			match(BNOT);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case LNOT:
		{
			TNode tmp173_AST = null;
			tmp173_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp173_AST);
			match(LNOT);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case LAND:
		{
			TNode tmp174_AST = null;
			tmp174_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp174_AST);
			match(LAND);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL___real:
		{
			TNode tmp175_AST = null;
			tmp175_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp175_AST);
			match(LITERAL___real);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL___imag:
		{
			TNode tmp176_AST = null;
			tmp176_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp176_AST);
			match(LITERAL___imag);
			unaryOperator_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = unaryOperator_AST;
	}
	
	public final void gnuAsmExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode gnuAsmExpr_AST = null;
		
		TNode tmp177_AST = null;
		tmp177_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp177_AST);
		match(LITERAL_asm);
		{
		switch ( LA(1)) {
		case LITERAL_volatile:
		{
			TNode tmp178_AST = null;
			tmp178_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp178_AST);
			match(LITERAL_volatile);
			break;
		}
		case LPAREN:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		TNode tmp179_AST = null;
		tmp179_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp179_AST);
		match(LPAREN);
		stringConst();
		astFactory.addASTChild(currentAST, returnAST);
		{
		if ((LA(1)==COLON) && (_tokenSet_47.member(LA(2)))) {
			TNode tmp180_AST = null;
			tmp180_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp180_AST);
			match(COLON);
			{
			switch ( LA(1)) {
			case StringLiteral:
			{
				strOptExprPair();
				astFactory.addASTChild(currentAST, returnAST);
				{
				_loop194:
				do {
					if ((LA(1)==COMMA)) {
						TNode tmp181_AST = null;
						tmp181_AST = (TNode)astFactory.create(LT(1));
						astFactory.addASTChild(currentAST, tmp181_AST);
						match(COMMA);
						strOptExprPair();
						astFactory.addASTChild(currentAST, returnAST);
					}
					else {
						break _loop194;
					}
					
				} while (true);
				}
				break;
			}
			case COLON:
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
			{
			if ((LA(1)==COLON) && (_tokenSet_47.member(LA(2)))) {
				TNode tmp182_AST = null;
				tmp182_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp182_AST);
				match(COLON);
				{
				switch ( LA(1)) {
				case StringLiteral:
				{
					strOptExprPair();
					astFactory.addASTChild(currentAST, returnAST);
					{
					_loop198:
					do {
						if ((LA(1)==COMMA)) {
							TNode tmp183_AST = null;
							tmp183_AST = (TNode)astFactory.create(LT(1));
							astFactory.addASTChild(currentAST, tmp183_AST);
							match(COMMA);
							strOptExprPair();
							astFactory.addASTChild(currentAST, returnAST);
						}
						else {
							break _loop198;
						}
						
					} while (true);
					}
					break;
				}
				case COLON:
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
			}
			else if ((LA(1)==COLON||LA(1)==RPAREN) && (_tokenSet_48.member(LA(2)))) {
			}
			else {
				throw new NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else if ((LA(1)==COLON||LA(1)==RPAREN) && (_tokenSet_48.member(LA(2)))) {
		}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		
		}
		{
		switch ( LA(1)) {
		case COLON:
		{
			TNode tmp184_AST = null;
			tmp184_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp184_AST);
			match(COLON);
			stringConst();
			astFactory.addASTChild(currentAST, returnAST);
			{
			_loop201:
			do {
				if ((LA(1)==COMMA)) {
					TNode tmp185_AST = null;
					tmp185_AST = (TNode)astFactory.create(LT(1));
					astFactory.addASTChild(currentAST, tmp185_AST);
					match(COMMA);
					stringConst();
					astFactory.addASTChild(currentAST, returnAST);
				}
				else {
					break _loop201;
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
		TNode tmp186_AST = null;
		tmp186_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp186_AST);
		match(RPAREN);
		if ( inputState.guessing==0 ) {
			gnuAsmExpr_AST = (TNode)currentAST.root;
			gnuAsmExpr_AST.setType(NGnuAsmExpr);
		}
		gnuAsmExpr_AST = (TNode)currentAST.root;
		returnAST = gnuAsmExpr_AST;
	}
	
	public final void strOptExprPair() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode strOptExprPair_AST = null;
		
		stringConst();
		astFactory.addASTChild(currentAST, returnAST);
		{
		switch ( LA(1)) {
		case LPAREN:
		{
			TNode tmp187_AST = null;
			tmp187_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp187_AST);
			match(LPAREN);
			expr();
			astFactory.addASTChild(currentAST, returnAST);
			TNode tmp188_AST = null;
			tmp188_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp188_AST);
			match(RPAREN);
			break;
		}
		case COMMA:
		case COLON:
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
		strOptExprPair_AST = (TNode)currentAST.root;
		returnAST = strOptExprPair_AST;
	}
	
	public final void primaryExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode primaryExpr_AST = null;
		
		switch ( LA(1)) {
		case ID:
		{
			TNode tmp189_AST = null;
			tmp189_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp189_AST);
			match(ID);
			primaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case Number:
		{
			TNode tmp190_AST = null;
			tmp190_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp190_AST);
			match(Number);
			primaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case CharLiteral:
		{
			charConst();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		case StringLiteral:
		{
			stringConst();
			astFactory.addASTChild(currentAST, returnAST);
			primaryExpr_AST = (TNode)currentAST.root;
			break;
		}
		default:
			boolean synPredMatched206 = false;
			if (((LA(1)==LPAREN) && (LA(2)==LCURLY))) {
				int _m206 = mark();
				synPredMatched206 = true;
				inputState.guessing++;
				try {
					{
					match(LPAREN);
					match(LCURLY);
					}
				}
				catch (RecognitionException pe) {
					synPredMatched206 = false;
				}
				rewind(_m206);
				inputState.guessing--;
			}
			if ( synPredMatched206 ) {
				TNode tmp191_AST = null;
				tmp191_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp191_AST);
				match(LPAREN);
				compoundStatement(getAScopeName());
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp192_AST = null;
				tmp192_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp192_AST);
				match(RPAREN);
				primaryExpr_AST = (TNode)currentAST.root;
			}
			else if ((LA(1)==LPAREN) && (_tokenSet_2.member(LA(2)))) {
				TNode tmp193_AST = null;
				tmp193_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp193_AST);
				match(LPAREN);
				expr();
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp194_AST = null;
				tmp194_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp194_AST);
				match(RPAREN);
				if ( inputState.guessing==0 ) {
					primaryExpr_AST = (TNode)currentAST.root;
					primaryExpr_AST.setType(NExpressionGroup);
				}
				primaryExpr_AST = (TNode)currentAST.root;
			}
		else {
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = primaryExpr_AST;
	}
	
	protected final void charConst() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode charConst_AST = null;
		
		TNode tmp195_AST = null;
		tmp195_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp195_AST);
		match(CharLiteral);
		charConst_AST = (TNode)currentAST.root;
		returnAST = charConst_AST;
	}
	
	public final void storageClassSpecifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode storageClassSpecifier_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_auto:
		{
			TNode tmp196_AST = null;
			tmp196_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp196_AST);
			match(LITERAL_auto);
			storageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_register:
		{
			TNode tmp197_AST = null;
			tmp197_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp197_AST);
			match(LITERAL_register);
			storageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_typedef:
		{
			TNode tmp198_AST = null;
			tmp198_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp198_AST);
			match(LITERAL_typedef);
			storageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_extern:
		case LITERAL_static:
		case LITERAL_inline:
		{
			functionStorageClassSpecifier();
			astFactory.addASTChild(currentAST, returnAST);
			storageClassSpecifier_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = storageClassSpecifier_AST;
	}
	
	public final void typeQualifier() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode typeQualifier_AST = null;
		
		switch ( LA(1)) {
		case LITERAL_const:
		{
			TNode tmp199_AST = null;
			tmp199_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp199_AST);
			match(LITERAL_const);
			typeQualifier_AST = (TNode)currentAST.root;
			break;
		}
		case LITERAL_volatile:
		{
			TNode tmp200_AST = null;
			tmp200_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp200_AST);
			match(LITERAL_volatile);
			typeQualifier_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = typeQualifier_AST;
	}
	
	public final void assignOperator() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode assignOperator_AST = null;
		
		switch ( LA(1)) {
		case ASSIGN:
		{
			TNode tmp201_AST = null;
			tmp201_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp201_AST);
			match(ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case DIV_ASSIGN:
		{
			TNode tmp202_AST = null;
			tmp202_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp202_AST);
			match(DIV_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case PLUS_ASSIGN:
		{
			TNode tmp203_AST = null;
			tmp203_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp203_AST);
			match(PLUS_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case MINUS_ASSIGN:
		{
			TNode tmp204_AST = null;
			tmp204_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp204_AST);
			match(MINUS_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case STAR_ASSIGN:
		{
			TNode tmp205_AST = null;
			tmp205_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp205_AST);
			match(STAR_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case MOD_ASSIGN:
		{
			TNode tmp206_AST = null;
			tmp206_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp206_AST);
			match(MOD_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case RSHIFT_ASSIGN:
		{
			TNode tmp207_AST = null;
			tmp207_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp207_AST);
			match(RSHIFT_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case LSHIFT_ASSIGN:
		{
			TNode tmp208_AST = null;
			tmp208_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp208_AST);
			match(LSHIFT_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case BAND_ASSIGN:
		{
			TNode tmp209_AST = null;
			tmp209_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp209_AST);
			match(BAND_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case BOR_ASSIGN:
		{
			TNode tmp210_AST = null;
			tmp210_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp210_AST);
			match(BOR_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		case BXOR_ASSIGN:
		{
			TNode tmp211_AST = null;
			tmp211_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp211_AST);
			match(BXOR_ASSIGN);
			assignOperator_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = assignOperator_AST;
	}
	
	public final void logicalAndExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode logicalAndExpr_AST = null;
		
		inclusiveOrExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop269:
		do {
			if ((LA(1)==LAND)) {
				TNode tmp212_AST = null;
				tmp212_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp212_AST);
				match(LAND);
				inclusiveOrExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop269;
			}
			
		} while (true);
		}
		logicalAndExpr_AST = (TNode)currentAST.root;
		returnAST = logicalAndExpr_AST;
	}
	
	public final void inclusiveOrExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode inclusiveOrExpr_AST = null;
		
		exclusiveOrExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop272:
		do {
			if ((LA(1)==BOR)) {
				TNode tmp213_AST = null;
				tmp213_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp213_AST);
				match(BOR);
				exclusiveOrExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop272;
			}
			
		} while (true);
		}
		inclusiveOrExpr_AST = (TNode)currentAST.root;
		returnAST = inclusiveOrExpr_AST;
	}
	
	public final void exclusiveOrExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode exclusiveOrExpr_AST = null;
		
		bitAndExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop275:
		do {
			if ((LA(1)==BXOR)) {
				TNode tmp214_AST = null;
				tmp214_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp214_AST);
				match(BXOR);
				bitAndExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop275;
			}
			
		} while (true);
		}
		exclusiveOrExpr_AST = (TNode)currentAST.root;
		returnAST = exclusiveOrExpr_AST;
	}
	
	public final void bitAndExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode bitAndExpr_AST = null;
		
		equalityExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop278:
		do {
			if ((LA(1)==BAND)) {
				TNode tmp215_AST = null;
				tmp215_AST = (TNode)astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, tmp215_AST);
				match(BAND);
				equalityExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop278;
			}
			
		} while (true);
		}
		bitAndExpr_AST = (TNode)currentAST.root;
		returnAST = bitAndExpr_AST;
	}
	
	public final void equalityExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode equalityExpr_AST = null;
		
		relationalExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop282:
		do {
			if ((LA(1)==EQUAL||LA(1)==NOT_EQUAL)) {
				{
				switch ( LA(1)) {
				case EQUAL:
				{
					TNode tmp216_AST = null;
					tmp216_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp216_AST);
					match(EQUAL);
					break;
				}
				case NOT_EQUAL:
				{
					TNode tmp217_AST = null;
					tmp217_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp217_AST);
					match(NOT_EQUAL);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				relationalExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop282;
			}
			
		} while (true);
		}
		equalityExpr_AST = (TNode)currentAST.root;
		returnAST = equalityExpr_AST;
	}
	
	public final void relationalExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode relationalExpr_AST = null;
		
		shiftExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop286:
		do {
			if (((LA(1) >= LT && LA(1) <= GTE))) {
				{
				switch ( LA(1)) {
				case LT:
				{
					TNode tmp218_AST = null;
					tmp218_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp218_AST);
					match(LT);
					break;
				}
				case LTE:
				{
					TNode tmp219_AST = null;
					tmp219_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp219_AST);
					match(LTE);
					break;
				}
				case GT:
				{
					TNode tmp220_AST = null;
					tmp220_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp220_AST);
					match(GT);
					break;
				}
				case GTE:
				{
					TNode tmp221_AST = null;
					tmp221_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp221_AST);
					match(GTE);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				shiftExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop286;
			}
			
		} while (true);
		}
		relationalExpr_AST = (TNode)currentAST.root;
		returnAST = relationalExpr_AST;
	}
	
	public final void shiftExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode shiftExpr_AST = null;
		
		additiveExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop290:
		do {
			if ((LA(1)==LSHIFT||LA(1)==RSHIFT)) {
				{
				switch ( LA(1)) {
				case LSHIFT:
				{
					TNode tmp222_AST = null;
					tmp222_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp222_AST);
					match(LSHIFT);
					break;
				}
				case RSHIFT:
				{
					TNode tmp223_AST = null;
					tmp223_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp223_AST);
					match(RSHIFT);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				additiveExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop290;
			}
			
		} while (true);
		}
		shiftExpr_AST = (TNode)currentAST.root;
		returnAST = shiftExpr_AST;
	}
	
	public final void additiveExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode additiveExpr_AST = null;
		
		multExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop294:
		do {
			if ((LA(1)==PLUS||LA(1)==MINUS)) {
				{
				switch ( LA(1)) {
				case PLUS:
				{
					TNode tmp224_AST = null;
					tmp224_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp224_AST);
					match(PLUS);
					break;
				}
				case MINUS:
				{
					TNode tmp225_AST = null;
					tmp225_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp225_AST);
					match(MINUS);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				multExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop294;
			}
			
		} while (true);
		}
		additiveExpr_AST = (TNode)currentAST.root;
		returnAST = additiveExpr_AST;
	}
	
	public final void multExpr() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode multExpr_AST = null;
		
		castExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop298:
		do {
			if ((_tokenSet_49.member(LA(1)))) {
				{
				switch ( LA(1)) {
				case STAR:
				{
					TNode tmp226_AST = null;
					tmp226_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp226_AST);
					match(STAR);
					break;
				}
				case DIV:
				{
					TNode tmp227_AST = null;
					tmp227_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp227_AST);
					match(DIV);
					break;
				}
				case MOD:
				{
					TNode tmp228_AST = null;
					tmp228_AST = (TNode)astFactory.create(LT(1));
					astFactory.makeASTRoot(currentAST, tmp228_AST);
					match(MOD);
					break;
				}
				default:
				{
					throw new NoViableAltException(LT(1), getFilename());
				}
				}
				}
				castExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop298;
			}
			
		} while (true);
		}
		multExpr_AST = (TNode)currentAST.root;
		returnAST = multExpr_AST;
	}
	
	public final void postfixSuffix() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode postfixSuffix_AST = null;
		
		{
		int _cnt305=0;
		_loop305:
		do {
			switch ( LA(1)) {
			case PTR:
			{
				TNode tmp229_AST = null;
				tmp229_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp229_AST);
				match(PTR);
				TNode tmp230_AST = null;
				tmp230_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp230_AST);
				match(ID);
				break;
			}
			case DOT:
			{
				TNode tmp231_AST = null;
				tmp231_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp231_AST);
				match(DOT);
				TNode tmp232_AST = null;
				tmp232_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp232_AST);
				match(ID);
				break;
			}
			case LPAREN:
			{
				functionCall();
				astFactory.addASTChild(currentAST, returnAST);
				break;
			}
			case LBRACKET:
			{
				TNode tmp233_AST = null;
				tmp233_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp233_AST);
				match(LBRACKET);
				expr();
				astFactory.addASTChild(currentAST, returnAST);
				TNode tmp234_AST = null;
				tmp234_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp234_AST);
				match(RBRACKET);
				break;
			}
			case INC:
			{
				TNode tmp235_AST = null;
				tmp235_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp235_AST);
				match(INC);
				break;
			}
			case DEC:
			{
				TNode tmp236_AST = null;
				tmp236_AST = (TNode)astFactory.create(LT(1));
				astFactory.addASTChild(currentAST, tmp236_AST);
				match(DEC);
				break;
			}
			default:
			{
				if ( _cnt305>=1 ) { break _loop305; } else {throw new NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt305++;
		} while (true);
		}
		postfixSuffix_AST = (TNode)currentAST.root;
		returnAST = postfixSuffix_AST;
	}
	
	public final void functionCall() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode functionCall_AST = null;
		TNode a_AST = null;
		
		TNode tmp237_AST = null;
		tmp237_AST = (TNode)astFactory.create(LT(1));
		astFactory.makeASTRoot(currentAST, tmp237_AST);
		match(LPAREN);
		{
		switch ( LA(1)) {
		case LITERAL_asm:
		case ID:
		case STAR:
		case LPAREN:
		case LAND:
		case BAND:
		case PLUS:
		case MINUS:
		case INC:
		case DEC:
		case LITERAL_sizeof:
		case BNOT:
		case LNOT:
		case CharLiteral:
		case StringLiteral:
		case Number:
		case LITERAL___alignof:
		case LITERAL___real:
		case LITERAL___imag:
		{
			argExprList();
			a_AST = (TNode)returnAST;
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
		TNode tmp238_AST = null;
		tmp238_AST = (TNode)astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, tmp238_AST);
		match(RPAREN);
		if ( inputState.guessing==0 ) {
			functionCall_AST = (TNode)currentAST.root;
			
			functionCall_AST.setType( NFunctionCallArgs );
			
		}
		functionCall_AST = (TNode)currentAST.root;
		returnAST = functionCall_AST;
	}
	
	public final void argExprList() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode argExprList_AST = null;
		
		assignExpr();
		astFactory.addASTChild(currentAST, returnAST);
		{
		_loop310:
		do {
			if ((LA(1)==COMMA)) {
				match(COMMA);
				assignExpr();
				astFactory.addASTChild(currentAST, returnAST);
			}
			else {
				break _loop310;
			}
			
		} while (true);
		}
		argExprList_AST = (TNode)currentAST.root;
		returnAST = argExprList_AST;
	}
	
	protected final void intConst() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode intConst_AST = null;
		
		switch ( LA(1)) {
		case IntOctalConst:
		{
			TNode tmp240_AST = null;
			tmp240_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp240_AST);
			match(IntOctalConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case LongOctalConst:
		{
			TNode tmp241_AST = null;
			tmp241_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp241_AST);
			match(LongOctalConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case UnsignedOctalConst:
		{
			TNode tmp242_AST = null;
			tmp242_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp242_AST);
			match(UnsignedOctalConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case IntIntConst:
		{
			TNode tmp243_AST = null;
			tmp243_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp243_AST);
			match(IntIntConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case LongIntConst:
		{
			TNode tmp244_AST = null;
			tmp244_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp244_AST);
			match(LongIntConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case UnsignedIntConst:
		{
			TNode tmp245_AST = null;
			tmp245_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp245_AST);
			match(UnsignedIntConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case IntHexConst:
		{
			TNode tmp246_AST = null;
			tmp246_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp246_AST);
			match(IntHexConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case LongHexConst:
		{
			TNode tmp247_AST = null;
			tmp247_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp247_AST);
			match(LongHexConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		case UnsignedHexConst:
		{
			TNode tmp248_AST = null;
			tmp248_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp248_AST);
			match(UnsignedHexConst);
			intConst_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = intConst_AST;
	}
	
	protected final void floatConst() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode floatConst_AST = null;
		
		switch ( LA(1)) {
		case FloatDoubleConst:
		{
			TNode tmp249_AST = null;
			tmp249_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp249_AST);
			match(FloatDoubleConst);
			floatConst_AST = (TNode)currentAST.root;
			break;
		}
		case DoubleDoubleConst:
		{
			TNode tmp250_AST = null;
			tmp250_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp250_AST);
			match(DoubleDoubleConst);
			floatConst_AST = (TNode)currentAST.root;
			break;
		}
		case LongDoubleConst:
		{
			TNode tmp251_AST = null;
			tmp251_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp251_AST);
			match(LongDoubleConst);
			floatConst_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = floatConst_AST;
	}
	
	public final void dummy() throws RecognitionException, TokenStreamException {
		
		returnAST = null;
		ASTPair currentAST = new ASTPair();
		TNode dummy_AST = null;
		
		switch ( LA(1)) {
		case NTypedefName:
		{
			TNode tmp252_AST = null;
			tmp252_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp252_AST);
			match(NTypedefName);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NInitDecl:
		{
			TNode tmp253_AST = null;
			tmp253_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp253_AST);
			match(NInitDecl);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NDeclarator:
		{
			TNode tmp254_AST = null;
			tmp254_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp254_AST);
			match(NDeclarator);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NStructDeclarator:
		{
			TNode tmp255_AST = null;
			tmp255_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp255_AST);
			match(NStructDeclarator);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NDeclaration:
		{
			TNode tmp256_AST = null;
			tmp256_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp256_AST);
			match(NDeclaration);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NCast:
		{
			TNode tmp257_AST = null;
			tmp257_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp257_AST);
			match(NCast);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NPointerGroup:
		{
			TNode tmp258_AST = null;
			tmp258_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp258_AST);
			match(NPointerGroup);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NExpressionGroup:
		{
			TNode tmp259_AST = null;
			tmp259_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp259_AST);
			match(NExpressionGroup);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NFunctionCallArgs:
		{
			TNode tmp260_AST = null;
			tmp260_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp260_AST);
			match(NFunctionCallArgs);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NNonemptyAbstractDeclarator:
		{
			TNode tmp261_AST = null;
			tmp261_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp261_AST);
			match(NNonemptyAbstractDeclarator);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NInitializer:
		{
			TNode tmp262_AST = null;
			tmp262_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp262_AST);
			match(NInitializer);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NStatementExpr:
		{
			TNode tmp263_AST = null;
			tmp263_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp263_AST);
			match(NStatementExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NEmptyExpression:
		{
			TNode tmp264_AST = null;
			tmp264_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp264_AST);
			match(NEmptyExpression);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NParameterTypeList:
		{
			TNode tmp265_AST = null;
			tmp265_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp265_AST);
			match(NParameterTypeList);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NFunctionDef:
		{
			TNode tmp266_AST = null;
			tmp266_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp266_AST);
			match(NFunctionDef);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NCompoundStatement:
		{
			TNode tmp267_AST = null;
			tmp267_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp267_AST);
			match(NCompoundStatement);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NParameterDeclaration:
		{
			TNode tmp268_AST = null;
			tmp268_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp268_AST);
			match(NParameterDeclaration);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NCommaExpr:
		{
			TNode tmp269_AST = null;
			tmp269_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp269_AST);
			match(NCommaExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NUnaryExpr:
		{
			TNode tmp270_AST = null;
			tmp270_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp270_AST);
			match(NUnaryExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NLabel:
		{
			TNode tmp271_AST = null;
			tmp271_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp271_AST);
			match(NLabel);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NPostfixExpr:
		{
			TNode tmp272_AST = null;
			tmp272_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp272_AST);
			match(NPostfixExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NRangeExpr:
		{
			TNode tmp273_AST = null;
			tmp273_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp273_AST);
			match(NRangeExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NStringSeq:
		{
			TNode tmp274_AST = null;
			tmp274_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp274_AST);
			match(NStringSeq);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NInitializerElementLabel:
		{
			TNode tmp275_AST = null;
			tmp275_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp275_AST);
			match(NInitializerElementLabel);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NLcurlyInitializer:
		{
			TNode tmp276_AST = null;
			tmp276_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp276_AST);
			match(NLcurlyInitializer);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NAsmAttribute:
		{
			TNode tmp277_AST = null;
			tmp277_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp277_AST);
			match(NAsmAttribute);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NGnuAsmExpr:
		{
			TNode tmp278_AST = null;
			tmp278_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp278_AST);
			match(NGnuAsmExpr);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		case NTypeMissing:
		{
			TNode tmp279_AST = null;
			tmp279_AST = (TNode)astFactory.create(LT(1));
			astFactory.addASTChild(currentAST, tmp279_AST);
			match(NTypeMissing);
			dummy_AST = (TNode)currentAST.root;
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		returnAST = dummy_AST;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"\"typedef\"",
		"\"asm\"",
		"\"volatile\"",
		"LCURLY",
		"RCURLY",
		"SEMI",
		"\"struct\"",
		"\"union\"",
		"\"enum\"",
		"\"auto\"",
		"\"register\"",
		"\"extern\"",
		"\"static\"",
		"\"const\"",
		"\"void\"",
		"\"char\"",
		"\"short\"",
		"\"int\"",
		"\"long\"",
		"\"float\"",
		"\"double\"",
		"\"signed\"",
		"\"unsigned\"",
		"ID",
		"COMMA",
		"COLON",
		"ASSIGN",
		"STAR",
		"LPAREN",
		"RPAREN",
		"LBRACKET",
		"RBRACKET",
		"VARARGS",
		"\"while\"",
		"\"do\"",
		"\"for\"",
		"\"goto\"",
		"\"continue\"",
		"\"break\"",
		"\"return\"",
		"\"case\"",
		"\"default\"",
		"\"if\"",
		"\"else\"",
		"\"switch\"",
		"DIV_ASSIGN",
		"PLUS_ASSIGN",
		"MINUS_ASSIGN",
		"STAR_ASSIGN",
		"MOD_ASSIGN",
		"RSHIFT_ASSIGN",
		"LSHIFT_ASSIGN",
		"BAND_ASSIGN",
		"BOR_ASSIGN",
		"BXOR_ASSIGN",
		"QUESTION",
		"LOR",
		"LAND",
		"BOR",
		"BXOR",
		"BAND",
		"EQUAL",
		"NOT_EQUAL",
		"LT",
		"LTE",
		"GT",
		"GTE",
		"LSHIFT",
		"RSHIFT",
		"PLUS",
		"MINUS",
		"DIV",
		"MOD",
		"INC",
		"DEC",
		"\"sizeof\"",
		"BNOT",
		"LNOT",
		"PTR",
		"DOT",
		"CharLiteral",
		"StringLiteral",
		"IntOctalConst",
		"LongOctalConst",
		"UnsignedOctalConst",
		"IntIntConst",
		"LongIntConst",
		"UnsignedIntConst",
		"IntHexConst",
		"LongHexConst",
		"UnsignedHexConst",
		"FloatDoubleConst",
		"DoubleDoubleConst",
		"LongDoubleConst",
		"NTypedefName",
		"NInitDecl",
		"NDeclarator",
		"NStructDeclarator",
		"NDeclaration",
		"NCast",
		"NPointerGroup",
		"NExpressionGroup",
		"NFunctionCallArgs",
		"NNonemptyAbstractDeclarator",
		"NInitializer",
		"NStatementExpr",
		"NEmptyExpression",
		"NParameterTypeList",
		"NFunctionDef",
		"NCompoundStatement",
		"NParameterDeclaration",
		"NCommaExpr",
		"NUnaryExpr",
		"NLabel",
		"NPostfixExpr",
		"NRangeExpr",
		"NStringSeq",
		"NInitializerElementLabel",
		"NLcurlyInitializer",
		"NAsmAttribute",
		"NGnuAsmExpr",
		"NTypeMissing",
		"Vocabulary",
		"Whitespace",
		"Comment",
		"CPPComment",
		"a line directive",
		"Space",
		"LineDirective",
		"BadStringLiteral",
		"Escape",
		"Digit",
		"LongSuffix",
		"UnsignedSuffix",
		"FloatSuffix",
		"Exponent",
		"Number",
		"\"__label__\"",
		"\"inline\"",
		"\"byte\"",
		"\"boolean\"",
		"\"Servo\"",
		"\"Wire\"",
		"\"typeof\"",
		"\"__complex\"",
		"\"__attribute\"",
		"\"__alignof\"",
		"\"__real\"",
		"\"__imag\""
	};
	
	protected void buildTokenTypeASTClassMap() {
		tokenTypeToASTClassMap=null;
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { 6710886000L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { 6710886002L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { 2305843015790362656L, 3401217L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { 268434512L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	private static final long[] mk_tokenSet_4() {
		long[] data = { 6710886096L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_4 = new BitSet(mk_tokenSet_4());
	private static final long[] mk_tokenSet_5() {
		long[] data = { 6710860864L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_5 = new BitSet(mk_tokenSet_5());
	private static final long[] mk_tokenSet_6() {
		long[] data = { 92610232016L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_6 = new BitSet(mk_tokenSet_6());
	private static final long[] mk_tokenSet_7() {
		long[] data = { 6576668672L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_7 = new BitSet(mk_tokenSet_7());
	private static final long[] mk_tokenSet_8() {
		long[] data = { 25635717728L, 0L, 2097152L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_8 = new BitSet(mk_tokenSet_8());
	private static final long[] mk_tokenSet_9() {
		long[] data = { 2306265159670169586L, 3401217L, 31453184L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_9 = new BitSet(mk_tokenSet_9());
	private static final long[] mk_tokenSet_10() {
		long[] data = { 268409920L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_10 = new BitSet(mk_tokenSet_10());
	private static final long[] mk_tokenSet_11() {
		long[] data = { 6710860992L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_11 = new BitSet(mk_tokenSet_11());
	private static final long[] mk_tokenSet_12() {
		long[] data = { 268180480L, 0L, 2064384L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_12 = new BitSet(mk_tokenSet_12());
	private static final long[] mk_tokenSet_13() {
		long[] data = { 2305843032970231968L, 3925505L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_13 = new BitSet(mk_tokenSet_13());
	private static final long[] mk_tokenSet_14() {
		long[] data = { -562924183740448L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_14 = new BitSet(mk_tokenSet_14());
	private static final long[] mk_tokenSet_15() {
		long[] data = { 17314086912L, 524288L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_15 = new BitSet(mk_tokenSet_15());
	private static final long[] mk_tokenSet_16() {
		long[] data = { 2305843016327233568L, 3401217L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_16 = new BitSet(mk_tokenSet_16());
	private static final long[] mk_tokenSet_17() {
		long[] data = { 2305843015790362784L, 3401217L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_17 = new BitSet(mk_tokenSet_17());
	private static final long[] mk_tokenSet_18() {
		long[] data = { -562924720611360L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_18 = new BitSet(mk_tokenSet_18());
	private static final long[] mk_tokenSet_19() {
		long[] data = { 2305843032970232224L, 3925505L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_19 = new BitSet(mk_tokenSet_19());
	private static final long[] mk_tokenSet_20() {
		long[] data = { -576460659693314848L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_20 = new BitSet(mk_tokenSet_20());
	private static final long[] mk_tokenSet_21() {
		long[] data = { -576460694053053216L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_21 = new BitSet(mk_tokenSet_21());
	private static final long[] mk_tokenSet_22() {
		long[] data = { 32749125328L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_22 = new BitSet(mk_tokenSet_22());
	private static final long[] mk_tokenSet_23() {
		long[] data = { 8992587776L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_23 = new BitSet(mk_tokenSet_23());
	private static final long[] mk_tokenSet_24() {
		long[] data = { 100931731184L, 0L, 4177920L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_24 = new BitSet(mk_tokenSet_24());
	private static final long[] mk_tokenSet_25() {
		long[] data = { 32615039552L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_25 = new BitSet(mk_tokenSet_25());
	private static final long[] mk_tokenSet_26() {
		long[] data = { 23622320128L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_26 = new BitSet(mk_tokenSet_26());
	private static final long[] mk_tokenSet_27() {
		long[] data = { 2305843076322557552L, 3401217L, 31444992L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_27 = new BitSet(mk_tokenSet_27());
	private static final long[] mk_tokenSet_28() {
		long[] data = { 8858370560L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_28 = new BitSet(mk_tokenSet_28());
	private static final long[] mk_tokenSet_29() {
		long[] data = { 2306265090950692848L, 3401217L, 31453184L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_29 = new BitSet(mk_tokenSet_29());
	private static final long[] mk_tokenSet_30() {
		long[] data = { 268311616L, 0L, 2064384L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_30 = new BitSet(mk_tokenSet_30());
	private static final long[] mk_tokenSet_31() {
		long[] data = { 32480566464L, 0L, 2064384L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_31 = new BitSet(mk_tokenSet_31());
	private static final long[] mk_tokenSet_32() {
		long[] data = { -562916130677536L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_32 = new BitSet(mk_tokenSet_32());
	private static final long[] mk_tokenSet_33() {
		long[] data = { 33285996144L, 0L, 4177920L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_33 = new BitSet(mk_tokenSet_33());
	private static final long[] mk_tokenSet_34() {
		long[] data = { 33285873376L, 0L, 4161536L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_34 = new BitSet(mk_tokenSet_34());
	private static final long[] mk_tokenSet_35() {
		long[] data = { 7381975584L, 0L, 2097152L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_35 = new BitSet(mk_tokenSet_35());
	private static final long[] mk_tokenSet_36() {
		long[] data = { -13153337360L, -1L, 33554431L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_36 = new BitSet(mk_tokenSet_36());
	private static final long[] mk_tokenSet_37() {
		long[] data = { 268434512L, 0L, 2088960L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_37 = new BitSet(mk_tokenSet_37());
	private static final long[] mk_tokenSet_38() {
		long[] data = { 6710869056L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_38 = new BitSet(mk_tokenSet_38());
	private static final long[] mk_tokenSet_39() {
		long[] data = { 23890754768L, 0L, 2080768L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_39 = new BitSet(mk_tokenSet_39());
	private static final long[] mk_tokenSet_40() {
		long[] data = { 2306265090816475808L, 3401217L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_40 = new BitSet(mk_tokenSet_40());
	private static final long[] mk_tokenSet_41() {
		long[] data = { -576460727875920672L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_41 = new BitSet(mk_tokenSet_41());
	private static final long[] mk_tokenSet_42() {
		long[] data = { -111669149712L, 4194303L, 31453184L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_42 = new BitSet(mk_tokenSet_42());
	private static final long[] mk_tokenSet_43() {
		long[] data = { 2306405828304831392L, 3401217L, 29364224L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_43 = new BitSet(mk_tokenSet_43());
	private static final long[] mk_tokenSet_44() {
		long[] data = { -103079215118L, 4194303L, 31453184L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_44 = new BitSet(mk_tokenSet_44());
	private static final long[] mk_tokenSet_45() {
		long[] data = { -562924720611616L, 4194303L, 31428608L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_45 = new BitSet(mk_tokenSet_45());
	private static final long[] mk_tokenSet_46() {
		long[] data = { -562812648684576L, 4194303L, 31461376L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_46 = new BitSet(mk_tokenSet_46());
	private static final long[] mk_tokenSet_47() {
		long[] data = { 9126805504L, 2097152L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_47 = new BitSet(mk_tokenSet_47());
	private static final long[] mk_tokenSet_48() {
		long[] data = { -562834257738976L, 2105343L, 2097152L, 0L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_48 = new BitSet(mk_tokenSet_48());
	private static final long[] mk_tokenSet_49() {
		long[] data = { 2147483648L, 6144L, 0L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_49 = new BitSet(mk_tokenSet_49());
	
	}

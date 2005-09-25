classfiles = LineObject.class PreprocessorInfoChannel.class StdCParser.class StdCLexer.class WParser.class WLexer.class WTreeParser.class WEmitter.class
javafiles = CSymbolTable.java TNode.java TNodeFactory.java CToken.java LineObject.java PreprocessorInfoChannel.java StdCParser.java StdCLexer.java WParser.java WLexer.java WTreeParser.java WEmitter.java


all : $(javafiles) $(classfiles)
	


clean : 
	


StdCParser.java StdCLexer.java : StdCParser.g
	java -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool StdCParser.g


WParser.java WLexer.java : WParser.g WParser.g
	java -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool -glib "StdCParser.g" WParser.g


WTreeParser.java : WTreeParser.g
	java -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool WTreeParser.g


WEmitter.java : WEmitter.g WTreeParser.g
	java -cp "..\\..\\build\\windows\\work\\lib\\antlr.jar" antlr.Tool -glib "WTreeParser.g" WEmitter.g



.SUFFIXES: .java .class

.java.class : 
	../../build/windows/work/jikes -cp "..\\..\\build\\windows\\work\\java\\lib\\rt.jar;..\\..\\build\\windows\\work\\lib\\mrj.jar;..\\..\\build\\windows\\work\\lib\\antlr.jar;."  $<




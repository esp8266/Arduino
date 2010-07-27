/*
 WString.cpp - String library for Wiring & Arduino
 Copyright (c) 2009-10 Hernando Barragan.  All rights reserved.
 Changes for version 1.0 by Xiaoyang Feng
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include "WProgram.h"
#include "WString.h"

String::String( const char *value )
{
	if ( value == NULL )
		value = "";
	getBuffer( _length = strlen( value ) );
	if(_buffer != NULL)			//added
		strcpy( _buffer, value );
}

String::String( const String &value )
{
	getBuffer( _length = value._length );
	if(_buffer != NULL)			//added
		strcpy( _buffer, value._buffer );
}

String::String( const char value )
{
	_length = 1;
	getBuffer(1);
	if(_buffer != NULL){			//added
		_buffer[0] = value;
		_buffer[1] = '\0';			//_buffer[1] = 0;
	}
}

String::String( const unsigned char value )
{
	_length = 1;
	getBuffer(1);
	if(_buffer != NULL){			//added
		_buffer[0] = value;
		_buffer[1] = '\0';			//_buffer[1] = 0;
	}
}

String::String( const int value, const int base )
{
	//char buf[33];  
	char* buf = (char *) malloc(33);
	if(buf != NULL){				//added
		itoa(value,buf,base);			
		getBuffer( _length = strlen(buf) );
		strcpy( _buffer, buf );
		free(buf);					//added
	}
}

String::String( const unsigned int value, const int base )
{
	//char buf[33];  
	char* buf = (char *) malloc(33);
	if(buf != NULL){				//added
		ultoa((unsigned long)value, buf, base);
		getBuffer( _length = strlen(buf) );
		strcpy( _buffer, buf );	
		free(buf);					//added	
	}
}

String::String( const long value, const int base )
{
	//char buf[33];  
	char* buf = (char *) malloc(33);
	if(buf != NULL){				//added
		ltoa(value, buf, base);
		getBuffer( _length = strlen(buf) );
		strcpy( _buffer, buf );	
		free(buf);					//added	
	}
}

String::String( const unsigned long value, const int base )
{
	//char buf[33];  
	char* buf = (char *) malloc(33);
	if(buf != NULL){				//added
		ultoa(value, buf, 10);
		getBuffer( _length = strlen(buf) );
		strcpy( _buffer, buf );	
		free(buf);					//added	
	}
}

const String & String::operator=( const String &rhs )
{
	if ( this == &rhs )
		return *this;
	
	if ( rhs._length > _length )
	{
		free(_buffer);
		getBuffer( rhs._length );
	}
	_length = rhs._length;
	strcpy( _buffer, rhs._buffer );
	return *this;
}

const String & String::operator+=( const char ch )
{
	if ( _length == _capacity )
	{	//doubleBuffer();					//only place to use
		char *temp = _buffer;
		_length++;
		getBuffer(_length);
		if(_buffer == NULL)	
			return "/*NOT ENOUGH MEMORY*/";
		strcpy(_buffer,temp);
	}
	_buffer[ _length-1 ] = ch;
	_buffer[ _length ] = '\0';
	return *this;
}
const String & String::operator+=( const String &rhs )
{
	_length += rhs._length;
	if ( _length > _capacity )
	{
		char *temp = _buffer;
		getBuffer( _length );
		strcpy( _buffer, temp );
		_buffer[_length] = '\0';
		free(temp);
	}
	strcat( _buffer, rhs._buffer );
	return *this;
}

int String::operator==( const String &rhs ) const
{
	return ( _length == rhs._length && strcmp( _buffer, rhs._buffer ) == 0 );
}

int String::operator!=( const String &rhs ) const
{
	return ( _length != rhs._length || strcmp( _buffer, rhs._buffer ) != 0 );
	//return ( _length != rhs.length() || strcmp( _buffer, rhs.toCharArray() ) != 0 );
}

int String::operator<( const String &rhs ) const
{
	return strcmp( _buffer, rhs._buffer ) < 0;
}

int String::operator>( const String &rhs ) const
{
	return strcmp( _buffer, rhs._buffer ) > 0;
}

int String::operator<=( const String &rhs ) const
{
	return strcmp( _buffer, rhs._buffer ) <= 0;
}

int String::operator>=( const String & rhs ) const
{
	return strcmp( _buffer, rhs._buffer ) >= 0;
}

char & String::operator[]( unsigned int index )
{
	if(index < 0 || index >= _length) exit(1);	//added
	return _buffer[ index ];
}

char String::operator[]( unsigned int index ) const
{
	if(index < 0 || index >= _length) exit(1);	//added
	return _buffer[ index ];
}

char String::charAt( unsigned int index ) const
{
	return operator[]( index );
}

void String::setCharAt( unsigned int index, const char ch ) 
{
	if(_length > index) {
		_buffer[index] = ch;
	}
	else{
		exit(1);									//added
	}
}

boolean String::startsWith( const String &prefix ) const
{
	if ( _length < prefix._length )
		return false;
	
	return startsWith( prefix, 0 );
}

boolean String::startsWith( const String &prefix, unsigned int offset ) const
{
	if ( offset > _length - prefix._length )
		return false;
	
	return (strncmp( &_buffer[offset], prefix._buffer, prefix._length ) == 0);
}

boolean String::endsWith( const String &suffix ) const
{
	if ( _length < suffix._length )
		return false;
	
	return (strcmp( &_buffer[ _length - suffix._length], suffix._buffer ) == 0);
}

boolean String::contains( const String &str ) const
{
	return (strstr(_buffer, str._buffer) != NULL );
}

boolean String::equals( const String &str ) const
{
	return ( _length == str._length && strcmp( _buffer,str._buffer ) == 0 );
}

boolean String::equalsIgnoreCase( const String &str ) const
{
	if ( this == &str )
		return true; 
	else if ( _length != str._length )
		return false;
	//return strcmp(toLowerCase().toCharArray(), s2.toLowerCase().toCharArray()) == 0;
	for(unsigned int i = 0; i < _length; i++ ){
		if(tolower( _buffer[i]) != tolower(str._buffer[i]))
			return false;
	}
	return true;	
}

String String::trim() const
{
	/*
	 String temp = _buffer;
	 unsigned int i,j;
	 
	 for ( i = 0; i < _length; i++ )
	 {
	 if ( !isspace(_buffer[i]) )
	 break;
	 }
	 
	 for ( j = temp._length - 1; j > i; j-- )
	 {
	 if ( !isspace(_buffer[j]) )
	 break;
	 }
	 
	 return temp.substring( i, j + 1);
	 */
	String temp = _buffer;
	unsigned int index = 0;
	unsigned int tempIndex = 0;
	while( index < _length )
	{
		if(_buffer[index] == '\t' ||
		   _buffer[index] == '\r' ||
		   _buffer[index] == '\n' ||
		   _buffer[index] == ' '  ||
		   _buffer[index] == 0x11)
		{
			memcpy(temp._buffer+tempIndex, _buffer+index+1, strlen(_buffer)-index-1);
			temp._buffer[temp._length-1] = '\0';
			temp._length--;
		}
		else{
			tempIndex++;
		}
		index++;
	}
	return temp;
}

String String::toLowerCase() const
{
	String temp = _buffer;
	
	for ( unsigned int i = 0; i < _length; i++ )
		temp._buffer[ i ] = (char)tolower( temp._buffer[ i ] );
	return temp;
}

String String::toUpperCase() const
{
	String temp = _buffer;
	
	for ( unsigned int i = 0; i < _length; i++ )
		temp._buffer[ i ] = (char)toupper( temp._buffer[ i ] );
	return temp;
}

String String::replace( char oldChar, char newChar )
{
	String newString = _buffer;
	char* temp = newString._buffer;
	while( temp = strchr( temp, oldChar ) )
		*temp = newChar;
	
	return newString;
}

String String::replace( const String& match, const String& replace )
{
	/*
	 String newString;
	 String temp = _buffer;
	 int loc;
	 while ( (loc = temp.indexOf( match )) != -1 )
	 {
	 newString += temp.substring( 0, loc );
	 newString += replace;
	 temp = temp.substring( loc + match._length );
	 }
	 newString += temp;  
	 return newString;
	 */
	
	const char* temp = strstr( _buffer, match._buffer);
	if(temp == NULL || match._length == 0){
		String newString = _buffer;
		return newString;
	}
	else{
		//get new buffer
		int count = 0;
		int lastIndex = 0;
		while(temp != NULL){
			count ++; 
			lastIndex = temp - _buffer;
			temp = strstr( &_buffer[ lastIndex + match._length ], match._buffer );
		}
		String newString;
		newString._length = newString._capacity = _length + (replace._length - match._length)*count;
		char* _buf = (char*)malloc(newString._length + 1);
		if(_buf == NULL)
			exit(1);
		newString._buffer = _buf;
		//string copy
		int fromIndex  = 0;
		int tempLength = 0;
		lastIndex = 0;
		temp = strstr( &_buffer[ lastIndex ], match._buffer);
		while (temp != NULL){
			fromIndex = temp - _buffer;
			memcpy(newString._buffer+tempLength, _buffer+lastIndex, fromIndex-lastIndex);
			memcpy(newString._buffer+tempLength+fromIndex-lastIndex, replace._buffer, replace._length);
			tempLength += (fromIndex-lastIndex + replace._length);
			lastIndex = fromIndex + match._length;
			temp = strstr( &_buffer[ lastIndex ], match._buffer);
		}
		memcpy(newString._buffer+tempLength, _buffer+lastIndex, _length-lastIndex);
		tempLength += (_length - lastIndex);
		newString._buffer[tempLength] = '\0';
		newString._length = newString._capacity = strlen(newString._buffer);
		return newString;
	}
}

String String::substring( unsigned int beginIndex ) const
{
	return substring( beginIndex, _length );
}

String String::substring( unsigned int beginIndex, unsigned int endIndex ) const
{
	if ( beginIndex > endIndex )
	{
		int temp = endIndex;
		endIndex = beginIndex;
		beginIndex = temp;
	}
	if ( endIndex > _length )
	{
		endIndex = _length;
	} 
	
	char temp = _buffer[ endIndex ];  // save the replaced character
	_buffer[ endIndex ] = '\0';	
	String outPut = ( _buffer + beginIndex );  // pointer arithmetic
	_buffer[ endIndex ] = temp;  //restore character
	return outPut;
}

const String & String::append( const String &str )
{
	return (*this) += str;
}

const String & String::append( const char ch)
{
	return (*this) += ch;
}

int String::compareTo( const String &str ) const
{
	return strcmp( _buffer, str._buffer );
}

int String::compareTo( const char* str ) const
{
	return strcmp( _buffer, str );
}

int String::indexOf( char ch ) const
{
	return indexOf( ch, 0 );
}

int String::indexOf( char ch, unsigned int fromIndex ) const
{
	if ( fromIndex >= _length )
		return -1;
	
	const char* temp = strchr( &_buffer[fromIndex], ch );
	if ( temp == NULL )
		return -1;
	
	return temp - _buffer;
}

int String::indexOf( const String &str ) const
{
	return indexOf( str, 0 );
}

int String::indexOf( const String &str, unsigned int fromIndex ) const
{
	if ( fromIndex >= _length )
		return -1;
	
	const char *temp = strstr( &_buffer[ fromIndex ], str._buffer );
	
	if ( temp == NULL )
		return -1;
	
	return temp - _buffer; // pointer subtraction
}

int String::lastIndexOf( char ch ) const
{
	return lastIndexOf( ch, _length - 1 );
}

int String::lastIndexOf( char ch, unsigned int fromIndex ) const
{
	if ( fromIndex >= _length )
		return -1;
	
	char tempchar = _buffer[fromIndex + 1];
	_buffer[fromIndex + 1] = '\0';
	char* temp = strrchr( _buffer, ch );
	_buffer[fromIndex + 1] = tempchar;
	
	if ( temp == NULL )
		return -1;
	
	return temp - _buffer;
}

int String::lastIndexOf( const String &str ) const
{
	return lastIndexOf( str, _length - str._length );
}

int String::lastIndexOf( const String &str, unsigned int fromIndex ) const
{
	// check for empty strings
	if ( str._length == 0 || str._length - 1 > fromIndex || fromIndex >= _length )
		return -1;
	/*
	 // matching first character
	 char temp = str[ 0 ];
	 
	 for ( unsigned int i = fromIndex; i >= 0; i-- )
	 {
	 if ( _buffer[ i ] == temp && (*this).substring( i, i + str._length ).equals( str ) )
	 return i;
	 }
	 return -1;
	 */
	char *temp = strstr(&_buffer[_length-fromIndex], str._buffer);
	if(temp == NULL)
		return -1;
	int index;
	while( temp != NULL){
		index = temp - _buffer;
		temp = strstr(&_buffer[index+1],str._buffer);
	}
	return index-_length+fromIndex;
}

//version() returns the version of the library:
String String::version(void) 
{
	return "1.0";
}

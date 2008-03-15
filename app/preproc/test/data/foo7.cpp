#include "hello" /* for blah */

int x[] = { 1, 2, 3 };
int y[2] = { 1, 2 };

class Foo {
public:
  Foo();
  Foo(int x);
  int bar() { return x; }
private:
  int x;
};

Foo::Foo(int x) : x(x) {}

Foo::Foo() {
	x = 0;
}

Foo foo(3);
Foo bar = Foo(2);

void setup() {
}

void
loop    (
 
)



{
}

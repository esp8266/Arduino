#!/usr/bin/env perl
#
#  Grab the Arduino reference documentation from the web site and
#  modify the pages to create an offline reference.
#
# Author: Tom Pollard <tomp at earthlink dot net>
# Written: Jan 12, 2008
#
use strict;
use warnings;

print "username: "; my $user = <STDIN>; chomp($user);
print "password: "; my $pass = <STDIN>; chomp($pass);

my $verbose = 1;
my $CURL_OPTIONS = "--silent --show-error -u $user:$pass"; 

my $ARDUINO = 'http://edit.arduino.cc/en_ref'; # base url for reference site
my $PUBLIC = 'http://arduino.cc/en'; # base url for public site

my %downloaded = ();  # keep track of the pages we download

my $guide = create_page('Guide_index.html', "$ARDUINO/Guide/HomePage");

my $faq = create_page('FAQ.html', "$ARDUINO/Main/FAQ");
my $env = create_page('environment.html', "$ARDUINO/Main/Environment");
my $css = create_page('arduinoUno.css', "$ARDUINO/pub/skins/arduinoUno/arduinoUno.css");
my $css2 = create_page('arduinoWide.css', "$ARDUINO/pub/skins/arduinoWide/arduinoWide.css");
my $css3 = create_page('arduinoWideRender.css', "$ARDUINO/pub/skins/arduinoWideRender/arduinoWideRender.css");
my $eeprom = create_page('EEPROM.html', "$ARDUINO/Reference/EEPROM");
my $stepper = create_page('Stepper.html', "$ARDUINO/Reference/Stepper");
my $softser = create_page('SoftwareSerial.html', "$ARDUINO/Reference/SoftwareSerial");
my $wire = create_page('Wire.html', "$ARDUINO/Reference/Wire");
my $sd = create_page('SD.html', "$ARDUINO/Reference/SD");
my $servo = create_page('Servo.html', "$ARDUINO/Reference/Servo");
my $spi = create_page('SPI.html', "$ARDUINO/Reference/SPI");
my $mousekeyboard = create_page('MouseKeyboard.html', "$ARDUINO/Reference/MouseKeyboard");
my $lcd = create_page('LiquidCrystal.html', "$ARDUINO/Reference/LiquidCrystal");
my $ethernet = create_page('Ethernet.html', "$ARDUINO/Reference/Ethernet");
my $serial = create_page('Serial.html', "$ARDUINO/Reference/Serial");
my $stream = create_page('Stream.html', "$ARDUINO/Reference/Stream");
my $string = create_page('StringObject.html', "$ARDUINO/Reference/StringObject");

create_linked_pages($guide,   qr!$ARDUINO/Guide/(\w+)!,             'Guide_%%.html');
create_linked_pages($softser, qr!$ARDUINO/Reference/(SoftwareSerial\w+)!, '%%.html');
create_linked_pages($eeprom,  qr!$ARDUINO/Reference/(EEPROM\w+)!,         '%%.html');
create_linked_pages($stepper, qr!$ARDUINO/Reference/(Stepper\w+)!,        '%%.html');
create_linked_pages($wire, qr!$ARDUINO/Reference/(Wire\w+)!,        '%%.html');
create_linked_pages($servo, qr!$ARDUINO/Reference/(Servo\w+)!,        '%%.html');
create_linked_pages($sd, qr!$ARDUINO/Reference/(SD\w+)!,        '%%.html');
create_linked_pages($sd, qr!$ARDUINO/Reference/(File\w+)!,        '%%.html');
create_linked_pages($spi, qr!$ARDUINO/Reference/(SPI\w+)!,        '%%.html');
create_linked_pages($mousekeyboard, qr!$ARDUINO/Reference/(Mouse\w+)!,        '%%.html');
create_linked_pages($mousekeyboard, qr!$ARDUINO/Reference/(Keyboard\w+)!,        '%%.html');
create_linked_pages($lcd, qr!$ARDUINO/Reference/(LiquidCrystal\w+)!,        '%%.html');
create_linked_pages($ethernet, qr!$ARDUINO/Reference/(Ethernet\w+)!,        '%%.html');
create_linked_pages($ethernet, qr!$ARDUINO/Reference/(Server\w+)!,        '%%.html');
create_linked_pages($ethernet, qr!$ARDUINO/Reference/(Client\w+)!,        '%%.html');
create_linked_pages($serial, qr!$ARDUINO/Serial/(\w+)!,    'Serial_%%.html');
create_linked_pages($string, qr!$ARDUINO/Reference/(String\w+)!,    '%%.html');
create_linked_pages($stream, qr!$ARDUINO/Reference/(Stream\w+)!,    '%%.html');
create_linked_pages($string, qr!$ARDUINO/Reference/(String\w+)!,    '%%.html');

my $index = create_page('index.html', "$ARDUINO/Reference/HomePage");

create_linked_pages($index, qr!$ARDUINO/Reference/(\w+)!,        '%%.html');

#my $ext = create_page('Extended.html', "$ARDUINO/Reference/Extended");

#create_linked_pages($ext, qr!$ARDUINO/Reference/(\w+)!, '%%.html');

exit 0;

#------------------------- end of main code ----------------------------

########################################################################
#  $original_text = create_page($filename, $url)
#
#    Download the web page at the given URL, change links to point to
#    the offline pages, and save it locally under the given filename. 
#    The original (unmodified) text of the downloaded page is returned.
#
sub create_page {
  my $page = shift;
  my $url = shift;

  print "$page\n" if $verbose;
  my $original_text = `curl $CURL_OPTIONS $url`;
  die "** Unable to download $url **\n" if $? or ! $original_text;
  $downloaded{$url} = $page;  # remember that we downloaded this page

  my $localized_text = localize_page($original_text);
  open(my $PAGE, "> $page") 
    or die "** Unable to open $page for writing. **\n";
  print $PAGE $localized_text;
  close $PAGE;

  return $original_text;
}

########################################################################
#  $localized_text = localize_page($text)
#
#    Rewrite links in the given text to point to the offline pages.
#
sub localize_page {
  my $text = shift;

  # replace links to unknown pages with links to '#'
  $text =~ s!$ARDUINO/Reference/[^?"']*\?[^'"]*!#!xg;

  # replace links to remote guide with links to local guide
  $text =~ s!$ARDUINO/Guide/([^']+)!Guide_$1.html!xg;

  # replace links to remote reference with links to local reference
  $text =~ s!$ARDUINO/Reference/([^']*)!$1.html!xg;

  # replace links to remove serial reference with links to local serial reference
  $text =~ s!$ARDUINO/Serial/([^']*)!Serial_$1.html!xg;

  # direct pages to the local style file
  $text =~ s!$ARDUINO/pub/skins/arduinoUno/arduinoUno.css!arduinoUno.css!xg;

  # direct pages to the local style file
  $text =~ s!$ARDUINO/pub/skins/arduinoWide/arduinoWide.css!arduinoWide.css!xg;

  # direct pages to the local style file
  $text =~ s!$ARDUINO/pub/skins/arduinoWideRender/arduinoWideRender.css!arduinoWideRender.css!xg;

  # change links to Main/FAQ to go to FAQ.html 
  $text =~ s!$ARDUINO/Main/FAQ!FAQ.html!xg;

  # change links to the reference HomePage to go to index.html 
  $text =~ s!HomePage.html!index.html!xg;

  # change links to the reference edit site to go to the public site 
  $text =~ s!$ARDUINO!$PUBLIC!xg;

  # change links to the root directory to go to the Arduino home page
  $text =~ s!href="/"!href="http://www.arduino.cc"/!xg;

  return $text;
}

########################################################################
#  create_linked_pages($text, $link_pattern, $page_name)
#
#    Scan the given text for links matching the $link_pattern and
#    create local files for the linked pages.  
#
#    The link_pattern is a regexp with one parenthesized subexpression -
#    the text matching the subexpression will replace the
#    special pattern '%%' in the $page_name to generate the name of
#    the local file.  
#
sub create_linked_pages {
  my $text = shift;
  my $link_pattern = shift;
  my $new_name = shift;

  while ($text =~ m!$link_pattern!g) {
    my ($url, $name) = ($&, $1);
    (my $page = $new_name) =~ s!%%!$name!;
    next if $name =~ /\?/ || $downloaded{$url};
    create_page($page, $url);
  }
}

#---------------------------- end of code ------------------------------

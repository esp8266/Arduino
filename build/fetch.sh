curl http://arduino.berlios.de/index.php/Reference/HomePage -o index.html
curl http://arduino.berlios.de/pub/skins/arduino/arduino.css -o arduino.css
for i in `grep -o "http://arduino.berlios.de/index.php/Reference/[^']*" index.html | sort -u | grep -v '?' | cut -d '/' -f 6`; do curl http://arduino.berlios.de/index.php/Reference/$i -o $i.html; done
perl -i -pe "s|http://arduino.berlios.de/index.php/Reference/[^?\"']*\?[^'\"]*|#|g" *.html
perl -i -pe "s|http://arduino.berlios.de/index.php/Reference/([^']*)|\1.html|g" *.html
perl -i -pe "s|http://arduino.berlios.de/pub/skins/arduino/arduino.css|arduino.css|g" *.html
perl -i -pe "s|HomePage.html|index.html|g" *.html
perl -i -pe "s|/\\?PHPSESSID=[^\"]*|http://arduino.berlios.de/|g" *.html

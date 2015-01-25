# Internationalization Tools

by @sgk at SwitchScience.

## Reflect the source code change

Sometimes, the developers add/delete/modify the strings which should be translated. You have to reflect the source code change to your translation. This can be done as below given your language code `xx`.

    % ./update.sh xx

This will extract the up-to-date set of strings from the source code, set the translated strings from the current "`Resources_xx.po`" file, and then write back to "`Resources_xx.po`" file. If the developers delete/modify the strings, corresponding translated strings disappear from the file. You may want to do like this.

    % git commit
    % ./update.sh xx
    % git diff Resrouces_xx.po
    % git add Resources_xx.po Resources_xx.properties
    % git commit

## Get the translated catalog from Transifex

You may want to retrieve the translation contribution from Transifex. This can be done as below given your language code `xx`.

    % ./pull.sh xx

Translation strings for only strings already in "`Resources_xx.po`" file will be updated. If Transifex has translated strings which are not in "`Resources_xx.po`" file, the strings will not appear in the file.

If you want to retrieve the newly translated language which is not in the Git repository, you will want to do like this.

    % cp Resources_en.po Resources_xx.po
    % ./pull.sh xx
    % more Resources_xx.po
    % git add Resources_xx.po Resources_xx.properties
    % git commit

## Send the translated catalog to Transifex

You can send the translated catalog file "`Resources_xx.po`" to Transifex using the following command line where `xx` is the language code.

    % ./push.sh xx

Be aware that this will overwrite the current result on Transifex. You may want to do "./update.sh" and "./pull.sh" before doing "./push.sh".

## Select "all" languages

**For the comitter only.** For all above scripts, you can select all languages by specifying "-a" instead of language codes like this.

    % ./update.sh -a

The "all" means all the languages currently in your working directory. This does not mean the languages on Transifex.

#!/bin/sh

rsync -avz --delete core/ fry@processing.org:dev/reference/core/javadoc/
rsync -avz --delete everything/ fry@processing.org:dev/reference/everything/javadoc/
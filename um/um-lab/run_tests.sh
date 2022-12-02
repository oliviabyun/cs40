#! /bin/sh

# lmao="Hello :)"
# echo $lmao
# echo $0
# echo $1
# echo $2
# echo $#
# echo $*
testFiles=$(ls *.um)
# echo $testFiles
for testFile in $testFiles; do
        testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
        # echo $testName
        # .././um $testFile > $testName.2
        um $testFile > $testName.2
        if [ -f $testName.1 ] ; then
                # umdump $testFile > $testName.dump
                # echo "diff $testName.1 $testName.2"
                echo "lmao $testName"
                diff $testName.1 $testName.2
        else
                echo $testName "has no output!"
        fi
done
# run_tests.sh
# Alyssa Williams (awilli36) and Olivia Byun (obyun01)
# 11/21/22
# This file contains a bash script to run all of our tests and diff them
# from expected output.

testFiles=$(ls *.um)
for testFile in $testFiles; do
        testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
        ./um $testFile > $testName.2
        # um $testFile > $testName.2
        if [ -f $testName.1 ] ; then
                echo "lmao $testName"
                diff $testName.1 $testName.2
        else
                echo $testName "has no output!"
        fi
done
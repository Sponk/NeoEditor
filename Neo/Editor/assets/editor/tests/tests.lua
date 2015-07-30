--- tests.lua - Contains tests for testing all code used by the editor
tllut = dofile("tllut/src/tllut.lua")

dofile("csv-test.lua")
dofile("translation-test.lua")

tllut:runAll()
tllut:printResult()

--os.exit()

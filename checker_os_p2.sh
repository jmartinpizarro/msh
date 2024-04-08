#!/bin/bash
#set -x

declare -i nota=0
declare -i incremento=1

testeo(){
   #$1 nombre del fichero test
   echo "Test command: " $1 
   sh     < $1 > salida_bash 2>/dev/null
   ./msh  < $1 > salida_msh  2>/dev/null
   if diff -i -w -q --ignore-all-space salida_bash salida_msh > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))  
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh
	 echo " === Expected Output  === "		
	 cat  salida_bash
   fi
   rm -f salida* res.txt
   #sleep 1
}

testeobg(){
   #$1 nombre del fichero test
   sh     < $1 > salida_bash 2>/dev/null
   ./msh  < $1 > salida_msh  2>/dev/null
   tail -n 1 salida_bash > salida_b
   tail -n 1 salida_msh > salida_m
   if diff -i -w -q --ignore-all-space salida_b salida_m > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_m
	 echo " === Expected Output  === "		
	 cat  salida_b
   fi
   rm -f salida* res.txt
   #sleep 1
}

testeoRedir(){
   #$1 nombre del fichero test
   sh     < $2
   ./msh  < $1 2>/dev/null
   if diff -i -w -q --ignore-all-space salida_bash salida_msh > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh
	 echo " === Expected Output  === "		
	 cat  salida_bash
   fi
   rm -f salida* res.txt
   #sleep 1
}

testeoRedirError(){
   #$1 nombre del fichero test
   sh     < $2 >/dev/null 2> salida_bash
   ./msh  < $1 >/dev/null 2>/dev/null
   if diff -i -w -q --ignore-all-space salida_bash salida_msh > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh
	 echo " === Expected Output  === "		
	 cat  salida_bash
   fi
   rm -f salida* res.txt
   #sleep 1
}


testeoRedirErrore(){
   #$1 nombre del fichero test
   sh     < $2 >/dev/null 2> salida_bash
   ./msh  < $1 >/dev/null 2> salida_msh 
   tail -n 1 salida_msh > salida_msh2
   if [ -s ${salida_msh2} ]
   then 
         echo " OK"
         echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
         echo " Error"
         echo -n "0 " >> excel
         echo " === test === "
         cat  $1
         echo " === Output ./msh  === "
         cat  salida_msh2
         echo " === Expected Output  === "
         cat  salida_bash
   fi
   rm -f salida* res.txt salida_bash2
   #sleep 1
}


testeo2(){
   
  ./msh  < $1 > salida_msh  2>/dev/null

   if diff -i -w -q --ignore-all-space $2 salida_msh > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh
	 echo " === Expected Output  === "		
	 cat  $2
   fi
   #sleep 1

}

testeo2Error(){
   
  ./msh  <$1 2> salida_msh  >/dev/null
  cat salida_msh | head -n 4 > msh2
  mv msh2 salida_msh 
   if diff -i -w -q --ignore-all-space $2 salida_msh; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat salida_msh
	 echo " === Expected Output  === "		
	 cat  $2
   fi   
   #sleep 1

}

testeo3(){
   
  ./msh  < $1 2> salida_msh  >/dev/null

   if diff -i -w -q --ignore-all-space $2 salida_msh > /dev/null; then
	 if diff -i -w -q $3 $4 > /dev/null; then
	 	echo " OK"
		echo -n "1 " >> excel
		nota=$(( nota + incremento ))
	 else
		echo " Error"
		echo -n "0 " >> excel
		echo " === test === "
		cat  $1
		echo "The files are different"
	 fi
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh
	 echo " === Expected Output  === "		
	 cat  $2
   fi

}

testeoCalc(){
	ENVIR=`cat var`
	if [ $ENVIR = "Acc=13" ]; then
		echo " OK"
		echo -n "1 " >> excel
		nota=$(( nota + incremento ))
	else
		echo " Error"
	 	echo -n "0 " >> excel
		echo " === test === "
		echo "Use of environment variable"
		echo " === Output ./msh  === "
		echo  $ENVIR
		echo " === Expected Output  === "		
		echo "Acc=13"
	fi
}

testeo2Calc(){
   
  ./msh  < $1 2> salida_msh1  >/dev/null
  ./msh  < $2 2> salida_msh2  >/dev/null
  var1=`diff -i -w -q $3 salida_msh1 > /dev/null`
  var2=`diff -i -w -q $3 salida_msh2 > /dev/null`
  var3=`diff -i -w -q $4 salida_msh1 > /dev/null`
  var4=`diff -i -w -q $4 salida_msh2 > /dev/null` 
   if [ ${var1} || ${var2}  || ${var3} || ${var4} ]; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1 $2
	 echo " === Output ./msh  === "
	 cat  salida_msh1 salida_msh2
	 echo " === Expected Output  === "		
	 cat  $3 $4
   fi
   #sleep 1

}

testeoPwdError(){
   #$1 nombre del fichero test
   ./msh  < $1 2> salida_msh  >/dev/null
   tail -n 2 salida_msh | head -n1 | tee salida_msh_2 >/dev/null
   if diff -i -w -q --ignore-all-space $2 salida_msh_2 > /dev/null; then
	 echo " OK"
	 echo -n "1 " >> excel
	 nota=$(( nota + incremento ))
   else
	 echo " Error"
	 echo -n "0 " >> excel
	 echo " === test === "
         cat  $1
	 echo " === Output ./msh  === "
	 cat  salida_msh_2
	 echo " === Expected Output  === "		
	 cat  $2
   fi
   rm -f salida* res.txt salida_msh_2
   #sleep 1
}


clear

echo "*** TESTING THE MINISHELL "

 
TDIR="./checker"

mkdir -p $TDIR

cp $1 $TDIR
cp Makefile $TDIR
cp *.so $TDIR

cd $TDIR

unzip $1

cp authors.txt ../authors.txt

echo "File : $1"

#cd $TDIR


if [ ! -f authors.txt ]; then
	echo "Error: authors.txt file is missing"
	cd ..
	rm -r $TDIR
	exit
fi
if [ ! -f msh.c ]; then
	echo "Error:  msh.c file is missing"
	cd ..
	rm -r $TDIR
	exit
fi


make clean 2> /dev/null > /dev/null
make       2> /dev/null > /dev/null


echo "Compiling"
if [ ! -f msh ]; then
	echo "Error: Does not compile"
	cd ..
#	rm -r $TDIR
	exit
else
	echo "OK"
fi



#Preparacion de las pruebas
cat > foo.txt << EOF 
123432
67890
12345
46789
90a
10a
EOF

#test

#simples
echo "wc -l  foo.txt"                 	> test1
echo "wc -l  < foo.txt"               	> test2
echo "cat foo.txt > salida_msh"        > test3
echo "cat foo.txt > salida_bash"       > test3.res
echo "cat noexiste.txt !> res.txt"    	> test4
cat noexiste.txt 2> res.txt    		   > test4.res

#1 pipe
echo "cat foo.txt | grep a"                     >test5
echo "grep 1      | grep a < foo.txt"           >test6
echo "10a"                                      >test6.res
echo "cat foo.txt | grep a > salida_msh"        >test7
echo "cat foo.txt | grep a > salida_bash"       >test7.res
echo "cat noexiste.txt | grep a !> salida_msh"  >test7error
echo "cat noexiste.txt | grep a"                >test7error.res

#2 pipes
echo "cat foo.txt | grep a | grep 1"                    >test8
echo "grep 1      | grep a | wc -l < foo.txt"           >test9
echo "1"                                                >test9.res
echo "cat foo.txt | grep a | wc -l > salida_msh"        >test10
echo "cat foo.txt | grep a | wc -l > salida_bash"       >test10.res
echo "cat noexiste.txt | grep a | wc -l !> salida_msh"  >test10Error
echo "cat noexiste.txt | grep a | wc -l"                >test10Error.res

#N pipes
echo "cat foo.txt | grep 1 | grep 2 | grep 3 | grep 4"  >test11

#Background
echo "sleep 1 &"			        > test21
echo "ps | grep sleep | wc -l"  >> test21
echo "sleep 1"			           > test21
echo "sleep 1 &"			        > test21.res
echo "ps | grep sleep | wc -l"  >> test21.res

#mycalc
echo "mycalc 3 add -8"                >calc1
#echo "MSH>> mycalc 3 add -8"         >calc1.res
echo "MSH>>[OK] 3 + -8 = -5; Acc -5"  >>calc1.res
echo "MSH>>"                          >>calc1.res
echo "mycalc 3 add -8"                >calc2
echo "mycalc 5 add 13"                >>calc2
echo "env | grep Acc > var"           >>calc2

#echo "MSH>> mycalc 3 add -8"         >calc2.res
echo "MSH>>[OK] 3 + -8 = -5; Acc -5"  >>calc2.res
#echo "MSH>> mycalc 5 add 13"         >>calc2.res
echo "MSH>>[OK] 5 + 13 = 18; Acc 13"  >>calc2.res
echo "MSH>>MSH>>"                     >>calc2.res

echo "mycalc 3 mul 8"                >calc3
#echo "MSH>> mycalc 10 mod 7"        > calc3.res
echo "MSH>>[OK] 3 * 8 = 24"          >>calc3.res
echo "MSH>>"                         >>calc3.res

echo "mycalc 10 div 7"                >calc4
#echo "MSH>> mycalc 10 mod 7"         > calc4.res
echo "MSH>>[OK] 10 / 7 = 1; Remainder 3"  >>calc4.res
echo "MSH>>"                          >>calc4.res

echo "mycalc 10 mod 7"                 >calc5
echo "[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>"  >calc5.res

echo "mycalc mycalc 8 mas"            >calc6
echo "[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>"  >calc6.res

#myhistory
echo "ls -las"                   > myhistory1
echo "ls -las | wc"             >> myhistory1
echo "myhistory"                >> myhistory1
echo "MSH>>MSH>>MSH>>0 ls -las"  > myhistory1.res
echo "1 ls -las | wc"           >> myhistory1.res
echo "MSH>>"                    >> myhistory1.res


echo "ls -las"                                  > myhistory2
echo "ls -las | wc"                            >> myhistory2
echo "myhistory 0"                             >> myhistory2
echo "MSH>>MSH>>MSH>>Running command 0"         > myhistory2.res
echo "MSH>>"                                   >> myhistory2.res

echo "myhistory 21" > myhistory3
echo "ERROR: Command not found"  >myhistory3.res



#Basicos
#mandato simple
echo -n "Test1: 1 command -> "
testeo test1
echo -n "Test2: 1 command + input redirection -> "
testeo test2
echo -n "Test3: 1 command + output redirection -> "
testeoRedir test3 test3.res	
echo -n "Test4: 1 command + error redirection -> "
testeo2 test4 test4.res


echo -n "Test5: 2 commands -> "
testeo test5
echo -n "Test6: 2 commands + input redirection -> "
testeo2 test6 test6.res
echo -n "Test7: 2 commands + output redirection -> "
testeoRedir test7 test7.res
echo -n "Test8: 2 commands + error redirection -> "
testeoRedirErrore test7error test7error.res


echo -n "Test9: 3 commands -> "
testeo test8
echo -n "Test10: 3 commands + input redirection -> "
testeo2 test9 test9.res
echo -n "Test11: 3 commands + output redirection -> "
testeoRedir test10 test10.res
echo -n "Test12: 3 commands + error redirection -> "
testeoRedirErrore test10Error test10Error.res


echo -n "Test13: N commands -> "
testeo test11

#mandatos internos
echo -n "Test14: Calculator 1 -> "
testeo2Error calc1 calc1.res  
echo -n "Test15: Calculator 2 -> "
testeo2Error calc2 calc2.res
echo -n "Test16: Calculator 3 -> "
testeo2Error calc3 calc3.res 
echo -n "Test17: Calculator 4 -> "
testeo2Error calc4 calc4.res  
echo -n "Test18: Calculator 5 -> "
testeo2 calc5 calc5.res
echo -n "Test19: Calculator 6 -> "
testeo2 calc6 calc6.res
echo -n "Test20: Calculator \$Acc -> "
testeoCalc

echo -n "Test21: myhistory 1 -> "
testeo2Error myhistory1 myhistory1.res
echo -n "Test22: myhistory 2 -> "
testeo2Error myhistory2 myhistory2.res
echo -n "Test23: myhistory 3 -> "
testeo2 myhistory3 myhistory3.res


#background
echo -n "Test22: Background -> "
testeo test21 test21.res

echo "Summary of tests: -> "
fnota=$((echo scale=2 ; echo $nota*10 / 24) | bc )
echo "grade: $fnota" 
echo $fnota > ../grade.txt
#cat excel

rm -fr foo.txt test* excel tmp/msh.c salida_msh time* pwd1 pwd2*

#salimos del directorio
cd ..
rm -r $TDIR

